/* tnt/regex.h
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef TNT_REGEX_H
#define TNT_REGEX_H

#include <string>
#include <sys/types.h>
#include <regex.h>

namespace tnt
{
  /// collects matches in a regex
  class regex_smatch
  {
      friend class regex;

      std::string str;
      regmatch_t matchbuf[10];

    public:
      unsigned size() const;
      std::string get(unsigned n) const;
      std::string format(const std::string& s) const;
  };

  /// regex(3)-wrapper.
  /// Warning: incomplete, but sufficient for tntnet.
  /// Regular expression is not automatically freed. Tntnet needs to
  /// put regex into a stl-container, so it needs to be copyable.
  /// For this class to be complete, the regex_t needs to be
  /// reference-counted. This is unneeded for tntnet, because the regex is
  /// never freed anyway.
  class regex
  {
      regex_t expr;

      void checkerr(int ret) const;

    public:
      explicit regex(const char* ex, int cflags = REG_EXTENDED)
      {
        checkerr(::regcomp(&expr, ex, cflags));
      }

      explicit regex(const std::string& ex, int cflags = REG_EXTENDED)
      {
        checkerr(::regcomp(&expr, ex.c_str(), cflags));
      }

      bool match(const std::string& str_, regex_smatch& smatch, int eflags = 0) const;
      bool match(const std::string& str_, int eflags = 0) const;

      void free();

    private:
      friend class value_type;
  };

}

#endif // TNT_REGEX_H

