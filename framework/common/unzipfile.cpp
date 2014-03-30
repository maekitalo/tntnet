/*
 * Copyright (C) 2003-2006 Tommi Maekitalo
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


#include "tnt/unzipfile.h"
#include <sstream>
#include "unzip.h"

namespace tnt
{
  namespace
  {
    int checkError(int ret, const char* function)
    {
      if (ret < 0)
      {
        switch (ret)
        {
          case UNZ_END_OF_LIST_OF_FILE:
            throw unzipEndOfListOfFile(function);

          case UNZ_PARAMERROR:
            throw unzipParamError(function);

          case UNZ_BADZIPFILE:
            throw unzipBadZipFile(function);

          case UNZ_INTERNALERROR:
            throw unzipInternalError(function);

          case UNZ_CRCERROR:
            throw unzipCrcError(function);
        }

        throw unzipError(ret, "unknown error", function);
      }
      return ret;
    }

  }

  std::string unzipError::formatMsg(int err, const char* msg, const char* function)
  {
    std::ostringstream s;
    s << "unzip-error " << err;
    if (function && function[0])
      s << " in function \"" << function << '"';
    s << ": " << msg;
    return s.str();
  }

  unzipEndOfListOfFile::unzipEndOfListOfFile(const char* function)
    : unzipError(UNZ_END_OF_LIST_OF_FILE, "end of list of file", function)
    { }

  unzipParamError::unzipParamError(const char* function)
    : unzipError(UNZ_PARAMERROR, "parameter error", function)
    { }

  unzipBadZipFile::unzipBadZipFile(const char* function)
    : unzipError(UNZ_PARAMERROR, "bad zip file", function)
    { }

  unzipInternalError::unzipInternalError(const char* function)
    : unzipError(UNZ_PARAMERROR, "internal error", function)
    { }

  unzipCrcError::unzipCrcError(const char* function)
    : unzipError(UNZ_PARAMERROR, "crc error", function)
    { }

  //////////////////////////////////////////////////////////////////////
  // unzipFile
  //

  struct unzipFile::unzFileStruct
  {
    unzFile file;
  };

  void unzipFile::open(const std::string& path)
  {
    close();
    _file = new unzFileStruct;
    if (!(_file->file = ::unzOpen(path.c_str())))
    {
      delete _file;
      _file = 0;
      throw unzipFileNotFound(path);
    }
  }

  void unzipFile::close()
  {
    if (_file)
    {
      unzClose(_file->file);
      delete _file;
      _file = 0;
    }
  }

  unzipFile::~unzipFile()
  {
    if (_file)
    {
      unzClose(_file->file);
      delete _file;
    }
  }

  void unzipFile::goToFirstFile()
    { checkError(::unzGoToFirstFile(_file->file), "unzGoToFirstFile"); }

  void unzipFile::goToNextFile()
    { checkError(::unzGoToNextFile(_file->file), "unzGoToNextFile"); }

  void unzipFile::locateFile(const std::string& fileName, bool caseSensitivity)
    { checkError(::unzLocateFile(_file->file, fileName.c_str(), caseSensitivity ? 1 : 0), "unzLocateFile"); }

  void unzipFile::openCurrentFile()
    { checkError(::unzOpenCurrentFile(_file->file), "unzOpenCurrentFile"); }

  void unzipFile::openCurrentFile(const std::string& pw)
    { checkError(::unzOpenCurrentFilePassword(_file->file, pw.c_str()), "unzOpenCurrentFilePassword"); }

  void unzipFile::closeCurrentFile()
    { checkError(::unzCloseCurrentFile(_file->file), "unzCloseCurrentFile"); }

  int unzipFile::readCurrentFile(void* buf, unsigned len)
    { return checkError(::unzReadCurrentFile(_file->file, buf, len), "unzReadCurrentFile"); }

  //////////////////////////////////////////////////////////////////////
  // unzipFileStreamBuf
  //

  unzipFileStreamBuf::int_type unzipFileStreamBuf::overflow(unzipFileStreamBuf::int_type c)
    { return traits_type::eof(); }

  unzipFileStreamBuf::int_type unzipFileStreamBuf::underflow()
  {
    int n = _file.readCurrentFile(_buffer, sizeof(_buffer));
    if (n == 0)
      return traits_type::eof();
    setg(_buffer, _buffer, _buffer + n);
    return traits_type::to_int_type(_buffer[0]);
  }

  int unzipFileStreamBuf::sync()
    { return 0; }
}

