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


#include "tnt/ecpp/parser.h"
#include "tnt/ecpp/parsehandler.h"
#include <sstream>
#include <fstream>
#include <cctype>
#include <cxxtools/log.h>

log_define("tntnet.parser")

namespace tnt
{
  namespace
  {
    inline bool isVariableNameChar(char ch)
    {
      return std::isalnum(ch) || ch == '_';
    }
  }

  namespace ecpp
  {
    static const char split_start = '{';
    static const char split_end = '}';

    void Parser::doInclude(const std::string& file)
    {
      log_debug("include \"" << file << '"');

      std::string fullname = file;
      std::ifstream inp(file.c_str());

      for (includes_type::const_iterator it = includes.begin();
           !inp && it != includes.end(); ++it)
      {
        fullname = *it + '/' + file;
        log_debug("try include \"" << fullname << '"');
        inp.open(fullname.c_str());
      }

      if (!inp)
      {
        std::ostringstream msg;
        throw std::runtime_error("cannot open include file \"" + file + '"');
      }

      std::string curfileSave = curfile;
      unsigned curlineSave = curline;
      curfile = fullname;
      curline = 0;

      log_debug("onInclude(\"" << fullname << "\")");
      handler.onInclude(fullname);

      parsePriv(inp);

      curfile = curfileSave;
      curline = curlineSave;

      log_debug("onIncludeEnd(\"" << fullname << "\")");
      handler.onIncludeEnd(fullname);
    }

