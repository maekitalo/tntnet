////////////////////////////////////////////////////////////////////////
// component.cpp
//

#include "tnt/component.h"
#include "tnt/http.h"

namespace tnt
{
  unsigned component::operator() (const httpRequest& request,
    httpReply& reply, query_params& qparam)
  {
    return DECLINED;
  }

  std::string component::getAttribute(const std::string& name,
    const std::string& def) const
  {
    return def;
  }

  unsigned component::getDataCount() const
  { return 0; }

  unsigned component::getDataLen(unsigned n) const
  { return 0; }

  const char* component::getDataPtr(unsigned n) const
  { return 0; }
}
