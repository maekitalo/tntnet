////////////////////////////////////////////////////////////////////////
// tnt/scope.h
//

#ifndef TNT_SCOPE_H
#define TNT_SCOPE_H

#include <map>
#include <string>
#include <tnt/object.h>
#include <cxxtools/thread.h>

namespace tnt
{
  class scope
  {
      typedef std::map<std::string, object*> container_type;
      container_type data;
      mutable cxxtools::Mutex mutex;
      unsigned refs;

    public:
      scope() : refs(1) {}
      scope(const scope& src);
      ~scope();

      typedef cxxtools::MutexLock lock_type;
      cxxtools::Mutex& getMutex()  { return mutex; }

      unsigned addRef()  { return ++refs; }
      unsigned release();

      scope& operator=(const scope& src);

      bool has(const std::string& key) const
        { return data.find(key) != data.end(); }

      object* get(const std::string& key) const;

      /// Put new object in scope. If key already exists,
      /// it is replaced and old object released.
      void replace(const std::string& key, object* o);

      /// Put new object in scope. If key already exists,
      /// o is released. Returns object identified by key.
      object* putNew(const std::string& key, object* o);

      void erase(const std::string& key);
      bool empty() const  { return data.empty(); }
      void clear()        { data.clear(); }
  };
}

#endif // TNT_SCOPE_H

