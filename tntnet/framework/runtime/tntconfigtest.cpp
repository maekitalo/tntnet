/* tntconfigtest.cpp
 * Copyright (C) 2003 Tommi Maekitalo
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
