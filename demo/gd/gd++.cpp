/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#include <gd++.h>
#include <gdfontt.h>
#include <gdfonts.h>
#include <gdfontmb.h>
#include <gdfontl.h>
#include <gdfontg.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace gd
{
Gd::Gd(int x, int y, bool trueColor)
  : _imagePtr(0)
{
  if (trueColor)
    _imagePtr = gdImageCreateTrueColor(x, y);
  else
    _imagePtr = gdImageCreate(x, y);
}

Gd::~Gd()
{
  if (_imagePtr)
    gdImageDestroy(_imagePtr);
}

int Gd::colorAllocate(int r, int g, int b)
{
  int ret = gdImageColorAllocate(_imagePtr, r, g, b);
  if (ret == -1)
  {
    std::ostringstream msg;
    msg << "failed to allocate new color (" << r << ", " << g << ", " << b << ')';
    throw std::runtime_error(msg.str());
  }

  return ret;
}

void Gd::setThickness(int thickness)
{
  gdImageSetThickness(_imagePtr, thickness);
}

void Gd::setAntiAliased(int color)
{
  gdImageSetAntiAliased(_imagePtr, color);
}

void Gd::line(int x1, int y1, int x2, int y2, int color)
{
  gdImageLine(_imagePtr, x1, y1, x2, y2, color);
}

void Gd::filledRectangle(int x1, int y1, int x2, int y2, int color)
{
  gdImageFilledRectangle(_imagePtr, x1, y1, x2, y2, color);
}

void Gd::arc(int cx, int cy, int w, int h, int s, int e, int color)
{
  gdImageArc(_imagePtr, cx, cy, w, h, s, e, color);
}

void Gd::drawChar(int x, int y, char ch, int color, FontStyle style)
{
  gdFontPtr font = style == FontTiny    ? gdFontGetTiny()
                 : style == FontSmall   ? gdFontGetSmall()
                 : style == FontMedium  ? gdFontGetMediumBold()
                 : style == FontLarge   ? gdFontGetLarge()
                 : style == FontGiant   ? gdFontGetGiant()
                 : gdFontGetMediumBold();

  gdImageChar(_imagePtr, font, x, y, ch, color);
}

void Gd::drawString(int x, int y, const char* str, int color, FontStyle style)
{
  gdFontPtr font = style == FontTiny    ? gdFontGetTiny()
                 : style == FontSmall   ? gdFontGetSmall()
                 : style == FontMedium  ? gdFontGetMediumBold()
                 : style == FontLarge   ? gdFontGetLarge()
                 : style == FontGiant   ? gdFontGetGiant()
                 : gdFontGetMediumBold();

  gdImageString(_imagePtr, font, x, y, const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(str)), color);
}

namespace
{
  int ostreamOut(void* context, const char* buffer, int len)
  {
    std::ostream* outPtr = reinterpret_cast<std::ostream*>(context);
    outPtr->write(buffer, len);
    return *outPtr ? len : -1;
  }

}

void Gd::printPng(std::ostream& out) const
{
  gdSink sink;
  sink.sink = ostreamOut;
  sink.context = &out;
  gdImagePngToSink(_imagePtr, &sink);
}

}
