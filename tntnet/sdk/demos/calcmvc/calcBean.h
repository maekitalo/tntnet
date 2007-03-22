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


#ifndef calcBean_H
#define calcBean_H

class calcBean
{
    double arg1;
    double arg2;
    char op;
    double result;
    bool resultOk;

  public:
    calcBean()
      : arg1(0),
        arg2(0),
        op('+'),
        resultOk(false)
        { }

    void   set(double arg1, double arg2, char op);

    double getArg1() const    { return arg1; }
    double getArg2() const    { return arg2; }
    char   getOp() const      { return op; }
    bool   isResultOk() const { return resultOk; }
    double getResult() const  { return result; }
};

#endif // calcBean_H

