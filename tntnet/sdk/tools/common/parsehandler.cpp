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
    void ParseHandler::start()
    {
    }

    void ParseHandler::end()
    {
    }

    void ParseHandler::onHtml(const std::string& html)
    {
    }

    void ParseHandler::onExpression(const std::string& code)
    {
    }

    void ParseHandler::onHtmlExpression(const std::string& code)
    {
    }

    void ParseHandler::onCpp(const std::string& code)
    {
    }

    void ParseHandler::onPre(const std::string& code)
    {
    }

    void ParseHandler::onDeclare(const std::string& code)
    {
    }

    void ParseHandler::onInit(const std::string& code)
    {
    }

    void ParseHandler::onCleanup(const std::string& code)
    {
    }

    void ParseHandler::onArg(const std::string& name,
      const std::string& value)
    {
    }

    void ParseHandler::onAttr(const std::string& name,
      const std::string& value)
    {
    }

    void ParseHandler::onCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const std::string& cppargs)
    {
    }

    void ParseHandler::onEndCall(const std::string& comp)
    {
    }

    void ParseHandler::onDeclareShared(const std::string& code)
    {
    }

    void ParseHandler::onShared(const std::string& code)
    {
    }

    void ParseHandler::onScope(scope_container_type container, scope_type scope,
      const std::string& type, const std::string& var, const std::string& init)
    {
    }

    void ParseHandler::startComp(const std::string& arg, const cppargs_type& cppargs)
    {
    }

    void ParseHandler::startClose()
    {
    }

    void ParseHandler::endClose()
    {
    }

    void ParseHandler::onComp(const std::string& code)
    {
    }

    void ParseHandler::onCondExpr(const std::string& cond, const std::string& expr)
    {
    }

    void ParseHandler::onConfig(const std::string& cond, const std::string& value)
    {
    }

    void ParseHandler::tokenSplit(bool start)
    {
    }

    void ParseHandler::onInclude(const std::string& file)
    {
    }

    void ParseHandler::onIncludeEnd(const std::string& file)
    {
    }

    void ParseHandler::startI18n()
    {
    }

    void ParseHandler::endI18n()
    {
    }

  }
}
