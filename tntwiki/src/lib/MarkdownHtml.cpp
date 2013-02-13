/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 */

#include <tntwiki/MarkdownHtml.h>
#include <cxxtools/log.h>

log_define("tntwiki.markdown.html")

namespace tntwiki
{
namespace markdown
{
void Html::onParaBegin()
{
  if (_sub)
    _sub->onParaBegin();
  else
  {
    log_debug("onParaBegin()");
    _sub = new Html(_html);
  }
}

void Html::onParaEnd()
{
  if (_sub)
  {
    if (_sub->_sub)
    {
      _sub->onParaEnd();
    }
    else
    {
      log_debug("onParaEnd()");
      _html << "<p>" << _sub->_html.str() << "</p>\n";
      delete _sub;
      _sub = 0;
    }
  }
  else
  {
    // shouldn't really happen
    log_debug("onParaEnd()");
    _out << "<p>" << _html.str() << "</p>\n";
    _html.str(std::string());
  }
}

void Html::onParaEndHeader(unsigned short level)
{
  if (_sub)
  {
    if (_sub->_sub)
    {
      _sub->onParaEndHeader(level);
    }
    else
    {
      log_debug("onParaEndHeader(" << level << ')');
      _html << "<h" << level << ">" << _sub->_html.str() << "</h" << level << ">\n";
      delete _sub;
      _sub = 0;
    }
  }
  else
  {
    // shouldn't really happen
    log_debug("onParaEndHeader(" << level << ')');
    _out << "<h" << level << ">" << _html.str() << "</h" << level << ">\n";
    _html.str(std::string());
  }
}

void Html::onHeaderBegin(unsigned short level)
{
  if (_sub)
    _sub->onHeaderBegin(level);
  else
  {
    log_debug("onHeaderBegin(" << level << ')');
    _sub = new Html(_html);
  }
}

void Html::onHeaderEnd(unsigned short level)
{
  if (_sub)
  {
    if (_sub->_sub)
    {
      _sub->onHeaderEnd(level);
    }
    else
    {
      log_debug("onHeaderEnd(" << level << ')');
      _html << "<h" << level << ">" << _sub->_html.str() << "</h" << level << ">\n";
      delete _sub;
      _sub = 0;
    }
  }
  else
  {
    // shouldn't really happen
    log_debug("onHeaderEnd(" << level << ')');
    _out << "<h" << level << ">" << _html.str() << "</h" << level << ">\n";
    _html.str(std::string());
  }
}

////////////////////////////////////////////////////////////////////////
// List
void Html::onListBegin(char ch)
{
  if (_sub)
    _sub->onListBegin(ch);
  else
  {
    log_debug("onListBegin('" << ch << "')");
    _sub = new Html(_html);
  }
}

void Html::onListEntryBegin()
{
  if (_sub)
    _sub->onListEntryBegin();
  else
  {
    log_debug("onListEntryBegin()");
    _html << "<li>";
  }
}

void Html::onListEntryEnd()
{
  if (_sub)
  {
    _sub->onListEntryEnd();
  }
  else
  {
    log_debug("onListEntryEnd()");
    _html << "</li>\n";
  }
}

void Html::onListEnd()
{
  if (_sub)
  {
    if (_sub->_sub)
    {
      _sub->onListEnd();
    }
    else
    {
      log_debug("onListEnd()");
      _out << "<ul>\n" << _sub->_html.str() << "</ul>\n";
      delete _sub;
      _sub = 0;
    }
  }
  else
  {
    // shouldn't really happen
    log_debug("onListEnd()");
    _out << "<ul>" << _html.str() << "</ul>\n";
    _html.str(std::string());
  }
}

////////////////////////////////////////////////////////////////////////
// OrderedList
void Html::onOrderedListBegin()
{
  if (_sub)
    _sub->onOrderedListBegin();
  else
  {
    log_debug("onOrderedListBegin()");
    _sub = new Html(_html);
  }
}

void Html::onOrderedListEntryBegin()
{
  if (_sub)
    _sub->onOrderedListEntryBegin();
  else
  {
    log_debug("onOrderedListEntryBegin()");
    _html << "<li>";
  }
}

void Html::onOrderedListEntryEnd()
{
  if (_sub)
  {
    _sub->onOrderedListEntryEnd();
  }
  else
  {
    log_debug("onOrderedListEntryEnd()");
    _html << "</li>\n";
  }
}

void Html::onOrderedListEnd()
{
  if (_sub)
  {
    if (_sub->_sub)
    {
      _sub->onListEnd();
    }
    else
    {
      log_debug("onOrderedListEnd()");
      _out << "<ol>\n" << _sub->_html.str() << "</ol>\n";
      delete _sub;
      _sub = 0;
    }
  }
  else
  {
    // shouldn't really happen
    log_debug("onOrderedListEnd()");
    _out << "<ol>" << _html.str() << "</ol>\n";
    _html.str(std::string());
  }
}

void Html::onCodeBegin()
{
  if (_sub)
    _sub->onCodeBegin();
  else
  {
    log_debug("onCodeBegin()");
    _sub = new Html(_html);
  }
}

void Html::onCodeEnd()
{
  if (_sub)
  {
    if (_sub->_sub)
    {
      _sub->onCodeEnd();
    }
    else
    {
      log_debug("onCodeEnd()");
      _html << "<code>" << _sub->_html.str() << "</code>\n";
      delete _sub;
      _sub = 0;
    }
  }
  else
  {
    // shouldn't really happen
    log_debug("onCodeEnd()");
    _out << "<code>" << _html.str() << "</code>\n";
    _html.str(std::string());
  }
}

void Html::onBlockQuoteBegin()
{
  if (_sub)
    _sub->onBlockQuoteBegin();
  else
  {
    log_debug("onBlockQuoteBegin()");
    _sub = new Html(_html);
  }
}

void Html::onBlockQuoteEnd()
{
  if (_sub)
  {
    if (_sub->_sub)
    {
      _sub->onBlockQuoteEnd();
    }
    else
    {
      log_debug("onBlockQuoteEnd()");
      _html << "<blockquote>\n" << _sub->_html.str() << "</blockquote>\n";
      delete _sub;
      _sub = 0;
    }
  }
  else
  {
    // shouldn't really happen
    log_debug("onBlockQuoteEnd()");
    _out << "<blockquote>\n" << _html.str() << "</blockquote>\n";
    _html.str(std::string());
  }
}

void Html::onData(const std::string& data)
{
  if (_sub)
    _sub->onData(data);
  else
  {
    log_debug("onData(\"" << data << "\")");
    _html << data;
  }
}

void Html::onLineBreak()
{
  log_debug("onLineBreak()");
  if (_sub)
    _sub->onLineBreak();
  else
    _html << "<br>\n";
}

void Html::onHorizontalRule()
{
  if (_sub)
    _sub->onHorizontalRule();
  else
  {
    log_debug("onHorizontalRule()");
    _html << "<hr>\n";
  }
}

void Html::onEmphasisBegin()
{
  if (_sub)
    _sub->onEmphasisBegin();
  else
  {
    log_debug("onEmphasisBegin()");
    _html << "<em>";
  }
}

void Html::onEmphasisEnd()
{
  if (_sub)
    _sub->onEmphasisEnd();
  else
  {
    log_debug("onEmphasisEnd()");
    _html << "</em>";
  }
}

void Html::onStrongEmphasisBegin()
{
  if (_sub)
    _sub->onStrongEmphasisBegin();
  else
  {
    log_debug("onStrongEmphasisBegin()");
    _html << "<strong>";
  }
}

void Html::onStrongEmphasisEnd()
{
  if (_sub)
    _sub->onStrongEmphasisEnd();
  else
  {
    log_debug("onStrongEmphasisEnd()");
    _html << "</strong>";
  }
}

void Html::onUnderscoreBegin()
{
  if (_sub)
    _sub->onUnderscoreBegin();
  else
  {
    log_debug("onUnderscoreBegin()");
    _html << "<em>";
  }
}

void Html::onUnderscoreEnd()
{
  if (_sub)
    _sub->onUnderscoreEnd();
  else
  {
    log_debug("onUnderscoreEnd()");
    _html << "</em>";
  }
}

void Html::onDoubleUnderscoreBegin()
{
  if (_sub)
    _sub->onDoubleUnderscoreBegin();
  else
  {
    log_debug("onDoubleUnderscoreBegin()");
    _html << "<strong>";
  }
}

void Html::onDoubleUnderscoreEnd()
{
  if (_sub)
    _sub->onDoubleUnderscoreEnd();
  else
  {
    log_debug("onDoubleUnderscoreEnd()");
    _html << "</strong>";
  }
}

void Html::onLink(const std::string& text, const std::string& target, const std::string& title)
{
  if (_sub)
    _sub->onLink(text, target, title);
  else
  {
    log_debug("onLink(\"" << text << "\", \"" << target << "\", \"" << title << "\")");
    _html << "<a href=\"" << target << '"';

    if (!title.empty())
    {
      if (title.find('"') == std::string::npos)
        _html << " title=\"" << title << '"';
      else
        _html << " title='" << title << '\'';
    }

    _html << '>' << text << "</a>";
  }
}

void Html::finalize()
{
  log_debug("finalize");
  _out << _html.str();
  _html.str(std::string());
}

}
}
