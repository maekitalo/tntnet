////////////////////////////////////////////////////////////////////////
// ecpp.h
//

#ifndef ECPP_H
#define ECPP_H

#include <tnt/component.h>
#include <map>

namespace tnt
{
  class urlmapper;
  class comploader;
  class ecppSubComponent;

  //////////////////////////////////////////////////////////////////////
  // subcompident
  //
  struct subcompident : public tnt::compident
  {
      std::string subname;

      subcompident(const tnt::compident& ci, const std::string& s)
        : tnt::compident(ci),
          subname(s)
         { }

      explicit subcompident(const std::string& ident);
      std::string toString() const;
  };

  //////////////////////////////////////////////////////////////////////
  // ecppComponent
  //
  class ecppComponent : public component
  {
      friend class ecppSubComponent;

      compident myident;
      const urlmapper& rootmapper;
      comploader& loader;

      typedef std::map<std::string, ecppSubComponent*> subcomps_type;
      subcomps_type subcomps;

      virtual subcomps_type& getSubcomps()               { return subcomps; }
      virtual const subcomps_type& getSubcomps() const   { return subcomps; }

    protected:
      virtual ~ecppComponent();
      void registerSubComp(const std::string& name, ecppSubComponent* comp);

    public:
      ecppComponent(const compident& ci, const urlmapper& um, comploader& cl);

      component& fetchComp(const std::string& url) const;
      component& fetchComp(const compident& ci) const;

      ecppSubComponent& fetchSubComp(const std::string& sub) const;

      unsigned callComp(const std::string& url, httpRequest& request,
        httpReply& reply, query_params& qparam);

      const compident& getCompident() const  { return myident; }
  };

  //////////////////////////////////////////////////////////////////////
  // ecppSubComponent
  //
  class ecppSubComponent : public ecppComponent
  {
      ecppComponent& main;
      std::string subcompname;

      virtual subcomps_type& getSubcomps()
        { return main.getSubcomps(); }
      virtual const subcomps_type& getSubcomps() const
        { return main.getSubcomps(); }

    public:
      ecppSubComponent(ecppComponent& p, const std::string& name)
        : ecppComponent(p.myident, p.rootmapper, p.loader),
          main(p),
          subcompname(name)
        {
          p.registerSubComp(name, this);
        }

      virtual bool drop();
      subcompident getCompident() const
        { return subcompident(main.getCompident(), subcompname); }

      ecppComponent& getMainComponent() const { return main; }
  };

  inline unsigned ecppComponent::callComp(const std::string& url, httpRequest& request,
    httpReply& reply, query_params& qparam)
  {
    return fetchComp(url)(request, reply, qparam);
  }

}

namespace ecpp_component
{
  using tnt::component;
  using tnt::compident;
  using tnt::urlmapper;
  using tnt::comploader;
  using tnt::ecppComponent;
  using tnt::ecppSubComponent;
  using tnt::httpRequest;
  using tnt::httpReply;
}

#endif // ECPP_H

