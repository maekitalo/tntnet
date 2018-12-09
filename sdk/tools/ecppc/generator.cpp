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


#include "tnt/ecppc/generator.h"
#include <tnt/stringescaper.h>
#include <iterator>
#include <zlib.h>
#include <vector>
#include <algorithm>
#include <cxxtools/log.h>
#include <tnt/httpmessage.h>
#include <tnt/componentfactory.h>

log_define("tntnet.generator")

namespace tnt
{
  namespace ecppc
  {
    namespace
    {
      class BStringPrinter
      {
        private:
          std::ostream& out;
          unsigned mincol, maxcol, col;

        public:
          explicit BStringPrinter(std::ostream& out_, unsigned mincol_,
            unsigned maxcol_, unsigned col_)
            : out(out_),
              mincol(mincol_),
              maxcol(maxcol_),
              col(col_)
            { }

          void print(const char* data, unsigned count);
          void print(char ch);
      };

      void BStringPrinter::print(const char* data, unsigned count)
      {
        for (unsigned n = 0; n < count; ++n)
          print(data[n]);
      }

      void BStringPrinter::print(char ch)
      {
        stringescaper s;
        const char* d = s(ch);
        unsigned len = strlen(d);
        col += len;
        if (col >= maxcol)
        {
          out << "\"\n";
          for (unsigned n = 0; n < mincol; ++n)
            out << ' ';
          out << '"';
          col = mincol + len;
        }
        out << d;
      }
    } // namespace

    Generator::Generator(const std::string& componentName)
      : _raw(false),
        _maincomp(componentName),
        _haveCloseComp(false),
        _closeComp(componentName),
        _currentComp(&_maincomp),
        _compress(false),
        _c_time(0),
        _linenumbersEnabled(true)
    {
      time_t cur;
      time(&cur);
      _gentime = asctime(localtime(&cur));
    }

    bool Generator::hasScopevars() const
    {
      if (_maincomp.hasScopevars())
        return true;
      for (subcomps_type::const_iterator it = _subcomps.begin(); it != _subcomps.end(); ++it)
        if (it->hasScopevars())
          return true;
      return false;
    }

    void Generator::addImage(const std::string& name, const std::string& content,
        const std::string& mime, time_t c_time)
    {
      MultiImageType mi;
      mi.name = name;
      mi.mime = mime;
      mi.c_time = c_time;
      _multiImages.push_back(mi);

      _data.push_back(content);
    }

    void Generator::onLine(unsigned lineno, const std::string& file)
    {
      log_debug("onLine(" << lineno << ", \"" << file << "\")");
      _curline = lineno;
      _curfile = file;
    }

    void Generator::onHtml(const std::string& html)
    {
      log_debug("onHtml(\"" << html << "\")");

      _data.push_back(html);

      std::ostringstream m;
      m << "  reply.out() << _tntChunks[" << (_data.count() - 1) << "]; // ";

      std::transform(html.begin(), html.end(),
        std::ostream_iterator<const char*>(m),
        stringescaper(false));

      m << '\n';
      _currentComp->addHtml(m.str());

      log_debug("onHtml(\"" << html << "\") end, _tntChunks.size()=" << _data.count());
    }

    void Generator::onExpression(const std::string& expr)
    {
      std::ostringstream m;
      printLine(m);
      m << "  reply.sout() << (" << expr << ");\n";
      _currentComp->addHtml(m.str());
    }

    void Generator::onHtmlExpression(const std::string& expr)
    {
      std::ostringstream m;
      printLine(m);
      m << "  reply.out() << (" << expr << ");\n";
      _currentComp->addHtml(m.str());
    }

    void Generator::onCpp(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      _currentComp->addHtml(m.str());
    }

    void Generator::onPre(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      _pre += m.str();
    }

    void Generator::onInit(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      _init += m.str();
    }

    void Generator::onCleanup(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      _cleanup += m.str();
    }

    void Generator::onArg(const std::string& name, const std::string& value)
      { _currentComp->addArg(name, value); }

