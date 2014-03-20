/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#ifndef TNT_MBCOMPONENT_H
#define TNT_MBCOMPONENT_H

#include <tnt/ecpp.h>
#include <vector>
#include <cxxtools/mutex.h>

namespace tnt
{

  class MbComponent : public EcppComponent
  {
    private:
      const char* _rawData;
      const char** _urls;
      const char** _mimetypes;
      const char** _ctimes;
      std::vector<std::string> _compressedData;
      cxxtools::ReadWriteMutex _mutex;

    protected:
      void init(const char* rawData, const char** urls, const char** mimetypes, const char** ctimes);
      void init(const char* rawData, const char* &mimetype, const char* &ctime)
        { init(rawData, 0, &mimetype, &ctime); }

    public:
      MbComponent(const Compident& ci, const Urlmapper& um, Comploader& cl)
        : EcppComponent(ci, um, cl),
          _rawData(0),
          _urls(0),
          _mimetypes(0),
          _ctimes(0)
        { }

      MbComponent(const Compident& ci, const Urlmapper& um, Comploader& cl, const char* rawData,
                  const char** urls, const char** mimetypes, const char** ctimes)
        : EcppComponent(ci, um, cl),
          _rawData(0),
          _urls(0),
          _mimetypes(0),
          _ctimes(0)
        { init(rawData, urls, mimetypes, ctimes); }

      MbComponent(const Compident& ci, const Urlmapper& um, Comploader& cl,
                  const char* rawData, const char* &mimetype, const char* &ctime)
        : EcppComponent(ci, um, cl),
          _rawData(0),
          _urls(0),
          _mimetypes(0),
          _ctimes(0)
        { init(rawData, mimetype, ctime); }

      unsigned operator() (HttpRequest& request, HttpReply& reply, tnt::QueryParams& qparam);
      unsigned topCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);

  private:
      unsigned doCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam, bool top);
  };
}

#endif // TNT_MBCOMPONENT_H

