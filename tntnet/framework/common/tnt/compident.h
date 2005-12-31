/* tnt/compident.h
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

#ifndef TNT_COMPIDENT_H
#define TNT_COMPIDENT_H

#include <string>
#include <iostream>

namespace tnt
{
struct Compident
{
  std::string libname;
  std::string compname;

  bool operator< (const Compident& ci) const
  {
    return libname < ci.libname
      || libname == ci.libname && compname < ci.compname;
  }

  Compident() { }
  Compident(const std::string& l, const std::string& n)
    : libname(l),
      compname(n)
  { }

  explicit Compident(const std::string& ident);

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
