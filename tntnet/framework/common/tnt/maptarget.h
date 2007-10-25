/* tnt/maptarget.h
 * Copyright (C) 2007 Tommi Maekitalo
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


#ifndef TNT_MAPTARGET_H
#define TNT_MAPTARGET_H

#include <tnt/compident.h>
#include <vector>
#include <string>

namespace tnt
{
  class Maptarget : public Compident
  {
    public:
      typedef std::vector<std::string> args_type;

    private:
      std::string pathinfo;
      args_type args;
      bool pathinfo_set;

    public:
      Maptarget()
        : pathinfo_set(false)
        { }

      explicit Maptarget(const std::string& ident)
        : Compident(ident),
          pathinfo_set(false)
        { }

      Maptarget(const Compident& ident)
        : Compident(ident),
          pathinfo_set(false)
        { }

      bool hasPathInfo() const
        { return pathinfo_set; }
      Maptarget& setPathInfo(const std::string& p)
        { pathinfo = p; pathinfo_set = true; return *this; }
      void setArgs(const args_type& a)
        { args = a; }
      const std::string& getPathInfo() const
        { return pathinfo; }
      const args_type& getArgs() const
        { return args; }
      args_type& getArgsRef()
        { return args; }
      Maptarget& pushArg(const std::string& arg)
        { args.push_back(arg); return *this; }
  };

}

#endif // TNT_MAPTARGET_H

