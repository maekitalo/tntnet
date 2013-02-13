/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef TNTWIKI_USER_H
#define TNTWIKI_USER_H

#include <string>

namespace tntdb
{
  class Connection;
}

namespace tntwiki
{
  class UserManager;

  class User
  {
      friend class UserManager;
      unsigned _id;
      std::string _name;

    public:
      User()
        : _id(0)
      { }

      const std::string& name() const
      { return _name; }

      unsigned id() const
      { return _id; }

      bool loggedIn() const
      {
        return _id > 0;
      }

      void logout()
      {
        _id = 0;
        _name.clear();
      }
  };
}

#endif // TNTWIKI_USER_H

