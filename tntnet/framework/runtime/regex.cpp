/* regex.cpp
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

#include "tnt/regex.h"
#include <stdexcept>
#include <locale>

namespace tnt
{
  static inline bool isdigit(char ch)
  {
    return ch >= '0' && ch <= '9';
  }

  unsigned regex_smatch::size() const
  {
    unsigned n;
    for (n = 0; n < 10 && matchbuf[n].rm_so >= 0; ++n)
      ;

    return n;
  }

  std::string regex_smatch::get(unsigned n) const
  {
    return str.substr(matchbuf[n].rm_so, matchbuf[n].rm_eo - matchbuf[n].rm_so);
  }

  std::string regex_smatch::format(const std::string& s) const
  {
    enum state_type
    {
      state_0,
      state_esc,
      state_var0,
      state_var1,
      state_1
    } state;

    state = state_0;
    std::string ret;

    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
    {
      char ch = *it;

      switch (state)
      {
        case state_0:
          if (ch == '$')
            state = state_var0;
          else if (ch == '\\')
          {
            ret = std::string(s.begin(), it);
            state = state_esc;
          }
          break;

        case state_esc:
          ret += ch;
          state = state_1;
          break;

        case state_var0:
          if (isdigit(ch))
          {
            ret = std::string(s.begin(), it - 1);
            regoff_t s = matchbuf[ch - '0'].rm_so;
            regoff_t e = matchbuf[ch - '0'].rm_eo;
            if (s >= 0 && e >= 0)
              ret.append(str, s, e-s);
            state = state_1;
          }
          else
            state = state_0;
          break;

        case state_1:
          if (ch == '$')
            state = state_var1;
          else if (state == '\\')
            state = state_esc;
          else
            ret += ch;
          break;

        case state_var1:
          if (isdigit(ch))
          {
            unsigned s = matchbuf[ch - '0'].rm_so;
            unsigned e = matchbuf[ch - '0'].rm_eo;
            if (s >= 0 && e >= 0)
              ret.append(str, s, e-s);
            state = state_1;
          }
          else if (ch == '$')
            ret += '$';
          else
          {
            ret += '$';
            ret += ch;
          }
          break;
      }
    }

    switch (state)
    {
      case state_0:
      case state_var0:
        return s;

      case state_esc:
        return ret + '\\';

      case state_var1:
        return ret + '$';

      case state_1:
        return ret;
    }

    return ret;
  }

  void regex::checkerr(int ret) const
  {
    if (ret != 0)
    {
      char errbuf[256];
      regerror(ret, &expr, errbuf, sizeof(errbuf));
      throw std::runtime_error(errbuf);
    }
  }

  bool regex::match(const std::string& str_, int eflags) const
  {
    regex_smatch smatch;
    return match(str_, smatch, eflags);
  }

  bool regex::match(const std::string& str_, regex_smatch& smatch, int eflags) const
  {
    smatch.str = str_;
    int ret = regexec(&expr, str_.c_str(),
        sizeof(smatch.matchbuf) / sizeof(regmatch_t), smatch.matchbuf, eflags);

    if (ret ==REG_NOMATCH)
      return false;

    checkerr(ret);
    return true;
  }

  void regex::free()
  {
    regfree(&expr);
  }
}
