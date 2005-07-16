/* tnt/contentdisposition.h
   Copyright (C) 2003 Tommi Maekitalo

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

