/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef TNTWIKI_RENDERER_H
#define TNTWIKI_RENDERER_H

#include <tntdb/connection.h>
#include <tntwiki/PageManager.h>
#include <iosfwd>

namespace tntwiki
{
  class Renderer
  {
    public:
      explicit Renderer(tntdb::Connection conn)
        : _pageManager(conn)
      { }

      void putHtml(std::ostream& out, const std::string& data);

    private:
      PageManager _pageManager;
  };
}

#endif // TNTWIKI_RENDERER_H

