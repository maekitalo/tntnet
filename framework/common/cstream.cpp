#include <tnt/cstream.h>

namespace tnt
{

cstreambuf::~cstreambuf()
{
  for (size_type n = 0; n < _chunks.size(); ++n)
    delete[] _chunks[n];
}

void cstreambuf::makeEmpty()
{
  if (_chunks.size() > 0)
  {
    if (_chunks.size() > 1)
    {
      for (size_type n = 1; n < _chunks.size(); ++n)
        delete[] _chunks[n];
      _chunks.resize(1);
    }

    setp(_chunks[0], _chunks[0] + _chunksize);
  }
}

std::streambuf::int_type cstreambuf::overflow(std::streambuf::int_type ch)
{
  char* chunk = new char[_chunksize];
  _chunks.push_back(chunk);
  setp(_chunks.back(), _chunks.back() + _chunksize);

  if (ch != traits_type::eof())
    sputc(traits_type::to_char_type(ch));

  return 0;
}

std::streambuf::int_type cstreambuf::underflow()
{
  return traits_type::eof();
}

int cstreambuf::sync()
{
  return 0;
}

void cstreambuf::rollback(size_type n)
{
  if (n == 0)
  {
    makeEmpty();
  }
  else
  {
    size_type c = (n-1) / _chunksize;

    for (size_type n = c + 1; n < _chunks.size(); ++n)
      delete[] _chunks[n];

    _chunks.resize(c + 1);

    setp(_chunks[c], _chunks[c] + _chunksize);
    pbump(n % _chunksize);
  }
}

std::string ocstream::str() const
{
  std::string ret;
  ret.reserve(size());
  for (unsigned n = 0; n < chunkcount(); ++n)
    ret.append(chunk(n), chunksize(n));
  return ret;
}

void ocstream::output(std::ostream& out) const
{
  for (unsigned n = 0; n < chunkcount(); ++n)
    out.write(chunk(n), chunksize(n));
}

}