    void Parser::parsePriv(std::istream& in)
    {
      enum state_type {
        state_html0,  // 0
        state_html,
        state_htmlesc,
        state_tagstart,
        state_expr,
        state_expre0,
        state_tag,
        state_tagarg0,
        state_tagarg,
        state_tagarge,
        state_defarg0,  // 10
        state_defarg,
        state_defargdefval0,
        state_defargdefval,
        state_defarg_end,
        state_cpp,
        state_cppse,
        state_cppe0,
        state_cppe1,
        state_cppetag,
        state_cppstring,  // 20
        state_cppstringesc,
        state_cppchar,
        state_cppcharesc,
        state_cppchare,
        state_cppcomment0,
        state_cppcomment,
        state_cppcommentc,
        state_cppcommentce,
        state_nl,
        state_cpp1,
        state_args0,
        state_args0comment,
        state_argsvar,  // 30
        state_argsvare,
        state_argsval,
        state_argsvalstring,
        state_argscomment0,
        state_argscomment,
        state_argsvalcomment0,
        state_attr0,
        state_attr0comment,
        state_attrvar,
        state_attrvare,  // 40
        state_attrval,
        state_attrvalstring,
        state_attrcomment0,
        state_attrcomment,
        state_attrvalcomment0,
        state_call0,
        state_callname_expr,
        state_callname_string,
        state_callname,
        state_call_cpparg0,  // 50
        state_call_cpparg1,
        state_call_cpparg_pe,
        state_call_cpparg_sp,
        state_call_cpparg_e,
        state_callend,
        state_callarg0,
        state_callarg,
        state_callarge,
        state_callval_expr,
        state_callval_string,  // 60
        state_callval_word,
        state_callval0,
        state_callvale,
        state_comment,
        state_commente,
        state_compe0,
        state_compe,
        state_cond0,
        state_cond,
        state_condexpr,
        state_condexpre,  // 70
        state_include0,
        state_include1,
        state_scopearg0,
        state_scopearg,
        state_scopeargeq,
        state_scopeargval0,
        state_scopeargval,
        state_scopevale,
        state_scope0,
        state_scope,  // 80
        state_scopeinit,
        state_scopeiniteq,
        state_scopee,
        state_scopee0,
        state_scopecomment0,
        state_scopecomment,
        state_endcall0,
        state_endcall,
        state_endcalle,
        state_doc,  // 90
        state_doce
      };

      state_type state = state_nl;
      std::string tag, etag, tagarg;
      std::string html, code, arg, argtype, value;
      std::string comp;
      std::string cond, expr;
      comp_args_type comp_args;
      std::string pass_cgi;
      std::string defarg, defval, paramname;
      cppargs_type cppargs;
      paramargs_type paramargs;
      unsigned bracket_count = 0;
      std::string scopetype, scopevar, scopeinit;
      scope_container_type scope_container = application_container;
      scope_type scope = global_scope;
      bool inComp = false;
      bool inClose = false;
      bool htmlExpr = false;
      bool splitBar = false;

      handler.start();

      char ch;
      while (in.get(ch))
      {
        if (ch == '\n')
          ++curline;

        switch(state)
        {
          case state_html0:
            if (ch == ' ' || ch == '\t')
              std::cerr << curfile << ':' << curline + 1 << ": warning: trailing white space after closing tag" << std::endl;
            else if (ch == '\r')
              std::cerr << curfile << ':' << curline + 1 << ": warning: trailing cr after closing tag (dos format?)" << std::endl;
            // no break - continue with state_html

          case state_html:
            if (ch == '<')
            {
              state = state_tagstart;
            }
            else if (ch == '\n')
            {
              if (state == state_html || !html.empty())
                html += ch;
              state = state_nl;
            }
            else if (splitBar && (ch == split_start|| ch == split_end))
            {
              if (!html.empty() || ch == split_end)
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              handler.tokenSplit(ch == split_start);
            }
            else if (ch == '\\')
              state = state_htmlesc;
            else
            {
              html += ch;
              state = state_html;
            }
            break;

          case state_htmlesc:
            if (ch != '\n')
              html += ch;
            state = state_html;
            break;

          case state_tagstart:
            if (ch == '%')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              tag.clear();
              handler.onLine(curline, curfile); //#
              state = state_tag;
            }
            else if (ch == '$')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              handler.onLine(curline, curfile); //#
              state = state_expr;
            }
            else if (ch == '&')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              handler.onLine(curline, curfile); //#
              state = state_call0;
            }
            else if (ch == '#')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              state = state_comment;
            }
            else if (ch == '{')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              handler.onLine(curline, curfile); //#
              state = state_cpp;
            }
            else if (ch == '?')
            {
              if (!html.empty())
              {
                handler.onHtml(html);
                html.clear();
              }
              handler.onLine(curline, curfile); //#
              state = state_cond0;
            }
            else if (ch == '/')
            {
              state = state_compe0;
            }
            else if (ch == '\\')
            {
              html += '<';
              state = state_html;
            }
            else
            {
              html += '<';
              html += ch;
              state = state_html;
            }
            break;

          case state_expr:
            if (ch == '$')
            {
              if (!htmlExpr && code.empty())
                // <$$ ... $>
                htmlExpr = true;
              else
                // expression might end
                state = state_expre0;
            }
            else
              code += ch;
            break;

          case state_expre0:
            if (ch == '>')
            {
              // expression ends, html continues
              if (htmlExpr)
              {
                log_debug("onHtmlExpression(\"" << code << "\")");
                handler.onHtmlExpression(code);
                htmlExpr = false;
              }
              else
              {
                log_debug("onExpression(\"" << code << "\")");
                handler.onExpression(code);
              }
              code.clear();
              state = state_html;
            }
            else
            {
              // expression does not end
              code += '$';
              code += ch;
              if (ch != '$')
                state = state_expr;
            }
            break;

          case state_tag:
            if (ch == '>')
            {
              if (tag == "args" || tag == "get" || tag == "post" || tag == "config")
                state = state_args0;
              else if (tag == "attr")
                state = state_attr0;
              else if (tag == "include")
                state = state_include0;
              else if (tag == "application")
              {
                scope_container = application_container;
                scope = component_scope;
                state = state_scope0;
              }
              else if (tag == "thread")
              {
                scope_container = thread_container;
                scope = component_scope;
                state = state_scope0;
              }
              else if (tag == "session")
              {
                scope_container = session_container;
                scope = component_scope;
                state = state_scope0;
              }
              else if (tag == "securesession")
              {
                scope_container = secure_session_container;
                scope = component_scope;
                state = state_scope0;
              }
              else if (tag == "request")
              {
                scope_container = request_container;
                scope = component_scope;
                state = state_scope0;
              }
              else if (tag == "param")
              {
                scope_container = param_container;
                scope = component_scope;
                state = state_scope0;
              }
              else if (!inClose && tag == "close")
              {
                handler.startClose();
                state = state_html0;
                inClose = true;
              }
              else if (tag == "i18n")
              {
                splitBar = true;
                handler.startI18n();
                state = state_html0;
              }
              else if (tag == "doc")
                state = state_doc;
              else
                state = state_cpp;
            }
            else if (!inComp && tag == "def" && std::isspace(ch))
              state = state_tagarg0;
            else if (std::isspace(ch))
            {
              if (tag == "application")
              {
                scope_container = application_container;
                state = state_scopearg0;
              }
              else if (tag == "thread")
              {
                scope_container = thread_container;
                state = state_scopearg0;
              }
              else if (tag == "session")
              {
                scope_container = session_container;
                state = state_scopearg0;
              }
              else if (tag == "securesession")
              {
                scope_container = secure_session_container;
                state = state_scopearg0;
              }
              else if (tag == "request")
              {
                scope_container = request_container;
                state = state_scopearg0;
              }
              else if (tag == "param")
              {
                scope_container = param_container;
                state = state_scopearg0;
              }
              else
              {
                state_type s = state;
                state = state_html0;
                throw parse_error("unknown tag " + tag, s, curfile, curline);
              }
            }
            else
              tag += ch;
            break;

          case state_tagarg0:
            if (ch == '>')
              state = state_cpp;
            else if (!std::isspace(ch))
            {
              tagarg = ch;
              state = state_tagarg;
            }
            break;

          case state_tagarg:
            if (std::isspace(ch))
              state = state_tagarge;
            else if (tag == "def" && ch == '(')
              state = state_defarg0;
            else if (ch == '>')
            {
              if (tag == "args" || tag == "get" || tag == "post")
                state = state_args0;
              else if (tag == "attr")
                state = state_attr0;
              else if (tag == "def")
              {
                handler.startComp(tagarg, cppargs);
                state = state_html0;
                inComp = true;
              }
              else if (tag == "close")
              {
                handler.startClose();
                state = state_html0;
                inClose = true;
              }
              else if (tag == "i18n")
              {
                splitBar = true;
                handler.startI18n();
                state = state_html0;
              }
              else
                state = state_cpp;
            }
            else
              tagarg += ch;
            break;

          case state_tagarge:
            if (ch == '>')
            {
              if (tag == "args" || tag == "get" || tag == "post")
                state = state_args0;
              else if (tag == "attr")
                state = state_attr0;
              else if (tag == "def")
              {
                handler.startComp(tagarg, cppargs);
                state = state_html0;
                inComp = true;
              }
              else if (tag == "close")
              {
                handler.startClose();
                state = state_html0;
                inClose = true;
              }
              else if (tag == "i18n")
              {
                splitBar = true;
                handler.startI18n();
                state = state_html0;
              }
              else
                state = state_cpp;
            }
            else if (tag == "def" && ch == '(')
              state = state_defarg0;
            break;

          case state_defarg0:
            if (ch == ')')
              state = state_defarg_end;
            else if (!std::isspace(ch))
            {
              defarg += ch;
              state = state_defarg;
            }
            break;

          case state_defarg:
            if (ch == '=')
              state = state_defargdefval0;
            else if (ch == ')')
            {
              cppargs.push_back(std::make_pair(defarg, std::string()));
              defarg.clear();
              state = state_defarg_end;
            }
            else if (ch == ',')
            {
              cppargs.push_back(std::make_pair(defarg, std::string()));
              defarg.clear();
              state = state_defarg0;
            }
            else
              defarg += ch;
            break;

          case state_defargdefval0:
            if (!std::isspace(ch))
            {
              defval = ch;
              state = state_defargdefval;
            }
            break;

          case state_defargdefval:
            if (ch == ')')
            {
              cppargs.push_back(std::make_pair(defarg, defval));
              defarg.clear();
              defval.clear();
              state = state_defarg_end;
            }
            else if (ch == ',')
            {
              cppargs.push_back(std::make_pair(defarg, defval));
              defarg.clear();
              defval.clear();
              state = state_defarg;
            }
            else
              defval += ch;
            break;

          case state_defarg_end:
            if (ch == '>')
            {
              handler.startComp(tagarg, cppargs);
              inComp = true;
              cppargs.clear();
              state = state_html0;
            }
            else if (!std::isspace(ch))
            {
              state_type s = state;
              state = state_html0;
              throw parse_error("def", s, curfile, curline);
            }
            break;

          case state_cppcomment0:
            if (ch == '/')
            {
              code += "//";
              state = state_cppcomment;
              break;
            }
            else if (state == '*')
            {
              code += "/*";
              state = state_cppcommentc;
              break;
            }

            code += '/';
            state = state_cpp;

            // no break

          case state_cpp:
            if (ch == '<')
              state = state_cppe0;
            else if (ch == '}')
              state = state_cppse;
            else if (ch == '/')
              state = state_cppcomment0;
            else
            {
              code += ch;
              if (ch == '"')
                state = state_cppstring;
              else if (ch == '\'')
                state = state_cppchar;
            }
            break;

          case state_cppstring:
            code += ch;
            if (ch == '"')
              state = state_cpp;
            else if (ch == '\\')
              state = state_cppstringesc;
            break;

          case state_cppstringesc:
            code += ch;
            state = state_cppstring;
            break;

          case state_cppchar:
            code += ch;
            if (ch == '\\')
              state = state_cppcharesc;
            else
              state = state_cppchare;
            break;

          case state_cppcharesc:
            code += ch;
            state = state_cppchare;
            break;

          case state_cppchare:
            code += ch;
            if (ch == '\'')
              state = state_cpp;
            break;

          case state_cppcomment:
            code += ch;
            if (ch == '\n')
              state = state_cpp;
            break;

          case state_cppcommentc:
            code += ch;
            if (ch == '*')
              state = state_cppcommentce;
            break;

          case state_cppcommentce:
            code += ch;
            if (ch == '/')
              state = state_cpp;
            break;

          case state_cppse:
            if (ch == '>')
            {
              log_debug("onCpp(\"" << code << "\")");
              handler.onCpp(code);
              code.clear();
              state = state_html;
            }
            else
            {
              code += '}';
              code += ch;
              state = state_cpp;
            }
            break;

          case state_cppe0:
            if (ch == '/')
            {
              etag.clear();
              state = state_cppe1;
            }
            else
            {
              code += '<';
              code += ch;
              state = state_cpp;
            }
            break;

          case state_cppe1:
            if (ch == '%')
              state = state_cppetag;
            else
            {
              code += "</";
              code += ch;
              state = state_cpp;
            }
            break;

          case state_cppetag:
            if (ch == '>')
            {
              if (tag != etag)
              {
                state_type s = state;
                state = state_html0;
                throw parse_error("invalid end-tag - "
                  + tag + " expected, "
                  + etag + " found", s, curfile, curline);
              }
              else if (tag == "pre")
              {
                log_debug("onPre(\"" << code << "\")");
                handler.onPre(code);
              }
              else if (tag == "init")
              {
                log_debug("onInit(\"" << code << "\")");
                handler.onInit(code);
              }
              else if (tag == "cleanup")
              {
                log_debug("onCleanup(\"" << code << "\")");
                handler.onCleanup(code);
              }
              else if (tag == "shared")
              {
                log_debug("onShared(\"" << code << "\")");
                handler.onShared(code);
              }
              else if (tag == "cpp")
              {
                log_debug("onCpp(\"" << code << "\")");
                handler.onCpp(code);
              }
              else if (tag == "out")
              {
                handler.onHtmlExpression(code);
              }
              else if (tag == "sout")
              {
                handler.onExpression(code);
              }
              else if (tag == "args"
                || tag == "get"
                || tag == "post"
                || tag == "attr"
                || tag == "config"
                || tag == "include"
                || tag == "session"
                || tag == "securesession"
                || tag == "application"
                || tag == "thread"
                || tag == "request"
                || tag == "param"
                || tag == "doc")
                ;
              else
              {
                state_type s = state;
                state = state_html0;
                throw parse_error("unknown tag " + tag, s, curfile, curline);
              }
              code.clear();
              state = state_html0;
            }
            else
            {
              etag += ch;
            }
            break;

          case state_nl:
            if (ch == '<')
            {
              state = state_tagstart;
            }
            else if (ch == '%')
            {
              // start of oneline-cpp
              handler.onLine(curline, curfile); //#
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              state = state_cpp1;
            }
            else if (ch == '\\')
              state = state_htmlesc;
            else if (splitBar && (ch == split_start|| ch == split_end))
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              handler.tokenSplit(ch == split_start);
              state = state_html;
            }
            else
            {
              html += ch;
              if (ch != '\n')
                state = state_html;
            }
            break;

          case state_cpp1:
            if (ch == '\n')
            {
              code += '\n';
              log_debug("onCpp(\"" << code << "\")");
              handler.onCpp(code);
              code.clear();
              state = state_nl;
            }
            else
            {
              code += ch;
            }
            break;

          case state_args0:
            if (ch == '<')
              state = state_cppe0;
            else if (ch == '/')
              state = state_args0comment;
            else if (!std::isspace(ch))
            {
              arg = ch;
              state = state_argsvar;
            }
            break;

          case state_args0comment:
            if (ch == '/')
              state = state_argscomment;
            else
              throw parse_error("7", state, curfile, curline);
            break;

          case state_argsvar:
            if (ch == '=')
            {
              value.clear();
              state = state_argsval;
            }
            else if (ch == '/')
            {
              processNV(tag, arg, std::string());
              arg.clear();
              state = state_argscomment0;
            }
            else if (ch == ';')
            {
              processNV(tag, arg, std::string());
              arg.clear();
              state = state_args0;
            }
            else
              arg += ch;
            break;

          case state_argsvare:
            if (ch == '=')
            {
              value.clear();
              state = state_argsval;
            }
            else if (ch == '\n' || ch == ';')
            {
              processNV(tag, arg, std::string());
              arg.clear();
              state = state_args0;
              if (ch == '\n')
                std::cerr << curfile << ':' << curline + 1 << ": warning: old syntax: ';' missing" << std::endl;
            }
            else if (!std::isspace(ch))
            {
              processNV(tag, arg, std::string());
              arg.clear();

              if (ch == '<')
                state = state_cppe0;
              else if (ch == '/')
                state = state_argscomment0;
              else
              {
                arg = ch;
                state = state_argsvar;
              }
            }
            break;

          case state_argsval:
            if (ch == '\n' || ch == ';')
            {
              processNV(tag, arg, value);
              arg.clear();
              value.clear();
              state = state_args0;
              if (ch == '\n')
                std::cerr << curfile << ':' << curline << ": warning: old syntax: ';' missing" << std::endl;
            }
            else if (ch == '"')
            {
              value += ch;
              state = state_argsvalstring;
            }
            else if (ch == '/')
              state = state_argsvalcomment0;
            else
              value += ch;
            break;

          case state_argsvalstring:
            if (ch == '"')
              state = state_argsval;
            value += ch;
            break;

          case state_argscomment0:
            if (ch == '/')
              state = state_argscomment;
            else
              throw parse_error("6", state, curfile, curline);
            break;

          case state_argscomment:
            if (ch == '\n')
              state = state_args0;
            break;

          case state_argsvalcomment0:
            if (ch == '/')
            {
              processNV(tag, arg, value);
              arg.clear();
              value.clear();
              state = state_argscomment;
              if (ch == '\n')
                std::cerr << curfile << ':' << curline << ": warning: old syntax: ';' missing" << std::endl;
            }
            else
            {
              value += '/';
              value += ch;
              state = state_argsval;
            }
            break;

          case state_attr0:
            if (ch == '<')
              state = state_cppe0;
            else if (ch == '/')
              state = state_attr0comment;
            else if (!std::isspace(ch))
            {
              arg = ch;
              state = state_attrvar;
            }
            break;

          case state_attr0comment:
            if (ch == '/')
              state = state_attrcomment;
            else
              throw parse_error("7", state, curfile, curline);
            break;

          case state_attrvar:
            if (ch == '=')
            {
              value.clear();
              state = state_attrval;
            }
            else if (std::isspace(ch))
              state = state_attrvare;
            else
              arg += ch;
            break;

          case state_attrvare:
            if (ch == '=')
            {
              if (arg.empty())
                throw parse_error("name expected", state, curfile, curline);
              value.clear();
              state = state_attrval;
            }
            else if (!std::isspace(ch))
              throw parse_error("'=' expected", state, curfile, curline);
            break;

          case state_attrval:
            if (ch == '\n' || ch == ';')
            {
              if (value.empty())
                throw parse_error("value expected", state, curfile, curline);
              log_debug("onAttr(\"" << arg << "\", \"" << value << "\")");
              handler.onAttr(arg, value);
              arg.clear();
              value.clear();
              state = state_attr0;
              if (ch == '\n')
                std::cerr << curfile << ':' << curline << ": warning: old syntax: ';' missing" << std::endl;
            }
            else if (ch == '"')
            {
              value += ch;
              state = state_attrvalstring;
            }
            else if (ch == '/')
              state = state_attrvalcomment0;
            else
              value += ch;
            break;

          case state_attrvalstring:
            if (ch == '"')
              state = state_attrval;
            value += ch;
            break;

          case state_attrcomment0:
            if (ch == '/')
              state = state_attrcomment;
            else
              throw parse_error("6", state, curfile, curline);
            break;

          case state_attrcomment:
            if (ch == '\n')
              state = state_attr0;
            break;

          case state_attrvalcomment0:
            if (ch == '/')
            {
              if (value.empty())
                throw parse_error("value expected", state, curfile, curline);
              log_debug("onAttr(\"" << arg << "\", \"" << value << "\")");
              handler.onAttr(arg, value);
              arg.clear();
              value.clear();
              state = state_attrcomment;
            }
            else
            {
              value += '/';
              value += ch;
              state = state_attrval;
            }
            break;

          case state_call0:
            if (ch == '(')
            {
              comp = ch;
              bracket_count = 1;
              state = state_callname_expr;
            }
            else if (ch == '"')
            {
              comp = ch;
              state = state_callname_string;
            }
            else if (!std::isspace(ch))
            {
              comp = ch;
              state = state_callname;
            }
            break;

          case state_callname_expr:
            comp += ch;
            if (ch == '(')
              ++bracket_count;
            else if (ch == ')' && --bracket_count == 0)
              state = state_callarg0;
            break;

          case state_callname_string:
            comp += ch;
            if (ch == '"')
              state = state_callarg0;
            else if (std::isspace(ch))
              throw parse_error("invalid componentname", state, curfile, curline);
            break;

          case state_callname:
            if (ch == '&' || ch == '>')
            {
              log_debug("onCall(\"" << comp << "comp_args (" << comp_args.size() << "), \"" << pass_cgi << "\", paramargs (" << paramargs.size() << "), defarg (" << defarg.size() << "))");
              handler.onCall(comp, comp_args, pass_cgi, paramargs, defarg);
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              paramargs.clear();
              defarg.clear();
              state = ch == '>' ? state_html : state_callend;
            }
            else if (std::isspace(ch))
              state = state_callarg0;
            else if (ch == '(')
            {
              state = state_call_cpparg0;
              expr.clear();
              paramargs.clear();
            }
            else
              comp += ch;
            break;

          case state_call_cpparg0:
            if (ch == ')')
              state = state_callarg;
            else if (std::isalpha(ch) || ch == '_')
            {
              expr = ch;
              state = state_call_cpparg_pe;
            }
            else if (!std::isspace(ch))
            {
              expr = ch;
              if (ch == '(')
                bracket_count = 1;
              state = state_call_cpparg_e;
            }
            break;

          case state_call_cpparg1:
            if (ch == ')')
              throw parse_error("')' unexpected", state, curfile, curline);
            else if (std::isalpha(ch) || ch == '_')
            {
              expr = ch;
              state = state_call_cpparg_pe;
            }
            else if (!std::isspace(ch))
            {
              expr = ch;
              if (ch == '(')
                bracket_count = 1;
              state = state_call_cpparg_e;
            }
            break;

          case state_call_cpparg_pe:
            if (isVariableNameChar(ch))
              expr += ch;
            else if (std::isspace(ch))
            {
              paramname = expr;
              state = state_call_cpparg_sp;
            }
            else if (ch == '=')
            {
              paramname = expr;
              expr.clear();
              bracket_count = 0;
              state = state_call_cpparg_e;
            }
            else if (ch == ')')
            {
              if (!defarg.empty())
                defarg += ',';
              defarg += expr;
              expr.clear();
              state = state_callarg0;
            }
            else
            {
              expr += ch;
              if (ch == '(')
                bracket_count = 1;
              state = state_call_cpparg_e;
            }
            break;

          case state_call_cpparg_sp:
            if (std::isspace(ch))
              expr += ch;
            else if (ch == '=')
            {
              expr.clear();
              bracket_count = 0;
              state = state_call_cpparg_e;
            }
            else
            {
              expr += ch;
              paramname.clear();
              if (ch == '(')
                bracket_count = 1;
              state = state_call_cpparg_e;
            }
            break;

          case state_call_cpparg_e:
            if (ch == ')' && bracket_count > 0)
            {
              --bracket_count;
              expr += ch;
            }
            else if (ch == ',' || ch == ')')
            {
              if (paramname.empty())
              {
                if (!defarg.empty())
                  defarg += ',';
                defarg += expr;
                expr.clear();
                state = ch == ',' ? state_call_cpparg1 : state_callarg0;
              }
              else
              {
                if (paramargs.find(paramname) != paramargs.end())
                  throw parse_error("duplicate parameter " + paramname, state, curfile, curline);
                paramargs[paramname] = expr;
                paramname.clear();
                expr.clear();
                state = ch == ',' ? state_call_cpparg1 : state_callarg0;
              }
            }
            else
            {
              if (ch == '(')
                ++bracket_count;
              expr += ch;
            }
            break;

          case state_callend:
            if (ch == '>')
              state = state_html;
            else
              throw parse_error("3", state, curfile, curline);
            break;

          case state_callarg0:
            if (std::isalpha(ch) || ch == '_')
            {
              arg = ch;
              state = state_callarg;
            }
            else if (ch == '&' || ch == '/' || ch == '>')
            {
              log_debug("onCall(\"" << comp << "comp_args (" << comp_args.size() << "), \"" << pass_cgi << "\", paramargs (" << paramargs.size() << "), defarg (" << defarg.size() << "))");
              handler.onCall(comp, comp_args, pass_cgi, paramargs, defarg);
              arg.clear();
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              paramargs.clear();
              defarg.clear();
              state = ch == '>' ? state_html : state_callend;
            }
            else if (ch == '(' && arg.empty() && pass_cgi.empty())
              state = state_call_cpparg0;
            else if (!std::isspace(ch))
              throw parse_error(std::string("invalid argumentname (") + ch + ')', state, curfile, curline);
            break;

          case state_callarg:
            if (isVariableNameChar(ch))
              arg += ch;
            else if (std::isspace(ch))
              state = state_callarge;
            else if (ch == '=')
              state = state_callval0;
            else if ((pass_cgi.empty() && ch == '&') || ch == '/' || ch == '>')
            {
              log_debug("onCall(\"" << comp << "comp_args (" << comp_args.size() << "), \"" << pass_cgi << "\", paramargs (" << paramargs.size() << "), defarg (" << defarg.size() << "))");
              handler.onCall(comp, comp_args, arg, paramargs, defarg);
              arg.clear();
              comp.clear();
              comp_args.clear();
              paramargs.clear();
              defarg.clear();
              state = ch == '>' ? state_html : state_callend;
            }
            else
              throw parse_error(std::string("invalid argumentname (") + ch + ')', state, curfile, curline);
            break;

          case state_callarge:
            if (ch == '=')
              state = state_callval0;
            else if (pass_cgi.empty() && (isVariableNameChar(ch)))
            {
              pass_cgi = arg;
              arg = ch;
              state = state_callarg;
            }
            else if ((pass_cgi.empty() && ch == '&') || ch == '/' || ch == '>')
            {
              log_debug("onCall(\"" << comp << "comp_args (" << comp_args.size() << "), \"" << pass_cgi << "\", paramargs (" << paramargs.size() << "), defarg (" << defarg.size() << "))");
              handler.onCall(comp, comp_args, arg, paramargs, defarg);
              arg.clear();
              comp.clear();
              comp_args.clear();
              paramargs.clear();
              defarg.clear();
              state = ch == '>' ? state_html : state_callend;
            }
            else if (!std::isspace(ch))
              throw parse_error("5", state, curfile, curline);
            break;

          case state_callval0:
            if (ch == '(')
            {
              value = ch;
              state = state_callval_expr;
              bracket_count = 1;
            }
            else if (ch == '"')
            {
              value = ch;
              state = state_callval_string;
            }
            else if (!std::isspace(ch))
            {
              value = ch;
              state = state_callval_word;
            }
            break;

          case state_callval_expr:
            value += ch;
            if (ch == '(')
              ++bracket_count;
            else if (ch == ')' && --bracket_count == 0)
            {
              comp_args.insert(comp_args_type::value_type(arg, value));
              arg.clear();
              value.clear();
              state = state_callarg0;
            }
            break;

          case state_callval_string:
            value += ch;
            if (ch == '"')
            {
              comp_args.insert(comp_args_type::value_type(arg, value));
              arg.clear();
              value.clear();
              state = state_callarg0;
            }
            break;

          case state_callval_word:
            if (std::isspace(ch))
            {
              comp_args.insert(comp_args_type::value_type(arg, value));
              arg.clear();
              value.clear();
              state = state_callarg0;
            }
            else
              value += ch;
            break;

          case state_callvale:
            if (ch == '>')
            {
              comp_args.insert(comp_args_type::value_type(arg, value));
              arg.clear();
              value.clear();

              log_debug("onCall(\"" << comp << "comp_args (" << comp_args.size() << "), \"" << pass_cgi << "\", paramargs (" << paramargs.size() << "), defarg (" << defarg.size() << "))");
              handler.onCall(comp, comp_args, pass_cgi, paramargs, defarg);
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              paramargs.clear();
              defarg.clear();
              state = state_html;
            }
            else
              throw parse_error("ivalid value", state, curfile, curline);
            break;

          case state_comment:
            if (ch == '#')
              state = state_commente;
            break;

          case state_commente:
            if (ch == '>')
              state = state_html;
            else if (ch != '#')
              state = state_comment;
            break;

          case state_compe0:
            if (ch == '%')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              state = state_compe;
              tag.clear();
            }
            else if (ch == '&')
            {
              state = state_endcall0;
            }
            else
            {
              html += "</";
              html += ch;
              state = state_html;
            }
            break;

          case state_compe:
            if (ch == '>')
            {
              if (inComp && tag == "def")
              {
                log_debug("onComp(\"" << code << "\")");
                handler.onComp(code);
                inComp = false;
                state = state_html0;
              }
              else if (inClose && tag == "close")
              {
                handler.endClose();
                inClose = false;
                state = state_html0;
              }
              else if (splitBar && tag == "i18n")
              {
                splitBar = false;
                handler.endI18n();
                state = state_html0;
              }
              else
              {
                html += "</%";
                html += tag;
                state = state_html;
              }
            }
            else
            {
              tag += ch;
              if (tag.size() > 5)
              {
                html += "</%";
                html += tag;
                state = state_html;
              }
            }
            break;

          case state_cond0:
            if (ch == '?')
              htmlExpr = true;
            else
              cond += ch;
            state = state_cond;
            break;

          case state_cond:
            if (ch == '?')
              state = state_condexpr;
            else
              cond += ch;
            break;

          case state_condexpr:
            if (ch == '?')
              state = state_condexpre;
            else if (expr.empty() && ch == '>')
            {
              // special case for xml-header (<?xml ... ?>)
              handler.onHtml("<?" + cond + "?>");
              cond.clear();
              state = state_html;
            }
            else
              expr += ch;
            break;

          case state_condexpre:
            if (ch == '>')
            {
              log_debug("onCondExpression(\"" << cond << ", " << code << "\")");
              handler.onCondExpr(cond, expr, htmlExpr);
              htmlExpr = false;
              cond.clear();
              expr.clear();
              state = state_html;
            }
            else
            {
              expr += '?';
              expr += ch;
              state = state_condexpr;
            }
            break;

          case state_include0:
            if (ch == '<')
              state = state_cppe0;
            else if (!std::isspace(ch))
            {
              expr = ch;
              state = state_include1;
            }
            break;

          case state_include1:
            if (std::isspace(ch) || ch == '<')
            {
              doInclude(expr);
              expr.clear(),
              state = (ch == '<' ? state_cppe0 : state_include0);
            }
            else
              expr += ch;
            break;

          case state_scopearg0:
            if (ch == '>')
            {
              state = state_scope0;
              scope = component_scope;
            }
            else if (!std::isspace(ch))
            {
              tagarg = ch;
              state = state_scopearg;
            }
            break;

          case state_scopearg:
            if (ch == '=')
            {
              state = state_scopeargval0;
              if (tagarg != "scope")
                throw parse_error("argument \"scope\" expected", state, curfile, curline);
              tagarg.clear();
            }
            else if (std::isspace(ch))
            {
              state = state_scopeargeq;
              if (tagarg != "scope")
                throw parse_error("argument \"scope\" expected", state, curfile, curline);
              tagarg.clear();
            }
            else
              tagarg += ch;
            break;

          case state_scopeargeq:
            if (ch == '=')
              state = state_scopeargval0;
            else if (!std::isspace(ch))
              throw parse_error("\"=\" expected", state, curfile, curline);
            break;

          case state_scopeargval0:
            if (ch == '"')
              state = state_scopeargval;
            else if (!std::isspace(ch))
              throw parse_error("argument expected", state, curfile, curline);
            break;

          case state_scopeargval:
            if (ch == '"')
            {
              if (value == "global")
                scope = global_scope;
              else if (value == "page")
                scope = page_scope;
              else if (value == "component")
                scope = component_scope;
              else
                throw parse_error("global|page|component expected", state, curfile, curline);

              value.clear();
              state = state_scopevale;
            }
            else if (ch == '\n')
              throw parse_error("'\"' expected", state, curfile, curline);
            else
              value += ch;
            break;

          case state_scopevale:
            if (ch == '>')
              state = state_scope0;
            else if (!std::isspace(ch))
              throw parse_error("'>' expected", state, curfile, curline);
            break;

          case state_scope0:
            if (ch == '<')
              state = state_scopee;
            else if (ch == '/')
              state = state_scopecomment0;
            else if (!std::isspace(ch))
            {
              handler.onLine(curline, curfile); //#
              scopevar = ch;
              state = state_scope;
            }
            break;

          case state_scope:
            if (ch == ';')
            {
              // scopetype contains type-definition
              // scopevar contains variable-definition
              // scopeinit is empty
              if (scopetype.size() > 0
                && std::isspace(scopetype.at(scopetype.size() - 1)))
                  scopetype.erase(scopetype.size() - 1);
              log_debug("onScope(" << scope_container << ", " << scope << ", "
                  << scopetype << ", " << scopevar << ", " << scopeinit << ')');
              handler.onScope(scope_container, scope, scopetype, scopevar, scopeinit);

              scopetype.clear();
              scopevar.clear();
              state = state_scope0;
            }
            else if (ch == '(')
            {
              state = state_scopeinit;
              bracket_count = 0;
            }
            else if (ch == '=')
            {
              state = state_scopeiniteq;
              bracket_count = 0;
            }
            else if (std::isspace(ch))
              scopevar += ch;
            else if (!isVariableNameChar(scopevar.at(scopevar.size() - 1)))
            {
              scopetype += scopevar;
              scopevar = ch;
            }
            else
              scopevar += ch;
            break;

          case state_scopee:
            if (ch == '/')
            {
              etag.clear();
              state = state_cppe1;
            }
            else
            {
              scopevar += '<';
              scopevar += ch;
              state = state_scope;
            }
            break;

          case state_scopeinit:
            if (bracket_count == 0 && ch == ')')
            {
              // scopevar contains variable-definition
              // scopeinit contains constructorparameter
              while (scopetype.size() > 0
                && std::isspace(scopetype.at(scopetype.size() - 1)))
                  scopetype.erase(scopetype.size() - 1);
              while (scopevar.size() > 0
                && std::isspace(scopevar.at(scopevar.size() - 1)))
                  scopevar.erase(scopevar.size() - 1);
              log_debug("onScope(" << scope_container << ", " << scope << ", "
                  << scopetype << ", " << scopevar << ", " << scopeinit << ')');
              handler.onScope(scope_container, scope, scopetype, scopevar, scopeinit);

              scopetype.clear();
              scopevar.clear();
              scopeinit.clear();
              state = state_scopee0;
            }
            else
            {
              scopeinit += ch;
              if (ch == '(')
                ++bracket_count;
              else if (ch == ')')
              {
                if (bracket_count == 0)
                  throw parse_error("unexpected ')'", state, curfile, curline);
                --bracket_count;
              }
            }
            break;

          case state_scopeiniteq:
            if (bracket_count == 0 && ch == ';')
            {
              // scopevar contains variable-definition
              // scopeinit contains constructorparameter
              while (scopetype.size() > 0
                && std::isspace(scopetype.at(scopetype.size() - 1)))
                  scopetype.erase(scopetype.size() - 1);
              while (scopevar.size() > 0
                && std::isspace(scopevar.at(scopevar.size() - 1)))
                  scopevar.erase(scopevar.size() - 1);
              log_debug("onScope(" << scope_container << ", " << scope << ", "
                  << scopetype << ", " << scopevar << ", " << scopeinit << ')');
              handler.onScope(scope_container, scope, scopetype, scopevar, scopeinit);

              scopetype.clear();
              scopevar.clear();
              scopeinit.clear();
              state = state_scope0;
            }
            else
            {
              scopeinit += ch;
              if (ch == '(')
                ++bracket_count;
              else if (ch == ')')
              {
                if (bracket_count == 0)
                  throw parse_error("unexpected ')'", state, curfile, curline);
                --bracket_count;
              }
            }
            break;

          case state_scopee0:
            if (ch == ';')
              state = state_scope0;
            else if (!std::isspace(ch))
              throw parse_error("invalid scopedefinition", state, curfile, curline);
            break;

          case state_scopecomment0:
            if (ch == '/')
              state = state_scopecomment;
            else
              throw parse_error("invalid scopedefinition - '/' expexted", state, curfile, curline);
            break;

          case state_scopecomment:
            if (ch == '\n')
              state = state_scope0;
            break;

          case state_endcall0:
            if (isVariableNameChar(ch))
            {
              comp = ch;
              state = state_endcall;
            }
            else if ('>')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              log_debug("onEndCall(\"" << comp << "\")");
              handler.onEndCall(comp);
              comp.clear();
              state = state_html;
            }
            else if (!std::isspace(ch))
              throw parse_error(std::string("character expected, \'") + ch
                + "\' found", state, curfile, curline);
            break;

          case state_endcall:
            if (ch == '>')
            {
              if (!html.empty())
              {
                log_debug("onHtml(\"" << html << "\")");
                handler.onHtml(html);
                html.clear();
              }
              log_debug("onEndCall(\"" << comp << "\")");
              handler.onEndCall(comp);
              comp.clear();
              state = state_html;
            }
            else if (std::isspace(ch))
            {
              log_debug("onEndCall(\"" << comp << "\")");
              handler.onEndCall(comp);
              comp.clear();
              state = state_endcalle;
            }
            else if (isVariableNameChar(ch) || ch == '@')
              comp += ch;
            else
              throw parse_error("character expected", state, curfile, curline);
            break;

          case state_endcalle:
            if (ch == '>')
              state = state_html;
            else if (!std::isspace(ch))
              throw parse_error("'>' expected", state, curfile, curline);
            break;

          case state_doc:
            if (ch == '<')
            {
              etag.clear();
              state = state_doce;
            }
            break;

          case state_doce:
            if (ch == '>')
            {
              if (etag == "/%doc")
                state = state_html0;
              else
                state = state_doc;
            }
            else if (ch == '<')
              etag.clear();
            else
              etag += ch;
            break;

        }  // switch(state)

        log_debug("line " << curline << " char " << ch << " state " << state << " bc " << bracket_count);
      }  // while(in.get(ch))

      if (state != state_html && state != state_html0 && state != state_nl)
        throw parse_error("parse error", state, curfile, curline);

      if (inComp)
        throw parse_error("</%def> missing", state, curfile, curline);

      if (inClose)
        throw parse_error("</%close> missing", state, curfile, curline);

      if (!html.empty())
      {
        log_debug("onHtml(\"" << html << "\")");
        handler.onHtml(html);
      }
    }

    void Parser::parse(std::istream& in)
    {
      parsePriv(in);
      handler.end();
    }

    void Parser::processNV(const std::string& tag, const std::string& name,
      const std::string& value)
    {
      if (tag == "args")
        handler.onArg(name, value);
      if (tag == "get")
        handler.onGet(name, value);
      if (tag == "post")
        handler.onPost(name, value);
      else if (tag == "config")
        handler.onConfig(name, value);
    }

    parse_error::parse_error(const std::string& txt, int state, const std::string& file, unsigned curline)
      : runtime_error(std::string())
    {
      std::ostringstream m;
      m << file << ':' << (curline + 1) << ": error: " << txt << " (state " << state << ')';
      msg = m.str();
    }

    const char* parse_error::what() const throw()
    {
      return msg.c_str();
    }
  }
}
