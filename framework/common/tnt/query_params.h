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
#include <cxxtools/serializationinfo.h>
#include <cxxtools/serializationerror.h>
#include <tnt/scope.h>
#include <stdexcept>
#include <vector>
#include <sstream>

namespace tnt
{
  class QueryParams;

  /// @cond internal
  namespace qhelper
  {
    ////////////////////////////////////////////////////////////////////////
    // generic helper functions
    //
    template <typename Type>
    class QArg
    {
      public:
        static Type get(const QueryParams& q, const std::string& name, const Type& def = Type());
        static std::vector<Type> getvector(const QueryParams& q, const std::string& name);
    };

    ////////////////////////////////////////////////////////////////////////
    // specialization for std::string
    //
    template <>
    class QArg<std::string>
    {
      public:
        static std::string get(const QueryParams& q, const std::string& name, const std::string& def = std::string());
        static std::vector<std::string> getvector(const QueryParams& q, const std::string& name);
    };

    ////////////////////////////////////////////////////////////////////////
    // specialization for char
    //
    template <>
    class QArg<char>
    {
      public:
        static char get(const QueryParams& q, const std::string& name, char def = '\0');
        static std::vector<char> getvector(const QueryParams& q, const std::string& name);
    };

    ////////////////////////////////////////////////////////////////////////
    // specialization for bool
    //
    template <>
    class QArg<bool>
    {
      public:
        static bool get(const QueryParams& q, const std::string& name);
        static bool get(const QueryParams& q, const std::string& name, bool def);
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
      template <typename T> friend class qhelper::QAdd;

      Scope* _paramScope;
      mutable cxxtools::SerializationInfo _si;
      mutable bool _siInit;

    public:
      /// Create an empty %QueryParams object
      QueryParams()
        : _paramScope(0),
          _siInit(false)
        { }

      QueryParams(const QueryParams& src)
        : cxxtools::QueryParams(src),
          _paramScope(src._paramScope),
          _siInit(false)
        {
          if (_paramScope)
            _paramScope->addRef();
        }

      explicit QueryParams(const std::string& url)
        : cxxtools::QueryParams(url),
          _paramScope(0),
          _siInit(false)
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

        _siInit = false;

        return *this;
      }

      ~QueryParams()
      {
        if (_paramScope && _paramScope->release() == 0)
          delete _paramScope;
      }

      void clear()
      {
        cxxtools::QueryParams::clear();
        _si.clear();
        _siInit = false;
      }

      cxxtools::SerializationInfo& si()
      {
        if (!_siInit)
        {
          _si <<= *this;
          _siInit = true;
        }
        return _si;
      }

      const cxxtools::SerializationInfo& si() const
      {
        if (!_siInit)
        {
          _si <<= *this;
          _siInit = true;
        }
        return _si;
      }

      Scope& getScope()
      {
        if (!_paramScope)
          _paramScope = new Scope();
        return *_paramScope;
      }

      /// Get parameter with name `name` if available. Returns `def` if not found.
      /// For string type the value is returned.
      /// Boolean returns `true` if it is found, `false` otherwise.
      /// Other types are converted using the cxxtools serialization framework.
      template <typename Type>
      Type get(const std::string& name, const Type& def) const
        { return qhelper::QArg<Type>::get(*this, name, def); }

      /// Get parameter with name `name` if available. Returns default value if not found.
      /// For string type the value is returned.
      /// Boolean returns `true` if it is found, `false` otherwise.
      /// Other types are converted using the cxxtools serialization framework.
      template <typename Type>
      Type get(const std::string& name) const
        { return qhelper::QArg<Type>::get(*this, name); }

      /// alias for `get`
      /// @deprecated
      template <typename Type>
      Type arg(const std::string& name, const Type& def) const
        { return qhelper::QArg<Type>::get(*this, name, def); }

      /// alias for `get`
      /// @deprecated
      template <typename Type>
      Type arg(const std::string& name) const
        { return qhelper::QArg<Type>::get(*this, name); }

