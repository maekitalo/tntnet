/* tnt/ecpp/parsehandler.h
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

#ifndef TNT_ECPP_PARSERHANDLER_H
#define TNT_ECPP_PARSERHANDLER_H

#include <tnt/ecpp/parser.h>

namespace tnt
{
  namespace ecpp
  {
    class ParseHandler
    {
      public:
        typedef Parser::comp_args_type comp_args_type;
        typedef Parser::cppargs_type cppargs_type;

        ParseHandler()
          { }

        virtual ~ParseHandler() {}

        virtual void start();
        virtual void end();
        virtual void onHtml(const std::string& html);
        virtual void onExpression(const std::string& expr);
        virtual void onHtmlExpression(const std::string& expr);
        virtual void onCpp(const std::string& code);
        virtual void onPre(const std::string& code);
        virtual void onDeclare(const std::string& code);
        virtual void onInit(const std::string& code);
        virtual void onCleanup(const std::string& code);
        virtual void onArg(const std::string& name,
          const std::string& value);
        virtual void onAttr(const std::string& name,
          const std::string& value);
        virtual void onCall(const std::string& comp,
          const comp_args_type& args, const std::string& pass_cgi,
          const std::string& cppargs);
        virtual void onEndCall(const std::string& comp);
        virtual void onDeclareShared(const std::string& code);
        virtual void onShared(const std::string& code);
        virtual void onScope(scope_container_type container, scope_type scope,
          const std::string& type, const std::string& var, const std::string& init);
        virtual void startComp(const std::string& name, const cppargs_type& cppargs);
        virtual void startClose();
        virtual void endClose();
        virtual void onComp(const std::string& code);
        virtual void onCondExpr(const std::string& cond, const std::string& expr);
        virtual void onConfig(const std::string& code, const std::string& value);
        virtual void tokenSplit(bool start);
        virtual void onInclude(const std::string& file);
        virtual void onIncludeEnd(const std::string& file);
        virtual void startI18n();
        virtual void endI18n();
    };

  }
}

#endif // TNT_ECPP_PARSERHANDLER_H

