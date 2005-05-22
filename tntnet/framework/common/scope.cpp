////////////////////////////////////////////////////////////////////////
// tnt/scope.cpp
//

#include <tnt/scope.h>
#include <cxxtools/thread.h>

namespace tnt
{
  scope::scope(const scope& src)
    : data(src.data),
      refs(1)
  {
    for (container_type::iterator it = data.begin(); it != data.end(); ++it)
      it->second->addRef();
  }

  scope& scope::operator=(const scope& src)
  {
    for (container_type::iterator it = data.begin(); it != data.end(); ++it)
      it->second->release();

    data = src.data;

    for (container_type::iterator it = data.begin(); it != data.end(); ++it)
      it->second->addRef();

    return *this;
  }

  scope::~scope()
  {
    for (container_type::iterator it = data.begin(); it != data.end(); ++it)
      it->second->release();
  }

  unsigned scope::release()
  {
    if (--refs == 0)
    {
      delete this;
      return 0;
    }
    else
      return refs;
  }

  object* scope::get(const std::string& key) const
  {
    cxxtools::MutexLock lock(mutex);
    container_type::const_iterator it = data.find(key);
    return it == data.end() ? 0 : it->second;
  }

  void scope::replace(const std::string& key, object* o)
  {
    o->addRef();
    cxxtools::MutexLock lock(mutex);
    container_type::iterator it = data.find(key);
    if (it == data.end())
      data.insert(container_type::value_type(key, o));
    else
    {
      it->second->release();
      it->second = o;
    }
  }

  object* scope::putNew(const std::string& key, object* o)
  {
    cxxtools::MutexLock lock(mutex);
    container_type::iterator it = data.find(key);
    if (it == data.end())
    {
      data.insert(container_type::value_type(key, o));
      return o;
    }
    else
    {
      o->release();
      return it->second;
    }
  }

  void scope::erase(const std::string& key)
  {
    cxxtools::MutexLock lock(mutex);
    container_type::iterator it = data.find(key);
    if (it != data.end())
    {
      it->second->release();
      data.erase(it);
    }
  }
}
