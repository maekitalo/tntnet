/* ecpplang.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef ECPPLANG_H
#define ECPPLANG_H

#include <map>
#include <iosfwd>
#include <tnt/ecpp/parsehandler.h>

class Ecpplang : public tnt::ecpp::ParseHandler
{
    bool inLang;
    unsigned count;

    typedef std::map<unsigned, std::string> data_type;
    data_type data;

    bool lang;
    bool nolang;

  public:
    Ecpplang()
      : inLang(false),
        count(0),
        lang(true),
        nolang(false)
      { }

    virtual void onHtml(const std::string& html);
    virtual void tokenSplit(bool start);

    void setLang(bool sw = true)
      { lang = sw; }
    void setNoLang(bool sw = true)
      { nolang = sw; }

    void print(std::ostream& out) const;
};

#endif // ECPPLANG_H

