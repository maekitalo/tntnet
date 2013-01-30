/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 */

#include <tntwiki/MarkdownParser.h>
#include <stdexcept>
#include <cxxtools/log.h>

log_define("tntwiki.markdown.parser")

namespace tntwiki
{
namespace markdown
{
void Parser::parse(char ch)
{
  if (ch == '\n')
    ++_lineNo;

  switch (_state)
  {
    case state_0:
      if (ch == '*' || ch == '-')
      {
        _state = state_hr0;
        _listChar = ch;
        _token = ch;
      }
      else if (ch == '+')
      {
        _listChar = ch;
        _event.onListBegin(_listChar);
        _state = state_list1;
      }
      else if (ch >= '0' && ch <= '9')
      {
        _token = ch;
        _state = state_orderedList_innumber0;
      }
      else if (ch == '>')
      {
        _event.onBlockQuoteBegin();
        log_debug("sub begin");
        _sub = new Parser(_event);
        _state = state_blockquote;
      }
      else if (ch == ' ')
      {
        _token = ch;
        _state = state_sp;
      }
      else if (ch == '#')
      {
        _token = ch;
        _state = state_header;
      }
      else
      {
        _event.onParaBegin();
        _data.parse(ch);
        _state = state_para;
      }
      break;

    case state_hr0:
      if (ch == _listChar)
        _state = state_hr;
      else if (ch == '\n')
      {
        _event.onHorizontalRule();
        _state = state_0;
      }
      else
      {
        _event.onListBegin(_listChar);
        _state = state_list1;
      }
      break;

    case state_hr:
      if (ch == '\n')
      {
        _event.onHorizontalRule();
        _state = state_0;
      }
      else if (ch == _listChar)
      {
        _token += ch;
      }
      else
      {
        _data.parse(_token, false);
        _data.parse(ch);
        _event.onParaBegin();
      }
      break;

    case state_list1:
      if (ch == '\n')
      {
        _token.clear();
        _state = state_listend;
      }
      else if (ch == ' ')
        ;
      else
      {
        _event.onListEntryBegin();
        _data.parse(ch);
        _state = state_list;
      }
      break;

    case state_list:
      if (ch == '\n')
      {
        _token.clear();
        _state = state_listend;
      }
      else
        _data.parse(ch);
      break;

    case state_listend:
      if (ch == _listChar)
      {
        _event.onData(_data.str());
        _event.onListEntryEnd();
        _data.reset();
        _state = state_list1;
      }
      else if (ch == ' ')
        ;
      else if (ch == '\n')
      {
        _event.onData(_data.str());
        _event.onListEntryEnd();
        _data.reset();
        _event.onListEnd();
        _state = state_0;
      }
      else
      {
        _data.parse('\n');
        _data.parse(_token, false);
        _token.clear();
        _data.parse(ch);
        _state = state_list;
      }
      break;

    case state_orderedList_innumber0:
      if (ch >= '0' && ch <= '9')
        _token += ch;
      else if (ch == '.')
      {
        _event.onOrderedListBegin();
        _event.onOrderedListEntryBegin();
        _state = state_orderedList_startentry;
      }
      else
      {
        _data.parse(_token, false);
        _state = state_0;
        parse(ch);
      }
      break;

    case state_orderedList_startentry:
      if (ch == '\n')
      {
        _token = ch;
        _state = state_orderedList_afterentry;
      }
      else if (ch == ' ')
        ;
      else
      {
        _data.parse(ch);
        _state = state_orderedList_inentry;
      }
      break;

    case state_orderedList_inentry:
      if (ch == '\n')
      {
        _token = ch;
        _state = state_orderedList_afterentry;
      }
      else
        _data.parse(ch);
      break;

    case state_orderedList_afterentry:
      if (ch == ' ')
        ;
      else if (ch >= '0' && ch <= '9')
      {
        _event.onData(_data.str());
        _event.onOrderedListEntryEnd();
        _data.reset();
        _token = ch;
        _state = state_orderedList_innumber;
      }
      else if (ch == '\n')
      {
        _event.onData(_data.str());
        _event.onOrderedListEntryEnd();
        _event.onOrderedListEnd();
        _data.reset();
        _state = state_0;
      }
      else
      {
        _data.parse(_token, false);
        _data.parse(ch);
        _state = state_orderedList_inentry;
      }
      break;

    case state_orderedList_innumber:
      if (ch >= '0' && ch <= '9')
        _token += ch;
      else if (ch == '.')
      {
        _event.onOrderedListEntryBegin();
        _state = state_orderedList_startentry;
      }
      else
      {
        _data.parse(_token, false);
        _event.onOrderedListEnd();
        _state = state_0;
        parse(ch);
      }
      break;

    case state_sp:
      if (ch == ' ')
      {
        _token += ch;
        if (_token.size() >= 4)
        {
          _event.onCodeBegin();
          _state = state_code;
        }
      }
      else
      {
        _state = state_0;
        parse(ch);
      }
      break;

    case state_blockquote:
      if (ch == '\n')
        _state = state_blockquote_end;
      else
        _sub->parse(ch);
      break;

    case state_blockquote_end:
      if (ch == '\n')
      {
        log_debug("sub finalize");
        _sub->finalize();
        _sub = 0;
        _event.onBlockQuoteEnd();
        _state = state_0;
      }
      else if (ch == '>')
      {
        _sub->parse('\n');
        _state = state_blockquote;
      }
      else
      {
        _sub->parse('\n');
        _sub->parse(ch);
      }
      break;

    case state_para:
      if (ch == '\n')
        _state = state_paraend;
      else
        _data.parse(ch);
      break;

    case state_code:
      if (ch == '\n')
      {
        _token.clear();
        _data.parse('\n');
        _state = state_codeend;
      }
      else
        _data.parse(ch);
      break;

    case state_codeend:
      if (ch == ' ')
      {
        _token += ch;
        if (_token.size() >= 4)
          _state = state_code;
      }
      else if (ch == '\n')
      {
        _data.parse(_token, false);
        _token.clear();
        _data.parse('\n');
      }
      else
      {
        _event.onData(_data.str());
        _data.reset();
        _event.onCodeEnd();
        _state = state_0;
        parse(ch);
      }
      break;

    case state_paraend:
      if (ch == '=')
      {
        _token = ch;
        _state = state_h1;
      }
      else if (ch == '-')
      {
        _token = ch;
        _state = state_h2;
      }
      else if (ch == '*')
      {
        _event.onData(_data.str());
        _event.onParaEnd();
        _data.reset();
        _listChar = ch;
        _token = ch;
        _state = state_hr0;
      }
      else if (ch == '>')
      {
        _event.onData(_data.str());
        _data.reset();
        _event.onParaEnd();
        _event.onBlockQuoteBegin();
        log_debug("sub begin");
        _sub = new Parser(_event);
        _state = state_blockquote;
      }
      else if (ch == '\n')
      {
        _event.onData(_data.str());
        _data.reset();
        _event.onParaEnd();
        _state = state_0;
      }
      else
      {
        _data.parse('\n');
        _data.parse(ch);
        _state = state_para;
      }
      break;

    case state_h1:
      if (ch == '=')
        _token += ch;
      else if (ch == '\n')
      {
        _event.onData(_data.str());
        _data.reset();
        _event.onParaEndHeader(1);
        _state = state_0;
      }
      else
      {
        _data.parse('\n');
        _data.parse(_token, false);
        _token.clear();
        _state = state_0;
      }
      break;

    case state_h2:
      if (ch == '-')
        _token += ch;
      else if (ch == '\n')
      {
        _event.onData(_data.str());
        _data.reset();
        _event.onParaEndHeader(2);
        _state = state_0;
      }
      else
      {
        _data.parse('\n');
        _data.parse(_token, false);
        _token.clear();
        _state = state_0;
      }
      break;

    case state_header:
      if (ch == '#')
        _token += '#';
      else if (ch == '\n')
      {
        _event.onData(_token);
        _state = state_0;
      }
      else
      {
        _event.onHeaderBegin(_token.size());
        _state = state_headerp;
      }
      break;

    case state_headerp:
      if (ch == '\n')
      {
        _event.onData(_data.str());
        _event.onHeaderEnd(_token.size());
        _data.reset();
        _state = state_0;
      }
      else
        _data.parse(ch);
      break;
  }
}

void Parser::finalize()
{
  log_debug("parser finalize; state=" << _state);
  _data.finalize();
  switch (_state)
  {
    case state_0:
      break;
    case state_hr0:
    case state_hr:
      _event.onHorizontalRule();
      break;

    case state_sp:
    case state_list1:
      _event.onListEnd();
      break;

    case state_list:
    case state_listend:
      _event.onListEntryEnd();
      _event.onListEnd();
      break;
      
    case state_orderedList_innumber0:
      _data.parse(_token, false);
      _event.onData(_token);
      break;

    case state_orderedList_startentry:
    case state_orderedList_innumber:
      _event.onData(_token);
      _event.onOrderedListEnd();
      _event.onOrderedListEntryEnd();
      break;

    case state_orderedList_inentry:
    case state_orderedList_afterentry:
      _event.onOrderedListEntryEnd();
      _data.reset();
      _event.onOrderedListEnd();
      break;

    case state_blockquote:
    case state_blockquote_end:
      _sub->finalize();
      _sub = 0;
      _event.onBlockQuoteEnd();
      break;

    case state_para:
    case state_paraend:
      _event.onParaEnd();
      break;

    case state_code:
    case state_codeend:
      _event.onData(_data.str());
      _event.onCodeEnd();
      break;

    case state_h1:
      _event.onData(_data.str());
      _event.onParaEndHeader(1);
      break;

    case state_h2:
      _event.onData(_data.str());
      _event.onParaEndHeader(2);
      break;

    case state_header:
      break;

    case state_headerp:
      _event.onHeaderEnd(_token.size());
      break;
  }

  _event.finalize();
}

}
}
