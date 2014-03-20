/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "static.h"
#include "unzip.h"

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/httperror.h>
#include <tnt/unzipfile.h>
#include <cxxtools/log.h>

log_define("tntnet.unzip")

namespace tnt
{
  class MimeHandler;

  ////////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class Unzip : public Static
  {
    friend class UnzipFactory;

    public:
      virtual unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
  };

  static ComponentFactoryImpl<Unzip> factory("unzip");

  ////////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  unsigned Unzip::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparams)
  {
    std::string pi = request.getPathInfo();

    log_debug("unzip archive \"" << request.getArg("file") << "\" file \"" << pi << '"');

    try
    {
      unzipFile f(request.getArg("file"));
      unzipFileStream in(f, pi, false);

      // set Content-Type
      std::string contentType = request.getArg("contenttype");
      if (contentType.empty())
        setContentType(request, reply);
      else
        reply.setContentType(contentType);

      reply.out() << in.rdbuf();
    }
    catch (const unzipEndOfListOfFile&)
    {
      log_debug("file \"" << pi << "\" not found in archive");
      return DECLINED;
    }

    return HTTP_OK;
  }
}

