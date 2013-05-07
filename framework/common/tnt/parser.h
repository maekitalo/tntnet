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


#ifndef TNT_PARSER_H
#define TNT_PARSER_H

#include <iostream>

namespace tnt
{
  class PrePostNop
  {
    protected:
      void pre(char ch)    { }
      bool post(bool ret)  { return ret; }
  };

  template <typename this_type, class PrePostProcessor = PrePostNop>
  class Parser : public PrePostProcessor
  {
    protected:

      typedef bool (this_type::*state_type)(char);
      state_type state;
      state_type nextState;

      bool failedFlag;

      bool state_skipws(char ch)
      {
        if (ch != ' ' && ch != '\t')
        {
          state = nextState;
          return (static_cast<this_type*>(this)->*state)(ch);
        }
        else
          return false;
      }

      void skipWs(state_type nextState_)
      {
        state = &Parser::state_skipws;
        nextState = nextState_;
      }

    public:
      explicit Parser(state_type initialState)
        : state(initialState),
          failedFlag(false)
        { }

      bool parse(char ch)
      {
        PrePostProcessor::pre(ch);
        return PrePostProcessor::post(
          (static_cast<this_type*>(this)->*state)(ch) );
      }

      bool parse(const char* str, unsigned size)
      {
        for (unsigned s = 0; s < size; ++s)
          if (parse(str[s]))
            return true;
        return false;
      }

      bool parse(std::istream& in)
      {
        std::streambuf* buf = in.rdbuf();
        while (buf->sgetc() != std::ios::traits_type::eof())
          if ( parse(buf->sbumpc()) )
            return true;

        in.setstate(std::ios::eofbit);
        return false;
      }

      bool failed() const
      {
        return failedFlag;
      }
  };
}

#endif // TNT_PARSER_H
