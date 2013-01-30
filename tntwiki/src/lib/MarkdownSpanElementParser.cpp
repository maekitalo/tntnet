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
      else
        _data += ch;
      break;

    // emphasis

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

  }
}

void SpanElementParser::finalize()
{
  if (_state != state_0)
    throw std::runtime_error("missing closing markup");

  if (!_data.empty())
    _event.onData(_data);
}
}
}
