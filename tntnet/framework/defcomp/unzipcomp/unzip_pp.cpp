////////////////////////////////////////////////////////////////////////
// unzip_pp.cpp
//

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
  return (int_type)(unsigned char)buffer[0];
}

int unzipFileStreamBuf::sync()
{
  return 0;
}
