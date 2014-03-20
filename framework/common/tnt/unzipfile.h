/*
 * Copyright (C) 2003/2006 Tommi Maekitalo
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


#ifndef TNT_UNZIPFILE_H
#define TNT_UNZIPFILE_H

#include <stdexcept>
#include <iostream>
#include <string>

namespace tnt
{
  class unzipError : public std::runtime_error
  {
    private:
      int _err;

      static std::string formatMsg(int e, const char* msg, const char* function);

    public:
      unzipError(int e, const std::string& msg = "unzipError")
        : std::runtime_error(msg),
          _err(e)
          { }
      unzipError(int e, const char* msg, const char* function)
        : std::runtime_error(formatMsg(e, msg, function)),
          _err(e)
          { }

      int getErr() const { return _err; }
  };

  class unzipFileNotFound : public unzipError
  {
    public:
      unzipFileNotFound(const std::string& file)
        : unzipError(0, "file not found " + file)
        { }
  };

  class unzipEndOfListOfFile : public unzipError
  {
    public:
      unzipEndOfListOfFile(const char* function = 0);
  };

  class unzipParamError : public unzipError
  {
    public:
      unzipParamError(const char* function = 0);
  };

  class unzipBadZipFile : public unzipError
  {
    public:
      unzipBadZipFile(const char* function = 0);
  };

  class unzipInternalError : public unzipError
  {
    public:
      unzipInternalError(const char* function = 0);
  };

  class unzipCrcError : public unzipError
  {
    public:
      unzipCrcError(const char* function = 0);
  };

  class unzipFile
  {
    private:
      struct unzFileStruct;
      unzFileStruct* _file;

    public:
      unzipFile()
        : _file(0)
        { }

      unzipFile(const std::string& path)
        : _file(0)
        { open(path); }

      ~unzipFile();

      void open(const std::string& path);
      void close();

      void goToFirstFile();
      void goToNextFile();
      void locateFile(const std::string& fileName, bool caseSensitivity = true);
      void openCurrentFile();
      void openCurrentFile(const std::string& pw);
      void closeCurrentFile();
      int readCurrentFile(void* buf, unsigned len);
  };

  class unzipFileStreamBuf : public std::streambuf
  {
      char_type _buffer[512];
      unzipFile& _file;

    public:
      unzipFileStreamBuf(unzipFile& file, const std::string& fileName, bool caseSensitivity)
        : _file(file)
      {
        file.locateFile(fileName, caseSensitivity);
        file.openCurrentFile();
      }

      unzipFileStreamBuf(unzipFile& file, const std::string& fileName, bool caseSensitivity,
        const std::string& password)
        : _file(file)
      {
        file.locateFile(fileName, caseSensitivity);
        file.openCurrentFile(password);
      }

      ~unzipFileStreamBuf()
        { _file.closeCurrentFile(); }

      /// overridden from std::streambuf
      int_type overflow(int_type c);
      /// overridden from std::streambuf
      int_type underflow();
      /// overridden from std::streambuf
      int sync();
  };

  class unzipFileStream : public std::istream
  {
    private:
      unzipFileStreamBuf _streambuf;

    public:
      unzipFileStream(unzipFile& file, const std::string& fileName, bool caseSensitivity = true)
        : std::istream(0),
          _streambuf(file, fileName, caseSensitivity)
        { init(&_streambuf); }
  };
}

#endif // UNZIPFILE_H