    void Generator::onGet(const std::string& name, const std::string& value)
      { _currentComp->addGet(name, value); }

    void Generator::onPost(const std::string& name,
      const std::string& value)
      { _currentComp->addPost(name, value); }

    void Generator::onAttr(const std::string& name,
      const std::string& value)
    {
      if (_attr.find(name) != _attr.end())
        throw std::runtime_error("duplicate attr " + name);
      _attr.insert(attr_type::value_type(name, value));
    }

    void Generator::onCall(const std::string& comp, const comp_args_type& args,
                           const std::string& passCgi, const paramargs_type& paramargs,
                           const std::string& cppargs)
      { _currentComp->addCall(_curline, _curfile, comp, args, passCgi, paramargs, cppargs); }

    void Generator::onEndCall(const std::string& comp)
      { _currentComp->addEndCall(_curline, _curfile, comp); }

    void Generator::onShared(const std::string& code)
      { _shared += code; }

    void Generator::startComp(const std::string& name, const cppargs_type& cppargs)
    {
      _subcomps.push_back(Subcomponent(name, _maincomp, cppargs));
      Subcomponent& s = _subcomps.back();
      _currentComp = &s;
      _maincomp.addSubcomp(name);
    }

    void Generator::onComp(const std::string& /* code */)
      { _currentComp = &_maincomp; }

    void Generator::startClose()
    {
      if (_haveCloseComp)
        throw std::runtime_error("duplicate close-part");
      _haveCloseComp = true;
      _currentComp = &_closeComp;
    }

    void Generator::endClose()
      { _currentComp = &_maincomp; }

    void Generator::onCondExpr(const std::string& cond, const std::string& expr, bool htmlexpr)
    {
      std::ostringstream m;
      printLine(m);
      m << "  if (" << cond << ")\n";
      if (htmlexpr)
        m << "    reply.out() << ";
      else
        m << "    reply.sout() << ";
      m << expr << ";\n";
      _currentComp->addHtml(m.str());
    }

    void Generator::onConfig(const std::string& name, const std::string& value)
      { _configs.push_back(tnt::ecppc::Variable(name, value)); }

    void Generator::onScope(scope_container_type container, scope_type scope,
                            const std::string& type, const std::string& var,
                            const std::string& init, const std::vector<std::string>& includes)
    {
      log_debug("onScope type=\"" << type << "\" var=\"" << var << "\" init=\"" << init << '"');
      tnt::ecppc::Component* comp = (scope == ecpp::page_scope ? &_maincomp : _currentComp);

      comp->addScopevar(Scopevar(container, scope, type, var, init, _curline, _curfile));

      std::ostringstream m;
      for (unsigned n = 0; n != includes.size(); ++n)
        m << "#include <" << includes[n] << ">\n";
      _pre += m.str();
    }

    void Generator::onInclude(const std::string& file)
      { _currentComp->addHtml("  // <%include> " + file + '\n'); }

    void Generator::onIncludeEnd(const std::string& /* file */)
      { _currentComp->addHtml("  // </%include>\n"); }

    void Generator::getIntro(std::ostream& out, const std::string& filename) const
    {
      out << "////////////////////////////////////////////////////////////////////////\n"
             "// " << filename << "\n"
             "// generated with ecppc\n"
             "//\n\n";
    }

    void Generator::getHeaderIncludes(std::ostream& out) const
    {
      if (_multiImages.empty() && !isRawMode())
        out << "#include <tnt/ecpp.h>\n";
      else
        out << "#include <tnt/mbcomponent.h>\n";
    }

    void Generator::getPre(std::ostream& out) const
    {
      out << "// <%pre>\n"
          << _pre
          << "// </%pre>\n";
    }

