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

int unzipFile::checkError(int err, const char* function) const
{
  if (err < 0)
  {
    std::ostringstream msg;
    msg << "unzip-error " << err << " in function \"" << function << '"';
    switch (err)
    {
      case UNZ_END_OF_LIST_OF_FILE:
        msg << ": end of list of file";
        break;

      case UNZ_PARAMERROR:
        msg << ": parameter error";
        break;

      case UNZ_BADZIPFILE:
        msg << ": bad zip file";
        break;

      case UNZ_INTERNALERROR:
        msg << ": internal error";
        break;

      case UNZ_CRCERROR:
        msg << ": crc error";
        break;
    }

    throw unzipError(err, msg.str());
  }
  return err;
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
