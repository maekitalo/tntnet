////////////////////////////////////////////////////////////////////////
// stringescaper.h
//

#ifndef STRINGESCAPER_H
#define STRINGESCAPER_H

#include <functional>
#include <string>

class stringescaper : public std::unary_function<const char*, char>
{
    bool escQuote;
    char data[5];

  public:
    stringescaper(bool escQuote_ = true)
      : escQuote(escQuote_)
    { }

    const char* operator() (char ch)
    {
      if (ch == '\n')
        strcpy(data, "\\n");
      else if (ch == '\t')
        strcpy(data, "\\t");
      else if (ch == '?')
        strcpy(data, "\\?");
      else if (escQuote && ch == '"')
        strcpy(data, "\\\"");
      else if (std::isprint(ch) && ch != '\\')
      {
        data[0] = ch;
        data[1] = '\0';
      }
      else
      {
        data[0] = '\\';
        data[1] = (char)(((unsigned char)ch >> 6) + '0');
        data[2] = (char)((((unsigned char)ch >> 3) & 0x7) + '0');
        data[3] = (char)(((unsigned char)ch & 0x7) + '0');
        data[4] = '\0';
      }

      return data;
    }
};

#endif // STRINGESCAPER_H

