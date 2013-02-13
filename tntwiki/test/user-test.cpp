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
#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

const std::string dburl = "postgresql:dbname=tntwiki";

class UserTest : public cxxtools::unit::TestSuite
{
  public:
    UserTest()
      : cxxtools::unit::TestSuite("user")
    {
      registerMethod("createUser", *this, &UserTest::testCreateUser);
      registerMethod("setPassword", *this, &UserTest::testSetPassword);
    }

    void testCreateUser()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::UserManager userManager(conn);

      tntdb::Transaction trans(conn);

      tntwiki::User newUser;
      CXXTOOLS_UNIT_ASSERT_NOTHROW(newUser = userManager.createUser("foo", "bar"));
      CXXTOOLS_UNIT_ASSERT_EQUALS(newUser.name(), "foo");

      tntwiki::User user = userManager.getUser("foo", "baz");
      CXXTOOLS_UNIT_ASSERT(!user.loggedIn());

      user = userManager.getUser("foo", "bar");
      CXXTOOLS_UNIT_ASSERT(user.loggedIn());
      CXXTOOLS_UNIT_ASSERT_EQUALS(user.name(), "foo");
    }

    void testSetPassword()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::UserManager userManager(conn);

      tntdb::Transaction trans(conn);

      tntwiki::User newUser = userManager.createUser("foo");
      userManager.setPassword(newUser, "bar");

      tntwiki::User user = userManager.getUser("foo", "bar");
      CXXTOOLS_UNIT_ASSERT(user.loggedIn());
      CXXTOOLS_UNIT_ASSERT_EQUALS(user.name(), "foo");
    }

};

cxxtools::unit::RegisterTest<UserTest> register_UserTest;
