////////////////////////////////////////////////////////////////////////
// contenttype.cpp
//

#include <tnt/contenttype.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctype.h>
#include <functional>

namespace
{
  bool istokenchar(char ch)
  {
    return ch >= 33
        && ch <= 126
        && ch != '(' && ch != ')' && ch != '<' && ch != '>'  && ch != '@'
        && ch != ',' && ch != ';' && ch != ':' && ch != '\\' && ch != '"'
        && ch != '/' && ch != '[' && ch != ']' && ch != '?'  && ch != '=';
  }

  bool isblank(char ch)
  {
    return ch == ' ' || ch == '\t';
  }
}

namespace tnt
{
  contenttype::contenttype(const std::string& ct)
  {
    std::istringstream in(ct);

    in >> *this;
    if (!in)
    {
      std::ostringstream msg;
      msg << "error 1 parsing content-type-header at "
          << in.tellg()
          << ": "
          << ct;
      throw std::runtime_error(msg.str());
    }

    if (in.get() != std::ios::traits_type::eof())
    {
      std::ostringstream msg;
      msg << "error 2 parsing content-type-header at "
          << in.tellg()
          << ": "
          << ct;
      throw std::runtime_error(msg.str());
    }
  }

  contenttype::return_type contenttype::onType(
    const std::string& t, const std::string& s)
  {
    if (s.empty())
      return FAIL;

    type = t;
    subtype = s;

    std::transform(type.begin(), type.end(), type.begin(),
      std::ptr_fun(tolower));
    std::transform(subtype.begin(), subtype.end(), subtype.begin(),
      std::ptr_fun(tolower));

    return OK;
  }

  contenttype::return_type contenttype::onParameter(
    const std::string& attribute, const std::string& value)
  {
    std::string att = attribute;
    std::transform(att.begin(), att.end(), att.begin(),
      std::ptr_fun(tolower));

    parameter.insert(parameter_type::value_type(att, value));
    if (attribute == "boundary")
      boundary = value;
    return OK;
  }
}
