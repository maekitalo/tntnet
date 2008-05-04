/* tnt/regex.h
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_REGEX_H
#define TNT_REGEX_H

#include <string>
#include <sys/types.h>
#include <regex.h>
#include <cxxtools/smartptr.h>

namespace tnt
{
  class RegexSMatch;

  template <typename objectType>
  class RegexDestroyPolicy;

  template <>
  class RegexDestroyPolicy<regex_t>
  {
    protected:
      void destroy(regex_t* expr)
      {
        ::regfree(expr);
        delete expr;
      }
  };

  /// regex(3)-wrapper.
  class Regex
  {
      cxxtools::SmartPtr<regex_t, cxxtools::ExternalRefCounted, RegexDestroyPolicy> expr;

      void checkerr(int ret) const;

    public:
      explicit Regex(const char* ex, int cflags = REG_EXTENDED)
        : expr(new regex_t())
      {
        checkerr(::regcomp(expr.getPointer(), ex, cflags));
      }

      explicit Regex(const std::string& ex, int cflags = REG_EXTENDED)
        : expr(new regex_t())
      {
        checkerr(::regcomp(expr.getPointer(), ex.c_str(), cflags));
      }

      bool match(const std::string& str_, RegexSMatch& smatch, int eflags = 0) const;
      bool match(const std::string& str_, int eflags = 0) const;

      void free()  { expr = 0; }
  };

  /// collects matches in a regex
  class RegexSMatch
  {
      friend class Regex;

      std::string str;
      regmatch_t matchbuf[10];

    public:
      /// returns the number of expressions, which were found
      unsigned size() const;
      /// returns the start position of the n-th expression
      regoff_t offsetBegin(unsigned n) const   { return matchbuf[n].rm_so; }
      /// returns the end position of the n-th expression
      regoff_t offsetEnd(unsigned n) const     { return matchbuf[n].rm_eo; }

      /// returns the n-th element. No range checking is done.
      std::string get(unsigned n) const;
      /// replace each occurence of "$n" with the n-th element (n: 0..9).
      std::string format(const std::string& s) const;
      /// returns the n-th element. No range checking is done.
      std::string operator[] (unsigned n) const
        { return get(n); }
  };

}

#endif // TNT_REGEX_H

