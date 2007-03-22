/* compident.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


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
