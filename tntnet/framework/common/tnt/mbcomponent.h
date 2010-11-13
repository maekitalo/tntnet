/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#ifndef TNT_MBCOMPONENT_H
#define TNT_MBCOMPONENT_H

#include <tnt/ecpp.h>
#include <vector>
#include <cxxtools/mutex.h>

namespace tnt
{

  class MbComponent : public EcppComponent
  {
      const char* rawData;
      const char** urls;
      const char** mimetypes;
      const char** ctimes;
      std::vector<std::string> compressedData;
      cxxtools::ReadWriteMutex mutex;

    protected:
      void init(const char* rawData_, const char** urls_,
                const char** mimetypes_, const char** ctimes_);
      void init(const char* rawData_, const char* &mimetype_, const char* &ctime_)
      { init(rawData_, 0, &mimetype_, &ctime_); }

    public:
      MbComponent(const Compident& ci, const Urlmapper& um, Comploader& cl)
        : EcppComponent(ci, um, cl),
          rawData(0),
          urls(0),
          mimetypes(0),
          ctimes(0)
      { }

      MbComponent(const Compident& ci, const Urlmapper& um, Comploader& cl,
                  const char* rawData, const char** urls,
                  const char** mimetypes, const char** ctimes)
        : EcppComponent(ci, um, cl),
          rawData(0),
          urls(0),
          mimetypes(0),
          ctimes(0)
      { init(rawData, urls, mimetypes, ctimes); }

      MbComponent(const Compident& ci, const Urlmapper& um, Comploader& cl,
                  const char* rawData, const char* &mimetype, const char* &ctime)
        : EcppComponent(ci, um, cl),
          rawData(0),
          urls(0),
          mimetypes(0),
          ctimes(0)
      { init(rawData, mimetype, ctime); }

      unsigned operator() (HttpRequest& request, HttpReply& reply, tnt::QueryParams& qparam);
      unsigned topCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);

  private:
      unsigned doCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam, bool top);
  };
}

#endif // TNT_MBCOMPONENT_H
