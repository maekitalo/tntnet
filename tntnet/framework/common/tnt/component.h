/* tnt/component.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <time.h>
#include <iostream>
#include <limits>

namespace cxxtools
{
  class query_params;
}

namespace tnt
{

class httpRequest;
class httpReply;

struct compident
{
  std::string libname;
  std::string compname;

  bool operator< (const compident& ci) const
  {
    return libname < ci.libname
      || libname == ci.libname && compname < ci.compname;
  }

  compident() { }
  compident(const std::string& l, const std::string& n)
    : libname(l),
      compname(n)
  { }

  explicit compident(const std::string& ident);

  std::string toString() const
  { return compname + '@' + libname; }

  bool empty() const
    { return libname.empty() && compname.empty(); }
  void clear()
    { libname.clear(); compname.clear(); }
};

inline std::ostream& operator<< (std::ostream& out, const compident& comp)
{ return out << comp.toString(); }

class component
{
    time_t atime;

  protected:
    virtual ~component() { }

  public:
    component()     { time(&atime); }

    void touch(time_t plus = 0)      { time(&atime); atime += plus; }
    time_t getLastAccesstime() const { return atime; }
    void lock()                      { atime = std::numeric_limits<time_t>::max(); }
    void unlock()                    { touch(); }

    virtual unsigned operator() (httpRequest& request,
      httpReply& reply, cxxtools::query_params& qparam);
    virtual bool drop() = 0;

    virtual std::string getAttribute(const std::string& name,
      const std::string& def = std::string()) const;

    virtual unsigned getDataCount(const httpRequest& request) const;
    virtual unsigned getDataLen(const httpRequest& request, unsigned n) const;
    virtual const char* getDataPtr(const httpRequest& request, unsigned n) const;
    std::string getData(const httpRequest& r, unsigned n) const
      { return std::string(getDataPtr(r, n), getDataLen(r, n)); }
};

}

#endif // COMPONENT_H
