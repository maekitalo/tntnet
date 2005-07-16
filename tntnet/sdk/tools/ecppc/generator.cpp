/* generator.cpp
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

#include "tnt/ecppc/generator.h"
#include "tnt/ecppc/htmlfilter.h"
#include "tnt/ecppc/cssfilter.h"
#include "tnt/ecppc/jsfilter.h"
#include <tnt/stringescaper.h>
#include <iterator>
#include <zlib.h>
#include <cxxtools/dynbuffer.h>
#include <tnt/httpmessage.h>

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Generator
    //
    Generator::Generator(const std::string& classname, const std::string& ns)
      : singleton(true),
        raw(false),
        maincomp(classname, ns),
        currentComp(&maincomp),
        filter(filter_null),
        compress(false),
        c_time(0)
    {
      time_t cur;
      time(&cur);
      gentime = asctime(localtime(&cur));
    }

    bool Generator::hasScopevars() const
    {
      if (maincomp.hasScopevars())
        return true;
      for (subcomps_type::const_iterator it = subcomps.begin(); it != subcomps.end(); ++it)
        if (it->hasScopevars())
          return true;
      return false;
    }

    void Generator::onHtml(const std::string& html)
    {
      std::ostringstream d;
      switch (filter)
      {
        case filter_null:
          d << html;
          break;

        case filter_html:
          for_each(html.begin(), html.end(), tnt::ecppc::Htmlfilter(d)).flush();
          break;

        case filter_css:
          for_each(html.begin(), html.end(), tnt::ecppc::Cssfilter(d));
          break;

        case filter_js:
          for_each(html.begin(), html.end(), tnt::ecppc::Jsfilter(d));
          break;

      }

      std::string chunk = d.str();
      data.push_back(chunk);
      if (!html.empty())
      {
        std::ostringstream m;

        m << "  reply.out() << DATA(dataComponent, request, " << (data.count() - 1) << "); // ";

        std::transform(
          chunk.begin(),
          chunk.end(),
          std::ostream_iterator<const char*>(m),
          stringescaper(false));

        m << '\n';
        currentComp->addHtml(m.str());
      }
    }

    void Generator::onExpression(const std::string& expr)
    {
      std::ostringstream m;
      m << "  reply.sout() << (" << expr << ");\n";
      currentComp->addHtml(m.str());
    }

    void Generator::onCpp(const std::string& code)
    {
      currentComp->addHtml(code);
    }

    void Generator::onPre(const std::string& code)
    {
      pre += code;
    }

    void Generator::onDeclare(const std::string& code)
    {
      singleton = false;
      declare += code;
    }

    void Generator::onInit(const std::string& code)
    {
      init += code;
    }

    void Generator::onCleanup(const std::string& code)
    {
      cleanup += code;
    }

    void Generator::onArg(const std::string& name,
      const std::string& value)
    {
      currentComp->addArg(name, value);
    }

    void Generator::onAttr(const std::string& name,
      const std::string& value)
    {
      if (attr.find(name) != attr.end())
        throw std::runtime_error("duplicate attr " + name);
      attr.insert(attr_type::value_type(name, value));
    }

    void Generator::onCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const std::string& cppargs)
    {
      currentComp->addCall(comp, args, pass_cgi, cppargs);
    }

    void Generator::onDeclareShared(const std::string& code)
    {
      declare_shared += code;
    }

    void Generator::onShared(const std::string& code)
    {
      shared += code;
    }

    void Generator::startComp(const std::string& name,
      const cppargs_type& cppargs)
    {
      subcomps.push_back(Subcomponent(name, maincomp, cppargs));
      Subcomponent& s = subcomps.back();
      currentComp = &s;
      maincomp.addSubcomp(name);
    }

    void Generator::onComp(const std::string& code)
    {
      currentComp = &maincomp;
    }

    void Generator::onCondExpr(const std::string& cond, const std::string& expr)
    {
      std::ostringstream m;
      m << "  if (" << cond << ")\n"
           "    reply.sout() << " << expr << ";\n";
      currentComp->addHtml(m.str());
    }

    void Generator::onConfig(const std::string& name, const std::string& value)
    {
      configs.push_back(tnt::ecppc::Variable(name, value));
    }

    void Generator::onScope(scope_container_type container, scope_type scope,
      const std::string& type, const std::string& var, const std::string& init)
    {
      currentComp->addScopevar(Scopevar(container, scope, type, var, init));
    }

    void Generator::getIntro(std::ostream& out, const std::string& filename) const
    {
      out << "////////////////////////////////////////////////////////////////////////\n"
             "// " << filename << "\n"
             "// generated with ecppc\n"
             "// date: " << gentime <<
             "//\n\n";
    }

    void Generator::getHeaderIncludes(std::ostream& out) const
    {
      out << "#include <tnt/ecpp.h>\n"
             "#include <tnt/convert.h>\n";
      if (!componentclass.empty())
        out << "#include \"" << componentclass << ".h\"\n";
      if (!baseclass.empty())
        out << "#include \"" << baseclass << ".h\"\n";
    }

    void Generator::getPre(std::ostream& out) const
    {
      out << "// <%pre>\n"
          << pre
          << "// </%pre>\n";
    }

    void Generator::getNamespaceStart(std::ostream& out) const
    {
      out << "namespace ecpp_component\n"
             "{\n";

      if (!maincomp.getNs().empty())
        out << "namespace " << maincomp.getNs() << "\n"
               "{\n";

      out << '\n';
    }

    void Generator::getNamespaceEnd(std::ostream& out) const
    {
      if (!maincomp.getNs().empty())
        out << "} // namespace " << maincomp.getNs() << '\n';
      out << "} // namespace ecpp_component\n";
    }

    void Generator::getCreatorDeclaration(std::ostream& out) const
    {
      out << "extern \"C\"\n"
             "{\n"
             "  Component* create_" << maincomp.getName() << "(const Compident& ci, const Urlmapper& um,\n"
             "    Comploader& cl);\n"
             "}\n\n";
    }

    void Generator::getDeclareShared(std::ostream& out) const
    {
      out << "// <%declare_shared>\n"
          << declare_shared 
          << "// </%declare_shared>\n";
    }

    void Generator::getClassDeclaration(std::ostream& out) const
    {
      out << "class " << maincomp.getName() << " : public ";
      if (componentclass.empty())
        out << "EcppComponent";
      else
        out << componentclass;
      if (!baseclass.empty())
        out << ", public " << baseclass;
      out << "\n"
             "{\n"
             "    friend Component* create_" << maincomp.getName() << "(const Compident& ci,\n"
             "      const Urlmapper& um, Comploader& cl);\n\n"
             "    " << maincomp.getName() << "& main()  { return *this; }\n\n" 
             "    // <%declare>\n"
          << declare
          << "    // </%declare>\n\n"
          << "    // <%config>\n";
      for (variable_declarations::const_iterator it = configs.begin();
           it != configs.end(); ++it)
        it->getConfigHDecl(out);
      out << "    // </%config>\n\n"
             "  protected:\n"
             "    " << maincomp.getName() << "(const Compident& ci, const Urlmapper& um, Comploader& cl);\n"
             "    ~" << maincomp.getName() << "();\n\n"
             "  public:\n"
             "    unsigned operator() (HttpRequest& request, HttpReply& reply, cxxtools::QueryParams& qparam);\n"
             "    bool drop();\n"
             "    unsigned    getDataCount(const HttpRequest& request) const;\n"
             "    unsigned    getDataLen(const HttpRequest& request, unsigned n) const;\n"
             "    const char* getDataPtr(const HttpRequest& request, unsigned n) const;\n";
      if (!attr.empty())
        out << "    std::string getAttribute(const std::string& name,\n"
               "      const std::string& def = std::string()) const;\n";

      out << '\n';

      // Deklaration der Subcomponenten
      for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
        i->getHeader(out);

      // Instanzen der Subcomponenten
      for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
        out << "    " << i->getName() << "_type " << i->getName() << ";\n";

      out << "};\n\n";
    }

    void Generator::getCppIncludes(std::ostream& out) const
    {
      out << "#include <tnt/httprequest.h>\n"
             "#include <tnt/httpreply.h>\n"
             "#include <tnt/httpheader.h>\n"
             "#include <tnt/http.h>\n"
             "#include <tnt/data.h>\n";
      if (hasScopevars())
        out << "#include <tnt/objecttemplate.h>\n"
               "#include <tnt/objectptr.h>\n";
      if (!configs.empty())
        out << "#include <tnt/comploader.h>\n"
               "#include <tnt/tntconfig.h>\n";

      if (compress)
        out << "#include <tnt/zdata.h>\n";

      out << "#include <cxxtools/thread.h>\n"
             "#include <cxxtools/log.h>\n"
             "#include <stdexcept>\n\n";

      if (externData)
        out << "#include <tnt/comploader.h>\n"
               "#include <stdlib.h>\n";
    }

    void Generator::getCppBody(std::ostream& code) const
    {
      std::string classname = maincomp.getName();
      std::string ns_classname;
      std::string ns_classname_dot;

      if (!maincomp.getNs().empty())
      {
        ns_classname = maincomp.getNs() + "::";
        ns_classname_dot = maincomp.getNs() + '.';
      }
      ns_classname += classname;
      ns_classname_dot += classname;

      code << "static cxxtools::Mutex mutex;\n\n"
              "log_define(\"component." << ns_classname_dot << "\")\n\n";

      if (compress)
      {
        code << "static tnt::zdata rawData(\n\"";

        uLongf s = data.size() * data.size() / 100 + 100;
        cxxtools::Dynbuffer<Bytef> p;
        p.reserve(s);

        int z_ret = ::compress(p.data(), &s, (const Bytef*)data.ptr(), data.size());

        if (z_ret != Z_OK)
        {
          throw std::runtime_error(std::string("error compressing data: ") +
            (z_ret == Z_MEM_ERROR ? "Z_MEM_ERROR" :
             z_ret == Z_BUF_ERROR ? "Z_BUF_ERROR" :
             z_ret == Z_DATA_ERROR ? "Z_DATA_ERROR" : "unknown error"));
        }

        std::transform(p.data(), p.data() + s,
          std::ostream_iterator<const char*>(code),
          stringescaper());

        code << "\",\n  " << s << ", " << data.size() << ");\n"
             << "static tnt::DataChunks<tnt::zdata> data(rawData);\n\n";
      }
      else
      {
        code << "static tnt::RawData rawData(\n\"";

        std::transform(
          data.ptr(),
          data.ptr() + data.size(),
          std::ostream_iterator<const char*>(code),
          stringescaper());

        code << "\",\n  " << data.size() << ");\n"
             << "static tnt::DataChunks<tnt::RawData> data(rawData);\n\n";
      }

      if (externData)
      {
        code << "#define DATA(dc, r, n) (tnt::DataChunk(dc->getDataPtr(r, n), dc->getDataLen(r, n)))\n"
                "#define DATA_SIZE(dc, r, n) (dc->getDataLen(r, n))\n\n";
         
      }
      else
      {
        code << "#define DATA(dc, r, n) data[n]\n"
                "#define DATA_SIZE(dc, r, n) data.size(n)\n\n";
      }

      // creator
      if (singleton)
      {
        code << "static Component* theComponent = 0;\n"
                "static unsigned refs = 0;\n\n"
                "Component* create_" << classname << "(const Compident& ci, const Urlmapper& um, Comploader& cl)\n"
                "{\n"
                "  cxxtools::MutexLock lock(mutex);\n"
                "  if (theComponent == 0)\n"
                "  {\n"
                "    log_debug(\"create new component \\\"\" << ci << '\"');\n"
                "    theComponent = new ::ecpp_component::" << ns_classname << "(ci, um, cl);\n";

        if (compress)
          code << "    rawData.addRef();\n";

        code << "    refs = 1;\n\n"
                "    // <%config>\n";
        for (variable_declarations::const_iterator it = configs.begin();
             it != configs.end(); ++it)
          it->getConfigInit(code, classname);
        code << "    // </%config>\n"
                "  }\n"
                "  else\n"
                "    ++refs;\n"
                "  return theComponent;\n"
                "}\n\n";
      }
      else
      {
        code << "Component* create_" << classname << "(const Compident& ci, const Urlmapper& um, Comploader& cl)\n"
                "{\n";

        if (compress)
          code << "  rawData.addRef();\n";

        code << "  // <%config>\n";
        if (!configs.empty())
        {
          code << "  if (!config_init)\n"
                  "  {\n"
                  "    cxxtools::MutexLock lock(mutex);\n"
                  "    if (!config_init)\n"
                  "    {\n";
          for (variable_declarations::const_iterator it = configs.begin();
               it != configs.end(); ++it)
            it->getConfigInit(code, classname);
          code << "      config_init = true;\n"
                  "    }\n"
                  "  }\n";
        }
        code << "  // </%config>\n\n"
                "  return new ::ecpp_component::" << classname << "(ci, um, cl);\n"
             << "}\n\n";
      }

      // logger, %shared and constructor
      //
      code << "// <%shared>\n"
           << shared
           << "// </%shared>\n\n";
      if (!singleton && !configs.empty())
        code << "bool config_init = false;\n";
      code << "// <%config>\n";
      for (variable_declarations::const_iterator it = configs.begin();
           it != configs.end(); ++it)
        it->getConfigDecl(code, classname);
      code << "// </%config>\n\n"
           << classname << "::" << classname << "(const Compident& ci, const Urlmapper& um, Comploader& cl)\n"
              "  : EcppComponent(ci, um, cl)";

      // initialize subcomponents
      for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
        code << ",\n"
                "    " << i->getName() << "(*this, \"" << i->getName() << "\")";

      code << "\n{\n";

      code << "  // <%init>\n"
           << init
           << "  // </%init>\n"
              "}\n\n"
           << classname << "::~" << classname << "()\n"
              "{\n"
              "  // <%cleanup>\n"
           << cleanup
           << "  // </%cleanup>\n"
              "}\n\n"
              "unsigned " << classname << "::operator() (HttpRequest& request, HttpReply& reply, cxxtools::QueryParams& qparam)\n"
           << "{\n";

      if (isDebug())
        code << "  log_trace(\"" << classname << " \" << qparam.getUrl());\n\n";

      if (raw)
        code << "  reply.setKeepAliveHeader();\n\n";
      if (!mimetype.empty())
        code << "  reply.setContentType(\"" << mimetype << "\");\n";

      if (c_time)
        code << "  {\n"
                "    std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);\n"
                "    if (s == \"" << tnt::HttpMessage::htdate(c_time) << "\")\n"
                "      return HTTP_NOT_MODIFIED;\n"
                "  }\n";

      if (externData && !data.empty())
        code << "  const Component* dataComponent = main().getDataComponent(request);\n\n";
      else
        code << "  const Component* dataComponent = this;\n";
      code << "  ::use(dataComponent);\n";

      if (c_time)
        code << "  reply.setHeader(tnt::httpheader::lastModified, \""
             << tnt::HttpMessage::htdate(c_time) << "\");\n";
      if (raw)
        code << "  reply.setContentLengthHeader(DATA_SIZE(dataComponent, request, 0));\n"
                "  reply.setDirectMode();\n";

      code << '\n';
      maincomp.getBody(code);
      code << "}\n\n";

      code << "bool " << classname << "::drop()\n"
           << "{\n";
      if (singleton)
      {
        code << "  cxxtools::MutexLock lock(mutex);\n"
             << "  if (--refs == 0)\n"
             << "  {\n"
             << "    delete this;\n"
             << "    theComponent = 0;\n";
        if (compress)
          code << "    rawData.release();\n";
        code << "    return true;\n"
             << "  }\n"
             << "  else\n"
             << "    return false;\n";
      }
      else
      {
        code << "  delete this;\n";
        if (!externData && compress)
          code << "  rawData.release();\n";
        code << "  return true;\n";
      }

      code << "}\n\n"
              "unsigned " << classname << "::getDataCount(const HttpRequest& request) const\n"
              "{ return data.size(); }\n\n";

      if (externData)
      {
        code << "unsigned " << classname << "::getDataLen(const HttpRequest& request, unsigned n) const\n"
                "{\n"
                "  const Component* dataComponent = getDataComponent(request);\n"
                "  if (dataComponent == this)\n"
                "  {\n"
                "    if (n >= data.size())\n"
                "      throw std::range_error(\"range_error in " << classname << "::getDataLen\");\n"
                "    return data[n].getLength();\n"
                "  }\n"
                "  else\n"
                "    return dataComponent->getDataLen(request, n);\n"
                "}\n\n"
                "const char* " << classname << "::getDataPtr(const HttpRequest& request, unsigned n) const\n"
                "{\n"
                "  const Component* dataComponent = getDataComponent(request);\n"
                "  if (dataComponent == this)\n"
                "  {\n"
                "    if (n >= data.size())\n"
                "      throw std::range_error(\"range_error in " << classname << "::getDataPtr\");\n"
                "    return data[n].getData();\n"
                "  }\n"
                "  else\n"
                "    return dataComponent->getDataPtr(request, n);\n"
                "}\n\n";
      }
      else
      {
        code << "unsigned " << classname << "::getDataLen(const HttpRequest& request, unsigned n) const\n"
                "{\n"
                "  if (n >= data.size())\n"
                "    throw std::range_error(\"range_error in " << classname << "::getDataLen\");\n"
                "  return data[n].getLength();\n"
                "}\n\n"
                "const char* " << classname << "::getDataPtr(const HttpRequest& request, unsigned n) const\n"
                "{\n"
                "  if (n >= data.size())\n"
                "    throw std::range_error(\"range_error in " << classname << "::getDataPtr\");\n"
                "  return data[n].getData();\n"
                "}\n\n";
      }

      if (!attr.empty())
      {
        code << "// <%attr>\n"
                "std::string " << classname << "::getAttribute(const std::string& name, const std::string& def) const\n"
                "{\n";
        for (attr_type::const_iterator it = attr.begin();
             it != attr.end(); ++it)
          code << "  if (name == \"" << it->first << "\")\n"
                  "    return " << it->second << ";\n";
        code << "  return def;\n"
                "} // </%attr>\n\n";
      }

      for (subcomps_type::const_iterator i = subcomps.begin();
           i != subcomps.end(); ++i)
        i->getDefinition(code, isDebug(), externData);
    }

    void Generator::getHeader(std::ostream& header, const std::string& filename) const
    {
      std::string ns_classname_;
      const std::string classname = maincomp.getName();
      const std::string ns = maincomp.getNs();
      if (ns.empty())
        ns_classname_ = classname;
      else
        ns_classname_ = ns + '_' + classname;

      getIntro(header, filename);
      header << "#ifndef ECPP_COMPONENT_" << ns_classname_ << "_H\n"
                "#define ECPP_COMPONENT_" << ns_classname_ << "_H\n\n";

      getHeaderIncludes(header);
      header << '\n';
      getPre(header);
      header << '\n';
      getNamespaceStart(header);
      getCreatorDeclaration(header);
      getDeclareShared(header);
      getClassDeclaration(header);
      getNamespaceEnd(header);

      header << "\n"
                "#endif\n";
    }

    void Generator::getCpp(std::ostream& code, const std::string& filename) const
    {
      std::string ns_classname_slash;
      if (!maincomp.getNs().empty())
        ns_classname_slash = maincomp.getNs() + '/';
      ns_classname_slash += maincomp.getName();

      getIntro(code, filename);
      getCppIncludes(code);

      code << "#include \"" << ns_classname_slash << ".h\"\n\n"
              "template <typename T> inline void use(const T&) { }\n\n";

      getNamespaceStart(code);
      getCppBody(code);
      getNamespaceEnd(code);
    }

    void Generator::getCppWoHeader(std::ostream& code, const std::string& filename) const
    {
      getIntro(code, filename);

      getHeaderIncludes(code);
      getCppIncludes(code);

      code << "\n" 
              "template <typename T> inline void use(const T&) { }\n\n";

      getPre(code);
      code << '\n';

      getNamespaceStart(code);

      getCreatorDeclaration(code);
      getDeclareShared(code);
      getClassDeclaration(code);

      getCppBody(code);

      getNamespaceEnd(code);
    }
  }
}
