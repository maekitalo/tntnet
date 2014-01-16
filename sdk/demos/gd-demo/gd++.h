/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef GDPP_H
#define GDPP_H

#include <gd.h>
#include <iosfwd>

namespace gd
{
  class Gd
  {
      gdImagePtr _imagePtr;

      // disable copy and assignment
      Gd(const Gd&);
      const Gd& operator=(const Gd&);

    public:
      Gd(int x, int y, bool trueColor = false);
      ~Gd();

      int colorAllocate(int r, int g, int b);
      void setThickness(int thickness);
      void setAntiAliased(int color);
      void line(int x1, int y1, int x2, int y2, int color);
      void filledRectangle(int x1, int y1, int x2, int y2, int color);
      void arc(int cx, int cy, int w, int h, int s, int e, int color);

      enum FontStyle {
        FontTiny,
        FontSmall,
        FontMedium,
        FontLarge,
        FontGiant
      };

      void drawChar(int x, int y, char ch, int color, FontStyle style = FontMedium);
      void drawString(int x, int y, const char* str, int color, FontStyle style = FontMedium);

      void printPng(std::ostream& out) const;
  };

  inline std::ostream& operator<< (std::ostream& out, const Gd& img)
  { img.printPng(out); return out; }
}

#endif // GDPP_H

