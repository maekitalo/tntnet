/* tnt/sodata.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef TNT_SODATA_H
#define TNT_SODATA_H

#include <string>

namespace tnt
{
  class Compident;

  //////////////////////////////////////////////////////////////////////
  // sodata stellt eine Klasse dar, die Komponentendaten aus einer
  // shared-library lädt. Die shared-library hat folgende exportierte
  // Datenstrukturen:
  //
  //   bei unkomprimierten Daten:
  //     - const char* (compname)_data
  //     - unsigned (compname)_datalen
  //
  //   bei komprimierten Daten:
  //     - const char* (compname)_zdata
  //     - unsigned (compname)_zdatalen
  //     - unsigned (compname)_datalen
  //
  class Sodata
  {
      std::string sosuffix;
      unsigned refs;
      char* data;

    public:
      Sodata(const std::string& sosuffix_ = std::string())
        : sosuffix(sosuffix_),
          refs(0),
          data(0)
      { }
      ~Sodata()
      { delete data; }

      void setSoSuffix(const std::string& sosuffix_)
      { sosuffix = sosuffix_; }
      void setLangSuffix();
      const std::string& getSoSuffix() const
      { return sosuffix; }

      void addRef(const Compident& ci);
      void release();

      operator const char* () const      { return data; }
  };
}

#endif // TNT_SODATA_H

