////////////////////////////////////////////////////////////////////////
// messageheader - Standard-header like rfc822
//

#ifndef MESSAGEHEADER_H
#define MESSAGEHEADER_H

#include <string>
#include <map>
#include <iosfwd>

namespace tnt
{
  /// Standard-message-header like rfc822
  class messageheader : public std::multimap<std::string, std::string>
  {
    protected:
      enum return_type
      {
        OK,
        FAIL,
        END
      };

    protected:
      virtual return_type onField(const std::string& name, const std::string& value);

    public:
      messageheader()
      { }
      virtual ~messageheader()
      { }

      void parse(std::istream& in);
  };

  std::istream& operator>> (std::istream& in, messageheader& data);
}

#endif // MESSAGEHEADER_H

