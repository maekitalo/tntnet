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


#ifndef TNT_CONTENTTYPE_H
#define TNT_CONTENTTYPE_H

#include <tnt/messageattribute.h>
#include <map>

namespace tnt
{
  /// Content-type-field like rfc2045
  class Contenttype : public MessageattributeParser
  {
    public:
      typedef std::multimap<std::string, std::string> parameter_type;
      typedef parameter_type::const_iterator parameter_iterator;

    private:
      virtual return_type onType(const std::string& type, const std::string& subtype);
      virtual return_type onParameter(const std::string& attribute, const std::string& value);

      std::string type;
      std::string subtype;
      parameter_type parameter;
      std::string boundary;

    public:
      Contenttype()
      { }

      explicit Contenttype(const std::string& ct);
      Contenttype(const std::string& type_, const std::string& subtype_)
        : type(type_),
          subtype(subtype_)
          { }

      const std::string& getType() const     { return type; }
      const std::string& getSubtype() const  { return subtype; }
      const std::string& getBoundary() const { return boundary; }
      bool isMultipart() const
        { return type == "multipart" && !boundary.empty(); }

      parameter_iterator parameter_begin() const
        { return parameter.begin(); }
      parameter_iterator parameter_end() const
        { return parameter.end(); }
      parameter_iterator parameter_find(parameter_type::key_type key) const
        { return parameter.find(key); }
      parameter_iterator parameter_upper_bound(parameter_type::key_type key) const
        { return parameter.upper_bound(key); }

      bool operator== (const Contenttype& ct) const
        { return type == ct.type
              && subtype == ct.subtype
              && parameter == ct.parameter
              && boundary == ct.boundary; }
  };

  inline std::istream& operator>> (std::istream& in, Contenttype& ct)
  {
    ct.parse(in);
    return in;
  }
}

#endif // TNT_CONTENTTYPE_H

