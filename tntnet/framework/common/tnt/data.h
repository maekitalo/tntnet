////////////////////////////////////////////////////////////////////////
// data.h
//

#ifndef DATA_H
#define DATA_H

#include <iostream>

namespace tnt
{
  class raw_data
  {
      const char* data;
      const unsigned data_len;

    public:
      raw_data(const char* data_, unsigned data_len_)
        : data(data_),
          data_len(data_len_)
      { }

      operator const char* () const      { return data; }
  };

  class data_chunk
  {
      const char* data;
      unsigned len;

    public:
      data_chunk(const char* data_, const char* end_)
        : data(data_),
          len(end_ - data_)
      { }
      data_chunk(const char* data_, unsigned len_)
        : data(data_),
          len(len_)
      { }

      const char* getData() const  { return data; }
      unsigned getLength() const   { return len; }
  };

  //////////////////////////////////////////////////////////////////////
  // interpretes raw data as a data-structure with chunks
  //   unsigned: number of chunks
  //   count X unsigned: offsets
  //   data
  //
  template <class data_ptr>
  class data_chunks
  {
      data_ptr& data_object;

      const unsigned* udata() const
      {
        const char* d = data_object;
        return reinterpret_cast<const unsigned*>(d);
      }

    public:
      data_chunks(data_ptr& d)
        : data_object(d)
      { }

      const char* data() const
      {
        return reinterpret_cast<const char*>(udata()[0]);
      }

      // Anzahl der chunks
      unsigned size() const
      { return (udata()[0] / sizeof(unsigned)) - 1; }

      // Offset des n-ten chunks vom Start des Datenbereiches
      unsigned offset(unsigned n) const
      { return udata()[n] - udata()[0]; }

      // Grösse des n-ten chunks in Byte
      unsigned size(unsigned n) const
      { return udata()[n + 1] - udata()[n]; }

      // Zeiger auf das n-te chunk
      const char* ptr(unsigned n) const
      {
        const char* d = data_object;
        return d + udata()[n];
      }

      data_chunk operator[] (unsigned n) const
      { return data_chunk(ptr(n), size(n)); }
  };

  inline std::ostream& operator << (std::ostream& out, const data_chunk& c)
  {
    out.write(c.getData(), c.getLength());
    return out;
  }

}

#endif // DATA_H

