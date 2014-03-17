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
  /** This class is an identifier for a tntnet component

      It encapsulates the name of a component plus optionally the library it is in.
   */
  struct Compident
  {
    private:
      mutable std::string compident;

    public:
      std::string libname;
      std::string compname;

      bool operator< (const Compident& ci) const
      {
        return libname < ci.libname
          || (libname == ci.libname && compname < ci.compname);
      }

      /// Create an empty Compident object
      Compident() { }

      /// Create a Compident object with the given library and component name
      Compident(const std::string& lib, const std::string& comp)
        : libname(lib),
          compname(comp)
        { }

      /** Create a Compident from an identification string

          The string has to be either only the component name or
          the library name + the character '@' + the component name.
       */
      explicit Compident(const std::string& ident);

      /// Get component identifier as a string
      const std::string& toString() const
        { return libname.empty() ? compname : (compident.empty() ? (compident = compname + '@' + libname) : compident); }

      /// Check whether the Compident is empty
      bool empty() const
        { return libname.empty() && compname.empty(); }

      /// Erase the content of the library and component strings
      void clear()
        { libname.clear(); compname.clear(); }
  };

  inline std::ostream& operator<< (std::ostream& out, const Compident& comp)
    { return out << comp.toString(); }
}

#endif // TNT_COMPIDENT_H

