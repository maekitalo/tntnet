/* unzip_pp.cpp
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

#include "unzip_pp.h"
#include <sstream>

std::string unzipError::formatMsg(int err, const char* msg, const char* function)
{
  std::ostringstream s;
  s << "unzip-error " << err;
  if (function && function[0])
    s << " in function \"" << function << '"';
  s << ": " << msg;
  return s.str();
}

int unzipFile::checkError(int ret, const char* function) const
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
