/* unzipfile.cpp
 * Copyright (C) 2003-2006 Tommi Maekitalo
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

#include "tnt/unzipfile.h"
#include <sstream>
#include "unzip.h"
#include <cxxtools/log.h>

log_define("tntnet.unzipfile")

namespace tnt
{
  namespace
  {
    int checkError(int ret, const char* function)
    {
      log_debug("checkError(" << ret << ", " << function << ')');
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
    file = new unzFileStruct;
    if (!(file->file = ::unzOpen(path.c_str())))
    {
      delete file;
      file = 0;
      throw unzipFileNotFound(path);
    }
  }

  void unzipFile::close()
  {
    if (file)
    {
      unzClose(file->file);
      delete file;
      file = 0;
    }
  }

  unzipFile::~unzipFile()
  {
    if (file)
    {
      unzClose(file->file);
      delete file;
    }
  }

  void unzipFile::goToFirstFile()
  {
    checkError(::unzGoToFirstFile(file->file), "unzGoToFirstFile");
  }

  void unzipFile::goToNextFile()
  {
    checkError(::unzGoToNextFile(file->file), "unzGoToNextFile");
  }

  void unzipFile::locateFile(const std::string& fileName, bool caseSensitivity)
  {
    checkError(::unzLocateFile(file->file, fileName.c_str(), caseSensitivity ? 1 : 0), "unzLocateFile");
  }

  void unzipFile::openCurrentFile()
  {
    checkError(::unzOpenCurrentFile(file->file), "unzOpenCurrentFile");
  }

  void unzipFile::openCurrentFile(const std::string& pw)
  {
    checkError(::unzOpenCurrentFilePassword(file->file, pw.c_str()), "unzOpenCurrentFilePassword");
  }

  void unzipFile::closeCurrentFile()
  {
    checkError(::unzCloseCurrentFile(file->file), "unzCloseCurrentFile");
  }

  int unzipFile::readCurrentFile(void* buf, unsigned len)
  {
    return checkError(::unzReadCurrentFile(file->file, buf, len), "unzReadCurrentFile");
  }

  //////////////////////////////////////////////////////////////////////
  // unzipFileStreamBuf
  //

  unzipFileStreamBuf::int_type unzipFileStreamBuf::overflow(unzipFileStreamBuf::int_type c)
  { return traits_type::eof(); }

  unzipFileStreamBuf::int_type unzipFileStreamBuf::underflow()
  {
    int n = file.readCurrentFile(buffer, sizeof(buffer));
    if (n == 0)
      return traits_type::eof();
    setg(buffer, buffer, buffer + n);
    return traits_type::to_int_type(buffer[0]);
  }

  int unzipFileStreamBuf::sync()
  {
    return 0;
  }
}
