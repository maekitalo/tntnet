/* tnt/contenttype.h
   Copyright (C) 2003 Tommi Mäkitalo

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

#ifndef TNT_CONTENTTYPE_H
#define TNT_CONTENTTYPE_H

#include <tnt/messageattribute.h>
#include <map>

/// Content-type-field like rfc2045
namespace tnt
{
  class contenttype : public messageattribute_parser
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
      contenttype()
      { }

      explicit contenttype(const std::string& ct);

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
  };

  inline std::istream& operator>> (std::istream& in, contenttype& ct)
  {
    ct.parse(in);
    return in;
  }
}

#endif // TNT_CONTENTTYPE_H

