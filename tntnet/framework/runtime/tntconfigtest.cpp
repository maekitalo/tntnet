/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#include <tnt/tntconfig.h>

int main(int argc, char* argv[])
{
  using namespace std;
  try
  {
    const char* fname = argc > 1 ? argv[1] : "tntnet.conf";
    tntconfig config(fname);

    const tntconfig::config_values_type& values = config.getConfigValues();
    tntconfig::config_values_type::const_iterator vi;
    for (vi = values.begin(); vi != values.end(); ++vi)
    {
      const tntconfig::config_entry_type& v = *vi;
      cout << "key=" << v.key << endl;
      for (tntconfig::config_value_type::const_iterator i
        = v.values.begin(); i != v.values.end(); ++i)
        cout << " value=" << *i << endl;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
