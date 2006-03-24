/* tnt/ecpp/parser.h
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
        ParseHandler& handler;
        std::string curfile;
        unsigned curline;

        std::istream& get(char& ch);
        void doInclude(const std::string& file);

        void parsePriv(std::istream& in);

      public:
        Parser(ParseHandler& handler_, const std::string& fname)
          : handler(handler_),
            curfile(fname),
            curline(0)
        { }

        void parse(std::istream& in);

        typedef std::multimap<std::string, std::string> comp_args_type;
        typedef std::list<std::pair<std::string, std::string> > cppargs_type;

      private:
        void processNV(const std::string& tag, const std::string& name,
            const std::string& value);
    };

    class parse_error : public std::runtime_error
    {
        std::string msg;

      public:
        parse_error(const std::string& msg, int state, unsigned line);
        ~parse_error() throw()
        { }
        const char* what() const throw();
    };

  }
}

#endif // TNT_ECPP_PARSER_H

