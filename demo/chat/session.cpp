/*
 * Copyright (C) 2014 Tommi Maekitalo
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

#include "session.h"

Session::Sessions Session::_sessions;
cxxtools::Mutex Session::_sessionsMutex;

Session::Session(unsigned max)
  : _max(max)
{
  cxxtools::MutexLock lock(_sessionsMutex);
  _sessions.insert(this);
}

Session::~Session()
{
  cxxtools::MutexLock lock(_sessionsMutex);
  _sessions.erase(this);
}

Session::Messages Session::messages()
{
  cxxtools::MutexLock lock(_messagesMutex);

  std::pair<std::string, bool> ret;

  while ((ret = _queue.tryGet()).second)
  {
    _messages.push_back(ret.first);
    if (_max > 0 && _messages.size() > _max)
      _messages.pop_front();
  }

  return _messages;
}

Session::Messages Session::messages(cxxtools::Seconds wait)
{
  if (wait == 0)
    return messages();

  std::pair<std::string, bool> ret = _queue.get(wait);

  cxxtools::MutexLock lock(_messagesMutex);
  while (ret.second)
  {
    _messages.push_back(ret.first);
    if (_max > 0 && _messages.size() > _max)
      _messages.pop_front();
    ret = _queue.tryGet();
  }

  return _messages;
}

void Session::broadcastMessage(const std::string& message)
{
  cxxtools::MutexLock lock(_sessionsMutex);

  for (Sessions::iterator it = _sessions.begin(); it != _sessions.end(); ++it)
  {
    (*it)->addMessage(message);
  }
}

