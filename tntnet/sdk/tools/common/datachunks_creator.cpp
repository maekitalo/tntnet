////////////////////////////////////////////////////////////////////////
// datachunks_creator.cpp
//

#include "tnt/datachunks_creator.h"

namespace tnt
{
  void datachunks_creator::create_chunks() const
  {
    unsigned offset = (chunks.size() + 1) * sizeof(unsigned);
    chunks_cache.append(reinterpret_cast<char*>(&offset), sizeof(unsigned));

    for (chunks_type::const_iterator it = chunks.begin();
         it != chunks.end(); ++it)
    {
      offset += it->size();
      chunks_cache.append(reinterpret_cast<char*>(&offset), sizeof(unsigned));
    }

    for (chunks_type::const_iterator it = chunks.begin();
         it != chunks.end(); ++it)
      chunks_cache.append(*it);
  }
}
