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

#include <tntwiki/UserManager.h>
#include <tntwiki/User.h>
#include <tntdb/error.h>
#include <tntdb/transaction.h>
#include <cxxtools/md5.h>
#include <stdexcept>
#include <cstdlib>

namespace tntwiki
{
  User UserManager::createUser(const std::string& name)
  {
    if (!_selId)
      _selId = _conn.prepareCached(
        "select max(id) + 1"
        "  from users");

    if (!_selUser)
      _selUser = _conn.prepareCached(
        "select id, passwd"
        "  from users"
        " where name = :name");

    if (!_insUser)
      _insUser = _conn.prepareCached(
        "insert into users (id, name)"
        " values (:id, :name)");

    tntdb::Transaction trans(_conn);
    trans.lockTable("users", true);

    try
    {
      _selUser.set("name", name);
      _selUser.selectRow();
      throw std::runtime_error("username \"" + name + "\" already allocated");
    }
    catch (const tntdb::NotFound&)
    {
    }

    tntdb::Value id_ = _selId.selectValue();
    unsigned id = 1;
    id_.get(id);  // get returns false, if id is not found and does not modify id

    _insUser.set("id", id)
            .set("name", name)
            .execute();

    User user;
    user._id = id;
    user._name = name;

    trans.commit();

    return user;
  }

  User UserManager::createUser(const std::string& name, const std::string& password)
  {
    tntdb::Transaction trans(_conn);
    User user = createUser(name);
    setPassword(user, password);
    return user;
  }

  User UserManager::getUser(const std::string& name, const std::string& password)
  {
    if (!_selUser)
      _selUser = _conn.prepareCached(
        "select id, passwd"
        "  from users"
        " where name = :name");

    unsigned id;
    std::string pwhash;

    tntdb::Row r;
    try
    {
      r = _selUser.set("name", name)
                  .selectRow();
    }
    catch (const tntdb::NotFound&)
    {
      return User();
    }

    r[0].get(id);
    if (!r[1].get(pwhash))
      return User();

    std::string salt(pwhash, 0, 2);
    std::string m = cxxtools::md5(password + salt);

    if (pwhash.compare(2, std::string::npos, m) != 0)
      return User();

    User user;
    user._id = id;
    user._name = name;

    return user;
  }

  void UserManager::setPassword(const User& user, const std::string& password)
  {
    if (!user.loggedIn())
      throw std::runtime_error("cannot set password: user not logged in");

    if (!_updPasswdByName)
      _updPasswdByName = _conn.prepareCached(
        "update users"
        "   set passwd = :passwd"
        " where name = :name");

    int r = std::rand();
    std::string salt;
    static const char dict[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    salt += 32 + r % sizeof(dict);
    salt += 32 + (r >> 8) % sizeof(dict);

    std::string m = cxxtools::md5(password + salt);
    _updPasswdByName.set("name", user.name())
                    .set("passwd", salt + m)
                    .execute();

    if (_updPasswdByName.execute() < 1)
      throw std::runtime_error("cannot set password: user \"" + user.name() + "\" not found");
  }

}
