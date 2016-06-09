/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

/*
 * This file specifies a data structure which specifies friendship between 2
 * persons.
 */

#ifndef FRIEND_H
#define FRIEND_H

#include <cxxtools/serializationinfo.h>

struct Friend
{
    unsigned personId;
    unsigned friendId;
};

struct FriendWithId : public Friend
{
    unsigned id;       // id of entry
};

// We define serialization and deserialiation operators so that it can be
// converted to and from json or other formats when needed.

inline void operator<<= (cxxtools::SerializationInfo& si, const Friend& f)
{
    si.addMember("personId") <<= f.personId;
    si.addMember("friendId") <<= f.friendId;
    si.setTypeName("Friend");
}

inline void operator>>= (const cxxtools::SerializationInfo& si, Friend& f)
{
    si.getMember("personId") >>= f.personId;
    si.getMember("friendId") >>= f.friendId;
}

inline void operator<<= (cxxtools::SerializationInfo& si, const FriendWithId& f)
{
    si.addMember("id") <<= f.id;
    si <<= static_cast<const Friend&>(f);

    si.setTypeName("FriendWithId");
}

inline void operator>>= (const cxxtools::SerializationInfo& si, FriendWithId& f)
{
    si.getMember("id") >>= f.id;
    si >>= static_cast<Friend&>(f);
}

#endif
