////////////////////////////////////////////////////////////////////////
// tnt/contentdisposition.h
//

#ifndef TNT_CONTENTDISPOSITION_H
#define TNT_CONTENTDISPOSITION_H

#include <tnt/messageattribute.h>

namespace tnt
{
  /// Content-Disposition-Header.
  /// (Content-Disposition: form-data; name="mein-upload-feld"; filename="ttt.sh")
  class contentdisposition : public messageattribute_parser
  {
      std::string type;
      std::string name;
      std::string filename;

      virtual return_type onType(const std::string& type,
        const std::string& subtype);
      virtual return_type onParameter(const std::string& attribute,
        const std::string& value);

    public:
      const std::string& getType() const      { return type; }
      const std::string& getName() const      { return name; }
      const std::string& getFilename() const  { return filename; }
  };

  inline std::istream& operator>> (std::istream& in, contentdisposition& ct)
  {
    ct.parse(in);
    return in;
  }
}

#endif // TNT_CONTENTDISPOSITION_H

