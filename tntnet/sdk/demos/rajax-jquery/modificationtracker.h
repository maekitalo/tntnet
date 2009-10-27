/*
 * Copyright (C) 2008 Tommi Maekitalo
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

#include <cxxtools/mutex.h>
#include <cxxtools/condition.h>
#include <cxxtools/clock.h>

template <typename T>
class ModificationTracker
{
    T _value;
    unsigned _serial;
    mutable cxxtools::Mutex _mutex;
    mutable cxxtools::Condition _condition;

  public:
    typedef std::pair<T, unsigned> ValueSerial;

    ModificationTracker()
      : _serial(0)
    { }

    /// set value
    void set(const T& value)
    {
      cxxtools::MutexLock lock(_mutex);
      _value = value;
      ++_serial;
      _condition.broadcast();
    }

    /// get value and serial number of value
    ValueSerial get() const
    {
      cxxtools::MutexLock lock(_mutex);
      return ValueSerial(_value, _serial);
    }

    /// wait value change up to specified timeout
    ValueSerial get(unsigned serial, unsigned timeoutMsec) const
    {
      cxxtools::MutexLock lock(_mutex);
      cxxtools::Clock clock;
      clock.start();
      while (_serial <= serial)
      {
        cxxtools::int64_t timeoutLeft = static_cast<cxxtools::int64_t>(timeoutMsec) - clock.stop().totalMSecs();
        if (timeoutLeft <= 0)
          break;

        _condition.wait(lock, timeoutLeft);
      }

      return ValueSerial(_value, _serial);
    }

};
