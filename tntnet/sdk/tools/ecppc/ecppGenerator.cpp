////////////////////////////////////////////////////////////////////////
// ecppGenerator.cpp
//

#include "ecppGenerator.h"
#include <tnt/stringescaper.h>
#include <iterator>
#include <zlib.h>
#include <cxxtools/dynbuffer.h>
#include <tnt/http.h>

////////////////////////////////////////////////////////////////////////
// htmlfilter
//
void htmlfilter::operator() (char ch)
{
  switch (state)
  {
    case state_start:
      if (ch == '<')
        state = state_token;
      else if (std::isspace(ch))
        state = state_space0;
      out << ch;
      break;

    case state_token:
      if (std::isspace(ch))
      {
        out << ' ';
        state = state_tokenspace;
      }
      else
      {
        if (ch == '>')
          state = state_space0;
        out << ch;
      }
      break;

    case state_tokenspace:
      if (!std::isspace(ch))
      {
        if (ch == '>')
          state = state_space0;
        else
          state = state_token;
        out << ch;
      }
      break;

    case state_space0:
      if (std::isspace(ch))
      {
        out << ch;
        state = state_space;
      }
      // no break!!!

    case state_space:
      if (!std::isspace(ch))
      {
        if (ch == '<')
          // hier lassen wir spaces zwischen Tags weg
          state = state_token;
        else
          // wenn zwischen Tokens was anderes ist, als spaces,
          // dann verändern wir das nicht
          state = state_data;
        out << ch;
      }
      break;

    case state_data:
      if (std::isspace(ch))
        html += ch;
      else if (ch == '<')
      {
        // spaces vor '<' wegwerfen
        html.clear();
        state = state_token;
        out << ch;
      }
      else
      {
        if (!html.empty())
        {
          out << html;
          html.clear();
        }
        out << ch;
      }
      break;
  }
}

void htmlfilter::flush()
{
  out << html;
  html.clear();
  out.flush();
}

////////////////////////////////////////////////////////////////////////
// cssfilter
//
void cssfilter::operator() (char ch)
{
  switch (state)
  {
    case state_start:
      if (ch == '/')
        state = state_slash;
      else if (std::isspace(ch))
      {
        out << ch;
        state = state_space;
      }
      else
        out << ch;
      break;

    case state_slash:
      if (ch == '*')
      {
        out << ' ';
        state = state_comment;
      }
      else
      {
        out << '/' << ch;
        state = state_start;
      }
      break;

    case state_space:
      if (ch == '/')
        state = state_slash;
      else if (!std::isspace(ch))
      {
        out << ch;
        state = state_start;
      }
      break;

    case state_comment:
      if (ch == '*')
        state = state_comment_end;
      break;

    case state_comment_end:
      if (ch == '/')
        state = state_start;
      else if (ch != '*')
        state = state_comment;
      break;
  }
}

////////////////////////////////////////////////////////////////////////
// jsfilter
//
void jsfilter::operator() (char ch)
{
  switch (state)
  {
    case state_start:
      if (ch == '/')
        state = state_slash;
      else
      {
        if (std::isspace(ch))
        {
          state = state_space;
        }
        else if (ch == '"' || ch == '\'')
        {
          delim = ch;
          state = state_string;
        }
        out << ch;
      }
      break;

    case state_slash:
      if (ch == '*')
      {
        out << ' ';
        state = state_comment;
      }
      else if (ch == '/')
      {
        out << ' ';
        state = state_comment1;
      }
      else
      {
        out << '/' << ch;
        state = state_start;
      }
      break;

    case state_space:
      if (ch == '/')
        state = state_slash;
      else if (!std::isspace(ch))
      {
        out << ch;
        state = state_start;
      }
      break;

    case state_comment:
      if (ch == '*')
        state = state_comment_end;
      break;

    case state_comment_end:
      if (ch == '/')
        state = state_space;
      else if (ch != '*')
        state = state_comment;
      break;

    case state_comment1:
      if (ch == '\n')
        state = state_space;
      break;

    case state_string:
      if (ch == '\\')
        state = state_stringesc;
      else
      {
        out << ch;
        if (ch == delim)
          state = state_start;
      }
      break;

    case state_stringesc:
      out << ch;
      break;
  }
}

