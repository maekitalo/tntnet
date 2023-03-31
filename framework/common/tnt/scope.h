/*
 * Copyright (C) 2005 Tommi Maekitalo
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


#ifndef TNT_SCOPE_H
#define TNT_SCOPE_H

#include <tnt/object.h>
#include <map>
#include <string>
#include <mutex>
#include <memory>

namespace tnt
{
class Scope
{
    std::map<std::string, std::unique_ptr<Object>> _data;
    mutable std::mutex _mutex;

    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;

public:
    Scope() = default;
    virtual ~Scope() { }

    void lock()   { _mutex.lock(); }
    void unlock() { _mutex.unlock(); }

    template <typename T> T*
    get(const std::string& key)
    {
        auto it = _data.find(key);
        return it == _data.end() ? 0 : it->second->cast<T>();
    }

    /// Put new Object in scope. If key already exists,
    /// it is replaced and old Object released.
    template <typename T>
    T* put(const std::string& key, std::unique_ptr<T>&& p)
    {
        return _data.emplace(key, std::move(p)).first->second.get();
    }

    bool empty() const                 { return _data.empty(); }
};
}

#endif // TNT_SCOPE_H