    void Generator::getClassDeclaration(std::ostream& out) const
    {
      if (_multiImages.empty() && !isRawMode())
      {
        out << "class _component_ : public tnt::EcppComponent\n"
               "{\n"
               "    _component_& main()  { return *this; }\n\n"
               "  protected:\n"
               "    ~_component_();\n\n"
               "  public:\n"
               "    _component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);\n\n"
               "    unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);\n";
      }
      else
      {
        out << "class _component_ : public tnt::MbComponent\n"
               "{\n"
               "    _component_& main()  { return *this; }\n\n" ;
        if (_compress)
          out << "    tnt::DataChunks _tntChunks;\n\n";
        out << "  protected:\n"
               "    ~_component_();\n\n"
               "  public:\n"
               "    _component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);\n\n";
      }

      if (_haveCloseComp)
        out << "    unsigned endTag(tnt::HttpRequest& request, tnt::HttpReply& reply,\n"
               "                    tnt::QueryParams& qparam);\n";
      if (!_attr.empty())
        out << "    std::string getAttribute(const std::string& name,\n"
               "      const std::string& def = std::string()) const;\n\n";

      if (!_configs.empty())
      {
        out << "    virtual void configure(const tnt::TntConfig& config);\n"
               "    // <%config>\n";
        for (variable_declarations::const_iterator it = _configs.begin(); it != _configs.end(); ++it)
          it->getConfigHDecl(out);
        out << "    // </%config>\n\n";
      }

      // declare subcomponents
      for (subcomps_type::const_iterator i = _subcomps.begin(); i != _subcomps.end(); ++i)
        i->getHeader(out);

      // instantiate subcomponents
      for (subcomps_type::const_iterator i = _subcomps.begin(); i != _subcomps.end(); ++i)
        out << "    " << i->getName() << "_type " << i->getName() << ";\n";

      out << "};\n\n";
    }

    void Generator::getCppIncludes(std::ostream& out) const
    {
      out << "#include <tnt/httprequest.h>\n"
             "#include <tnt/httpreply.h>\n"
             "#include <tnt/httpheader.h>\n"
             "#include <tnt/http.h>\n"
             "#include <tnt/data.h>\n"
             "#include <tnt/componentfactory.h>\n";

      if (!_configs.empty())
        out << "#include <tnt/comploader.h>\n"
               "#include <tnt/tntconfig.h>\n";

      if (_compress)
        out << "#include <tnt/zdata.h>\n";

      out << "#include <cxxtools/log.h>\n"
             "#include <stdexcept>\n\n";
    }

