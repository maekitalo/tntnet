////////////////////////////////////////////////////////////////////////
// tnt/datachunks_creator.h
//

#ifndef TNT_DATACHUNKS_CREATOR_H
#define TNT_DATACHUNKS_CREATOR_H

#include <list>
#include <string>

namespace tnt
{
  class datachunks_creator
  {
      typedef std::list<std::string> chunks_type;
      chunks_type chunks;
      mutable std::string chunks_cache;

      void create_chunks() const;

    public:
      datachunks_creator()
      { }

      void push_back(const std::string& data)
        { chunks.push_back(data); chunks_cache.clear(); }
      void push_back(const char* data, unsigned len)
        { push_back(std::string(data, len)); }

      const char* ptr() const
      {
        if (chunks_cache.empty())
          create_chunks();
        return chunks_cache.data();
      }

      unsigned count() const
      { return chunks.size(); }

      unsigned size() const
      {
        if (chunks_cache.empty())
          create_chunks();
        return chunks_cache.size();
      }

      bool empty() const
      { return chunks.empty(); }

      operator const char*() const
      { return ptr(); }
  };
}

#endif // TNT_DATACHUNKS_CREATOR_H

