////////////////////////////////////////////////////////////////////////
// ecpplang.h
//

#ifndef ECPPLANG_H
#define ECPPLANG_H

#include <map>
#include <iosfwd>
#include <tnt/ecppParser.h>

class ecpplang : public tnt::ecppParser
{
    bool inLang;
    unsigned count;

    typedef std::map<unsigned, std::string> data_type;
    data_type data;

    bool lang;
    bool nolang;
    bool textformat;

  public:
    ecpplang()
      : inLang(false),
        count(0),
        lang(true),
        nolang(false),
        textformat(false)
      { }

    virtual void processHtml(const std::string& html);
    virtual void tokenSplit(bool start);

    void setLang(bool sw = true)
      { lang = sw; }
    void setNoLang(bool sw = true)
      { nolang = sw; }

    void setTextformat(bool sw = true)
      { textformat = sw; }

    void print(std::ostream& out) const;
};

#endif // ECPPLANG_H

