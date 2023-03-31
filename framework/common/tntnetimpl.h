/*
 * Copyright (C) 2015 Tommi Maekitalo
 *
 */

#ifndef TNTNETIMPL_H
#define TNTNETIMPL_H

#include <tnt/job.h>
#include <tnt/poller.h>
#include <tnt/dispatcher.h>
#include <tnt/maptarget.h>
#include <tnt/scopemanager.h>
#include <cxxtools/sslctx.h>
#include <set>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace tnt
{
class Listener;
class Worker;
struct TntConfig;

class TntnetImpl
{
    friend class Worker;

    unsigned _minthreads;
    unsigned _maxthreads;

    Jobqueue _queue;

    static bool _stop;

    std::vector<std::unique_ptr<Listener>> _listeners;
    std::vector<std::unique_ptr<Worker>> _worker;
    mutable std::mutex _workerMutex;

    unsigned countWorker() const
    {
        std::lock_guard<std::mutex> lock(_workerMutex);
        return _worker.size();
    }

    std::unique_ptr<std::thread> _pollerthread;
    Poller _poller;
    Dispatcher _dispatcher;

    ScopeManager _scopemanager;
    std::string _appname;

    std::ofstream _accessLog;
    std::mutex _accessLogMutex;

    void workerFinished(const Worker* w);

    void timerTask();

    static std::condition_variable _timerStopCondition;
    static std::mutex _timeStopMutex;

    TntnetImpl(const TntnetImpl&) = delete;
    TntnetImpl& operator= (const TntnetImpl&) = delete;

public:
    TntnetImpl();
    ~TntnetImpl();

    void init(Tntnet& app, const TntConfig& config);

    void listen(Tntnet& app, const std::string& ipaddr, unsigned short int port, const cxxtools::SslCtx& sslCtx = cxxtools::SslCtx());

    void run();

    static void shutdown();
    static bool shouldStop()                { return _stop; }

    Jobqueue&   getQueue()                  { return _queue; }
    Poller&     getPoller()                 { return _poller; }
    const Dispatcher& getDispatcher() const { return _dispatcher; }
    ScopeManager& getScopemanager()         { return _scopemanager; }

    unsigned getMinThreads() const          { return _minthreads; }
    void setMinThreads(unsigned n);

    unsigned getMaxThreads() const          { return _maxthreads; }
    void setMaxThreads(unsigned n)          { _maxthreads = n; }

    Mapping& mapUrl(const std::string& url, const std::string& ci)
      { return _dispatcher.addUrlMapEntry(std::string(), url, Maptarget(ci)); }

    Mapping& mapUrl(const std::string& url, const Compident& ci)
      { return _dispatcher.addUrlMapEntry(std::string(), url, Maptarget(ci)); }

    void mapUrl(const std::string& url, const std::string& pathinfo, const std::string& ci)
      { _dispatcher.addUrlMapEntry(std::string(), url, Maptarget(ci)).setPathInfo(pathinfo); }

    Mapping& mapUrl(const std::string& url, const Maptarget& ci)
      { return _dispatcher.addUrlMapEntry(std::string(), url, ci); }

    Mapping& vMapUrl(const std::string& vhost, const std::string& url, const Maptarget& ci)
      { return _dispatcher.addUrlMapEntry(vhost, url, ci); }

    void setAppName(const std::string& appname)
      { _appname = appname; }

    const std::string& getAppName() const
      { return _appname; }

    void setAccessLog(const std::string& logfile_path)
      { _accessLog.open(logfile_path.c_str(), std::ios::out | std::ios::app); }
};
}

#endif // TNTNETIMPL_H

