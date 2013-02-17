/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 */

#include <tntwiki/MarkdownParser.h>
#include <stdexcept>

namespace tntwiki
{
namespace markdown
{
void SpanElementParser::parse(char ch)
{
  switch (_state)
  {
    case state_0:
      if (ch == '*')
      {
        if (!_data.empty())
        {
          _event.onData(_data);
          _data.clear();
        }
        _state = state_em0;
      }
      else if (ch == '_')
      {
        if (!_data.empty())
        {
          _event.onData(_data);
          _data.clear();
        }
        _state = state_underscore0;
      }
      else if (ch == '`')
      {
        if (!_data.empty())
        {
          _event.onData(_data);
          _data.clear();
        }
        _state = state_code0;
      }
      else if (ch == '\\')
        _state = state_esc;
      else if (ch == '[')
      {
        _linktext.clear();
        _linktarget.clear();
        _linktitle.clear();
        _save = ch;
        _state = state_linktext;
      }
      else
        _data += ch;
      break;

    case state_esc:
      _data += ch;
      _state = state_0;
      break;

    case state_em0:
      if (ch == '*')
      {
        _event.onStrongEmphasisBegin();
        _state = state_em2;
      }
      else
      {
        _event.onEmphasisBegin();
        _data += ch;
        _state = state_em;
      }
      break;

    case state_em:
      if (ch == '*')
      {
        _event.onData(_data);
        _data.clear();
        _event.onEmphasisEnd();
        _state = state_0;
      }
      else if (ch == '\\')
        _state = state_emesc;
      else
        _data += ch;
      break;

    case state_emesc:
      _data += ch;
      _state = state_em;
      break;

    case state_em2:
      if (ch == '*')
        _state = state_em2e;
      else if (ch == '\\')
        _state = state_em2esc;
      else
        _data += ch;
      break;

    case state_em2esc:
      _data += ch;
      _state = state_em2;
      break;

    case state_em2e:
      if (ch == '*')
      {
        _event.onData(_data);
        _data.clear();
        _event.onStrongEmphasisEnd();
        _state = state_0;
      }
      else
      {
        _data += '*';
        if (ch == '\\')
          _state = state_em2esc;
        else
        {
          _data += ch;
          _state = state_em2;
        }
      }
      break;

    // code

    case state_underscore0:
      if (ch == '_')
      {
        _event.onDoubleUnderscoreBegin();
        _state = state_underscore2;
      }
      else
      {
        _event.onUnderscoreBegin();
        _data += ch;
        _state = state_underscore;
      }
      break;

    case state_underscore:
      if (ch == '_')
      {
        _event.onData(_data);
        _data.clear();
        _event.onUnderscoreEnd();
        _state = state_0;
      }
      else if (ch == '\\')
        _state = state_underscoreesc;
      else
        _data += ch;
      break;

    case state_underscoreesc:
      _data += ch;
      _state = state_underscore;
      break;

    case state_underscore2:
      if (ch == '_')
        _state = state_underscore2e;
      else if (ch == '\\')
        _state = state_underscore2esc;
      else
        _data += ch;
      break;

    case state_underscore2esc:
      _data += ch;
      _state = state_underscore2;
      break;

    case state_underscore2e:
      if (ch == '_')
      {
        _event.onData(_data);
        _data.clear();
        _event.onDoubleUnderscoreEnd();
        _state = state_0;
      }
      else
      {
        _data += '_';
        if (ch == '\\')
          _state = state_underscore2esc;
        else
        {
          _data += ch;
          _state = state_underscore2;
        }
      }
      break;

    // code

    case state_code0:
      if (ch == '`')
      {
        _event.onCodeBegin();
        _state = state_code2;
      }
      else
      {
        _event.onCodeBegin();

        if (ch == '<')
          _data += "&lt;";
        else if (ch == '>')
          _data += "&gt;";
        else if (ch == '&')
          _data += "&et;";
        else
          _data += ch;

        _state = state_code;
      }
      break;

    case state_code:
      if (ch == '`')
      {
        _event.onData(_data);
        _data.clear();
        _event.onCodeEnd();
        _state = state_0;
      }
      else if (ch == '\\')
        _state = state_codeesc;
      else if (ch == '<')
        _data += "&lt;";
      else if (ch == '>')
        _data += "&gt;";
      else if (ch == '&')
        _data += "&et;";
      else
        _data += ch;
      break;

    case state_codeesc:
      _data += ch;
      _state = state_code;
      break;

    case state_code2:
      if (ch == '`')
        _state = state_code2e;
      else if (ch == '\\')
        _state = state_code2esc;
      else if (ch == '<')
        _data += "&lt;";
      else if (ch == '>')
        _data += "&gt;";
      else if (ch == '&')
        _data += "&et;";
      else
        _data += ch;
      break;

    case state_code2esc:
      _data += ch;
      _state = state_code2;
      break;

    case state_code2e:
      if (ch == '`')
      {
        _event.onData(_data);
        _data.clear();
        _event.onCodeEnd();
        _state = state_0;
      }
      else
      {
        _data += '`';

        if (ch == '\\')
          _state = state_code2esc;
        else
        {
          _state = state_code2;

          if (ch == '\\')
            _state = state_code2esc;
          else if (ch == '<')
            _data += "&lt;";
          else if (ch == '>')
            _data += "&gt;";
          else if (ch == '&')
            _data += "&et;";
          else
            _data += ch;
        }
      }
      break;

    case state_linktext:
      _save += ch;
      if (ch == ']')
        _state = state_linktextend;
      else
        _linktext += ch;
      break;

    case state_linktextend:
      _save += ch;
      if (ch == '(')
      {
        _linktarget.clear();
        _state = state_link;
      }
      else if (ch == ' ' || ch == '\t')
        ;
      else
      {
        // no link
        _state = state_esc;
        std::string save = _save;
        _save.clear();
        parse(save, false);
      }
      break;

    case state_link:
      _save += ch;
      if (ch == ')')
      {
        if (!_data.empty())
        {
          _event.onData(_data);
          _data.clear();
        }
        _event.onLink(_linktext, _linktarget, _linktitle);
        _linktext.clear();
        _linktarget.clear();
        _linktitle.clear();
        _save.clear();
        _state = state_0;
      }
      else if (ch == ' ' || ch == '\t')
        _state = state_linktitle0;
      else
        _linktarget += ch;
      break;

    case state_linktitle0:
      _save += ch;
      if (ch == '\'' || ch == '"')
      {
        _delimiter = ch;
        _state = state_linktitle;
      }
      else if (ch == ' ' || ch == '\t')
        ;
      else
      {
        // no link
        _state = state_esc;
        std::string save = _save;
        _save.clear();
        parse(save, false);
      }
      break;

    case state_linktitle:
      _save += ch;
      if (ch == _delimiter)
        _state = state_linktitleend;
      else
        _linktitle += ch;
      break;

    case state_linktitleend:
      _save += ch;
      if (ch == ' ' || ch == '\t')
        ;
      else if (ch == ')')
      {
        if (!_data.empty())
        {
          _event.onData(_data);
          _data.clear();
        }
        _event.onLink(_linktext, _linktarget, _linktitle);
        _linktext.clear();
        _linktarget.clear();
        _linktitle.clear();
        _save.clear();
        _state = state_0;
      }
      else
      {
        // no link
        _state = state_esc;
        std::string save = _save;
        _save.clear();
        parse(save, false);
      }
      break;
  }
}

void SpanElementParser::finalize()
{
  switch (_state)
  {
    case state_0:
    case state_esc:
      break;
    case state_em0:
    case state_em:
    case state_emesc:
      _event.onData(_data);
      _data.clear();
      _event.onEmphasisEnd();
      break;

    case state_em2:
    case state_em2esc:
    case state_em2e:
      _event.onData(_data);
      _data.clear();
      _event.onStrongEmphasisEnd();
      break;

    case state_underscore0:
    case state_underscore:
    case state_underscoreesc:
      _event.onData(_data);
      _data.clear();
      _event.onUnderscoreEnd();
      break;

    case state_underscore2:
    case state_underscore2esc:
    case state_underscore2e:
      _event.onData(_data);
      _data.clear();
      _event.onDoubleUnderscoreEnd();
      break;

    case state_code0:
    case state_code:
    case state_codeesc:
      _event.onData(_data);
      _data.clear();
      _event.onCodeEnd();
      break;

    case state_code2:
    case state_code2esc:
    case state_code2e:
      _event.onData(_data);
      _data.clear();
      _event.onCodeEnd();
      break;

    case state_linktext:
    case state_linktextend:
    case state_link:
    case state_linktitle0:
    case state_linktitle:
    case state_linktitleend:
      _state = state_esc;
      parse(_save, true);
      return;
  }

  if (!_data.empty())
    _event.onData(_data);
}
}
}
