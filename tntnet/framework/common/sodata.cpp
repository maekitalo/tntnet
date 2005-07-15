/* sodata.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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

#include <tnt/sodata.h>
#include <tnt/component.h>

#include <cxxtools/thread.h>
#include <cxxtools/dlloader.h>
#include <cxxtools/log.h>

#include <zlib.h>
#include <stdlib.h>
#include <string.h>

log_define("tntnet.data")

namespace tnt
{
  static cxxtools::Mutex mutex;

  void sodata::setLangSuffix()
  {
    const char* LANG = getenv("LANG");
    std::string sosuffix = std::string(1, '.');
    if (LANG)
      sosuffix += LANG;
    setSoSuffix(sosuffix);
  }

  void sodata::addRef(const compident& ci)
  {
    cxxtools::MutexLock lock(mutex);
    if (refs++ == 0)
    {
      // read data from shared library
      log_debug("load library " << ci.libname << sosuffix);
      cxxtools::dl::library so((ci.libname + sosuffix).c_str());

      cxxtools::dl::symbol datalen_sym = so.sym((ci.compname + "_datalen").c_str());
      unsigned datalen = *reinterpret_cast<unsigned*>(datalen_sym.getSym());
      try
      {
        // try to read compressed data
        cxxtools::dl::symbol zdata_sym = so.sym((ci.compname + "_zdata").c_str());
        cxxtools::dl::symbol zdatalen_sym = so.sym((ci.compname + "_zdatalen").c_str());

        const char** zdata = reinterpret_cast<const char**>(zdata_sym.getSym());
        unsigned zdatalen = *reinterpret_cast<unsigned*>(zdatalen_sym.getSym());

        log_debug(zdatalen << " bytes compressed data, " << datalen
          << " bytes uncompressed");

        // ... and uncompress it
        data = new char[datalen];
        uLong destlen = datalen;
        int z_ret = uncompress((Bytef*)data, &destlen, (const Bytef*)*zdata, zdatalen);
        if (z_ret != Z_OK)
        {
          throw std::runtime_error(std::string("error uncompressing data: ") +
            (z_ret == Z_MEM_ERROR ? "Z_MEM_ERROR" :
             z_ret == Z_BUF_ERROR ? "Z_BUF_ERROR" :
             z_ret == Z_DATA_ERROR ? "Z_DATA_ERROR" : "unknown error"));
        }

        log_debug("uncompress ready");
      }
      catch(const cxxtools::dl::symbol_not_found&)
      {
        // compressed data not found - try uncompressed
        cxxtools::dl::symbol data_sym = so.sym((ci.compname + "_data").c_str());
        const char** srcdata = reinterpret_cast<const char**>(data_sym.getSym());

        log_debug(datalen << " bytes data");

        // ... and copy it
        data = new char[datalen];
        memcpy(data, *srcdata, datalen);
      }
    }
  }

  void sodata::release()
  {
    cxxtools::MutexLock lock(mutex);
    if (--refs <= 0)
    {
      log_debug("release");
      delete[] data;
      data = 0;
    }
  }

}
