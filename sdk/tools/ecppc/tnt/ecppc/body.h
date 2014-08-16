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


#ifndef TNT_ECPPC_BODY_H
#define TNT_ECPPC_BODY_H

#include <cxxtools/smartptr.h>
#include <cxxtools/refcounted.h>
#include <string>
#include <list>
#include <set>
#include <stack>
#include <tnt/ecpp/parser.h>

namespace tnt
{
  namespace ecppc
  {
    class Body;

    class Bodypart : public cxxtools::RefCounted
    {
        unsigned _curline;
        std::string _curfile;
        static bool _linenumbersEnabled;

      public:
        Bodypart()
          : _curline(0)
          { }
        Bodypart(unsigned line, const std::string& file)
          : _curline(line),
            _curfile(file)
          { }
        virtual ~Bodypart();

        virtual void getBody(std::ostream& out) const = 0;
        void printLine(std::ostream& out) const;

        static void enableLinenumbers(bool sw = true) { _linenumbersEnabled = sw; }
        static bool isLinenumbersEnabled()            { return _linenumbersEnabled; }
    };

    class BodypartStatic : public Bodypart
    {
        std::string _data;

      public:
        explicit BodypartStatic(const std::string& data)
          : _data(data)
          { }
        void getBody(std::ostream& out) const;
    };

    class BodypartCall : public Bodypart
    {
        typedef ecpp::Parser::comp_args_type comp_args_type;
        typedef ecpp::Parser::paramargs_type paramargs_type;
        typedef std::set<std::string> subcomps_type;

        static unsigned _nextNumber;

        unsigned _number;

        std::string _comp;
        comp_args_type _args;
        std::string _passCgi;
        paramargs_type _paramargs;
        std::string _cppargs;
        const subcomps_type& _subcomps;
        bool _haveEndCall;

        void call(std::ostream& out, const std::string& qparam) const;
        void callLocal(std::ostream& out, const std::string& qparam) const;
        void callByIdent(std::ostream& out, const std::string& qparam) const;
        void callByIdent(std::ostream& out, const std::string& ident, const std::string& qparam) const;
        void callByExpr(std::ostream& out, const std::string& qparam) const;

      public:
        BodypartCall(unsigned line, const std::string& file,
                     const std::string& comp,
                     const comp_args_type& args,
                     const std::string& passCgi,
                     const paramargs_type& paramargs,
                     const std::string& cppargs,
                     const subcomps_type& subcomps)
          : Bodypart(line, file),
            _number(_nextNumber++),
            _comp(comp),
            _args(args),
            _passCgi(passCgi),
            _paramargs(paramargs),
            _cppargs(cppargs),
            _subcomps(subcomps),
            _haveEndCall(false)
            { }

        const std::string& getName() const  { return _comp; }
        unsigned getNumber() const          { return _number; }
        void setHaveEndCall(bool sw = true) { _haveEndCall = sw; }
        void getBody(std::ostream& out) const;
    };

    class BodypartEndCall : public Bodypart
    {
        BodypartCall& _bpc;

      public:
        BodypartEndCall(unsigned line, const std::string& file, BodypartCall& bpc)
          : Bodypart(line, file),
            _bpc(bpc)
          { _bpc.setHaveEndCall(); }
        void getBody(std::ostream& out) const;
    };

    class Body
    {
        typedef ecpp::Parser::comp_args_type comp_args_type;
        typedef ecpp::Parser::paramargs_type paramargs_type;
        typedef cxxtools::SmartPtr<Bodypart> body_part_pointer;
        typedef std::list<body_part_pointer> body_type;
        typedef std::set<std::string> subcomps_type;

        body_type _data;
        subcomps_type _mysubcomps;
        const subcomps_type& _subcomps;

        std::stack<BodypartCall*> _callStack;

      public:
        Body()
          : _subcomps(_mysubcomps)
          { }
        Body(const Body& main, int)
          : _subcomps(main._mysubcomps)
          { }

        void addHtml(const std::string& code)
        {
          _data.push_back(
            body_part_pointer(
              new BodypartStatic(code)));
        }

        void addCall(unsigned line, const std::string& file, const std::string& comp,
                     const comp_args_type& args, const std::string& passCgi,
                     const paramargs_type& paramargs, const std::string& cppargs);
        void addEndCall(unsigned line, const std::string& file, const std::string& comp);

        void addSubcomp(const std::string& comp)
          { _mysubcomps.insert(comp); }

        bool hasSubcomp(const std::string& comp)
          { return _subcomps.find(comp) != _subcomps.end(); }

        void getBody(std::ostream& out) const;
    };
  }
}

#endif // TNT_ECPPC_BODY_H

