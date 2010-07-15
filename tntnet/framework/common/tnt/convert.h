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
  class ConversionError : public std::runtime_error
  {
      explicit ConversionError(const std::string& msg);

    public:
      static void doThrow(const char* argname, const char* typeto, const std::string& value);
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

  inline std::string toString(char value)
  { return std::string(1, value); }

  inline std::string toString(char value, const std::locale&)
  { return std::string(1, value); }

  inline std::string toString(bool value)
  {
    static const std::string trueValue = "1";
    return value ? trueValue : std::string();
  }

  inline std::string toString(bool value, const std::locale&)
  { return toString(value); }

  // convert std::string to some type

  template <typename T>
  inline T stringTo(const char* argname, const char* typeto, const std::string& value)
  {
    T ret;
    std::istringstream s(value);
    s >> ret;
    if (!s)
      ConversionError::doThrow(argname, typeto, value);
    return ret;
  }

  template <typename T>
  inline T stringTo(const char* argname, const char* typeto, const std::string& value, const std::locale& loc)
  {
    T ret;
    std::istringstream s(value);
    s.imbue(loc);
    s >> ret;
    if (!s)
      ConversionError::doThrow(argname, typeto, value);
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

  // specializations

  template <>
  inline std::string stringTo<std::string>(const char* /*argname*/, const char* /*typeto*/, const std::string& value)
  { return value; }

  template <>
  inline std::string stringTo<std::string>(const char* /*argname*/, const char* /*typeto*/, const std::string& value, const std::locale&)
  { return value; }

  template <>
  inline char stringTo<char>(const char* argname, const char* typeto, const std::string& value)
  {
    if (value.empty())
      ConversionError::doThrow(argname, typeto, value);
    return value[0];
  }

  template <>
  inline char stringTo<char>(const char* argname, const char* typeto, const std::string& value, const std::locale&)
  { return stringTo<char>(argname, typeto, value); }

  template <>
  inline bool stringTo<bool>(const char* /*argname*/, const char* /*typeto*/, const std::string& value)
  {
    // consider empty value as false
    return !value.empty();
  }

  template <>
  inline bool stringTo<bool>(const char* /*argname*/, const char* /*typeto*/, const std::string& value, const std::locale&)
  { return stringTo<bool>(0, 0, value); }

  template <>
  inline std::string stringToWithDefault<std::string>(const std::string& value, const std::string&)
  { return value; }

  template <>
  inline std::string stringToWithDefault<std::string>(const std::string& value, const std::string&, const std::locale&)
  { return value; }

  template <>
  inline char stringToWithDefault<char>(const std::string& value, const char& def)
  { return value.empty() ? def : value[0]; }

  template <>
  inline char stringToWithDefault<char>(const std::string& value, const char& def, const std::locale&)
  { return value.empty() ? def : value[0]; }

  template <>
  inline bool stringToWithDefault<bool>(const std::string& value, const bool& def)
  {
    return value.empty() ? def
                         : stringTo<bool>(0, 0, value);
  }

  template <>
  inline bool stringToWithDefault<bool>(const std::string& value, const bool& def, const std::locale&)
  { return stringToWithDefault<bool>(value, def); }

  template <typename iteratorType, typename vectorType>
  void convertRange(const char* argname, const char* typeto, iteratorType b, iteratorType e, vectorType& v, const std::locale& l)
  {
    for (iteratorType it = b; it != e; ++it)
      v.push_back(stringTo<typename vectorType::value_type>(argname, typeto, *it, l));
  }

}

#endif // TNT_CONVERT_H

