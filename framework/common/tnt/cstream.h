#ifndef TNT_CSTREAM
#define TNT_CSTREAM

#include <iostream>
#include <vector>

namespace tnt
{
class cstreambuf : public std::streambuf
{
    typedef std::vector<char*> _chunks_type;
    unsigned _chunksize;
    _chunks_type _chunks;

  public:
    typedef _chunks_type::size_type size_type;

    explicit cstreambuf(unsigned chunksize = 32768)
    : _chunksize(chunksize)
    { }

    ~cstreambuf();

    size_type chunkcount() const
    { return _chunks.size(); }

    size_type size() const
    { return _chunks.size() == 0 ? 0
           : (_chunks.size() - 1) * _chunksize + pptr() - _chunks.back(); }

    size_type chunksize(size_type n) const
    {
      return _chunks.size() == 0 ? 0
           : n + 1  < _chunks.size() ? _chunksize
           : n + 1 == _chunks.size() ? static_cast<size_type>(pptr() - _chunks.back())
           : 0;
    }

    const char* chunk(size_type n) const
    { return _chunks[n]; }

    void rollback(size_type n);

    void makeEmpty();

  private:
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    int sync();
};

class ocstream : public std::ostream
{
    cstreambuf _streambuf;

  public:
    typedef cstreambuf::size_type size_type;

    explicit ocstream(unsigned chunksize = 32768)
      : std::ostream(0),
        _streambuf(chunksize)
    {
      init(&_streambuf);
    }

    size_type chunkcount() const
    { return _streambuf.chunkcount(); }

    const char* chunk(size_type n) const
    { return _streambuf.chunk(n); }

    size_type chunksize(size_type n) const
    { return _streambuf.chunksize(n); }

    size_type size() const
    { return _streambuf.size(); }

    void rollback(size_type n)
    { _streambuf.rollback(n); }

    void makeEmpty()
    { _streambuf.makeEmpty(); }

    std::string str() const;

    void output(std::ostream& out) const;
};

}

#endif // TNT_CSTREAM
