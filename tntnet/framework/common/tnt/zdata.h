////////////////////////////////////////////////////////////////////////
// zdata.h
//

#ifndef ZDATA_H
#define ZDATA_H

namespace tnt
{
  class zdata
  {
      const char* zptr;
      const unsigned zdata_len;
      const unsigned data_len;

      unsigned refs;
      char* data;

    public:
      zdata(const char* zptr_, unsigned zdata_len_, unsigned data_len_)
        : zptr(zptr_),
          zdata_len(zdata_len_),
          data_len(data_len_),
          refs(0),
          data(0)
      { }

      void addRef();
      void release();

      operator const char* () const      { return data; }
  };
}

#endif // ZDATA_H

