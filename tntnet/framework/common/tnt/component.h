////////////////////////////////////////////////////////////////////////
// component.h
//

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <time.h>
#include <iostream>
#include <limits>

class query_params;

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
      httpReply& reply, query_params& qparam);
    virtual bool drop() = 0;

    virtual std::string getAttribute(const std::string& name,
      const std::string& def = std::string()) const;

    virtual unsigned getDataCount() const;
    virtual unsigned getDataLen(unsigned n) const;
    virtual const char* getDataPtr(unsigned n) const;
    std::string getData(unsigned n) const
      { return std::string(getDataPtr(n), getDataLen(n)); }
};

}

#endif // COMPONENT_H
