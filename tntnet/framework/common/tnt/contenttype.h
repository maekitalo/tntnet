////////////////////////////////////////////////////////////////////////
// contenttype.h
//

#ifndef CONTENTTYPE_H
#define CONTENTTYPE_H

#include <tnt/messageattribute.h>
#include <map>

/// Content-type-field like rfc2045
namespace tnt
{
  class contenttype : public messageattribute_parser
  {
    public:
      typedef std::multimap<std::string, std::string> parameter_type;
      typedef parameter_type::const_iterator parameter_iterator;

    private:
      virtual return_type onType(const std::string& type, const std::string& subtype);
      virtual return_type onParameter(const std::string& attribute, const std::string& value);

      std::string type;
      std::string subtype;
      parameter_type parameter;
      std::string boundary;

    public:
      contenttype()
      { }

      explicit contenttype(const std::string& ct);

      const std::string& getType() const     { return type; }
      const std::string& getSubtype() const  { return subtype; }
      const std::string& getBoundary() const { return boundary; }
      bool isMultipart() const
        { return type == "multipart" && !boundary.empty(); }

      parameter_iterator parameter_begin() const
        { return parameter.begin(); }
      parameter_iterator parameter_end() const
        { return parameter.end(); }
      parameter_iterator parameter_find(parameter_type::key_type key) const
        { return parameter.find(key); }
      parameter_iterator parameter_upper_bound(parameter_type::key_type key) const
        { return parameter.upper_bound(key); }
  };

  inline std::istream& operator>> (std::istream& in, contenttype& ct)
  {
    ct.parse(in);
    return in;
  }
}

#endif // CONTENTTYPE_H

