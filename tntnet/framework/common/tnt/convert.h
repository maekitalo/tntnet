/* tnt/convert.h
   Copyright (C) 2004,2006 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TNT_CONVERT_H
#define TNT_CONVERT_H

#include <sstream>
#include <stdexcept>
#include <locale>

namespace tnt
{
  class convertException : public std::runtime_error
  {
      std::string value;

    public:
      explicit convertException(const std::string& value_)
        : std::runtime_error("cannot cast \"" + value_ + '"'),
          value(value_)
        { }
      ~convertException() throw ()
        { }

      const std::string& getValue() const   { return value; }
  };

  template <typename T>
  inline std::string toString(const T& value)
  {
    std::ostringstream s;
    s << value;
    return s.str();
  }

  template <typename T>
  inline std::string toString(const T& value, const std::locale& loc)
  {
    std::ostringstream s;
    s.imbue(loc);
    s << value;
    return s.str();
  }

  template <>
  inline std::string toString(const std::string& value)
  { return value; }

  template <>
  inline std::string toString(const std::string& value, const std::locale&)
  { return value; }

  inline std::string toString(const char* value)
  { return std::string(value); }

  inline std::string toString(const char* value, const std::locale&)
  { return std::string(value); }

  template <typename T>
  inline T stringTo(const std::string& value)
  {
    T ret;
    std::istringstream s(value);
    s >> ret;
    if (!s)
      throw convertException(value);
    return ret;
  }

  template <typename T>
  inline T stringTo(const std::string& value, const std::locale& loc)
  {
    T ret;
    std::istringstream s(value);
    s.imbue(loc);
    s >> ret;
    if (!s)
      throw convertException(value);
    return ret;
  }

  template <typename T>
  inline T stringToWithDefault(const std::string& value, const T& def)
  {
    T ret;
    std::istringstream s(value);
    s >> ret;
    if (!s)
      return def;
    return ret;
  }

  template <typename T>
  inline T stringToWithDefault(const std::string& value, const T& def, const std::locale& loc)
  {
    T ret;
    std::istringstream s(value);
    s.imbue(loc);
    s >> ret;
    if (!s)
      return def;
    return ret;
  }

  template <>
  inline std::string stringTo<std::string>(const std::string& value)
  { return value; }

  template <>
  inline std::string stringTo<std::string>(const std::string& value, const std::locale&)
  { return value; }

  template <>
  inline std::string stringToWithDefault<std::string>(const std::string& value, const std::string&)
  { return value; }

  template <>
  inline std::string stringToWithDefault<std::string>(const std::string& value, const std::string&, const std::locale&)
  { return value; }

  template <typename T>
  class stringToConverter : public std::unary_function<std::string, T>
  {
      std::locale loc;
    public:
      stringToConverter() { }
      explicit stringToConverter(const std::locale& loc_)
        : loc(loc_)
        { }

      T operator() (std::string s)
      { return stringTo<T>(s, loc); }
  };

  template <typename T>
  class stringToWithDefaultConverter : public std::unary_function<std::string, T>
  {
      T def;
      std::locale loc;

    public:
      explicit stringToWithDefaultConverter(const T& def_) : def(def_) { }
      stringToWithDefaultConverter(const T& def_, const std::locale& loc_)
        : def(def_), loc(loc_)
        { }

      T operator() (std::string s)
      { return stringToWithDefault<T>(s, def, loc); }
  };
}

#endif // TNT_CONVERT_H

