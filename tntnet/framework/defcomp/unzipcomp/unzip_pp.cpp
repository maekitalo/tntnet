////////////////////////////////////////////////////////////////////////
// unzip_pp.cpp
//

#include "unzip_pp.h"

unzipFileStreamBuf::int_type unzipFileStreamBuf::overflow(unzipFileStreamBuf::int_type c)
{ return traits_type::eof(); }

unzipFileStreamBuf::int_type unzipFileStreamBuf::underflow()
{
  try
  {
    int n = file.readCurrentFile(buffer, sizeof(buffer));
    if (n == 0)
      return traits_type::eof();
    setg(buffer, buffer, buffer + n);
    return (int_type)(unsigned char)buffer[0];
  }
  catch (const unzipError& e)
  {
    return traits_type::eof();
  }
}

int unzipFileStreamBuf::sync()
{
  return 0;
}
