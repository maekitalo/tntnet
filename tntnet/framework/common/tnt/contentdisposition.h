/* tnt/contentdisposition.h
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

