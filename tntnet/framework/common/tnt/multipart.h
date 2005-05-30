/* tnt/multipart.h
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

#ifndef TNT_MULTIPART_H
#define TNT_MULTIPART_H

#include <tnt/messageheader.h>
#include <tnt/contentdisposition.h>
#include <vector>

namespace tnt
{
  /// header of a MIME-multipart-object
  class partheader : public messageheader
  {
      contentdisposition cd;

    protected:
      return_type onField(const std::string& name, const std::string& value);

    public:
      const contentdisposition& getContentDisposition() const
        { return cd; }
  };

  /// Part of a MIME-multipart-object
  class part
  {
    public:
      typedef std::string::const_iterator const_iterator;
      typedef std::string::const_iterator iterator;

    private:
      partheader header;
      const_iterator body_begin;
      const_iterator body_end;

    public:
      part(const_iterator b, const_iterator e);

      /// returns the partheader-object of this part.
      const partheader& getHeader() const      { return header; }
      /// returns a single header-value or empty string if not set.
      std::string getHeader(const std::string& key) const;

      /// returns the name of this part (name-attribute of html-input-field)
      const std::string& getName() const
        { return header.getContentDisposition().getName(); }
      /// returns the passed filename of the part or empty string.
      const std::string& getFilename() const
        { return header.getContentDisposition().getFilename(); }
      /// returns a const iterator to the start of data.
      const_iterator getBodyBegin() const
        { return body_begin; }
      /// returns a const iterator past the end of data.
      const_iterator getBodyEnd() const
        { return body_end; }
      /// less efficient (a temporary string is created), but easier to use:
      std::string getBody() const
        { return std::string(getBodyBegin(), getBodyEnd()); }
  };

  /// a MIME-Multipart-Object
  class multipart
  {
    public:
      typedef part part_type;
      typedef std::vector<part_type> parts_type;
      typedef parts_type::const_iterator const_iterator;
      typedef parts_type::value_type value_type;

    private:
      std::string body;
      parts_type parts;

    public:
      multipart()
        { }
      void set(const std::string& boundary, const std::string& body);

      const_iterator begin() const { return parts.begin(); }
      const_iterator end() const   { return parts.end(); }
      const_iterator find(const std::string& part_name,
        const_iterator start) const;
      const_iterator find(const std::string& part_name) const
        { return find(part_name, begin()); }
  };

}

#endif // TNT_MULTIPART_H

