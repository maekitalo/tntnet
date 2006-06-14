/* tnt/multipart.h
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
 */

#ifndef TNT_MULTIPART_H
#define TNT_MULTIPART_H

#include <tnt/messageheader.h>
#include <tnt/contentdisposition.h>
#include <vector>

namespace tnt
{
  /// header of a MIME-multipart-object
  class Partheader : public Messageheader
  {
      Contentdisposition cd;

    protected:
      return_type onField(const std::string& name, const std::string& value);

    public:
      const Contentdisposition& getContentDisposition() const
        { return cd; }
      std::string getMimetype() const;
  };

  /// Part of a MIME-multipart-object
  class Part
  {
    public:
      typedef std::string::const_iterator const_iterator;
      typedef std::string::const_iterator iterator;
      typedef std::string::size_type size_type;
      typedef std::string::value_type value_type;
      typedef std::string::pointer pointer;
      typedef std::string::reference reference;
      typedef std::string::const_reference const_reference;
      typedef std::string::difference_type difference_type;

    private:
      Partheader header;
      const_iterator bodyBegin;
      const_iterator bodyEnd;

    public:
      Part()
        : bodyBegin(0), bodyEnd(0)
        { }

      Part(const_iterator b, const_iterator e);

      /// returns the Partheader-object of this Part.
      const Partheader& getHeader() const      { return header; }
      /// returns a single header-value or empty string if not set.
      std::string getHeader(const std::string& key) const;

      /// returns the type of this Part
      const std::string& getType() const
        { return header.getContentDisposition().getType(); }
      /// returns the type of this Part
      std::string getMimetype() const
        { return header.getMimetype(); }
      /// returns the name of this Part (name-attribute of html-input-field)
      const std::string& getName() const
        { return header.getContentDisposition().getName(); }
      /// returns the passed filename of the Part or empty string.
      const std::string& getFilename() const
        { return header.getContentDisposition().getFilename(); }
      /// returns a const iterator to the start of data.
      const_iterator getBodyBegin() const
        { return bodyBegin; }
      /// returns a const iterator past the end of data.
      const_iterator getBodyEnd() const
        { return bodyEnd; }
      /// less efficient (a temporary string is created), but easier to use:
      std::string getBody() const
        { return std::string(getBodyBegin(), getBodyEnd()); }
      bool isEmpty() const
        { return getBodyBegin() == getBodyEnd(); }
      size_type getSize() const
        { return getBodyEnd() - getBodyBegin(); }

      // stl-style accessors
      const_iterator begin() const  { return getBodyBegin(); }
      const_iterator end() const    { return getBodyEnd(); }
      size_type size() const        { return getSize(); }
      bool empty() const            { return isEmpty(); }
  };

  /// a MIME-Multipart-Object
  class Multipart
  {
    public:
      typedef Part part_type;
      typedef std::vector<part_type> parts_type;
      typedef parts_type::const_iterator const_iterator;
      typedef parts_type::value_type value_type;

    private:
      std::string body;
      parts_type parts;

    public:
      Multipart()
        { }
      void set(const std::string& boundary, const std::string& body);

      const_iterator begin() const { return parts.begin(); }
      const_iterator end() const   { return parts.end(); }
      const_iterator find(const std::string& partName,
        const_iterator start) const;
      const_iterator find(const std::string& partName) const
        { return find(partName, begin()); }
  };

}

#endif // TNT_MULTIPART_H