////////////////////////////////////////////////////////////////////////
// ecppGenerator
//
ecppGenerator::ecppGenerator()
  : singleton(true),
    raw(false),
    currentComp(&maincomp),
    filter(filter_null),
    compress(false),
    c_time(0)
{ }

void ecppGenerator::processHtml(const std::string& html)
{
  std::ostringstream d;
  switch (filter)
  {
    case filter_null:
      d << html;
      break;

    case filter_html:
      for_each(html.begin(), html.end(), htmlfilter(d)).flush();
      break;

    case filter_css:
      for_each(html.begin(), html.end(), cssfilter(d));
      break;

    case filter_js:
      for_each(html.begin(), html.end(), jsfilter(d));
      break;

  }

  std::string chunk = d.str();
  data.push_back(chunk);
  if (!html.empty())
  {
    std::ostringstream m;

    m << "  reply.out() << DATA(" << (data.count() - 1) << "); // ";

    std::transform(
      chunk.begin(),
      chunk.end(),
      std::ostream_iterator<const char*>(m),
      stringescaper(false));

    m << '\n';
    currentComp->main += m.str();
  }
}

void ecppGenerator::processExpression(const std::string& expr)
{
  std::ostringstream m;
  m << "  reply.out() << (" << expr << ");\n";
  currentComp->main += m.str();
}

void ecppGenerator::processCpp(const std::string& code)
{
  currentComp->main += code;
}

void ecppGenerator::processPre(const std::string& code)
{
  pre += code;
}

void ecppGenerator::processDeclare(const std::string& code)
{
  singleton = false;
  declare += code;
}

void ecppGenerator::processInit(const std::string& code)
{
  init += code;
}

void ecppGenerator::processCleanup(const std::string& code)
{
  cleanup += code;
}

void ecppGenerator::processArg(const std::string& name,
  const std::string& value)
{
  std::ostringstream a;
  a << "  std::string "
    << name
    << " = qparam.param(\""
    << name << '"';
  if (!value.empty())
    a << ", " << value;
  a << ");\n";

  currentComp->args += a.str();
}

void ecppGenerator::processAttr(const std::string& name,
  const std::string& value)
{
  if (attr.find(name) != attr.end())
    throw std::runtime_error("duplicate attr " + name);
  attr.insert(attr_type::value_type(name, value));
}

void ecppGenerator::processCall(const std::string& comp,
  const comp_args_type& args, const std::string& pass_cgi,
  const std::string& cppargs)
{
  std::ostringstream m;
  m << "  // <& " << comp << " ...\n";

  if (args.empty())
  {
    if (pass_cgi.empty())
    {
      m << "  {\n"
        << "    query_params cq(qparam, false);\n";
      if (comp[0] == '.')
      {
        m << "    " << comp.substr(1) << "(request, reply, cq";
        if (!cppargs.empty())
          m << ", " << cppargs;
        m << ");\n";
      }
      else
        m << "    callComp(" << comp << ", request, reply, cq);\n";
      m << "  }\n";
    }
    else
    {
      if (comp[0] == '.')
      {
        m << "    " << comp.substr(1) << "(request, reply, " << pass_cgi;
        if (!cppargs.empty())
          m << ", " << cppargs;
        m << ");\n";
      }
      else
        m << "  callComp(" << comp << ", request, reply, " << pass_cgi << ");\n";
    }
  }
  else
  {
    m << "  {\n";
    if (pass_cgi.empty())
      m << "    query_params cq(qparam, false);\n";
    else
      m << "    query_params cq(" << pass_cgi << ");\n";

    for (comp_args_type::const_iterator i = args.begin();
         i != args.end(); ++i)
      m << "    cq.add(\"" << i->first << "\", "
        << i->second << ");\n";

    if (comp[0] == '.')
    {
      m << "    " << comp.substr(1) << "(request, reply, cq";
      if (!cppargs.empty())
        m << ", " << cppargs;
      m << ");\n";
    }
    else
      m << "    callComp(" << comp << ", request, reply, cq);\n";
    m << "  }\n";
  }

  m << "  // &>\n";

  currentComp->main += m.str();
}

void ecppGenerator::processDeclareShared(const std::string& code)
{
  declare_shared += code;
}

void ecppGenerator::processShared(const std::string& code)
{
  shared += code;
}

