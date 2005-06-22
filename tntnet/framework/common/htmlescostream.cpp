#include "tnt/htmlescostream.h"

namespace tnt
{
  std::streambuf::int_type HtmlEscStreamBuf::overflow(std::streambuf::int_type ch)
  {
    switch (ch)
    {
      case '<': return sink->sputn("&lt;", 4);
      case '>': return sink->sputn("&gt;", 4);
      case '"': return sink->sputn("&quot;", 6);
      case '\'': return sink->sputn("&#39;", 5);
      default: return sink->sputc(ch);
    }
  }

  std::streambuf::int_type HtmlEscStreamBuf::underflow()
  {
    return traits_type::eof();
  }

  std::streambuf::int_type HtmlEscStreamBuf::sync()
  {
    return sink->pubsync();
  }

}
