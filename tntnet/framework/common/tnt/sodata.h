////////////////////////////////////////////////////////////////////////
// tnt/sodata.h
//

#ifndef TNT_SODATA_H
#define TNT_SODATA_H

#include <string>

namespace tnt
{
  class compident;

  //////////////////////////////////////////////////////////////////////
  // sodata stellt eine Klasse dar, die Komponentendaten aus einer
  // shared-library lädt. Die shared-library hat folgende exportierte
  // Datenstrukturen:
  //
  //   bei unkomprimierten Daten:
  //     - const char* (compname)_data
  //     - unsigned (compname)_datalen
  //
  //   bei komprimierten Daten:
  //     - const char* (compname)_zdata
  //     - unsigned (compname)_zdatalen
  //     - unsigned (compname)_datalen
  //
  class sodata
  {
      std::string sosuffix;
      unsigned refs;
      char* data;

    public:
      sodata(const std::string& sosuffix_ = std::string())
        : sosuffix(sosuffix_),
          refs(0),
          data(0)
      { }
      ~sodata()
      { delete data; }

      void setSoSuffix(const std::string& sosuffix_)
      { sosuffix = sosuffix_; }
      void setLangSuffix();
      const std::string& getSoSuffix() const
      { return sosuffix; }

      void addRef(const compident& ci);
      void release();

      operator const char* () const      { return data; }
  };
}

#endif // TNT_SODATA_H

