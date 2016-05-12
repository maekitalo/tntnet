/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <cxxtools/serializationinfo.h>

struct Person
{
    Person() { }
    Person(
        const std::string& firstname_,
        const std::string& lastname_,
        const std::string& phone_)
        : firstname(firstname_),
          lastname(lastname_),
          phone(phone_)
          { }

    std::string firstname;
    std::string lastname;
    std::string phone;
};

inline void operator<<= (cxxtools::SerializationInfo& si, const Person& p)
{
    si.addMember("firstname") <<= p.firstname;
    si.addMember("lastname") <<= p.lastname;
    si.addMember("phone") <<= p.phone;
}

inline void operator>>= (const cxxtools::SerializationInfo& si, Person& p)
{
    si.getMember("firstname") >>= p.firstname;
    si.getMember("lastname") >>= p.lastname;
    si.getMember("phone") >>= p.phone;
}

#endif
