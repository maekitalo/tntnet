////////////////////////////////////////////////////////////////////////
// calc.h
// generated with makerequest
//

#ifndef ECPP_COMPONENT_calc_H
#define ECPP_COMPONENT_calc_H

#include <tnt/ecpp.h>


// <%pre>


using namespace std;

// </%pre>

namespace ecpp_component
{
extern "C"
{
  component* create_calc(const compident& ci, const urlmapper& um,
    comploader& cl);
}

// <%declare_shared>
// </%declare_shared>
class calc : public ecppComponent
{
    friend component* create_calc(const compident& ci,
      const urlmapper& um, comploader& cl);
    // <%declare>

    // </%declare>

  protected:
    calc(const compident& ci, const urlmapper& um, comploader& cl);
    ~calc();

  public:
    unsigned operator() (const httpRequest& request, httpReply& reply, query_params& qparam);
    bool drop();
    unsigned    getDataCount() const;
    unsigned    getDataLen(unsigned n) const;
    const char* getDataPtr(unsigned n) const;
};

}; // namespace ecpp_component

#endif
