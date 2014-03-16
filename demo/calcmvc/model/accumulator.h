/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef MODEL_ACCUMULATOR_H
#define MODEL_ACCUMULATOR_H

namespace model
{
  class Accumulator
  {
      int _value;

    public:
      Accumulator()
        : _value(0)
      { }

      int value() const  { return _value; }

      void increment()   { ++_value; }
      void decrement()   { --_value; }
  };
}

#endif // MODEL_ACCUMULATOR_H

