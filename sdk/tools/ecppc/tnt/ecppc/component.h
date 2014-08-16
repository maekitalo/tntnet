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


#ifndef TNT_ECPPC_COMPONENT_H
#define TNT_ECPPC_COMPONENT_H

#include "tnt/ecppc/body.h"
#include "tnt/ecppc/variable.h"
#include "tnt/ecppc/scopevar.h"

namespace tnt
{
  namespace ecppc
  {
    class Component
    {
        typedef ecpp::Parser::comp_args_type comp_args_type;
        typedef ecpp::Parser::paramargs_type paramargs_type;
        typedef std::list<Variable> variables_type;
        typedef std::list<Scopevar> scopevars_type;

        std::string _componentName;
        std::string _logCategory;

        variables_type _args;
        variables_type _get;
        variables_type _post;
        scopevars_type _scopevars;
        Body _compbody;

      protected:
        Component(const Component& main, const std::string& componentName,
                  const std::string& /* ns */ = std::string())
          : _componentName(componentName),
            _compbody(main._compbody, 1)
          { }

      public:
        explicit Component(const std::string& componentName)
          : _componentName(componentName)
          { }
        virtual ~Component() { }

        const std::string& getName() const { return _componentName; }
        std::string getLogCategory() const;
        void setLogCategory(const std::string& logCategory)
          { _logCategory = logCategory; }

        void addHtml(const std::string& code)
          { _compbody.addHtml(code); }

        void addCall(unsigned line, const std::string& file, const std::string& comp,
                     const comp_args_type& args, const std::string& passCgi,
                     const paramargs_type& paramargs, const std::string& cppargs)
          { _compbody.addCall(line, file, comp, args, passCgi, paramargs, cppargs); }

        void addArg(const std::string& name, const std::string& value)
          { _args.push_back(Variable(name, value)); }

        void addGet(const std::string& name, const std::string& value)
          { _get.push_back(Variable(name, value)); }

        void addPost(const std::string& name, const std::string& value)
          { _post.push_back(Variable(name, value)); }

        void addEndCall(unsigned line, const std::string& file, const std::string& comp)
          { _compbody.addEndCall(line, file, comp); }

        void addSubcomp(const std::string& comp)
          { _compbody.addSubcomp(comp); }

        void addScopevar(const Scopevar& s)
          { _scopevars.push_back(s); }

        void getBody(std::ostream& o, bool linenumbersEnabled) const;
        void getArgs(std::ostream& o) const;
        void getGet(std::ostream& o) const;
        void getPost(std::ostream& o) const;
        virtual void getScopevars(std::ostream& o, bool linenumbersEnabled) const;
        void getScopevars(std::ostream& o, ecpp::scope_type scope, bool linenumbersEnabled) const;

        bool hasScopevars() const { return !_scopevars.empty(); }
    };
  }
}

#endif // TNT_ECPPC_COMPONENT_H

