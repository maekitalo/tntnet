/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

/*
 * This file specifies data structures for handling persons.
 */

#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <cxxtools/string.h>
#include <cxxtools/serializationinfo.h>

struct Person
{
    // We use cxxtools::String instead of std::string since we want to process
    // unicode correctly.

    cxxtools::String firstname;
    cxxtools::String lastname;
    std::string phone;
};

struct PersonId : public Person
{
    unsigned id;
};

// We define serialization and deserialiation operators so that it can be
// converted to and from json or other formats when needed.

inline void operator<<= (cxxtools::SerializationInfo& si, const Person& p)
{
    si.addMember("firstname") <<= p.firstname;
    si.addMember("lastname") <<= p.lastname;
    si.addMember("phone") <<= p.phone;

    si.setTypeName("Person");
}

inline void operator>>= (const cxxtools::SerializationInfo& si, Person& p)
{
    si.getMember("firstname") >>= p.firstname;
    si.getMember("lastname") >>= p.lastname;
    si.getMember("phone") >>= p.phone;
}

inline void operator<<= (cxxtools::SerializationInfo& si, const PersonId& p)
{
    si.addMember("id") <<= p.id;
    si <<= static_cast<const Person&>(p);

    si.setTypeName("PersonId");
}

inline void operator>>= (const cxxtools::SerializationInfo& si, PersonId& p)
{
    si.getMember("id") >>= p.id;
    si >>= static_cast<Person&>(p);
}

#endif
