/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef COUNTER_H
#define COUNTER_H

// This is the data object of our small mvc application.
//
// The data objects held the state of our web application.
//
// Since our application has just one state - the counter value - the
// data object is trivial. Since it is that trivial, we inline everything
// and hence do not need a implementation file model/counter.cpp.
//
// Typical applications have more complex and many data objects.
//
class Counter
{
    int _value;

  public:
    Counter()
      : _value(0)
    { }

    int value() const
    { return _value; }

    void increment()
    { ++_value; }

    void decrement()
    { --_value; }

};

#endif // COUNTER_H

