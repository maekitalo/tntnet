////////////////////////////////////////////////////////////////////////
// component.cpp
//

#include "tnt/component.h"
#include "tnt/http.h"

namespace tnt
{
  compident::compident(const std::string& ident)
  {
    std::string::size_type pos = ident.find('@');
    if (pos == std::string::npos)
      compname = ident;
    else
    {
      compname = ident.substr(0, pos);
      libname = ident.substr(pos + 1);
    }
  }

  unsigned component::operator() (httpRequest& request,
    httpReply& reply, query_params& qparam)
  {
    return DECLINED;
  }

  std::string component::getAttribute(const std::string& name,
    const std::string& def) const
  {
    return def;
  }

  unsigned component::getDataCount(const httpRequest& request) const
  { return 0; }

  unsigned component::getDataLen(const httpRequest& request, unsigned n) const
  { return 0; }

  const char* component::getDataPtr(const httpRequest& request, unsigned n) const
  { return 0; }
}
