/*
 * Copyright (C) 2007 Tommi Maekitalo
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


#ifndef TNT_POLLERIMPL_H
#define TNT_POLLERIMPL_H

#include <config.h>
#include "tnt/job.h"
#include "tnt/poller.h"
#include <cxxtools/posix/pipe.h>
#include <mutex>

#if defined(HAVE_EPOLL_CREATE) || defined(HAVE_EPOLL_CREATE1)

#  define WITH_EPOLL
#  include <map>

#else

#  include <deque>
#  include <vector>
#  include <sys/poll.h>

#endif

namespace tnt
{
/// @cond internal
class PollerImpl : public PollerIf
{
private:
    Jobqueue& _queue;

    cxxtools::posix::Pipe _notifyPipe;
    std::mutex _mutex;

#ifdef WITH_EPOLL

    int _pollFd;

    typedef std::map<int, std::unique_ptr<Job>> jobs_type;
    typedef std::vector<std::unique_ptr<Job>> new_jobs_type;
    jobs_type _jobs;
    new_jobs_type _newJobs;
    int _pollTimeout;

    void addFd(int fd);
    bool removeFd(int fd);
    void appendNewJobs();

#else

    typedef std::deque<std::unique_ptr<Job>> jobs_type;
    typedef std::vector<pollfd> pollfds_type;

    jobs_type _currentJobs;
    pollfds_type _pollfds;
    jobs_type _newJobs;

    int _pollTimeout;

    void appendNewJobs();
    void append(std::unique_ptr<Job>&& job);
    void dispatch();
    void remove(jobs_type::size_type n);

#endif // #else WITH_EPOLL

public:
    PollerImpl(Jobqueue& q);
#ifdef WITH_EPOLL
    ~PollerImpl();
#endif

    virtual void run();
    void doStop();
    void addIdleJob(std::unique_ptr<Job>&& job);
};
/// @endcond internal
}

#endif
