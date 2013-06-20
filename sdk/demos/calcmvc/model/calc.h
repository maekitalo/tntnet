/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef MODEL_CALC_H
#define MODEL_CALC_H

namespace model
{
  class Calc
  {
      double _arg1;
      double _arg2;
      char   _op;
      double _result;
      bool   _resultOk;

    public:
      Calc()
        : _arg1(0),
          _arg2(0),
          _resultOk(false)
        { }

      void arg1(double v)      { _arg1 = v; }
      void arg2(double v)      { _arg2 = v; }
      void op(char v)          { _op = v; }
      void result(double v)    { _result = v; }
      void resultOk(bool v)    { _resultOk = v; }

      double arg1() const      { return _arg1; }
      double arg2() const      { return _arg2; }
      char   op() const        { return _op; }
      double result() const    { return _result; }
      bool   resultOk() const  { return _resultOk; }
  };
}

#endif // MODEL_CALC_H

