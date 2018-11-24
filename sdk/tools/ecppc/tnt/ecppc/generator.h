/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
#include "tnt/ecppc/closecomponent.h"
#include <map>

namespace tnt
{
  namespace ecppc
  {
    class Generator : public tnt::ecpp::ParseHandler
    {
        typedef std::list<tnt::ecppc::Variable> variable_declarations;
        typedef std::list<tnt::ecppc::Subcomponent> subcomps_type;
        typedef std::map<std::string, std::string> attr_type;

        bool _raw;
        std::string _mimetype;

        variable_declarations _configs;

        std::string _pre;
        std::string _init;
        std::string _cleanup;
        std::string _shared;

        tnt::ecppc::Component _maincomp;
        subcomps_type _subcomps;

        bool _haveCloseComp;
        tnt::ecppc::Closecomponent _closeComp;

        tnt::ecppc::Component* _currentComp;

        attr_type _attr;

        tnt::DatachunksCreator _data;

        bool _compress;

        time_t _c_time;
        const char* _gentime;

        struct MultiImageType
        {
          std::string name;
          std::string mime;
          time_t c_time;
        };

        typedef std::list<MultiImageType> MultiImagesType;
        MultiImagesType _multiImages;

        unsigned _curline;
        std::string _curfile;

        bool _linenumbersEnabled;

        bool hasScopevars() const;

        // codegenerator helper
        void getIntro(std::ostream& out, const std::string& basename) const;

        void getHeaderIncludes(std::ostream& out) const;
        void getPre(std::ostream& out) const;
        void getClassDeclaration(std::ostream& out) const;

        void getCppIncludes(std::ostream& out) const;
        void getFactoryDeclaration(std::ostream& out) const;
        void getCppBody(std::ostream& out) const;

        void printLine(std::ostream& out) const;

      public:
        Generator(const std::string& componentName);

        void setMimetype(const std::string& type)   { _mimetype = type; }
        const std::string& getMimetype()            { return _mimetype; }

        void setRawMode(bool sw = true)             { _raw = sw; }
        bool isRawMode() const                      { return _raw; }

        void setCompress(bool sw = true)            { _compress = sw; }
        bool isCompress() const                     { return _compress; }

        void setLastModifiedTime(time_t t)          { _c_time = t; }
        time_t getLastModifiedTime() const          { return _c_time; }

        void enableLinenumbers(bool sw = true)      { _linenumbersEnabled = sw; }
        bool isLinenumbersEnabled() const           { return _linenumbersEnabled; }

        void setLogCategory(const std::string& cat) { _maincomp.setLogCategory(cat); }

        void addImage(const std::string& name, const std::string& content,
                      const std::string& mime, time_t c_time);

        virtual void onLine(unsigned lineno, const std::string& file);
        virtual void onHtml(const std::string& html);
        virtual void onExpression(const std::string& expr);
        virtual void onHtmlExpression(const std::string& expr);
        virtual void onCpp(const std::string& cpp);
        virtual void onPre(const std::string& code);
        virtual void onInit(const std::string& code);
        virtual void onCleanup(const std::string& code);
        virtual void onArg(const std::string& name, const std::string& value);
        virtual void onGet(const std::string& name, const std::string& value);
        virtual void onPost(const std::string& name, const std::string& value);
        virtual void onAttr(const std::string& name, const std::string& value);
        virtual void onCall(const std::string& comp, const comp_args_type& args,
                            const std::string& passCgi, const paramargs_type& paramargs,
                            const std::string& cppargs);
        virtual void onEndCall(const std::string& comp);
        virtual void onShared(const std::string& code);
        virtual void startComp(const std::string& name, const cppargs_type& cppargs);
        virtual void onComp(const std::string& code);
        virtual void startClose();
        virtual void endClose();
        virtual void onCondExpr(const std::string& cond, const std::string& expr, bool htmlexpr);
        virtual void onConfig(const std::string& name, const std::string& value);
        virtual void onScope(scope_container_type container, scope_type scope,
                             const std::string& type, const std::string& var,
                             const std::string& init,
                             const std::vector<std::string>& includes);
        virtual void onInclude(const std::string& file);
        virtual void onIncludeEnd(const std::string& file);

        void getCpp(std::ostream& out, const std::string& filename) const;
    };
  }
}

#endif // TNT_ECPP_GENERATOR_H

