/*
 * Copyright (C) 2004,2006 Tommi Maekitalo
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


#ifndef TNT_CONVERT_H
#define TNT_CONVERT_H

#include <sstream>
#include <stdexcept>
#include <locale>

namespace tnt
{
  class convertError : public std::runtime_error
  {
      std::string value;

    public:
      explicit convertError(const std::string& value_)
        : std::runtime_error("cannot cast \"" + value_ + '"'),
          value(value_)
        { }
      ~convertError() throw ()
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
      throw convertError(value);
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
      throw convertError(value);
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

