#include "calcBean.h"
#include <cxxtools/log.h>

log_define("calcBean")

void calcBean::set(double arg1_, double arg2_, char op_)
{
  log_debug("calcBean::set(" << arg1_ << ", " << arg2_ << ", " << op_ << ")");

  arg1 = arg1_;
  arg2 = arg2_;
  op = op_;

  resultOk = true;
  switch (op)
  {
    case '+': result = arg1 + arg2; break;
    case '-': result = arg1 - arg2; break;
    case '*': result = arg1 * arg2; break;
    case '/': result = arg1 / arg2; break;
    default: resultOk = false;
  }

  log_debug("calcBean::set: result_ok = " << resultOk);
}
