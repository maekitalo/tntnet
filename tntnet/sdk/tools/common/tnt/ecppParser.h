/* tnt/ecppParser.h
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef ECPPPARSER_H
#define ECPPPARSER_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <list>

namespace tnt
{

class ecppParser
{
    bool inComp;
    bool debug;
    bool splitBar;

    char split_start;
    char split_end;

    std::string tag;

  public:
    ecppParser()
      : inComp(false),
        debug(false),
        splitBar(false),
        split_start('{'),
        split_end('}')
    { }
    virtual ~ecppParser()
    { }

    void parse(std::istream& in);

    void setDebug(bool sw = true)     { debug = sw; }
    bool isDebug() const              { return debug; }

    void setSplitBar(bool sw = true)  { splitBar = sw; }
    bool isSplitBar() const           { return splitBar; }

    void setSplitChars(char start, char end)
    {
      split_start = start;
      split_end = end;
    }

    char getSplitStartChar() const  { return split_start; }
    char getSplitEndChar() const    { return split_end; }

  protected:
    typedef std::multimap<std::string, std::string> comp_args_type;
    typedef std::list<std::pair<std::string, std::string> > cppargs_type;

  private:
    virtual void start();
    virtual void end();
    virtual void processHtml(const std::string& html);
    virtual void processExpression(const std::string& expr);
    virtual void processCpp(const std::string& code);
    virtual void processPre(const std::string& code);
    virtual void processDeclare(const std::string& code);
    virtual void processInit(const std::string& code);
    virtual void processCleanup(const std::string& code);
    virtual void processArg(const std::string& name,
      const std::string& value);
    virtual void processAttr(const std::string& name,
      const std::string& value);
    virtual void processCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const std::string& cppargs);
    virtual void processDeclareShared(const std::string& code);
    virtual void processShared(const std::string& code);
    virtual void startComp(const std::string& name, const cppargs_type& cppargs);
    virtual void processComp(const std::string& code);
    virtual void processCondExpr(const std::string& cond, const std::string& expr);
    virtual void processConfig(const std::string& code, const std::string& value);
    virtual void tokenSplit(bool start);

    void processNV(const std::string& name, const std::string& value);
};

class parse_error : public std::runtime_error
{
    std::string msg;

  public:
    parse_error(const std::string& msg, int state, unsigned line);
    ~parse_error() throw()
    { }
    const char* what() const throw();
};

}

#endif // ECPPPARSER_H

