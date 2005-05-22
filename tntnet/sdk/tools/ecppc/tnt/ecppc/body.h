/* tnt/ecppc/body.h
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

#ifndef TNT_ECPPC_BODY_H
#define TNT_ECPPC_BODY_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <list>
#include <set>
#include <tnt/ecpp/parser.h>

namespace tnt
{
  namespace ecppc
  {
    class body;

    class bodypart
    {
      public:
        virtual void getBody(std::ostream& out) const = 0;
    };

    class bodypart_static : public bodypart
    {
        std::string data;

      public:
        bodypart_static(const std::string& data_)
          : data(data_)
          { }
        void getBody(std::ostream& out) const;
    };

    class bodypart_call : public bodypart
    {
        typedef ecpp::parser::comp_args_type comp_args_type;
        typedef std::set<std::string> subcomps_type;

        std::string comp;
        comp_args_type args;
        std::string pass_cgi;
        std::string cppargs;
        const subcomps_type& subcomps;

        void call(std::ostream& out, const std::string& qparam) const;
        void callLocal(std::ostream& out, const std::string& qparam) const;
        void callByIdent(std::ostream& out, const std::string& qparam) const;
        void callByIdent(std::ostream& out, const std::string& ident, const std::string& qparam) const;
        void callByExpr(std::ostream& out, const std::string& qparam) const;

      public:
        bodypart_call(const std::string& comp_,
                      const comp_args_type& args_,
                      const std::string& pass_cgi_,
                      const std::string& cppargs_,
                      const subcomps_type& subcomps_)
          : comp(comp_),
            args(args_),
            pass_cgi(pass_cgi_),
            cppargs(cppargs_),
            subcomps(subcomps_)
            { }

        const std::string& getName() const  { return comp; }
        void getBody(std::ostream& out) const;
    };

    class body
    {
        typedef ecpp::parser::comp_args_type comp_args_type;
        typedef boost::shared_ptr<bodypart> body_part_pointer;
        typedef std::list<body_part_pointer> body_type;
        typedef std::set<std::string> subcomps_type;
        body_type data;
        subcomps_type mysubcomps;
        const subcomps_type& subcomps;

      public:
        body()
          : subcomps(mysubcomps)  { }
        body(const body& main, int)
          : subcomps(main.mysubcomps)  { }

        void addHtml(const std::string& code)
        {
          data.push_back(
            body_part_pointer(
              new bodypart_static(code)));
        }

        void addCall(const std::string& comp,
                     const comp_args_type& args,
                     const std::string& pass_cgi,
                     const std::string& cppargs)
        {
          data.push_back(
            body_part_pointer(
              new bodypart_call(comp, args, pass_cgi, cppargs, subcomps)));
        }

        void addSubcomp(const std::string& comp)
        {
          mysubcomps.insert(comp);
        }

        bool hasSubcomp(const std::string& comp)
        { return subcomps.find(comp) != subcomps.end(); }

        void getBody(std::ostream& out) const;
    };
  }
}

#endif // TNT_ECPPC_BODY_H

