/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#ifndef TNT_CONTENTDISPOSITION_H
#define TNT_CONTENTDISPOSITION_H

#include <tnt/messageattribute.h>

namespace tnt
{
  /** Content-Disposition-Header.

      (Content-Disposition: form-data; name="mein-upload-feld"; filename="ttt.sh")
   */
  class Contentdisposition : public MessageattributeParser
  {
    private:
      std::string _type;
      std::string _name;
      std::string _filename;

      virtual return_type onType(const std::string& type, const std::string& subtype);
      virtual return_type onParameter(const std::string& attribute, const std::string& value);

    public:
      const std::string& getType() const     { return _type; }
      const std::string& getName() const     { return _name; }
      const std::string& getFilename() const { return _filename; }
  };

  inline std::istream& operator>> (std::istream& in, Contentdisposition& ct)
  {
    ct.parse(in);
    return in;
  }
}

#endif // TNT_CONTENTDISPOSITION_H

