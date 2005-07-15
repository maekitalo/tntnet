////////////////////////////////////////////////////////////////////////
// 
//

#ifndef TNT_HTMLESCOSTREAM_H
#define TNT_HTMLESCOSTREAM_H

#include <iostream>

namespace tnt
{
  class HtmlEscStreamBuf : public std::streambuf
  {
      std::streambuf* sink;

      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();

    public:
      HtmlEscStreamBuf(std::streambuf* sink_)
        : sink(sink_)
        { }

      void setSink(std::streambuf* sink_)
        { sink = sink_; }
  };

  class HtmlEscOstream : public std::ostream
  {
      HtmlEscStreamBuf streambuf;

    public:
      HtmlEscOstream(std::ostream& sink)
        : std::ostream(&streambuf),
          streambuf(sink.rdbuf())
        { }
      HtmlEscOstream(std::streambuf* sink)
        : std::ostream(&streambuf),
          streambuf(sink)
        { }

      void setSink(std::ostream& sink)
        { streambuf.setSink(sink.rdbuf()); }
      void setSink(std::streambuf* sink)
        { streambuf.setSink(sink); }
  };
}

#endif // TNT_HTMLESCOSTREAM_H

