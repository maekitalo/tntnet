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


#ifndef TNT_ECPP_PARSERHANDLER_H
#define TNT_ECPP_PARSERHANDLER_H

#include <tnt/ecpp/parser.h>
#include <vector>

namespace tnt
{
  namespace ecpp
  {
    class ParseHandler
    {
      public:
        typedef Parser::comp_args_type comp_args_type;
        typedef Parser::cppargs_type cppargs_type;
        typedef Parser::paramargs_type paramargs_type;

        ParseHandler() { }

        virtual ~ParseHandler() { }

        virtual void start();
        virtual void end();
        virtual void onLine(unsigned lineno, const std::string& file);
        virtual void onHtml(const std::string& html);
        virtual void onExpression(const std::string& expr);
        virtual void onHtmlExpression(const std::string& expr);
        virtual void onCpp(const std::string& code);
        virtual void onPre(const std::string& code);
        virtual void onInit(const std::string& code);
        virtual void onCleanup(const std::string& code);
        virtual void onArg(const std::string& name, const std::string& value);
        virtual void onGet(const std::string& name, const std::string& value);
        virtual void onPost(const std::string& name, const std::string& value);
        virtual void onAttr(const std::string& name, const std::string& value);
        virtual void onCall(const std::string& comp, const comp_args_type& args,
                            const std::string& passCgi, const paramargs_type& paramargs,
                            const std::string& cppargs);
        virtual void onEndCall(const std::string& comp);
        virtual void onShared(const std::string& code);
        virtual void onScope(scope_container_type container, scope_type scope,
                             const std::string& type, const std::string& var,
                             const std::string& init, const std::vector<std::string>& includes);
        virtual void startComp(const std::string& name, const cppargs_type& cppargs);
        virtual void startClose();
        virtual void endClose();
        virtual void onComp(const std::string& code);
        virtual void onCondExpr(const std::string& cond, const std::string& expr, bool htmlexpr);
        virtual void onConfig(const std::string& code, const std::string& value);
        virtual void tokenSplit(bool start);
        virtual void onInclude(const std::string& file);
        virtual void onIncludeEnd(const std::string& file);
    };
  }
}

#endif // TNT_ECPP_PARSERHANDLER_H

