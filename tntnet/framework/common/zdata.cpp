////////////////////////////////////////////////////////////////////////
// zdata.cpp
//

#include <tnt/zdata.h>
#include <zlib.h>
#include <stdexcept>
#include <tnt/log.h>

#ifndef NOTHREAD
#define THREAD 1
#endif

#ifdef THREAD
#include <cxxtools/thread.h>
#endif

namespace tnt
{
#ifdef THREAD
  static Mutex mutex;
#endif

  void zdata::addRef()
  {
#ifdef THREAD
    MutexLock lock(mutex);
#endif
    if (refs++ == 0)
    {
      // allocate uncompressed data
      data = new char[data_len];

      // uncompress zdata => data
      log_debug("uncompress " << zdata_len << " to " << data_len << " bytes");
      uLong dest_len = data_len;
      int z_ret = uncompress((Bytef*)data, &dest_len, (const Bytef*)zptr, zdata_len);
      if (z_ret != Z_OK)
      {
        throw std::runtime_error(std::string("error uncompressing data: ") +
          (z_ret == Z_MEM_ERROR ? "Z_MEM_ERROR" :
           z_ret == Z_BUF_ERROR ? "Z_BUF_ERROR" :
           z_ret == Z_DATA_ERROR ? "Z_DATA_ERROR" : "unknown error"));
      }
      log_debug("uncompress ready");
    }
  }

  void zdata::release()
  {
#ifdef THREAD
    MutexLock lock(mutex);
#endif
    if (--refs <= 0)
    {
      log_debug("release " << data_len << " uncompressed bytes");
      delete[] data;
      data = 0;
    }
  }

}
