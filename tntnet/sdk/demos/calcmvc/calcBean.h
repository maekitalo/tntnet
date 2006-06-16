////////////////////////////////////////////////////////////////////////
//
//

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

