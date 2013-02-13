/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#ifndef TNTWIKI_USERMANAGER_H
#define TNTWIKI_USERMANAGER_H

#include <string>
#include <tntdb/connection.h>
#include <tntdb/statement.h>

namespace tntwiki
{
  class User;
  class UserManager
  {
    public:
      explicit UserManager(tntdb::Connection conn)
        : _conn(conn)
      { }

      User createUser(const std::string& name);
      User createUser(const std::string& name, const std::string& password);
      User getUser(const std::string& name, const std::string& password);
      void setPassword(const User& user, const std::string& password);

    private:
      tntdb::Connection _conn;
      tntdb::Statement _selId;
      tntdb::Statement _insUser;
      tntdb::Statement _selUser;
      tntdb::Statement _updPasswdByName;
  };
}

#endif // TNTWIKI_USERMANAGER_H

/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#ifndef TNTWIKI_USERMANAGER_H
#define TNTWIKI_USERMANAGER_H

#include <string>
#include <tntdb/connection.h>
#include <tntdb/statement.h>

namespace tntwiki
{
  class User;
  class UserManager
  {
    public:
      explicit UserManager(tntdb::Connection conn)
        : _conn(conn)
      { }

      User createUser(const std::string& name);
      User createUser(const std::string& name, const std::string& password);
      User getUser(const std::string& name, const std::string& password);
      void setPassword(const User& user, const std::string& password);

    private:
      tntdb::Connection _conn;
      tntdb::Statement _selId;
      tntdb::Statement _insUser;
      tntdb::Statement _selUser;
      tntdb::Statement _updPasswdByName;
  };
}

#endif // TNTWIKI_USERMANAGER_H

