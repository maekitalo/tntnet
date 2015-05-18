/*
 * Copyright (C) 2015 Tommi Maekitalo
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


#ifndef TNT_MAPPING_H
#define TNT_MAPPING_H

#include <tnt/maptarget.h>
#include <cxxtools/regex.h>

namespace tnt
{
  class HttpRequest;

  class Mapping
  {
      std::string _vhost;
      std::string _url;
      std::string _method;
      int _ssl;

      cxxtools::Regex _regexVhost;
      cxxtools::Regex _regexUrl;
      cxxtools::Regex _regexMethod;

      Maptarget _target;

    public:
      typedef Maptarget::args_type args_type;

      Mapping() { }

      Mapping(const std::string& vhost, const std::string& url, const std::string& method, int ssl, const Maptarget& target);

      const std::string& getVHost() const  { return _vhost; }
      const std::string& getUrl() const    { return _url; }
      const std::string& getMethod() const { return _method; }
      int getSsl() const                   { return _ssl; }

      const Maptarget& getTarget() const   { return _target; }

      Mapping& setPathInfo(const std::string& p)
      {
        _target.setPathInfo(p);
        return *this;
      }

      Mapping& setHttpReturn(unsigned httpreturn)
      {
        _target.setHttpReturn(httpreturn);
        return *this;
      }

      Mapping& setArgs(const args_type& a)
      {
        _target.setArgs(a);
        return *this;
      }

      Mapping& setArg(const std::string& name, const std::string& value)
      {
        _target.setArg(name, value);
        return *this;
      }

      /// Emulate old positional argument list.
      /// @deprecated
      Mapping& pushArg(const std::string& value);

      Mapping& setVHost(const std::string& vhost)
      {
        _vhost = vhost;
        _regexVhost = cxxtools::Regex(vhost);
        return *this;
      }

      Mapping& setUrl(const std::string& url)
      {
        _url = url;
        _regexUrl = cxxtools::Regex(url);
        return *this;
      }

      Mapping& setMethod(const std::string& method)
      {
        _method = method;
        _regexMethod = cxxtools::Regex(method);
        return *this;
      }

      Mapping& setSsl(bool sw)
      {
        _ssl = (sw ? SSL_YES : SSL_NO);
        return *this;
      }

      Mapping& unsetSsl()
      {
        _ssl = SSL_ALL;
        return *this;
      }

      bool match(const HttpRequest& request, cxxtools::RegexSMatch& smatch) const;
  };

}

#endif // TNT_MAPPING_H
