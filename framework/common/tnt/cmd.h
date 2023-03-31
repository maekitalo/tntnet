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


#ifndef TNT_CMD_H
#define TNT_CMD_H

#include <iosfwd>
#include <tnt/tntnet.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/scope.h>
#include <tnt/scopemanager.h>
#include <tnt/comploader.h>
#include <tnt/compident.h>

namespace tnt
{
class Cmd
{
    Tntnet _application;
    ScopeManager _scopeManager;
    HttpRequest _request;
    HttpReply _reply;
    Comploader _comploader;
    std::string _sessionId;

    // thread context methods
    class MyThreadContext : public ThreadContext
    {
        Scope _threadScope;

    public:
        void touch() { }
        Scope& getScope() { return _threadScope; }
    } threadContext;

public:
    explicit Cmd(std::ostream& out);

    Tntnet& getApplication() { return _application; }

    HttpRequest& request()  { return _request; }
    void call(const Compident& ci, const QueryParams& q);
    void call(const Compident& ci);
};
}

#endif // TNT_CMD_H
