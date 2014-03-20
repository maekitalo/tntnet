/*
 * Copyright (C) 2007 Tommi Maekitalo
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


#ifndef TNT_MAPTARGET_H
#define TNT_MAPTARGET_H

#include <tnt/compident.h>
#include <map>
#include <string>

namespace tnt
{
  /// @cond internal
  class Maptarget : public Compident
  {
    friend class Dispatcher;

    public:
      typedef std::map<std::string, std::string> args_type;

    private:
      std::string _pathinfo;
      args_type _args;
      bool _pathinfo_set;

    public:
      Maptarget()
        : _pathinfo_set(false)
        { }

      explicit Maptarget(const std::string& ident)
        : Compident(ident),
          _pathinfo_set(false)
        { }

      Maptarget(const Compident& ident)
        : Compident(ident),
          _pathinfo_set(false)
        { }

      bool hasPathInfo() const
        { return _pathinfo_set; }
      Maptarget& setPathInfo(const std::string& p)
        { _pathinfo = p; _pathinfo_set = true; return *this; }
      void setArgs(const args_type& a)
        { _args = a; }
      const std::string& getPathInfo() const
        { return _pathinfo; }
      const args_type& getArgs() const
        { return _args; }
      Maptarget& setArg(const std::string& name, const std::string& value)
        { _args[name] = value; return *this; }
  };
  /// @endcond internal
}

#endif // TNT_MAPTARGET_H