void ecppGenerator::startComp(const std::string& name,
  const cppargs_type& cppargs)
{
  subcomps.push_back(subcomp());
  subcomp& s = subcomps.back();
  s.name = name;
  s.cppargs = cppargs;
  currentComp = &s;
}

void ecppGenerator::processComp(const std::string& code)
{
  currentComp = &maincomp;
}

void ecppGenerator::processCondExpr(const std::string& cond, const std::string& expr)
{
  std::ostringstream m;
  m << "  if (" << cond << ")\n"
       "    reply.out() << (" << expr << ");\n";
  currentComp->main += m.str();
}

std::string ecppGenerator::getHeader(const std::string& basename,
  const std::string& classname)
{
  std::ostringstream header;
  header << "////////////////////////////////////////////////////////////////////////\n"
         << "// " << basename << '\n'
         << "// generated with ecppc\n"
         << "//\n\n"
         << "#ifndef ECPP_COMPONENT_" << classname << "_H\n"
         << "#define ECPP_COMPONENT_" << classname << "_H\n\n"
         << "#include <tnt/ecpp.h>\n\n";

  if (!componentclass.empty())
    header << "#include \"" << componentclass << ".h\"\n";
  if (!baseclass.empty())
    header << "#include \"" << baseclass << ".h\"\n";

  header << "\n"
         << "// <%pre>\n"
         << pre
         << "// </%pre>\n\n"
         << "namespace ecpp_component\n"
         << "{\n"
         << "extern \"C\"\n"
         << "{\n"
         << "  component* create_" << classname << "(const compident& ci, const urlmapper& um,\n"
         << "    comploader& cl);\n"
         << "}\n\n"
         << "// <%declare_shared>\n"
         << declare_shared
         << "// </%declare_shared>\n"
         << "class " << classname << " : public ";
  if (componentclass.empty())
    header << "ecppComponent";
  else
    header << componentclass;
  if (!baseclass.empty())
    header << ", public " << baseclass;
  header << "\n"
            "{\n"
            "    friend component* create_" << classname << "(const compident& ci,\n"
            "      const urlmapper& um, comploader& cl);\n"
            "    // <%declare>\n\n"
         << declare
         << "    // </%declare>\n\n";
  if (externData)
    header << "  private:\n"
              "    mutable compident dataCompident;\n"
              "    component* getDataComponent() const;\n\n";
  header << "  protected:\n"
            "    " << classname << "(const compident& ci, const urlmapper& um, comploader& cl);\n"
            "    ~" << classname << "();\n\n"
            "  public:\n"
            "    unsigned operator() (const httpRequest& request, httpReply& reply, query_params& qparam);\n";
  for (subcomps_type::iterator i = subcomps.begin(); i != subcomps.end(); ++i)
  {
    header << "    unsigned " << i->name << "(const httpRequest& request, httpReply& reply, query_params& qparam";
    for (cppargs_type::const_iterator j = i->cppargs.begin();
         j != i->cppargs.end(); ++j)
    {
      header << ", " << j->first;
      if (!j->second.empty())
        header << '=' << j->second;
    }
    header << ");\n";
  }
  header << "    bool drop();\n"
         << "    unsigned    getDataCount() const;\n"
         << "    unsigned    getDataLen(unsigned n) const;\n"
         << "    const char* getDataPtr(unsigned n) const;\n";
  if (!attr.empty())
    header << "    std::string getAttribute(const std::string& name,\n"
              "      const std::string& def = std::string()) const;\n";

  header << "};\n\n"
         << "}; // namespace ecpp_component\n\n"
         << "#endif\n";
  return header.str();
}

