/*
 * Copyright (C) 2015 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <tntnetimpl.h>
#include <tnt/worker.h>
#include <tnt/listener.h>
#include <tnt/http.h>
#include <tnt/httpreply.h>
#include <tnt/sessionscope.h>
#include <tnt/tntconfig.h>

#include <cxxtools/net/tcpstream.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/log.h>

#include <condition_variable>
#include <unistd.h>
#include <signal.h>

#include <config.h>

log_define("tntnet.tntnet.impl")

namespace tnt
{
namespace
{
    void configureDispatcher(Dispatcher& dis)
    {
        const TntConfig::MappingsType& mappings = TntConfig::it().mappings;
        for (TntConfig::MappingsType::const_iterator it = mappings.begin(); it != mappings.end(); ++it)
        {
            Maptarget ci(it->target);
            if (!it->pathinfo.empty())
                ci.setPathInfo(it->pathinfo);
            ci.setHttpReturn(it->httpreturn);
            ci.setArgs(it->args);
            dis.addUrlMapEntry(it->vhost, it->url, it->method, it->ssl, ci);
        }
    }

    typedef std::set<TntnetImpl*> TntnetInstancesType;
    std::mutex allTntnetInstancesMutex;
    TntnetInstancesType allRunningTntnetInstances;

}

////////////////////////////////////////////////////////////////////////
// Tntnet
//
TntnetImpl::TntnetImpl()
  : _minthreads(TntConfig::it().minThreads),
    _maxthreads(TntConfig::it().maxThreads),
    _poller(_queue)
{ }

TntnetImpl::~TntnetImpl()
{ }

bool TntnetImpl::_stop = false;
std::mutex TntnetImpl::_timeStopMutex;
std::condition_variable TntnetImpl::_timerStopCondition;

void TntnetImpl::init(Tntnet& app, const TntConfig& config)
{
    TntConfig::it() = config;

    log_init(config.logConfiguration);

    _minthreads = config.minThreads;
    _maxthreads = config.maxThreads;

    _queue.setCapacity(config.queueSize);

    for (TntConfig::EnvironmentType::const_iterator it = config.environment.begin(); it != config.environment.end(); ++it)
    {
        std::string name = it->first;
        std::string value = it->second;
#ifdef HAVE_SETENV
        log_debug("setenv " << name << "=\"" << value << '"');
        ::setenv(name.c_str(), value.c_str(), 1);
#else
        char* env = new char[name.size() + value.size() + 2];
        name.copy(env, name.size());
        env[name.size()] = '=';
        value.copy(env + name.size() + 1, value.size());
        env[name.size() + value.size() + 1] = '\0';

        log_debug("putenv(" << env << ')');
        ::putenv(env);
#endif
    }

    configureDispatcher(_dispatcher);

    // initialize listeners
    for (TntConfig::ListenersType::const_iterator it = config.listeners.begin(); it != config.listeners.end(); ++it)
    {
        cxxtools::SslCtx sslCtx;
        if (!it->certificate.empty())
        {
            if (it->secure)
                sslCtx = cxxtools::SslCtx::secure();
            sslCtx.loadCertificateFile(it->certificate, it->key)
                  .setVerify(it->sslVerifyLevel, it->sslCa);
            if (!it->ciphers.empty())
                sslCtx.setCiphers(it->ciphers);
            sslCtx.setProtocolVersion(it->minProtocolVersion, it->maxProtocolVersion);
            if (it->cipherServerPreference)
                sslCtx.setCipherServerPreference(true);
        }

        listen(app, it->ip, it->port, sslCtx);
    }
}

void TntnetImpl::listen(Tntnet& app, const std::string& ip, unsigned short int port, const cxxtools::SslCtx& sslCtx)
{
    log_debug("listen on ip " << ip << " port " << port << (sslCtx.enabled() ? " ssl" : ""));
    _listeners.emplace_back(new Listener(app, ip, port, _queue, sslCtx));
}

void TntnetImpl::run()
{
    log_debug("worker-process");

    _stop = false;

    if (_listeners.empty())
        throw std::runtime_error("no listeners defined");

    log_debug(_listeners.size() << " listeners");

    if (_listeners.size() >= _minthreads)
    {
        log_warn("at least one more worker than listeners needed - set MinThreads to "
          << _listeners.size() + 1);
        _minthreads = _listeners.size() + 1;
    }

    if (_maxthreads < _minthreads)
    {
        log_warn("MaxThreads < MinThreads - set MaxThreads = MinThreads = " << _minthreads);
        _maxthreads = _minthreads;
    }

    // initialize worker-process

    // SIGPIPE must be ignored
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, 0) == -1)
        cxxtools::throwSystemError("sigaction");

    // create worker-threads
    log_info("create " << _minthreads << " worker threads");
    {
        std::lock_guard<std::mutex> lock(_workerMutex);
        for (unsigned i = 0; i < _minthreads; ++i)
        {
            log_debug("create worker " << i);
            _worker.emplace_back(Worker::create(*this));
        }
    }

    // create poller-thread
    log_debug("start poller thread");
    _pollerthread.reset(new std::thread(&Poller::run, &_poller));

    log_debug("start timer thread");
    std::thread timerThread(&TntnetImpl::timerTask, this);

    {
        std::lock_guard<std::mutex> lock(allTntnetInstancesMutex);
        allRunningTntnetInstances.insert(this);
    }

    // mainloop
    std::mutex mutex;
    while (!_stop)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            _queue.noWaitThreads.wait(lock);
        }

        if (_stop)
            break;

        {
            std::lock_guard<std::mutex> lock(_workerMutex);
            if (_worker.size() < _maxthreads)
            {
                log_info("create workerthread");
                _worker.emplace_back(Worker::create(*this));
            }
            else
                log_info("max worker-threadcount " << _maxthreads << " reached");
        }

        if (TntConfig::it().threadStartDelay > 0)
            usleep(TntConfig::it().threadStartDelay.totalUSecs());
    }

    log_info("stopping TntnetImpl");

    {
        std::lock_guard<std::mutex> lock(allTntnetInstancesMutex);
        allRunningTntnetInstances.erase(this);
    }

    log_info("stop listener");
    for (auto it = _listeners.begin(); it != _listeners.end(); ++it)
        (*it)->terminate();

    log_info("stop poller thread");
    _poller.doStop();
    _pollerthread->join();

    log_info("stop timer thread");
    timerThread.join();

    if (countWorker() > 0)
    {
        log_info("wait for " << countWorker() << " worker threads to stop");
        unsigned c;
        while ((c = countWorker()) > 0)
        {
            log_debug("wait for worker threads to stop; " << c << " left");
            usleep(100);
        }
    }

    log_debug("destroy listener");
    _listeners.clear();

    HttpReply::postRunCleanup();
    HttpRequest::postRunCleanup();

    log_info("all threads stopped");
}

void TntnetImpl::setMinThreads(unsigned n)
{
    if (_listeners.size() >= n)
    {
        log_warn("at least one more worker than listeners needed - set MinThreads to "
          << _listeners.size() + 1);
        _minthreads = _listeners.size() + 1;
    }
    else
        _minthreads = n;

}

void TntnetImpl::workerFinished(const Worker* w)
{
    std::lock_guard<std::mutex> lock(_workerMutex);

    log_debug("end worker thread " << w->_threadId << " - " << _worker.size()
      << " threads left - " << getQueue().getWaitThreadCount()
      << " waiting threads");

    for (auto it = _worker.begin(); it != _worker.end(); ++it)
    {
        if (it->get() == w)
        {
            _worker.erase(it);
            break;
        }
    }
}

void TntnetImpl::timerTask()
{
    log_debug("timer thread");

    while (true)
    {
        {
            std::unique_lock<std::mutex> timeStopLock(_timeStopMutex);
            if (_stop || _timerStopCondition.wait_for(timeStopLock, std::chrono::milliseconds(TntConfig::it().timerSleep)) != std::cv_status::timeout)
                break;
        }

        getScopemanager().checkSessionTimeout();

        time_t currentTime;
        time(&currentTime);

        std::lock_guard<std::mutex> lock(_workerMutex);
        for (auto& w: _worker)
            w->healthCheck(currentTime);
    }

    _queue.noWaitThreads.notify_all();
    _minthreads = _maxthreads = 0;
}

void TntnetImpl::shutdown()
{
    _stop = true;
    _timerStopCondition.notify_all();
}
}

