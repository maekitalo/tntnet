////////////////////////////////////////////////////////////////////////
// static.h
//

#ifndef STATIC_H
#define STATIC_H

#include <tnt/component.h>
#include <tnt/tntconfig.h>

namespace tnt
{
  class urlmapper;
  class comploader;
}

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::component* create_static(const tnt::compident& ci,
    const tnt::urlmapper& um, tnt::comploader& cl);
  bool config_static(const tnt::tntconfig::name_type& key,
    const tnt::tntconfig::config_value_type& values);
}

namespace tntcomp
{
  //////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class staticcomp : public tnt::component
  {
      static std::string document_root;

    protected:
      virtual ~staticcomp() { };

    public:
      virtual unsigned operator() (tnt::httpRequest& request,
        tnt::httpReply& reply, query_params& qparam);
      virtual bool drop();

      static void setDocumentRoot(const std::string& s)
      { document_root = s; }
      static const std::string& getDocumentRoot()
      { return document_root; }
  };
}

#endif // STATIC_H

