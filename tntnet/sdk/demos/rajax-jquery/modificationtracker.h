/*
 * Copyright (C) 2008 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cxxtools/mutex.h>
#include <cxxtools/condition.h>
#include <cxxtools/clock.h>
#include <stdint.h>

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
        int64_t timeoutLeft = static_cast<int64_t>(timeoutMsec) - clock.stop().totalMSecs();
        if (timeoutLeft <= 0)
          break;

        _condition.wait(lock, timeoutLeft);
      }

      return ValueSerial(_value, _serial);
    }

};
