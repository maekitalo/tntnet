/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#include <tntwiki/Renderer.h>
#include <cxxtools/regex.h>
#include <iostream>

namespace tntwiki
{
  void Renderer::putHtml(std::ostream& out, const std::string& data)
  {
    // TODO parse markup language
    static cxxtools::Regex h1("== ([^=]+) ==");
    static cxxtools::Regex h2("=== ([^=]+) ===");
    static cxxtools::Regex italic("''([^']*)''");
    static cxxtools::Regex bold("'''([^']*)'''");

    std::string data1 = data;
    std::string data2;

    cxxtools::RegexSMatch m;

    while (h2.match(data1, m))
    {
      data2 += data1.substr(0, m.offsetBegin(0));
      data2 += "<h2>";
      data2 += m.get(1);
      data2 += "</h2>";
      data1.erase(0, m.offsetEnd(0));
    }
    data1 = data2 + data1;
    data2.clear();

    while (h1.match(data1, m))
    {
      data2 += data1.substr(0, m.offsetBegin(0));
      data2 += "<h1>";
      data2 += m.get(1);
      data2 += "</h1>";
      data1.erase(0, m.offsetEnd(0));
    }
    data1 = data2 + data1;
    data2.clear();

    while (bold.match(data1, m))
    {
      data2 += data1.substr(0, m.offsetBegin(0));
      data2 += "<b>";
      data2 += m.get(1);
      data2 += "</b>";
      data1.erase(0, m.offsetEnd(0));
    }
    data1 = data2 + data1;
    data2.clear();

    while (italic.match(data1, m))
    {
      data2 += data1.substr(0, m.offsetBegin(0));
      data2 += "<i>";
      data2 += m.get(1);
      data2 += "</i>";
      data1.erase(0, m.offsetEnd(0));
    }
    data1 = data2 + data1;
    data2.clear();

    out << data1;
  }
}
