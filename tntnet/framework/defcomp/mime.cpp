/*
 * Copyright (C) 2003,2007 Tommi Maekitalo
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

#include "mime.h"
#include "mimehandler.h"
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/comploader.h>

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // factory
  //
  class MimeFactory : public ComponentFactory
  {
    public:
      MimeFactory(const std::string& componentName)
        : ComponentFactory(componentName)
        { }
      virtual Component* doCreate(const Compident& ci,
        const Urlmapper& um, Comploader& cl);
      virtual void doConfigure(const TntConfig& config);
  };

  Component* MimeFactory::doCreate(const Compident&,
    const Urlmapper&, Comploader&)
  {
    return new Mime();
  }

  void MimeFactory::doConfigure(const TntConfig& config)
  {
    if (Mime::handler == 0)
      Mime::handler = new MimeHandler();
  }

  static MimeFactory mimeFactory("mime");

  ////////////////////////////////////////////////////////////////////////
  // componentdefinition
  //

  MimeHandler* Mime::handler = 0;

  unsigned Mime::operator() (HttpRequest& request,
    HttpReply& reply, QueryParams& qparams)
  {
    if (request.getArgs().size() > 0)
      reply.setContentType(request.getArg(0).c_str());
    else if (handler)
      reply.setContentType(handler->getMimeType(request.getPathInfo()).c_str());

    // we do not produce any content, so we pass the request
    // to the next handler:
    return DECLINED;
  }

}
