/* tnt/contentdisposition.h
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_CONTENTDISPOSITION_H
#define TNT_CONTENTDISPOSITION_H

#include <tnt/messageattribute.h>

namespace tnt
{
  /// Content-Disposition-Header.
  /// (Content-Disposition: form-data; name="mein-upload-feld"; filename="ttt.sh")
  class Contentdisposition : public MessageattributeParser
  {
      std::string type;
      std::string name;
      std::string filename;

      virtual return_type onType(const std::string& type,
        const std::string& subtype);
      virtual return_type onParameter(const std::string& attribute,
        const std::string& value);

    public:
      const std::string& getType() const      { return type; }
      const std::string& getName() const      { return name; }
      const std::string& getFilename() const  { return filename; }
  };

  inline std::istream& operator>> (std::istream& in, Contentdisposition& ct)
  {
    ct.parse(in);
    return in;
  }
}

#endif // TNT_CONTENTDISPOSITION_H