    void Generator::getFactoryDeclaration(std::ostream& code) const
    {
      if (_configs.empty())
      {
        code << "static tnt::EcppComponentFactoryImpl<_component_> Factory(\"" << _maincomp.getName() << "\");\n\n";
      }
      else
      {
        code << "class _component_Factory : public tnt::EcppComponentFactoryImpl<_component_>\n"
                "{\n"
                "  public:\n"
                "    _component_Factory()\n"
                "      : tnt::EcppComponentFactoryImpl<_component_>(\"" << _maincomp.getName() << "\")\n"
                "      { }\n"
                "    tnt::Component* doCreate(const tnt::Compident& ci,\n"
                "      const tnt::Urlmapper& um, tnt::Comploader& cl);\n"
                "};\n\n"
                "tnt::Component* _component_Factory::doCreate(const tnt::Compident& ci,\n"
                "  const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                "{\n"
                "  return new _component_(ci, um, cl);\n"
                "}\n\n"
                "static _component_Factory factory;\n\n";
      }
    }

    void Generator::getCppBody(std::ostream& code) const
    {
      getFactoryDeclaration(code);

      if (!_data.empty())
      {
        if (_compress)
        {
          code << "static tnt::Zdata rawData(\n\"";

          uLongf s = _data.size() + _data.size() / 100 + 100;
          std::vector<char> p(s);
          char* buffer = &p[0];

          int z_ret = ::compress(reinterpret_cast<Bytef*>(buffer), &s, (const Bytef*)_data.ptr(), _data.size());

          if (z_ret != Z_OK)
          {
            throw std::runtime_error(std::string("error compressing data: ") +
              (z_ret == Z_MEM_ERROR ? "Z_MEM_ERROR" :
               z_ret == Z_BUF_ERROR ? "Z_BUF_ERROR" :
               z_ret == Z_DATA_ERROR ? "Z_DATA_ERROR" : "unknown error"));
          }

          BStringPrinter bs(code, 2, 120, 30);
          bs.print(buffer, s);

          code << "\",\n  " << s << ", " << _data.size() << ");\n\n";
        }
        else
        {
          code << "static const char* rawData = \"";

          BStringPrinter bs(code, 2, 120, 30);
          bs.print(_data.ptr(), _data.size());

          code << "\";\n\n";
        }
      }

      // multi-images
      if (!_multiImages.empty())
      {
        code << "static const char* urls[] = {\n";

        for (MultiImagesType::const_iterator it = _multiImages.begin(); it != _multiImages.end(); ++it)
          code << "  \"" << it->name << "\",\n";

        code << "};\n\n"
                "static const char* mimetypes[] = {\n";

        for (MultiImagesType::const_iterator it = _multiImages.begin();
             it != _multiImages.end(); ++it)
          code << "  \"" << it->mime << "\",\n";

        code << "};\n\n"
                "const char* ctimes[] = {\n";

        for (MultiImagesType::const_iterator it = _multiImages.begin(); it != _multiImages.end(); ++it)
          code << "  \"" << tnt::HttpMessage::htdate(it->c_time) << "\",\n";

        code << "};\n\n";

        if (_compress)
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl)\n"
                  "{\n"
                  "  ::rawData.addRef();\n"
                  "  _tntChunks.setData(::rawData);\n"
                  "  init(::rawData, ::urls, ::mimetypes, ::ctimes);\n"
                  "}\n\n"
                  "_component_::~_component_()\n"
                  "{\n"
                  "  ::rawData.release();\n"
                  "}\n\n";
        }
        else
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl, ::rawData, ::urls, ::mimetypes, ::ctimes)\n"
                  "{ }\n\n"
                  "_component_::~_component_()\n"
                  "{ }\n\n";
        }
      }
      else if (isRawMode())
      {
        code << "static const char* mimetype = \"" << _mimetype << "\";\n"
                "static const char* c_time = \"" << tnt::HttpMessage::htdate(_c_time) << "\";\n\n";

        if (_compress)
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl)\n"
                  "{\n"
                  "  ::rawData.addRef();\n"
                  "  _tntChunks.setData(::rawData);\n"
                  "  init(::rawData, ::mimetype, ::c_time);\n"
                  "}\n\n"
                  "_component_::~_component_()\n"
                  "{\n"
                  "  ::rawData.release();\n"
                  "}\n\n";
        }
        else
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl, ::rawData, ::mimetype, ::c_time)\n"
                  "{ }\n\n"
                  "_component_::~_component_()\n"
                  "{ }\n\n";
        }
      }
      else
      {
        // logger, %shared and constructor
        //
        code << "// <%shared>\n"
             << _shared
             << "// </%shared>\n\n";
        code << "// <%config>\n";
        for (variable_declarations::const_iterator it = _configs.begin(); it != _configs.end(); ++it)
          it->getConfigDecl(code);
        code << "// </%config>\n\n"
             << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                "  : EcppComponent(ci, um, cl)";

        // initialize subcomponents
        for (subcomps_type::const_iterator i = _subcomps.begin(); i != _subcomps.end(); ++i)
          code << ",\n"
                  "    " << i->getName() << "(*this, \"" << i->getName() << "\")";

        code << "\n{\n";

        if (_compress)
          code << "  rawData.addRef();\n";

        code << "  // <%init>\n"
             << _init
             << "  // </%init>\n"
                "}\n\n"
                "_component_::~_component_()\n"
                "{\n"
                "  // <%cleanup>\n"
             << _cleanup
             << "  // </%cleanup>\n";

        if (_compress)
          code << "  rawData.release();\n";

        code << "}\n\n";

        if (!_configs.empty())
        {
          code << "void _component_::configure(const tnt::TntConfig& config)\n"
                  "{\n"
                  "  // <%config>\n";
          for (variable_declarations::const_iterator it = _configs.begin(); it != _configs.end(); ++it)
            it->getConfigInit(code);
          code << "  // </%config>\n"
                  "}\n\n";
        }

        code << "template <typename T>\n"
                "inline void _tnt_ignore_unused(const T&) { }\n\n"

                "unsigned _component_::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)\n"
                "{\n"
                "  log_trace(\"" << _maincomp.getName() << " \" << qparam.getUrl());\n\n";

        if (_raw)
          code << "  reply.setKeepAliveHeader();\n\n";
        if (!_mimetype.empty())
          code << "  reply.setContentType(\"" << _mimetype << "\");\n";

        if (_c_time)
          code << "  {\n"
                  "    std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);\n"
                  "    if (s == \"" << tnt::HttpMessage::htdate(_c_time) << "\")\n"
                  "      return HTTP_NOT_MODIFIED;\n"
                  "  }\n";

        if (!_data.empty())
        {
          code << "  tnt::DataChunks _tntChunks(rawData);\n";
        }

        if (_multiImages.empty())
        {
          if (_c_time)
            code << "  reply.setHeader(tnt::httpheader::lastModified, \""
                 << tnt::HttpMessage::htdate(_c_time) << "\");\n";
          if (_raw)
            code << "  reply.setContentLengthHeader(_tntChunks.size(0));\n"
                    "  reply.setDirectMode();\n";

          code << '\n';
          _maincomp.getBody(code, _linenumbersEnabled);
          code << "}\n\n";
        }
        else
        {
          // multi-image-component
          code << "  const char* url = request.getPathInfo().c_str();\n\n"
                  "  log_debug(\"search for \\\"\" << url << '\"');\n\n"
                  "  urls_iterator it = std::lower_bound(urls_begin, urls_end, url, charpLess);\n"
                  "  if (it == urls_end || strcmp(url, *it) != 0)\n"
                  "  {\n"
                  "    log_info(\"binary file \\\"\" << url << \"\\\" not found\");\n"
                  "    return DECLINED;\n"
                  "  }\n"
                  "  unsigned url_idx = it - urls_begin;\n\n"

                  "  reply.setKeepAliveHeader();\n"
                  "  reply.setContentType(mimetypes[url_idx]);\n\n"

                  "  std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);\n"
                  "  if (s == url_c_time[url_idx])\n"
                  "    return HTTP_NOT_MODIFIED;\n\n"

                  "  reply.setHeader(tnt::httpheader::lastModified, url_c_time[url_idx]);\n"
                  "  reply.setContentLengthHeader(_tntChunks.size(url_idx));\n"
                  "  reply.setDirectMode();\n"
                  "  reply.out() << _tntChunks[url_idx];\n"
                  "  return DEFAULT;\n"
                  "}\n\n";
        }

        if (_haveCloseComp)
          _closeComp.getDefinition(code, _linenumbersEnabled);

        if (!_attr.empty())
        {
          code << "// <%attr>\n"
                  "std::string _component_::getAttribute(const std::string& name, const std::string& def) const\n"
                  "{\n";
          for (attr_type::const_iterator it = _attr.begin(); it != _attr.end(); ++it)
            code << "  if (name == \"" << it->first << "\")\n"
                    "    return " << it->second << ";\n";
          code << "  return def;\n"
                  "} // </%attr>\n\n";
        }

        for (subcomps_type::const_iterator it = _subcomps.begin(); it != _subcomps.end(); ++it)
          it->getDefinition(code, _linenumbersEnabled);
      }
    }

    void Generator::printLine(std::ostream& out) const
    {
      if (_linenumbersEnabled)
        out << "#line " << (_curline + 1) << " \"" << _curfile << "\"\n";
    }

    void Generator::getCpp(std::ostream& code, const std::string& filename) const
    {
      getIntro(code, filename);

      getHeaderIncludes(code);
      getCppIncludes(code);

      if (_multiImages.empty() && !isRawMode())
        code << "log_define(\"" << _maincomp.getLogCategory() << "\")\n\n";

      getPre(code);

      code << "\n"
              "namespace\n"
              "{\n";

      getClassDeclaration(code);

      getCppBody(code);

      code << "} // namespace\n";
    }
  }
}

