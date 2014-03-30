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


#ifndef TNT_MULTIPART_H
#define TNT_MULTIPART_H

#include <tnt/messageheader.h>
#include <tnt/contentdisposition.h>
#include <vector>
#include <iterator>

namespace tnt
{
  /// header of a MIME-multipart-object
  class Partheader : public Messageheader
  {
    private:
      Contentdisposition _cd;

    protected:
      return_type onField(const char* name, const char* value);

    public:
      const Contentdisposition& getContentDisposition() const
        { return _cd; }
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
      Partheader _header;
      const_iterator _bodyBegin;
      const_iterator _bodyEnd;

    public:
      Part() { }

      Part(const_iterator b, const_iterator e);

      /// returns the Partheader-object of this Part.
      const Partheader& getHeader() const { return _header; }

      /// returns a single header-value or empty string if not set.
      std::string getHeader(const std::string& key) const;

      /// returns the type of this Part
      const std::string& getType() const
        { return _header.getContentDisposition().getType(); }

      /// returns the type of this Part
      std::string getMimetype() const
        { return _header.getMimetype(); }

      /// returns the name of this Part (name-attribute of html-input-field)
      const std::string& getName() const
        { return _header.getContentDisposition().getName(); }

      /// returns the passed filename of the Part or empty string.
      const std::string& getFilename() const
        { return _header.getContentDisposition().getFilename(); }

      /// returns a const iterator to the start of data.
      const_iterator getBodyBegin() const
        { return _bodyBegin; }

      /// returns a const iterator past the end of data.
      const_iterator getBodyEnd() const
        { return _bodyEnd; }

      /// less efficient (a temporary string is created), but easier to use:
      std::string getBody() const
        { return std::string(getBodyBegin(), getBodyEnd()); }
      bool isEmpty() const
        { return getBodyBegin() == getBodyEnd(); }
      size_type getSize() const
        { return getBodyEnd() - getBodyBegin(); }

      // stl-style accessors
      const_iterator begin() const { return getBodyBegin(); }
      const_iterator end() const   { return getBodyEnd(); }
      size_type size() const       { return getSize(); }
      bool empty() const           { return isEmpty(); }
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
      std::string _body;
      parts_type _parts;

    public:
      Multipart() { }
      void set(const std::string& boundary, const std::string& body);

      const_iterator begin() const { return _parts.begin(); }
      const_iterator end() const   { return _parts.end(); }
      const_iterator find(const std::string& partName, const_iterator start) const;
      const_iterator find(const std::string& partName) const
        { return find(partName, begin()); }
  };
}

#endif // TNT_MULTIPART_H

