/*
 * Copyright (C) 2005 Tommi Maekitalo
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


#include "tnt/ecppc/component.h"

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Component
    //

    std::string Component::getLogCategory() const
    {
      if (!logCategory.empty())
        return logCategory;

      std::string ret = "component." + componentName;
      std::string::size_type pos;
      while ((pos = ret.find('/')) != std::string::npos)
        ret[pos] = '.';
      return ret;
    }

    void Component::getBody(std::ostream& body, bool linenumbersEnabled) const
    {
      if (!args.empty())
      {
        body << "  // <%args>\n";
        getArgs(body);
        body << "  // </%args>\n\n";
      }

      if (!get.empty())
      {
        body << "  // <%get>\n";
        getGet(body);
        body << "  // </%get>\n\n";
      }

      if (!post.empty())
      {
        body << "  // <%post>\n";
        getPost(body);
        body << "  // </%post>\n\n";
      }

      getScopevars(body, linenumbersEnabled);

      body << "  // <%cpp>\n";

      compbody.getBody(body);

      body << "  // <%/cpp>\n"
           << "  return HTTP_OK;\n";
    }

    void Component::getArgs(std::ostream& body) const
    {
      for (variables_type::const_iterator it = args.begin();
           it != args.end(); ++it)
        it->getParamCode(body, "qparam");
    }

    void Component::getGet(std::ostream& body) const
    {
      for (variables_type::const_iterator it = get.begin();
           it != get.end(); ++it)
        it->getParamCode(body, "request.getGetParams()");
    }

    void Component::getPost(std::ostream& body) const
    {
      for (variables_type::const_iterator it = post.begin();
           it != post.end(); ++it)
        it->getParamCode(body, "request.getPostParams()");
    }

    void Component::getScopevars(std::ostream& body, bool linenumbersEnabled) const
    {
      for (scopevars_type::const_iterator it = scopevars.begin();
           it != scopevars.end(); ++it)
      {
        it->get(body, linenumbersEnabled);
      }
    }

    void Component::getScopevars(std::ostream& body, ecpp::scope_type scope, bool linenumbersEnabled) const
    {
      for (scopevars_type::const_iterator it = scopevars.begin();
           it != scopevars.end(); ++it)
      {
        if (it->getScope() == scope)
          it->get(body, linenumbersEnabled);
      }
    }
  }
}
