////////////////////////////////////////////////////////////////////////
// ecpplang.cpp
//

#include "ecpplang.h"

#include <tnt/stringescaper.h>

#include <iostream>
#include <iterator>

void ecpplang::processHtml(const std::string& html)
{
  if (inLang && lang
    || !inLang && nolang)
    data[count] = html;
  ++count;
}

void ecpplang::tokenSplit(bool start)
{
  inLang = start;
}

void ecpplang::print(std::ostream& out) const
{
  for (data_type::const_iterator it = data.begin();
       it != data.end(); ++it)
  {
    if (!textformat)
      out << it->first << '\t';
    std::transform(
      it->second.begin(),
      it->second.end(),
      std::ostream_iterator<const char*>(out),
      stringescaper(false));
    out << '\n';
  }
}
