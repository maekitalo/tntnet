/*
 * Copyright (C) 2005 Tommi Maekitalo
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


#include "tnt/ecppc/scopevar.h"
#include <tnt/stringescaper.h>
#include <iterator>
#include <iostream>
#include <algorithm>

namespace tnt
{
  namespace ecppc
  {
    void Scopevar::get(std::ostream& out, bool linenumbersEnabled) const
    {
      std::string tag =
          _scopeContainer == ecpp::application_container    ? "application"
        : _scopeContainer == ecpp::thread_container         ? "thread"
        : _scopeContainer == ecpp::session_container        ? "session"
        : _scopeContainer == ecpp::secure_session_container ? "securesession"
        : _scopeContainer == ecpp::request_container        ? "request"
        :                                                     "param";

      std::string macro;
      if (_scopeContainer == ecpp::param_container)
      {
        macro = "TNT_PARAM";
      }
      else
      {
        std::string container =
            _scopeContainer == ecpp::application_container    ? "APPLICATION_"
          : _scopeContainer == ecpp::thread_container         ? "THREAD_"
          : _scopeContainer == ecpp::session_container        ? "SESSION_"
          : _scopeContainer == ecpp::secure_session_container ? "SECURE_SESSION_"
          : _scopeContainer == ecpp::request_container        ? "REQUEST_"
          :                                                     "PARAM_";

        std::string key =
            _scope == ecpp::shared_scope             ? "SHARED_"
          : _scope == ecpp::page_scope               ? "PAGE_"
          : _scope == ecpp::default_scope            ? "FILE_"
          : _scopeContainer != ecpp::param_container ? "COMPONENT_"
          : std::string();

        macro = "TNT_" + container + key + "VAR";
      }

      if (linenumbersEnabled)
        out << "#line " << (_myline + 1) << " \"" << _myfile << "\"\n";

      std::string t = _type;
      if (t.empty())
        t = "std::string";

      out << "  typedef " << t << ' ' << _var << "_type;\n";
      if (_type.find(',') == std::string::npos)
      {
        out << "  " << macro << '(' << t << ", " << _var;
        if (_scope == ecpp::default_scope)
          out << ", " << _myfile;
        out << ", (" << _init << ")); "
               "  // <%" << tag << "> " << t << ' ' << _var;
      }
      else
      {
        out << "  " << macro << '(' << _var << "_type, " << _var;
        if (_scope == ecpp::default_scope)
          out << ", " << _myfile;
        out << ", (" << _init << ")); "
               "  // <%" << tag << "> " << _type << ' ' << _var;
      }
      if (!_init.empty())
      {
        out << '(';
        std::transform(_init.begin(), _init.end(),
                       std::ostream_iterator<const char*>(out),
                       stringescaper(false));
        out << ')';
      }

      out << "\n"
             "  _tnt_ignore_unused<" << _var << "_type>(" << _var << ");\n";
    }
  }
}
