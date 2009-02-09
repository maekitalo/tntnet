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


#ifndef TNT_COMPIDENT_H
#define TNT_COMPIDENT_H

#include <string>
#include <iostream>

namespace tnt
{
/// This is a class, which names tntnet components.
/// tntnet components are identified by the library, where they reside and a name
/// inside the library. The name is actually a string constant, which was passed
/// to the component factory ctor.
struct Compident
{
  std::string libname;
  std::string compname;

  bool operator< (const Compident& ci) const
  {
    return libname < ci.libname
      || (libname == ci.libname && compname < ci.compname);
  }

  Compident() { }

  /// Creates a component identifyer with a library and component name.
  Compident(const std::string& l, const std::string& n)
    : libname(l),
      compname(n)
  { }

  /// looks for '@' and splits the passed string into libname and compname parts.
  /// When no '@' is found, the library part is left empty.
  explicit Compident(const std::string& ident);

  /// return component identifyer as a string
  std::string toString() const
  { return libname.empty() ? compname : (compname + '@' + libname); }

  bool empty() const
    { return libname.empty() && compname.empty(); }
  void clear()
    { libname.clear(); compname.clear(); }
};

inline std::ostream& operator<< (std::ostream& out, const Compident& comp)
{ return out << comp.toString(); }

}

#endif // TNT_COMPIDENT_H
