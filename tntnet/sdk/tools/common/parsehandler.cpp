/* parserhandler.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "tnt/ecpp/parsehandler.h"

namespace tnt
{
  namespace ecpp
  {
    void parseHandler::start()
    {
    }

    void parseHandler::end()
    {
    }

    void parseHandler::onHtml(const std::string& html)
    {
    }

    void parseHandler::onExpression(const std::string& code)
    {
    }

    void parseHandler::onCpp(const std::string& code)
    {
    }

    void parseHandler::onPre(const std::string& code)
    {
    }

    void parseHandler::onDeclare(const std::string& code)
    {
    }

    void parseHandler::onInit(const std::string& code)
    {
    }

    void parseHandler::onCleanup(const std::string& code)
    {
    }

    void parseHandler::onArg(const std::string& name,
      const std::string& value)
    {
    }

    void parseHandler::onAttr(const std::string& name,
      const std::string& value)
    {
    }

    void parseHandler::onCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const std::string& cppargs)
    {
    }

    void parseHandler::onDeclareShared(const std::string& code)
    {
    }

    void parseHandler::onShared(const std::string& code)
    {
    }

    void parseHandler::onScope(scope_container_type container, scope_type scope,
      const std::string& type, const std::string& var, const std::string& init)
    {
    }

    void parseHandler::startComp(const std::string& arg, const cppargs_type& cppargs)
    {
    }

    void parseHandler::onComp(const std::string& code)
    {
    }

    void parseHandler::onCondExpr(const std::string& cond, const std::string& expr)
    {
    }

    void parseHandler::onConfig(const std::string& cond, const std::string& value)
    {
    }

    void parseHandler::tokenSplit(bool start)
    {
    }

    void parseHandler::onInclude(const std::string& file)
    {
    }
  }
}
