/* tnt/messageattribute.h
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

#ifndef TNT_MESSAGEATTRIBUTE_H
#define TNT_MESSAGEATTRIBUTE_H

#include <string>
#include <iosfwd>

namespace tnt
{
  /// Messageattribute like form-data; name="my-upload-field"; filename="ttt.sh"
  ///                    or application/x-shellscript
  /// see also rfc2045
  class MessageattributeParser
  {
    public:
      enum return_type
      {
        OK,
        FAIL,
        END
      };

    private:
      virtual return_type onType(const std::string& type,
        const std::string& subtype) = 0;
      virtual return_type onParameter(const std::string& attribute,
        const std::string& value) = 0;

    public:
      virtual ~MessageattributeParser()
        { }
      void parse(std::istream& in);
  };
}

#endif // TNT_MESSAGEATTRIBUTE_H

