////////////////////////////////////////////////////////////////////////
// tnt/messageattribute.h
//

#ifndef TNT_MESSAGEATTRIBUTE_H
#define TNT_MESSAGEATTRIBUTE_H

#include <string>
#include <iosfwd>

namespace tnt
{
  /// Messageattribute like form-data; name="my-upload-field"; filename="ttt.sh"
  ///                    or application/x-shellscript
  /// see also rfc2045
  class messageattribute_parser
  {
    public:
      enum return_type
      {
        OK,
        FAIL,
        END
      };

    private:
      virtual return_type onType(const std::string& type,
        const std::string& subtype) = 0;
      virtual return_type onParameter(const std::string& attribute,
        const std::string& value) = 0;

    public:
      virtual ~messageattribute_parser()
        { }
      void parse(std::istream& in);
  };
}

#endif // TNT_MESSAGEATTRIBUTE_H

