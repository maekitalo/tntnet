/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#ifndef TNT_QUERY_PARAMS_H
#define TNT_QUERY_PARAMS_H

#include <cxxtools/query_params.h>
#include <tnt/scope.h>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace tnt
{
  class QueryParams;

  class ConversionError : public std::runtime_error
  {
    public:
      explicit ConversionError(const std::string& msg)
        : std::runtime_error(msg)
        { }

      static void doThrow(const std::string& argname, unsigned argnum, const char* typeto, const std::string& value);
  };

  /// @cond internal
  namespace qhelper
  {
    template <typename Type>
    class QArg
    {
      public:
        static Type arg(const QueryParams& q, const std::string& name, unsigned n, const Type& def);
        static Type argt(const QueryParams& q, const std::string& name, unsigned n, const char* typeName);
    };

    template <>
    class QArg<std::string>
    {
      public:
        static std::string arg(const QueryParams& q, const std::string& name, unsigned n, const std::string& def);
        static std::string argt(const QueryParams& q, const std::string& name, unsigned n, const char* typeName);
    };

    template <>
    class QArg<char>
    {
      public:
        static char arg(const QueryParams& q, const std::string& name, unsigned n, char);
        static char argt(const QueryParams& q, const std::string& name, unsigned n, const char* typeName);
    };

    template <>
    class QArg<bool>
    {
      public:
        static bool arg(const QueryParams& q, const std::string& name, unsigned n, bool);
        static bool argt(const QueryParams& q, const std::string& name, unsigned n, const char* typeName);
    };

    template <typename Type>
    class QAdd
    {
      public:
        static void add(QueryParams& q, const std::string& name, const Type& value);
    };

    template <>
    class QAdd<std::string>
    {
      public:
        static void add(QueryParams& q, const std::string& name, const std::string& value);
    };

    template <>
    class QAdd<char>
    {
      public:
        static void add(QueryParams& q, const std::string& name, char value);
    };

    template <>
    class QAdd<bool>
    {
      public:
        static void add(QueryParams& q, const std::string& name, bool value);
    };

  }
  /// @endcond internal

  /// Container for GET and POST parameters
  class QueryParams : public cxxtools::QueryParams
  {
      Scope* _paramScope;
      std::locale _locale;

    public:
      /// Create an empty %QueryParams object
      QueryParams()
        : _paramScope(0),
          _locale(std::locale::classic())
        { }

      QueryParams(const QueryParams& src)
        : cxxtools::QueryParams(src),
          _paramScope(src._paramScope),
          _locale(src._locale)
        {
          if (_paramScope)
            _paramScope->addRef();
        }

      explicit QueryParams(const std::string& url)
        : cxxtools::QueryParams(url),
          _paramScope(0),
          _locale(std::locale::classic())
        { }

      QueryParams& operator= (const QueryParams& src)
      {
        if (this == &src)
          return *this;

        cxxtools::QueryParams::operator=(src);
        if (_paramScope != src._paramScope)
        {
          if (_paramScope->release() == 0)
            delete _paramScope;
          _paramScope = src._paramScope;
          if (_paramScope)
            _paramScope->addRef();
        }
        _locale = src._locale;
        return *this;
      }

      ~QueryParams()
      {
        if (_paramScope && _paramScope->release() == 0)
          delete _paramScope;
      }

      Scope& getScope()
      {
        if (!_paramScope)
          _paramScope = new Scope();
        return *_paramScope;
      }

      /// Get the locale used for parsing floating-point numbers
      const std::locale& locale() const   { return _locale; }

      /// Set the locale used for parsing floating-point numbers
      void locale(const std::locale& loc) { _locale = loc; }

      template <typename Type>
      Type arg(const std::string& name, const Type& def = Type()) const
        { return qhelper::QArg<Type>::arg(*this, name, 0, def); }

      template <typename Type>
      Type argt(const std::string& name, const char* typeName = 0) const
        { return qhelper::QArg<Type>::argt(*this, name, 0, typeName); }

      template <typename Type>
      Type argn(const std::string& name, size_type n, const Type& def = Type()) const
        { return qhelper::QArg<Type>::arg(*this, name, n, def); }

      template <typename Type>
      Type argnt(const std::string& name, size_type n, const char* typeName = 0) const
        { return qhelper::QArg<Type>::argt(*this, name, n, typeName); }

      template <typename Type>
      Type argt(const std::string& name, size_type n, const char* typeName = 0) const
        { return qhelper::QArg<Type>::argt(*this, name, n, typeName); }

      template <typename Type>
      Type arg(size_type n, const Type& def = Type()) const
        { return qhelper::QArg<Type>::arg(*this, std::string(), n, def); }

      template <typename Type>
      Type argt(size_type n, const char* typeName = 0) const
        { return qhelper::QArg<Type>::argt(*this, std::string(), n, typeName); }

      template <typename Type>
      typename std::vector<Type> args(const std::string& name, const Type& def = Type()) const
      {
        typename std::vector<Type> ret(paramcount(name));
        for (typename std::vector<Type>::size_type n = 0; n < ret.size(); ++n)
          ret[n] = qhelper::QArg<Type>::arg(*this, name, n, def);
        return ret;
      }

      template <typename Type>
      typename std::vector<Type> argst(const std::string& name, const char* typeName) const
      {
        typename std::vector<Type> ret(paramcount(name));
        for (typename std::vector<Type>::size_type n = 0; n < ret.size(); ++n)
          ret[n] = qhelper::QArg<Type>::argt(*this, name, n, typeName);
        return ret;
      }

      template <typename Type>
      QueryParams& add(const std::string& name, const Type& value)
        { qhelper::QAdd<Type>::add(*this, name, value); return *this; }

      QueryParams& add(const std::string& name, const char* value)
      {
        cxxtools::QueryParams::add(name, value);
        return *this;
      }

      QueryParams& add(const cxxtools::QueryParams& q)
      {
        cxxtools::QueryParams::add(q);
        return *this;
      }
  };

  namespace qhelper
  {
    template <typename Type>
    Type QArg<Type>::arg(const QueryParams& q, const std::string& name, unsigned n, const Type& def)
    {
      std::string v = q.param(name, n);
      std::istringstream s(v);
      s.imbue(q.locale());
      Type ret;
      s >> ret;
      if (!s)
        return def;
      return ret;
    }

    template <typename Type>
    Type QArg<Type>::argt(const QueryParams& q, const std::string& name, unsigned n, const char* typeName)
    {
      std::string v = q.param(name, n);
      std::istringstream s(v);
      s.imbue(q.locale());
      Type ret;
      s >> ret;
      if (!s)
        ConversionError::doThrow(name, n, typeName, v);
      return ret;
    }

    inline std::string QArg<std::string>::arg(const QueryParams& q, const std::string& name, unsigned n, const std::string& def)
      { return q.param(name, n, def); }

    inline std::string QArg<std::string>::argt(const QueryParams& q, const std::string& name, unsigned n, const char*)
      { return q.param(name, n); }

    inline char QArg<char>::arg(const QueryParams& q, const std::string& name, unsigned n, char def)
    {
      std::string v = q.param(name, n);
      return v.empty() ? def : v[0];
    }

    inline char QArg<char>::argt(const QueryParams& q, const std::string& name, unsigned n, const char* typeName)
    {
      std::string v = q.param(name, n);
      if (v.empty())
        ConversionError::doThrow(name, n, typeName, v);
      return v[0];
    }

    inline bool QArg<bool>::argt(const QueryParams& q, const std::string& name, unsigned n, const char* /* typeName */)
    {
      std::string v = q.param(name, n);
      return !v.empty();
    }

    inline bool QArg<bool>::arg(const QueryParams& q, const std::string& name, unsigned n, bool)
    {
      std::string v = q.param(name, n);
      return !v.empty();
    }

    template <typename Type>
    void QAdd<Type>::add(QueryParams& q, const std::string& name, const Type& value)
    {
      std::ostringstream s;
      s.imbue(q.locale());
      s << value;
      static_cast<cxxtools::QueryParams&>(q).add(name, s.str());
    }

    inline void QAdd<std::string>::add(QueryParams& q, const std::string& name, const std::string& value)
      { static_cast<cxxtools::QueryParams&>(q).add(name, value); }

    inline void QAdd<char>::add(QueryParams& q, const std::string& name, char value)
      { static_cast<cxxtools::QueryParams&>(q).add(name, std::string(1, value)); }

    inline void QAdd<bool>::add(QueryParams& q, const std::string& name, bool value)
      { static_cast<cxxtools::QueryParams&>(q).add(name, value ? std::string(1, '1') : std::string()); }
  }
}

#endif // TNT_QUERY_PARAMS_H

