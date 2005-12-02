/* tnt/ecppc/component.h
   Copyright (C) 2005 Tommi Maekitalo

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
        std::string classname;
        std::string ns;
        typedef ecpp::Parser::comp_args_type comp_args_type;
        typedef std::list<Variable> variables_type;
        typedef std::list<Scopevar> scopevars_type;

        variables_type args;
        scopevars_type scopevars;
        Body compbody;

      protected:
        Component(const Component& main, const std::string& classname_,
          const std::string& ns_ = std::string())
          : classname(classname_),
            ns(ns_),
            compbody(main.compbody, 1)
          { }

      public:
        explicit Component(const std::string& classname_, const std::string& ns_ = std::string())
          : classname(classname_),
            ns(ns_)
          { }

        const std::string& getName() const  { return classname; }
        const std::string& getNs() const    { return ns; }

        void addHtml(const std::string& code)
          { compbody.addHtml(code); }

        void addCall(const std::string& comp,
                     const comp_args_type& args,
                     const std::string& pass_cgi,
                     const std::string& cppargs)
          { compbody.addCall(comp, args, pass_cgi, cppargs); }
        void addArg(const std::string& name, const std::string& value)
        {
          args.push_back(Variable(name, value));
        }
        void addEndCall(const std::string& comp)
          { compbody.addEndCall(comp); }

        void addSubcomp(const std::string& comp)
          { compbody.addSubcomp(comp); }

        void addScopevar(const Scopevar& s)
        {
          scopevars.push_back(s);
        }

        void getBody(std::ostream& o) const;
        void getArgs(std::ostream& o) const;
        void getScopevars(std::ostream& o) const;
        bool hasScopevars() const  { return !scopevars.empty(); }
    };
  }
}

#endif // TNT_ECPPC_COMPONENT_H

