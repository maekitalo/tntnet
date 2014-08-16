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


#ifndef TNT_ECPP_PARSER_H
#define TNT_ECPP_PARSER_H

#include <string>
#include <stdexcept>
#include <map>
#include <list>
#include <iosfwd>
#include <tnt/ecpp/scopetypes.h>

namespace tnt
{
  namespace ecpp
  {
    class ParseHandler;

    class Parser
    {
        typedef std::list<std::string> includes_type;

        ParseHandler& _handler;
        std::string _curfile;
        unsigned _curline;

        includes_type _includes;

        std::istream& get(char& ch);
        void doInclude(const std::string& file);

        void parsePriv(std::istream& in);

      public:
        typedef std::multimap<std::string, std::string> comp_args_type;
        typedef std::list<std::pair<std::string, std::string> > cppargs_type;
        typedef std::map<std::string, std::string> paramargs_type;

        Parser(ParseHandler& handler, const std::string& fname)
          : _handler(handler),
            _curfile(fname),
            _curline(0)
        { }

        void addInclude(const std::string& dir)
          { _includes.push_back(dir); }

        void parse(std::istream& in);

      private:
        void processNV(const std::string& tag, const std::string& name, const std::string& value);
    };

    class parse_error : public std::runtime_error
    {
        std::string _msg;

      public:
        parse_error(const std::string& msg, int state, const std::string& file, unsigned line);
        ~parse_error() throw() { }
        const char* what() const throw();
    };
  }
}

#endif // TNT_ECPP_PARSER_H

