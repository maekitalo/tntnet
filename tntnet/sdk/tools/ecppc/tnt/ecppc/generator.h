/* generator.h
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

#ifndef TNT_ECPP_GENERATOR_H
#define TNT_ECPP_GENERATOR_H

#include <tnt/ecpp/parsehandler.h>
#include <sstream>
#include <list>
#include <functional>
#include <tnt/datachunks_creator.h>
#include "tnt/ecppc/variable.h"
#include "tnt/ecppc/component.h"
#include "tnt/ecppc/subcomponent.h"
#include <map>

namespace tnt
{
  namespace ecppc
  {

    ////////////////////////////////////////////////////////////////////////
    // Generator
    //
    class Generator : public tnt::ecpp::ParseHandler
    {
        bool debug;
        bool singleton;
        bool raw;
        std::string mimetype;
        std::string componentclass;
        std::string baseclass;

        typedef std::list<tnt::ecppc::Variable> variable_declarations;

        variable_declarations configs;

        std::string pre;
        std::string declare;
        std::string init;
        std::string cleanup;
        std::string shared;
        std::string declare_shared;

        tnt::ecppc::Component maincomp;

        typedef std::list<tnt::ecppc::Subcomponent> subcomps_type;
        subcomps_type subcomps;

        tnt::ecppc::Component* currentComp;

        typedef std::map<std::string, std::string> attr_type;
        attr_type attr;

        tnt::DatachunksCreator data;

        enum filter_enum
        {
          filter_null,
          filter_html,
          filter_css,
          filter_js
        };
        filter_enum filter;
        bool compress;
        bool externData;

        time_t c_time;
        const char* gentime;

        bool hasScopevars() const;

        // codegenerator helper
        void getIntro(std::ostream& out, const std::string& basename) const;

        void getHeaderIncludes(std::ostream& out) const;
        void getPre(std::ostream& out) const;
        void getNamespaceStart(std::ostream& out) const;
        void getNamespaceEnd(std::ostream& out) const;
        void getCreatorDeclaration(std::ostream& out) const;
        void getDeclareShared(std::ostream& out) const;
        void getClassDeclaration(std::ostream& out) const;

        void getCppIncludes(std::ostream& out) const;
        void getCppBody(std::ostream& out) const;

      public:
        Generator(const std::string& classname, const std::string& ns);

        void setDebug(bool sw)                       { debug = sw; }
        bool isDebug() const                         { return debug; }

        void setSingleton(bool sw)                   { singleton = sw; }
        bool isSingleton() const                     { return singleton; }

        void setMimetype(const std::string& type)    { mimetype = type; }
        const std::string& getMimetype()             { return mimetype; }

        void setComponentclass(const std::string& c) { componentclass = c; }
        const std::string& getComponentclass()       { return componentclass; }

        void setBaseclass(const std::string& c)      { baseclass = c; }
        const std::string& getBaseclass()            { return baseclass; }

        void setRawMode(bool sw = true)              { raw = sw; }
        bool isRawMode() const                       { return raw; }

        void setHtmlCompress(bool sw = true)         { filter = sw ? filter_html : filter_null; }
        bool isHtmlCompress()                        { return filter == filter_html; }

        void setCssCompress(bool sw = true)          { filter = sw ? filter_css : filter_null; }
        bool isCssCompress()                         { return filter == filter_css; }

        void setJsCompress(bool sw = true)           { filter = sw ? filter_js : filter_null; }
        bool isJsCompress()                          { return filter == filter_js; }

        void setCompress(bool sw = true)             { compress = sw; }
        bool isCompress() const                      { return compress; }

        void setExternData(bool sw = true)           { externData = sw; }
        bool isExternData() const                    { return externData; }

        void setLastModifiedTime(time_t t)           { c_time = t; }
        time_t getLastModifiedTime() const           { return c_time; }

        virtual void onHtml(const std::string& html);
        virtual void onExpression(const std::string& expr);
        virtual void onCpp(const std::string& cpp);
        virtual void onPre(const std::string& code);
        virtual void onDeclare(const std::string& code);
        virtual void onInit(const std::string& code);
        virtual void onCleanup(const std::string& code);
        virtual void onArg(const std::string& name,
          const std::string& value);
        virtual void onAttr(const std::string& name,
          const std::string& value);
        virtual void onCall(const std::string& comp,
          const comp_args_type& args, const std::string&  pass_cgi,
          const std::string& cppargs);
        virtual void onDeclareShared(const std::string& code);
        virtual void onShared(const std::string& code);
        virtual void startComp(const std::string& name, const cppargs_type& cppargs);
        virtual void onComp(const std::string& code);
        virtual void onCondExpr(const std::string& cond, const std::string& expr);
        virtual void onConfig(const std::string& name, const std::string& value);
        virtual void onScope(scope_container_type container, scope_type scope,
          const std::string& type, const std::string& var, const std::string& init);

        void getHeader(std::ostream& out, const std::string& filename) const;
        void getCpp(std::ostream& out, const std::string& filename) const;
        void getCppWoHeader(std::ostream& out, const std::string& filename) const;
    };

  }
}

#endif // TNT_ECPP_GENERATOR_H

