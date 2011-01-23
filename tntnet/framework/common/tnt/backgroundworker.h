/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef TNT_BACKGROUNDWORKER_H
#define TNT_BACKGROUNDWORKER_H

#include <cxxtools/refcounted.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/threadpool.h>
#include <cxxtools/condition.h>
#include <cxxtools/mutex.h>
#include <cxxtools/timespan.h>
#include <cxxtools/clock.h>
#include <list>

namespace tnt
{
  class BackgroundTask : public cxxtools::AtomicRefCounted
  {
          friend class BackgroundWorker;

      public:
          typedef cxxtools::SmartPtr<BackgroundTask> Pointer;

          BackgroundTask(const cxxtools::Timespan& livetime = cxxtools::Timespan(0, 0))
              : _progress(0),
                _id(_nextId++),
                _livetime(livetime),
                _jobReady(0)
              { }
          virtual ~BackgroundTask()
              { }

          unsigned id() const
              { return _id; }
          unsigned progress() const;
          bool ready() const
              { return progress() >= 100; }
          const cxxtools::DateTime& timeoutTime() const
              { return _timeoutTime; }
          const cxxtools::Timespan& livetime() const
              { return _livetime; }
          void livetime(const cxxtools::Timespan& livetime);

          void updateProgress(unsigned p);

          virtual void execute() = 0;

      private:
          void doTask();
          unsigned _progress;
          unsigned _id;
          static unsigned _nextId;
          cxxtools::DateTime _timeoutTime;
          cxxtools::Timespan _livetime;
          mutable cxxtools::Mutex _mutex;
          cxxtools::Condition* _jobReady;
  };

  class BackgroundWorker
  {
          BackgroundWorker();
          ~BackgroundWorker();

      public:
          static BackgroundWorker& it();

          unsigned runTask(BackgroundTask::Pointer task);

          BackgroundTask::Pointer getTask(unsigned id) const;
          void removeTask(unsigned id);

          static void setMaxJobs(unsigned n)
          { _maxJobs = n; }
          static unsigned getMaxJobs()
          { return _maxJobs; }

      private:
          static unsigned _maxJobs;
          mutable cxxtools::Mutex _mutex;
          cxxtools::Condition _jobReady;
          cxxtools::ThreadPool _threadPool;
          bool _running;
          void timeoutCheckerFunc();

          typedef std::list<BackgroundTask::Pointer> Tasks;
          Tasks tasks;

  };
}

#endif // TNT_BACKGROUNDWORKER_H
