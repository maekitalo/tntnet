/* parser.cpp
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

#include "tnt/ecpp/parser.h"
#include <sstream>

namespace tnt
{
  namespace ecpp
  {

    void parser::parse(std::istream& in)
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
        state_nl,
        state_cpp1,
        state_args0,
        state_args0comment,
        state_argsvar,
        state_argsvare,
        state_argsval,
        state_argsvalstring,
        state_argscomment0,
        state_argscomment,  // 30
        state_argsvalcomment0,
        state_attr0,
        state_attr0comment,
        state_attrvar,
        state_attrvare,
        state_attrval,
        state_attrvalstring,
        state_attrcomment0,
        state_attrcomment,
        state_attrvalcomment0,  // 40
        state_call0,
        state_callname_expr,
        state_callname_string,
        state_callname,
        state_call_cpparg0,
        state_call_cpparg,
        state_callend,
        state_callarg0,
        state_callarg,
        state_callarge,   // 50
        state_callval_expr,
        state_callval_string,
        state_callval_word,
        state_callval0,
        state_callvale,
        state_comment,
        state_commente,
        state_compe0,
        state_compe,
        state_cond,  // 60
        state_condexpr,
        state_condexpre };

      state_type state = state_nl;
      std::string etag, tagarg;
      std::string html, code, arg, argtype, value;
      std::string comp;
      std::string cond, expr;
      comp_args_type comp_args;
      std::string pass_cgi;
      unsigned curline = 1;
      std::string defarg, defval;
      cppargs_type cppargs;
      unsigned bracket_count = 0;

      start();

      char ch;
      while (in.get(ch))
      {
        if (ch == '\n')
          ++curline;

        switch(state)
        {
          case state_html0:
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
                processHtml(html);
                html.clear();
              }
              tokenSplit(ch == split_start);
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
                processHtml(html);
                html.clear();
              }
              tag.clear();
              state = state_tag;
            }
            else if (ch == '$')
            {
              if (!html.empty())
              {
                processHtml(html);
                html.clear();
              }
              state = state_expr;
            }
            else if (ch == '&')
            {
              if (!html.empty())
              {
                processHtml(html);
                html.clear();
              }
              state = state_call0;
            }
            else if (ch == '#')
            {
              if (!html.empty())
              {
                processHtml(html);
                html.clear();
              }
              state = state_comment;
            }
            else if (ch == '{')
            {
              if (!html.empty())
              {
                processHtml(html);
                html.clear();
              }
              state = state_cpp;
            }
            else if (ch == '?')
            {
              if (!html.empty())
              {
                processHtml(html);
                html.clear();
              }
              state = state_cond;
            }
            else if (inComp && ch == '/')
            {
              if (!html.empty())
              {
                processHtml(html);
                html.clear();
              }
              state = state_compe0;
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
              // expression might end
              state = state_expre0;
            else
              code += ch;
            break;

          case state_expre0:
            if (ch == '>')
            {
              // expression ends, html continues
              processExpression(code);
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
              if (tag == "args" || tag == "config")
                state = state_args0;
              else if (tag == "attr")
                state = state_attr0;
              else
                state = state_cpp;
            }
            else if (!inComp && tag == "def" && std::isspace(ch))
              state = state_tagarg0;
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
            if (ch == ' ')
              state = state_tagarge;
            else if (tag == "def" && ch == '(')
              state = state_defarg0;
            else if (ch == '>')
            {
              if (tag == "args")
                state = state_args0;
              else if (tag == "attr")
                state = state_attr0;
              else if (tag == "def")
              {
                startComp(tagarg, cppargs);
                state = state_html0;
                inComp = true;
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
              if (tag == "args")
                state = state_args0;
              else if (tag == "attr")
                state = state_attr0;
              else if (tag == "def")
              {
                startComp(tagarg, cppargs);
                state = state_html0;
                inComp = true;
              }
              else
                state = state_cpp;
            }
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
              startComp(tagarg, cppargs);
              inComp = true;
              cppargs.clear();
              state = state_html0;
            }
            else if (!std::isspace(ch))
              throw parse_error("def", state, curline);
            break;

          case state_cpp:
            if (ch == '<')
              state = state_cppe0;
            else if (ch == '}')
              state = state_cppse;
            else
            {
              code += ch;
              if (ch == '"')
                state = state_cppstring;
            }
            break;

          case state_cppstring:
            code += ch;
            if (ch == '"')
              state = state_cpp;
            break;

          case state_cppse:
            if (ch == '>')
            {
              processCpp(code);
              code.clear();
              state = state_html0;
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
              code += "</%";
              state = state_cpp;
            }
            break;

          case state_cppetag:
            if (ch == '>')
            {
              if (tag != etag)
                throw parse_error("invalid end-tag - "
                  + tag + " expected, "
                  + etag + " found", state, curline);

              if (tag == "pre")
                processPre(code);
              else if (tag == "declare")
                processDeclare(code);
              else if (tag == "init")
                processInit(code);
              else if (tag == "cleanup")
                processCleanup(code);
              else if (tag == "declare_shared")
                processDeclareShared(code);
              else if (tag == "shared")
                processShared(code);
              else if (tag == "cpp")
                processCpp(code);
              else if (tag == "args" || tag == "attr" || tag == "config")
                ;
              else
                throw parse_error("unknown tag " + tag, state, curline);
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
              if (!html.empty())
              {
                processHtml(html);
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
                processHtml(html);
                html.clear();
              }
              tokenSplit(ch == split_start);
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
              processCpp(code);
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
              throw parse_error("7", state, curline);
            break;

          case state_argsvar:
            if (ch == '=')
            {
              value.clear();
              state = state_argsval;
            }
            else if (ch == '/')
            {
              processNV(arg, std::string());
              arg.clear();
              state = state_argscomment0;
            }
            else if (ch == '\n')
            {
              processNV(arg, std::string());
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
            else if (ch == '\n')
            {
              processNV(arg, std::string());
              arg.clear();
              state = state_args0;
            }
            else if (!std::isspace(ch))
            {
              processNV(arg, std::string());
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
            if (ch == '\n')
            {
              processNV(arg, value);
              arg.clear();
              value.clear();
              state = state_args0;
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
              throw parse_error("6", state, curline);
            break;

          case state_argscomment:
            if (ch == '\n')
              state = state_args0;
            break;

          case state_argsvalcomment0:
            if (ch == '/')
            {
              processNV(arg, value);
              arg.clear();
              value.clear();
              state = state_argscomment;
            }
            else if (ch == '\n')
            {
              processNV(arg, value);
              arg.clear();
              value.clear();
              state = state_args0;
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
              throw parse_error("7", state, curline);
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
                throw parse_error("name expected", state, curline);
              value.clear();
              state = state_attrval;
            }
            else if (!std::isspace(ch))
              throw parse_error("'=' expected", state, curline);
            break;

          case state_attrval:
            if (ch == '\n')
            {
              if (value.empty())
                throw parse_error("value expected", state, curline);
              processAttr(arg, value);
              arg.clear();
              value.clear();
              state = state_attr0;
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
              throw parse_error("6", state, curline);
            break;

          case state_attrcomment:
            if (ch == '\n')
              state = state_attr0;
            break;

          case state_attrvalcomment0:
            if (ch == '/')
            {
              if (value.empty())
                throw parse_error("value expected", state, curline);
              processAttr(arg, value);
              arg.clear();
              value.clear();
              state = state_attrcomment;
            }
            else if (ch == '\n')
            {
              if (value.empty())
                throw parse_error("value expected", state, curline);
              processAttr(arg, value);
              arg.clear();
              value.clear();
              state = state_attr0;
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
              throw parse_error("invalid componentname", state, curline);
            break;

          case state_callname:
            if (ch == '&' || ch == '/')
            {
              processCall(comp, comp_args, pass_cgi, defarg);
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              defarg.clear();
              state = state_callend;
            }
            else if (std::isspace(ch))
              state = state_callarg0;
            else if (ch == '(')
              state = state_call_cpparg0;
            else
              comp += ch;
            break;

          case state_call_cpparg0:
            if (!std::isspace(ch))
            {
              if (ch == ')')
                state = state_callarg;
              else
              {
                defarg = ch;
                if (ch == '(')
                  ++bracket_count;
                state = state_call_cpparg;
              }
            }
            break;

          case state_call_cpparg:
            if (bracket_count == 0 && ch == ')')
              state = state_callarg0;
            else
            {
              defarg += ch;
              if (ch == '(')
                ++bracket_count;
              else if (ch == ')')
                --bracket_count;
            }
            break;

          case state_callend:
            if (ch == '>')
              state = state_html;
            else
              throw parse_error("3", state, curline);
            break;

          case state_callarg0:
            if (std::isalpha(ch))
            {
              arg = ch;
              state = state_callarg;
            }
            else if (ch == '&' || ch == '/')
            {
              processCall(comp, comp_args, pass_cgi, defarg);
              arg.clear();
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              defarg.clear();
              state = state_callend;
            }
            else if (ch == '(' && arg.empty() && pass_cgi.empty())
              state = state_call_cpparg0;
            else if (!std::isspace(ch))
              throw parse_error(std::string("invalid argumentname (") + ch + ')', state, curline);
            break;

          case state_callarg:
            if (std::isalnum(ch))
              arg += ch;
            else if (std::isspace(ch))
              state = state_callarge;
            else if (ch == '=')
              state = state_callval0;
            else if (pass_cgi.empty() && ch == '&' || ch == '/')
            {
              processCall(comp, comp_args, arg, defarg);
              arg.clear();
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              defarg.clear();
              state = state_callend;
            }
            else
              throw parse_error(std::string("invalid argumentname (") + ch + ')', state, curline);
            break;

          case state_callarge:
            if (ch == '=')
              state = state_callval0;
            else if (pass_cgi.empty() && std::isalnum(ch))
            {
              pass_cgi = arg;
              arg = ch;
              state = state_callarg;
            }
            else if (pass_cgi.empty() && ch == '&' || ch == '/')
            {
              processCall(comp, comp_args, arg, defarg);
              arg.clear();
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              defarg.clear();
              state = state_callend;
            }
            else if (!std::isspace(ch))
              throw parse_error("5", state, curline);
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

              processCall(comp, comp_args, pass_cgi, defarg);
              comp.clear();
              comp_args.clear();
              pass_cgi.clear();
              defarg.clear();
              state = state_html;
            }
            else
              throw parse_error("ivalid value", state, curline);
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
              state = state_compe;
              tag.clear();
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
              if (tag == "def")
              {
                processComp(code);
                inComp = false;
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
              if (tag.size() > 3)
              {
                html += "</%";
                html += tag;
                state = state_html;
              }
            }
            break;

          case state_cond:
            if (ch == '?')
            {
              state = state_condexpr;
            }
            else
            {
              cond += ch;
            }
            break;

          case state_condexpr:
            if (ch == '?')
            {
              state = state_condexpre;
            }
            else
            {
              expr += ch;
            }
            break;

          case state_condexpre:
            if (ch == '>')
            {
              processCondExpr(cond, expr);
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

        }  // switch(state)

      }  // while(in.get(ch))

      if (state != state_html && state != state_nl)
        throw parse_error("parse error", state, curline);

      if (!html.empty())
        processHtml(html);

      end();
    }

    void parser::start()
    {
    }

    void parser::end()
    {
    }

    void parser::processHtml(const std::string& html)
    {
    }

    void parser::processExpression(const std::string& code)
    {
    }

    void parser::processCpp(const std::string& code)
    {
    }

    void parser::processPre(const std::string& code)
    {
    }

    void parser::processDeclare(const std::string& code)
    {
    }

    void parser::processInit(const std::string& code)
    {
    }

    void parser::processCleanup(const std::string& code)
    {
    }

    void parser::processArg(const std::string& name,
      const std::string& value)
    {
    }

    void parser::processAttr(const std::string& name,
      const std::string& value)
    {
    }

    void parser::processCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const std::string& cppargs)
    {
    }

    void parser::processDeclareShared(const std::string& code)
    {
    }

    void parser::processShared(const std::string& code)
    {
    }

    void parser::startComp(const std::string& arg, const cppargs_type& cppargs)
    {
    }

    void parser::processComp(const std::string& code)
    {
    }

    void parser::processCondExpr(const std::string& cond, const std::string& expr)
    {
    }

    void parser::processConfig(const std::string& cond, const std::string& value)
    {
    }

    void parser::processNV(const std::string& name, const std::string& value)
    {
      if (tag == "args")
        processArg(name, value);
      else if (tag == "config")
        processConfig(name, value);
    }

    void parser::tokenSplit(bool start)
    {
    }

    parse_error::parse_error(const std::string& txt, int state, unsigned curline)
      : runtime_error(std::string())
    {
      std::ostringstream m;
      m << "ERROR: " << txt << " in state " << state << " at line " << curline;
      msg = m.str();
    }

    const char* parse_error::what() const throw()
    {
      return msg.c_str();
    }
  }
}
