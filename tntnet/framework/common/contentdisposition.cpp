////////////////////////////////////////////////////////////////////////
// contentdisposition.h
//

#include <tnt/contentdisposition.h>
#include <ctype.h>
#include <functional>

namespace tnt
{
  contentdisposition::return_type contentdisposition::onType(
    const std::string& t,
    const std::string& subtype)
  {
    if (!subtype.empty())
      return FAIL;

    type = t;
    std::transform(type.begin(), type.end(), type.begin(),
      std::ptr_fun(tolower));

    return OK;
  }

  contentdisposition::return_type contentdisposition::onParameter(
    const std::string& attribute,
    const std::string& value)
  {
    if (attribute == "name")
    {
      name = value;
      std::transform(name.begin(), name.end(), name.begin(),
        std::ptr_fun(tolower));
    }
    else if (attribute == "filename")
      filename = value;

    return OK;
  }
}
