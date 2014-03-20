/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef TNT_SESSIONUNLOCKER_H
#define TNT_SESSIONUNLOCKER_H

#include <tnt/httprequest.h>

namespace tnt
{
  /** Unlocks the session as long as the object is in scope.

      Normally the session is locked as soon as a request uses somewhere
      a session variable. Sometimes it is desirable to explicitely release
      that lock, e.g. when a request has some longer task to do.

      This frequently happens in reverse ajax, where the request blocks while
      waiting for a event.
   */
  class SessionUnlocker
  {
      HttpRequest& _request;
      bool _locked;

    public:
      explicit SessionUnlocker(HttpRequest& request, bool release = true)
        : _request(request)
          _locked(request.sessionScopeLocked)
      {
        if (locked && release)
          _request.releaseSessionScopeLock();
      }

      ~SessionUnlocker()
      {
        if (locked)
          _request.ensureSessionScopeLock();
      }

      void unlock()
      {
        _request.releaseSessionScopeLock();
        _locked = false;
      }

      void lock()
      {
        _request.ensureSessionScopeLock();
        _locked = true;
      }
  };
}

#endif // TNT_SESSIONUNLOCKER_H

