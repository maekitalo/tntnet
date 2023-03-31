/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "tnt/tcpjob.h"
#include "tntnetimpl.h"

#include <cxxtools/log.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

log_define("tntnet.tcpjob")

namespace tnt
{
////////////////////////////////////////////////////////////////////////
// Tcpjob
//
std::string Tcpjob::getPeerIp() const
{
    return _socket.getPeerAddr();
}

std::string Tcpjob::getServerIp() const
{
    return _socket.getSockAddr();
}

bool Tcpjob::isSsl() const
{
    return _sslCtx.enabled();
}

cxxtools::SslCertificate Tcpjob::getSslCertificate() const
{
    return _socket.getSslPeerCertificate();
}

void Tcpjob::accept()
{
    _socket.accept(_listener);
    log_debug("connection accepted from " << getPeerIp());
}

void Tcpjob::regenerateJob()
{
  if (TntnetImpl::shouldStop())
  {
      _queue.put(nullptr);
  }
  else
  {
      _queue.put(std::unique_ptr<Job>(new Tcpjob(getRequest().getApplication(), _listener, _queue, _sslCtx)));
  }
}

std::iostream& Tcpjob::getStream()
{
    if (!_socket.isConnected())
    {
        try
        {
            log_debug("accept socket");
            accept();
            touch();
        }
        catch (const std::exception& e)
        {
            regenerateJob();
            log_debug("exception occured in accept: " << e.what());
            throw;
        }

        regenerateJob();
    }

    if (!_socket.isSslConnected() && _sslCtx.enabled())
    {
        log_debug("accept ssl " << getFd());
        _socket.sslAccept(_sslCtx);
        touch();
    }

    return _socket;
}

int Tcpjob::getFd() const
{
    return _socket.getFd();
}

void Tcpjob::setRead()
{
    _socket.setTimeout(TntConfig::it().socketReadTimeout);
}

void Tcpjob::setWrite()
{
    _socket.setTimeout(TntConfig::it().socketWriteTimeout);
}

//////////////////////////////////////////////////////////////////////
// Jobqueue
//
void Jobqueue::put(std::unique_ptr<Job> j, bool force)
{
    j->touch();

    std::unique_lock<std::mutex> lock(_mutex);

    if (!force && _capacity > 0)
    {
        while (_jobs.size() >= _capacity)
        {
            log_warn("Jobqueue full");
            _notFull.wait(lock);
        }
    }

    _jobs.emplace_back(std::move(j));

    if (_waitThreads == 0)
        noWaitThreads.notify_one();

    _notEmpty.notify_one();
}

std::unique_ptr<Job> Jobqueue::get()
{
    std::unique_lock<std::mutex> lock(_mutex);

    // wait, until a job is available
    ++_waitThreads;

    while (_jobs.empty())
        _notEmpty.wait(lock);

    --_waitThreads;

    // take next job (queue is locked)
    auto j = std::move(_jobs.front());
    _jobs.pop_front();

    // if there are threads waiting, wake another
    if (!_jobs.empty() && _waitThreads > 0)
        _notEmpty.notify_one();

    _notFull.notify_one();

    return j;
}
}