      template <typename Type>
      std::vector<Type> getvector(const std::string& name) const
        { return qhelper::QArg<Type>::getvector(*this, name); }

      /// alias for `getvector`
      /// @deprecated
      template <typename Type>
      std::vector<Type> args(const std::string& name) const
        { return qhelper::QArg<Type>::getvector(*this, name); }


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
    ////////////////////////////////////////////////////////////////////////
    // generic helper functions
    //
    template <typename Type>
    Type QArg<Type>::get(const QueryParams& q, const std::string& name, const Type& def)
    {
      const cxxtools::SerializationInfo* pi;
      try
      {
        pi = &q.si().getMember(name);
      }
      catch (const cxxtools::SerializationMemberNotFound&)
      {
        return def;
      }

      Type ret;
      *pi >>= ret;
      return ret;
    }

    template <typename Type>
    std::vector<Type> QArg<Type>::getvector(const QueryParams& q, const std::string& name)
    {
      std::vector<Type> ret;

      const cxxtools::SerializationInfo* pi;
      try
      {
        pi = &q.si().getMember(name);
      }
      catch (const cxxtools::SerializationMemberNotFound&)
      {
        return ret;
      }

      *pi >>= ret;
      return ret;
    }


    ////////////////////////////////////////////////////////////////////////
    // specialization for std::string
    //
    inline std::string QArg<std::string>::get(const QueryParams& q, const std::string& name, const std::string& def)
    {
      return q.param(name, def);
    }

    inline std::vector<std::string> QArg<std::string>::getvector(const QueryParams& q, const std::string& name)
    {
      return std::vector<std::string>(q.begin(name + "[]"), q.end());
    }


    ////////////////////////////////////////////////////////////////////////
    // specialization for char
    //
    inline char QArg<char>::get(const QueryParams& q, const std::string& name, char def)
    {
      std::string p = q.param(name);
      return p.empty() ? def : p[0];
    }

    inline std::vector<char> QArg<char>::getvector(const QueryParams& q, const std::string& name)
    {
      std::vector<char> ret;
      for (QueryParams::const_iterator it = q.begin(name + "[]"); it != q.end(); ++it)
        ret.push_back(it->empty() ? '\0' : (*it)[0]);
      return ret;
    }


    ////////////////////////////////////////////////////////////////////////
    // specialization for bool
    //
    inline bool QArg<bool>::get(const QueryParams& q, const std::string& name)
    {
      return !q.param(name).empty();
    }

    inline bool QArg<bool>::get(const QueryParams& q, const std::string& name, bool def)
    {
      try
      {
        bool ret;
        q.si().getMember(name) >>= ret;
        return ret;
      }
      catch (const cxxtools::SerializationMemberNotFound&)
      {
        return def;
      }
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename Type>
    void QAdd<Type>::add(QueryParams& q, const std::string& name, const Type& value)
    {
      std::ostringstream s;
      s << value;
      static_cast<cxxtools::QueryParams&>(q).add(name, s.str());
      q._si.clear();
      q._siInit = false;
    }

    inline void QAdd<std::string>::add(QueryParams& q, const std::string& name, const std::string& value)
    {
      static_cast<cxxtools::QueryParams&>(q).add(name, value);
      q._si.clear();
      q._siInit = false;
    }

    inline void QAdd<char>::add(QueryParams& q, const std::string& name, char value)
    {
      static_cast<cxxtools::QueryParams&>(q).add(name, std::string(1, value));
      q._si.clear();
      q._siInit = false;
    }

    inline void QAdd<bool>::add(QueryParams& q, const std::string& name, bool value)
    {
      static_cast<cxxtools::QueryParams&>(q).add(name, value ? std::string(1, '1') : std::string());
      q._si.clear();
      q._siInit = false;
    }
  }
}

#endif // TNT_QUERY_PARAMS_H

