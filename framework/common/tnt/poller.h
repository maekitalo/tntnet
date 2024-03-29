/*
 * Copyright (C) 2005-2006 Tommi Maekitalo
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


#ifndef TNT_POLLER_H
#define TNT_POLLER_H

#include <tnt/job.h>

namespace tnt
{
/// @cond internal
class PollerIf
{
    PollerIf(const PollerIf&) { }
    PollerIf& operator=(const PollerIf&) { return *this; }

public:
    PollerIf() { }
    virtual ~PollerIf();
    virtual void run() = 0;
    virtual void doStop() = 0;
    virtual void addIdleJob(std::unique_ptr<Job>&& job) = 0;
};

class Poller
{
    Poller(const Poller&) { }
    Poller& operator=(const Poller&) { return *this; }

    PollerIf* _impl;

public:
    explicit Poller(Jobqueue& q);
    ~Poller()
      { delete _impl; }

    void run();
    void doStop()                          { _impl->doStop(); }
    void addIdleJob(std::unique_ptr<Job>&& job) { _impl->addIdleJob(std::move(job)); }
};
/// @endcond internal
}

#endif // TNT_POLLER_H
