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
    // generator
    //
    generator::generator(const std::string& classname, const std::string& ns)
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

    bool generator::hasScopevars() const
    {
      if (maincomp.hasScopevars())
        return true;
      for (subcomps_type::const_iterator it = subcomps.begin(); it != subcomps.end(); ++it)
        if (it->hasScopevars())
          return true;
      return false;
    }

    void generator::processHtml(const std::string& html)
    {
      std::ostringstream d;
      switch (filter)
      {
        case filter_null:
          d << html;
          break;

        case filter_html:
          for_each(html.begin(), html.end(), tnt::ecppc::htmlfilter(d)).flush();
          break;

        case filter_css:
          for_each(html.begin(), html.end(), tnt::ecppc::cssfilter(d));
          break;

        case filter_js:
          for_each(html.begin(), html.end(), tnt::ecppc::jsfilter(d));
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

    void generator::processExpression(const std::string& expr)
    {
      std::ostringstream m;
      m << "  reply.out() << (" << expr << ");\n";
      currentComp->addHtml(m.str());
    }

    void generator::processCpp(const std::string& code)
    {
      currentComp->addHtml(code);
    }

    void generator::processPre(const std::string& code)
    {
      pre += code;
    }

    void generator::processDeclare(const std::string& code)
    {
      singleton = false;
      declare += code;
    }

    void generator::processInit(const std::string& code)
    {
      init += code;
    }

    void generator::processCleanup(const std::string& code)
    {
      cleanup += code;
    }

    void generator::processArg(const std::string& name,
      const std::string& value)
    {
      currentComp->addArg(name, value);
    }

    void generator::processAttr(const std::string& name,
      const std::string& value)
    {
      if (attr.find(name) != attr.end())
        throw std::runtime_error("duplicate attr " + name);
      attr.insert(attr_type::value_type(name, value));
    }

    void generator::processCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const std::string& cppargs)
    {
      currentComp->addCall(comp, args, pass_cgi, cppargs);
    }

    void generator::processDeclareShared(const std::string& code)
    {
      declare_shared += code;
    }

    void generator::processShared(const std::string& code)
    {
      shared += code;
    }

    void generator::startComp(const std::string& name,
      const cppargs_type& cppargs)
    {
      subcomps.push_back(subcomponent(name, maincomp, cppargs));
      subcomponent& s = subcomps.back();
      currentComp = &s;
      maincomp.addSubcomp(name);
    }

    void generator::processComp(const std::string& code)
    {
      currentComp = &maincomp;
    }

    void generator::processCondExpr(const std::string& cond, const std::string& expr)
    {
      std::ostringstream m;
      m << "  if (" << cond << ")\n"
           "    reply.out() << " << expr << ";\n";
      currentComp->addHtml(m.str());
    }

    void generator::processConfig(const std::string& name, const std::string& value)
    {
      configs.push_back(tnt::ecppc::variable(name, value));
    }

    void generator::processScope(scope_container_type container, scope_type scope,
      const std::string& type, const std::string& var, const std::string& init)
    {
      currentComp->addScopevar(scopevar(container, scope, type, var, init));
    }

    std::string generator::getHeader(const std::string& basename) const
    {
      std::string ns_classname_;
      const std::string classname = maincomp.getName();
      const std::string ns = maincomp.getNs();
      if (ns.empty())
        ns_classname_ = classname;
      else
        ns_classname_ = ns + '_' + classname;

      std::ostringstream header;
      header << "////////////////////////////////////////////////////////////////////////\n"
                "// " << basename << "\n"
                "// generated with ecppc\n"
                "// date: " << gentime <<
                "//\n\n"
                "#ifndef ECPP_COMPONENT_" << ns_classname_ << "_H\n"
                "#define ECPP_COMPONENT_" << ns_classname_ << "_H\n\n"
                "#include <tnt/ecpp.h>\n"
                "#include <tnt/convert.h>\n";

      if (!componentclass.empty())
        header << "#include \"" << componentclass << ".h\"\n";
      if (!baseclass.empty())
        header << "#include \"" << baseclass << ".h\"\n";

      header << "\n"
                "// <%pre>\n"
             << pre
             << "// </%pre>\n\n"
                "namespace ecpp_component\n"
                "{\n\n";
      if (!ns.empty())
        header << "namespace " << ns << "\n"
                  "{\n\n";
      header << "extern \"C\"\n"
                "{\n"
                "  component* create_" << classname << "(const compident& ci, const urlmapper& um,\n"
                "    comploader& cl);\n"
                "}\n\n"
                "// <%declare_shared>\n"
             << declare_shared
             << "// </%declare_shared>\n"
                "class " << classname << " : public ";
      if (componentclass.empty())
        header << "ecppComponent";
      else
        header << componentclass;
      if (!baseclass.empty())
        header << ", public " << baseclass;
      header << "\n"
                "{\n"
                "    friend component* create_" << classname << "(const compident& ci,\n"
                "      const urlmapper& um, comploader& cl);\n\n"
                "    " << classname << "& main()  { return *this; }\n\n" 
                "    // <%declare>\n"
             << declare
             << "    // </%declare>\n\n"
             << "    // <%config>\n";
      for (variable_declarations::const_iterator it = configs.begin();
           it != configs.end(); ++it)
        it->getConfigHDecl(header);
      header << "    // </%config>\n\n"
                "  protected:\n"
                "    " << classname << "(const compident& ci, const urlmapper& um, comploader& cl);\n"
                "    ~" << classname << "();\n\n"
                "  public:\n"
                "    unsigned operator() (httpRequest& request, httpReply& reply, cxxtools::query_params& qparam);\n"
                "    bool drop();\n"
                "    unsigned    getDataCount(const httpRequest& request) const;\n"
                "    unsigned    getDataLen(const httpRequest& request, unsigned n) const;\n"
                "    const char* getDataPtr(const httpRequest& request, unsigned n) const;\n";
      if (!attr.empty())
        header << "    std::string getAttribute(const std::string& name,\n"
                  "      const std::string& def = std::string()) const;\n";

      header << '\n';

      // Deklaration der Subcomponenten
      for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
        i->getHeader(header);

      // Instanzen der Subcomponenten
      for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
        header << "    " << i->getName() << "_type " << i->getName() << ";\n";

      header << "};\n\n";
      if (!ns.empty())
        header << "} // namespace " << ns << "\n\n";
      header << "} // namespace ecpp_component\n\n"
             << "#endif\n";
      return header.str();
    }

    std::string generator::getCpp(const std::string& basename) const
    {
      std::string ns_classname;
      std::string ns_classname_dot;
      std::string ns_classname_slash;
      std::string classname = maincomp.getName();
      std::string ns = maincomp.getNs();
      if (ns.empty())
      {
        ns_classname = classname;
        ns_classname_dot = classname;
        ns_classname_slash = classname;
      }
      else
      {
        ns_classname = ns + "::" + classname;
        ns_classname_dot = ns + '.' + classname;
        ns_classname_slash = ns + '/' + classname;
      }
      std::ostringstream code;
      code << "////////////////////////////////////////////////////////////////////////\n"
              "// " << basename << "\n"
              "// generated with ecppc\n"
              "// date: " << gentime <<
              "//\n\n"
              "#include <tnt/httprequest.h>\n"
              "#include <tnt/httpreply.h>\n"
              "#include <tnt/http.h>\n"
              "#include <tnt/data.h>\n";
      if (hasScopevars())
        code << "#include <tnt/objecttemplate.h>\n"
                "#include <tnt/objectptr.h>\n";
      if (!configs.empty())
        code << "#include <tnt/comploader.h>\n"
                "#include <tnt/tntconfig.h>\n";

      if (compress)
        code << "#include <tnt/zdata.h>\n";

      code << "#include <cxxtools/thread.h>\n"
              "#include <cxxtools/log.h>\n"
              "#include <stdexcept>\n\n";

      if (externData)
        code << "#include <tnt/comploader.h>\n"
                "#include <stdlib.h>\n";

      code << "#include \"" << ns_classname_slash << ".h\"\n\n"
              "template <typename T> inline void use(const T&) { };\n\n"
              "namespace ecpp_component\n"
              "{\n\n";

      if (!ns.empty())
        code << "namespace " << ns << "\n"
                "{\n\n";

      code << "static cxxtools::Mutex mutex;\n\n"
              "log_define(\"component." << ns_classname_dot << "\");\n\n";

      if (compress)
      {
        code << "static tnt::zdata raw_data(\n\"";

        uLongf s = data.size() * data.size() / 100 + 100;
        cxxtools::dynbuffer<Bytef> p;
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
             << "static tnt::data_chunks<tnt::zdata> data(raw_data);\n\n";
      }
      else
      {
        code << "static tnt::raw_data raw_data(\n\"";

        std::transform(
          data.ptr(),
          data.ptr() + data.size(),
          std::ostream_iterator<const char*>(code),
          stringescaper());

        code << "\",\n  " << data.size() << ");\n"
             << "static tnt::data_chunks<tnt::raw_data> data(raw_data);\n\n";
      }

      if (externData)
      {
        code << "#define DATA(dc, r, n) (tnt::data_chunk(dc->getDataPtr(r, n), dc->getDataLen(r, n)))\n"
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
        code << "static component* theComponent = 0;\n"
                "static unsigned refs = 0;\n\n"
                "component* create_" << classname << "(const compident& ci, const urlmapper& um, comploader& cl)\n"
                "{\n"
                "  cxxtools::MutexLock lock(mutex);\n"
                "  if (theComponent == 0)\n"
                "  {\n"
                "    log_debug(\"create new component \\\"\" << ci << '\"');\n"
                "    theComponent = new ::ecpp_component::" << ns_classname << "(ci, um, cl);\n";

        if (compress)
          code << "    raw_data.addRef();\n";

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
        code << "component* create_" << classname << "(const compident& ci, const urlmapper& um, comploader& cl)\n"
                "{\n";

        if (compress)
          code << "  raw_data.addRef();\n";

        code << "  // <%config>\n";
        if (!configs.empty())
        {
                "  if (!config_init)\n"
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
           << classname << "::" << classname << "(const compident& ci, const urlmapper& um, comploader& cl)\n"
              "  : ecppComponent(ci, um, cl)";

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
              "unsigned " << classname << "::operator() (httpRequest& request, httpReply& reply, cxxtools::query_params& qparam)\n"
           << "{\n";

      if (isDebug())
        code << "  log_trace(\"" << classname << " \" << qparam.getUrl());\n\n";

      if (raw)
        code << "  reply.setKeepAliveHeader();\n\n";
      if (!mimetype.empty())
        code << "  reply.setContentType(\"" << mimetype << "\");\n";

      if (c_time)
        code << "  {\n"
                "    std::string s = request.getHeader(tnt::httpMessage::IfModifiedSince);\n"
                "    if (s == \"" << tnt::httpMessage::htdate(c_time) << "\")\n"
                "      return HTTP_NOT_MODIFIED;\n"
                "  }\n";

      if (externData && !data.empty())
        code << "  const component* dataComponent = main().getDataComponent(request);\n\n";
      else
        code << "  const component* dataComponent = this;\n";
      code << "  ::use(dataComponent);\n";

      if (c_time)
        code << "  reply.setHeader(tnt::httpMessage::Last_Modified, \""
             << tnt::httpMessage::htdate(c_time) << "\");\n";
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
          code << "    raw_data.release();\n";
        code << "    return true;\n"
             << "  }\n"
             << "  else\n"
             << "    return false;\n";
      }
      else
      {
        code << "  delete this;\n";
        if (!externData && compress)
          code << "  raw_data.release();\n";
        code << "  return true;\n";
      }

      code << "}\n\n"
              "unsigned " << classname << "::getDataCount(const httpRequest& request) const\n"
              "{ return data.size(); }\n\n";

      if (externData)
      {
        code << "unsigned " << classname << "::getDataLen(const httpRequest& request, unsigned n) const\n"
                "{\n"
                "  const component* dataComponent = getDataComponent(request);\n"
                "  if (dataComponent == this)\n"
                "  {\n"
                "    if (n >= data.size())\n"
                "      throw std::range_error(\"range_error in " << classname << "::getDataLen\");\n"
                "    return data[n].getLength();\n"
                "  }\n"
                "  else\n"
                "    return dataComponent->getDataLen(request, n);\n"
                "}\n\n"
                "const char* " << classname << "::getDataPtr(const httpRequest& request, unsigned n) const\n"
                "{\n"
                "  const component* dataComponent = getDataComponent(request);\n"
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
        code << "unsigned " << classname << "::getDataLen(const httpRequest& request, unsigned n) const\n"
                "{\n"
                "  if (n >= data.size())\n"
                "    throw std::range_error(\"range_error in " << classname << "::getDataLen\");\n"
                "  return data[n].getLength();\n"
                "}\n\n"
                "const char* " << classname << "::getDataPtr(const httpRequest& request, unsigned n) const\n"
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

      if (!ns.empty())
        code << "} // namespace " << ns << "\n\n";
      code << "} // namespace ecpp_component\n";

      return code.str();
    }

    std::string generator::getDataCpp(const std::string& basename) const
    {
      std::ostringstream code;
      code << "////////////////////////////////////////////////////////////////////////\n"
              "// " << basename << "\n"
              "// generated with ecppc\n"
              "// date: " << gentime <<
              "//\n\n";

      if (compress)
      {
        code << "const char* " << maincomp.getName() << "_zdata = \n\"";

        uLongf s = data.size() * data.size() / 100 + 100;
        Bytef* p = new Bytef[s];
        std::auto_ptr<Bytef> ap(p);

        int z_ret = ::compress(p, &s, (const Bytef*)data.ptr(), data.size());
        if (z_ret != Z_OK)
        {
          throw std::runtime_error(std::string("error compressing data: ") +
            (z_ret == Z_MEM_ERROR ? "Z_MEM_ERROR" :
             z_ret == Z_BUF_ERROR ? "Z_BUF_ERROR" :
             z_ret == Z_DATA_ERROR ? "Z_DATA_ERROR" : "unknown error"));
        }

        std::transform(
          p, p + s,
          std::ostream_iterator<const char*>(code),
          stringescaper());
        code << "\"\n"
                "unsigned " << maincomp.getName() << "_zdatalen = " << s << ";\n";
      }
      else
      {
        code << "const char* " << maincomp.getName() << "_data = \n\"";
        std::transform(
          data.ptr(),
          data.ptr() + data.size(),
          std::ostream_iterator<const char*>(code),
          stringescaper());

        code << "\";\n";
      }
      code << "unsigned " << maincomp.getName() << "_datalen = " << data.size() << ";\n";

      return code.str();
    }
  }
}
