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


#include "tnt/ecppc/dependencygenerator.h"
#include <sstream>
#include <cxxtools/log.h>

log_define("tntnet.ecppc.dependency")

namespace tnt
{
  namespace ecppc
  {
    void Dependencygenerator::onInclude(const std::string& file)
    {
      log_trace("onInclude(\"" << file << "\")");

      _dependencies.push_back(file);
    }

    void Dependencygenerator::getDependencies(std::ostream& out) const
    {
      log_trace("getDependencies");

      std::string::size_type p = _inputfile.find_last_of('.');
      out << _inputfile.substr(0, p) << ".cpp: " << _inputfile;
      for (dependencies_type::const_iterator it = _dependencies.begin();
           it != _dependencies.end(); ++it)
        out << " \\\n  " << *it;
      out << '\n';
    }

    void Dependencygenerator::getCMakeDependencies(std::ostream& out) const
    {
      log_trace("getCMakeDependencies");

      std::string::size_type p = _inputfile.find_last_of('.');
      out << _inputfile.substr(0, p) << ".cpp;" << _inputfile;
      for (dependencies_type::const_iterator it = _dependencies.begin();
           it != _dependencies.end(); ++it)
        out << ';' << *it;
      out << '\n';
    }

  }
}

