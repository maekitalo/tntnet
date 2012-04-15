/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <tnt/backgroundworker.h>
#include <tnt/tntconfig.h>
#include <cxxtools/log.h>

log_define("tntnet.backgroundworker")

namespace tnt
{

unsigned BackgroundTask::_nextId = 1;

BackgroundWorker& BackgroundWorker::it()
{
    static BackgroundWorker* theWorker = 0;
    static cxxtools::Mutex mutex;

    static class D
    {
      public:
        ~D()
        { delete theWorker; }
    } d;

    cxxtools::MutexLock lock(mutex);
    if (theWorker == 0)
        theWorker = new BackgroundWorker();

    return *theWorker;
}

BackgroundWorker::BackgroundWorker()
    : _threadPool(TntConfig::it().backgroundTasks + 1),
      _running(true)
{
    _threadPool.schedule(cxxtools::callable(*this, &BackgroundWorker::timeoutCheckerFunc));
}

BackgroundWorker::~BackgroundWorker()
{
    _running = false;
    cxxtools::MutexLock lock(_mutex);
    _jobReady.signal();
    lock.unlock();

    log_info("wait for background jobs to finish");
    _threadPool.stop(true);
    log_info("background jobs finished");
}

void BackgroundWorker::timeoutCheckerFunc()
{
    while (_running)
    {
        cxxtools::MutexLock lock(_mutex);

        if (!_running)
            break;

        cxxtools::DateTime current = cxxtools::Clock::getSystemTime();
        cxxtools::DateTime nextTimeout = current + cxxtools::Timespan(3600, 0);

        for (Tasks::iterator it = tasks.begin(); it != tasks.end(); )
        {
            // check if timed out
            bool ready = (*it)->ready();

            if (ready && current >= (*it)->timeoutTime())
            {
                log_info("timeout reached for job " << (*it)->id());
                it = tasks.erase(it);
            }
            else
            {
                // check for next timeout
                if (ready
                    && (*it)->timeoutTime() < nextTimeout)
                {
                    nextTimeout = (*it)->timeoutTime();
                }

                ++it;
            }
        }

        _jobReady.wait(_mutex, (nextTimeout - current).totalMSecs());
    }

    log_debug("timeoutCheckerFunc end");
}

unsigned BackgroundTask::progress() const
{
    cxxtools::MutexLock lock(_mutex);
    return _progress;
}

void BackgroundTask::doTask()
{
    log_debug("job id " << id() << " started");
    execute();

    if (progress() < 100)
        updateProgress(100);

    log_debug("job id " << id() << " finished");
}

void BackgroundTask::updateProgress(unsigned p)
{
    cxxtools::MutexLock lock(_mutex);
    _progress = p;
    if (_progress >= 100)
    {
        _timeoutTime = cxxtools::Clock::getSystemTime() + _livetime;
        log_debug("signal job ready");
        _jobReady->signal();
    }
}

void BackgroundTask::livetime(const cxxtools::Timespan& livetime)
{
    cxxtools::MutexLock lock(_mutex);
    _livetime = livetime;
    if (_progress >= 100)
        _timeoutTime = cxxtools::Clock::getSystemTime() + _livetime;
}

unsigned BackgroundWorker::runTask(BackgroundTask::Pointer task)
{
    cxxtools::MutexLock lock(_mutex);
    task->_jobReady = &_jobReady;
    tasks.push_back(task);
    _threadPool.schedule(cxxtools::callable(*task.getPointer(), &BackgroundTask::doTask));
    return task->id();
}

BackgroundTask::Pointer BackgroundWorker::getTask(unsigned id) const
{
    cxxtools::MutexLock lock(_mutex);
    for (Tasks::const_iterator it = tasks.begin(); it != tasks.end(); ++it)
    {
        if ((*it)->id() == id)
        {
            BackgroundTask::Pointer ret = *it;
            return ret;
        }
    }
    return 0;
}

void BackgroundWorker::removeTask(unsigned id)
{
    cxxtools::MutexLock lock(_mutex);
    for (Tasks::iterator it = tasks.begin(); it != tasks.end(); )
    {
        if ((*it)->id() == id)
        {
            tasks.erase(it);
            break;
        }
    }
}

}
