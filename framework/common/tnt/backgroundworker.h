/*
 * Copyright (C) 2011 Tommi Maekitalo
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

      private:
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
