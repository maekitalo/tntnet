////////////////////////////////////////////////////////////////////////
// ecpp.h
//

#ifndef ECPP_H
#define ECPP_H

#include <tnt/component.h>

namespace tnt
{
class urlmapper;
class comploader;

class ecppComponent : public component
{
    compident myident;
    const urlmapper& rootmapper;
    comploader& loader;

  protected:
    virtual ~ecppComponent();

  public:
    ecppComponent(const compident& ci, const urlmapper& um, comploader& cl);

    component& fetchComp(const std::string& url) const;
    component& fetchComp(const compident& ci) const;
    unsigned callComp(const std::string& url, httpRequest& request,
      httpReply& reply, query_params& qparam)
    { return fetchComp(url)(request, reply, qparam); }
    const compident& getCompident() const  { return myident; }
};
}

namespace ecpp_component
{
  using tnt::component;
  using tnt::compident;
  using tnt::urlmapper;
  using tnt::comploader;
  using tnt::ecppComponent;
  using tnt::httpRequest;
  using tnt::httpReply;
}

#endif // ECPP_H