std::string ecppGenerator::getCpp(const std::string& basename,
  const std::string& classname)
{
  std::ostringstream code;
  code << "////////////////////////////////////////////////////////////////////////\n"
       << "// " << basename << '\n'
       << "// generated with ecppc\n"
       << "//\n\n"
       << "#include <tnt/http.h>\n"
       << "#include <tnt/data.h>\n";

  if (compress)
    code << "#include <tnt/zdata.h>\n";

  code << "#include <cxxtools/thread.h>\n"
          "#include <stdexcept>\n";

  if (externData)
    code << "#include <tnt/comploader.h>\n"
            "#include <stdlib.h>\n";

  if (isDebug())
    code << "#include <tnt/log.h>\n";

  code << "#include \"" << classname << ".h\"\n\n";

  code << "namespace ecpp_component\n"
       << "{\n"
       << "static Mutex mutex;\n\n";

  if (compress)
  {
    code << "static tnt::zdata raw_data(\n\"";

    uLongf s = data.size() * data.size() / 100 + 100;
    DynBuffer p;
    p.reserve(s);

    int z_ret = ::compress((Bytef*)p.data(), &s,
      (const Bytef*)data.ptr(), data.size());

    if (z_ret != Z_OK)
    {
      throw std::runtime_error(std::string("error compressing data: ") +
        (z_ret == Z_MEM_ERROR ? "Z_MEM_ERROR" :
         z_ret == Z_BUF_ERROR ? "Z_BUF_ERROR" :
         z_ret == Z_DATA_ERROR ? "Z_DATA_ERROR" : "unknown error"));
    }

    std::transform(
      (Bytef*)p.data(), (Bytef*)p.data() + s,
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
    code << "#define DATA(n) tnt::data_chunk(dataComponent->getDataPtr(n), dataComponent->getDataLen(n))\n"
            "#define DATA_COUNT() dataComponent->getDataCount()\n\n";
     
  }
  else
  {
    code << "#define DATA(n) data[n]\n"
            "#define DATA_COUNT() data.size()\n\n";
  }

  if (singleton)
  {
    code << "static component* theComponent = 0;\n"
         << "static unsigned refs = 0;\n\n"
         << "component* create_" << classname << "(const compident& ci, const urlmapper& um, comploader& cl)\n"
         << "{\n"
         << "  MutexLock lock(mutex);\n"
         << "  if (theComponent == 0)\n"
         << "  {\n"
         << "    theComponent = new ecpp_component::" << classname << "(ci, um, cl);\n";

    if (compress)
      code << "    raw_data.addRef();\n";

    code << "    refs = 1;\n"
         << "  }\n"
		 << "  else\n"
		 << "    ++refs;\n"
         << "  return theComponent;\n"
         << "}\n\n";
  }
  else
  {
    code << "component* create_" << classname << "(const compident& ci, const urlmapper& um, comploader& cl)\n"
            "{\n";

    if (compress)
      code << "  raw_data.addRef();\n";

    code << "  return new " << classname << "(ci, um, cl);\n"
         << "}\n\n";
  }

  code << "// <%shared>\n"
       << shared
       << "// </%shared>\n\n"
       << classname << "::" << classname << "(const compident& ci, const urlmapper& um, comploader& cl)\n"
          "  : ecppComponent(ci, um, cl)\n"
          "{\n"
          "  // <%init>\n"
       << init
       << "  // </%init>\n"
          "}\n\n"
       << classname << "::~" << classname << "()\n"
          "{\n"
          "  // <%cleanup>\n"
       << cleanup
       << "  // </%cleanup>\n"
          "}\n\n";

  if (externData)
    code << "component* " << classname << "::getDataComponent() const\n"
            "{\n"
            "  if (dataCompident.empty())\n"
            "  {\n"
            "    MutexLock lock(mutex);\n"
            "    if (dataCompident.empty())\n"
            "    {\n"
            "      const char* LANG = ::getenv(\"LANG\");\n"
            "      if (LANG)\n"
            "      {\n"
            "        try\n"
            "        {\n"
            "          dataCompident = compident(getCompident().libname + '.' + LANG,\n"
            "                                    getCompident().compname);\n"
            "          return &fetchComp(dataCompident);\n"
            "        }\n"
            "        catch (const dl::error&)\n"
            "        {\n"
            "          dataCompident = getCompident();\n"
            "        }\n"
            "      }\n"
            "      else\n"
            "        dataCompident = getCompident();\n"
            "    }\n"
            "  }\n"
            "\n"
            "  return &fetchComp(dataCompident);\n"
            "}\n\n";

  code << "unsigned " << classname << "::operator() (const httpRequest& request, httpReply& reply, query_params& qparam)\n"
       << "{\n";

  if (isDebug())
    code << "  log_debug_ns(compcall, \"execute " << classname << " \" << qparam.getUrl());\n";

  if (externData && !data.empty())
    code << "  component* dataComponent = getDataComponent();\n";

  if (!mimetype.empty())
    code << "  reply.setContentType(\"" << mimetype << "\");\n";
  if (c_time)
    code << "  reply.setHeader(\"Last-Modified\", \""
         << tnt::httpMessage::htdate(c_time) << "\");\n";
  if (raw)
    code << "  reply.setDirectMode();\n";

  code << maincomp.getBody(isDebug() ? classname : std::string())
       << "}\n\n";

  for (subcomps_type::iterator i = subcomps.begin(); i != subcomps.end(); ++i)
  {
    code << "unsigned " << classname << "::" << i->name << "(const httpRequest& request, httpReply& reply, query_params& qparam";
    for (cppargs_type::const_iterator j = i->cppargs.begin();
         j != i->cppargs.end(); ++j)
      code << ", " << j->first;
    code << ")\n"
            "{\n";

    if (isDebug())
      code << "  log_debug_ns(compcall, \"execute " << classname << "::" << i->name << " \" << qparam.getUrl());\n";

    if (externData && !data.empty())
      code << "  component* dataComponent = getDataComponent();\n";

    code << i->getBody(isDebug() ? classname + "::" + i->name : std::string())
         << "}\n\n";
  }
  code << "bool " << classname << "::drop()\n"
       << "{\n";
  if (singleton)
  {
    code << "  MutexLock lock(mutex);\n"
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
          "unsigned " << classname << "::getDataCount() const\n"
          "{ return data.size(); }\n\n";

  if (externData)
  {
    code << "unsigned " << classname << "::getDataLen(unsigned n) const\n"
            "{\n"
            "  component* dataComponent = getDataComponent();\n"
            "  if (dataComponent == this)\n"
            "  {\n"
            "    if (n >= data.size())\n"
            "      throw std::range_error(\"range_error in " << classname << "::getDataLen\");\n"
            "    return data[n].getLength();\n"
            "  }\n"
            "  else\n"
            "    return dataComponent->getDataLen(n);\n"
            "}\n\n"
            "const char* " << classname << "::getDataPtr(unsigned n) const\n"
            "{\n"
            "  component* dataComponent = getDataComponent();\n"
            "  if (dataComponent == this)\n"
            "  {\n"
            "    if (n >= data.size())\n"
            "      throw std::range_error(\"range_error in " << classname << "::getDataPtr\");\n"
            "    return data[n].getData();\n"
            "  }\n"
            "  else\n"
            "    return dataComponent->getDataPtr(n);\n"
            "}\n\n";
  }
  else
  {
    code << "unsigned " << classname << "::getDataLen(unsigned n) const\n"
            "{\n"
            "  if (n >= data.size())\n"
            "    throw std::range_error(\"range_error in " << classname << "::getDataLen\");\n"
            "  return data[n].getLength();\n"
            "}\n\n"
            "const char* " << classname << "::getDataPtr(unsigned n) const\n"
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

  code << "}; // namespace ecpp_component\n";

  return code.str();
}

std::string ecppGenerator::getDataCpp(const std::string& basename,
  const std::string& classname)
{
  std::ostringstream code;
  code << "////////////////////////////////////////////////////////////////////////\n"
          "// " << basename << "\n"
          "// generated with makerequest\n"
          "//\n\n";

  if (compress)
  {
    code << "const char* " << classname << "_zdata = \n\"";

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
      (Bytef*)p, (Bytef*)p + s,
      std::ostream_iterator<const char*>(code),
      stringescaper());
    code << "\"\n"
            "unsigned " << classname << "_zdatalen = " << s << ";\n";
  }
  else
  {
    code << "const char* " << classname << "_data = \n\"";
    std::transform(
      data.ptr(),
      data.ptr() + data.size(),
      std::ostream_iterator<const char*>(code),
      stringescaper());

    code << "\";\n";
  }
  code << "unsigned " << classname << "_datalen = " << data.size() << ";\n";

  return code.str();
}

std::string ecppGenerator::comp::getBody(const std::string& signature) const
{
  std::ostringstream body;
  body << "  // <%args>\n"
       << args
       << "  // </%args>\n\n"
       << "  // <%cpp>\n"
       << main
       << "  // <%/cpp>\n";

  if (!signature.empty())
    body << "  log_debug_ns(compcall, \"" << signature << " OK\");\n";

  body << "  return HTTP_OK;\n";
  return body.str();
}
