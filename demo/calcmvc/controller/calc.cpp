/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <model/calc.h>

namespace controller
{
  class Calc : public tnt::Component
  {
    public:
      unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
  };

  static tnt::ComponentFactoryImpl<Calc> factory("controller/calc");

  unsigned Calc::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
  {
    TNT_REQUEST_SHARED_VAR(model::Calc, calc, ());

    calc.arg1(qparam.get<double>("arg1"));
    calc.arg2(qparam.get<double>("arg2"));
    calc.op(qparam.get<char>("op"));

    calc.resultOk(true);
    switch (calc.op())
    {
      case '+':
        calc.result(calc.arg1() + calc.arg2());
        break;

      case '-':
        calc.result(calc.arg1() - calc.arg2());
        break;

      case '*':
        calc.result(calc.arg1() * calc.arg2());
        break;

      case '/':
        if (calc.arg2() == 0)
          calc.resultOk(false);
        else
          calc.result(calc.arg1() / calc.arg2());
        break;

      default:
        calc.resultOk(false);
    }

    return DECLINED;
  }
}
