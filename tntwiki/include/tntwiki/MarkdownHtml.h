/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 */

#ifndef MARKDOWN_HTML_H
#define MARKDOWN_HTML_H

#include <tntwiki/MarkdownParser.h>
#include <sstream>

namespace tntwiki
{
  namespace markdown
  {
    class Html : public ParserEvent
    {
      protected:
        std::ostream& _out;
        std::ostringstream _html;
        Html* _sub;

      public:
        explicit Html(std::ostream& out)
          : _out(out),
            _sub(0)
        { }
        ~Html()
        {
          delete _sub;
        }

        virtual void onParaBegin();
        virtual void onParaEnd();
        virtual void onParaEndHeader(unsigned short level);

        virtual void onBlockQuoteBegin();
        virtual void onBlockQuoteEnd();

        virtual void onListBegin(char ch);
        virtual void onListEntryBegin();
        virtual void onListEntryEnd();
        virtual void onListEnd();

        virtual void onOrderedListBegin();
        virtual void onOrderedListEntryBegin();
        virtual void onOrderedListEntryEnd();
        virtual void onOrderedListEnd();

        virtual void onCodeBegin();
        virtual void onCodeEnd();

        virtual void onData(const std::string& data);
        virtual void onLineBreak();
        virtual void onHorizontalRule();

        virtual void onHeaderBegin(unsigned short level);
        virtual void onHeaderEnd(unsigned short level);

        virtual void onEmphasisBegin();
        virtual void onEmphasisEnd();

        virtual void onStrongEmphasisBegin();
        virtual void onStrongEmphasisEnd();

        virtual void onUnderscoreBegin();
        virtual void onUnderscoreEnd();

        virtual void onDoubleUnderscoreBegin();
        virtual void onDoubleUnderscoreEnd();

        virtual void onLink(const std::string& text, const std::string& target, const std::string& title);

        virtual void finalize();
    };

  }
}

#endif // MARKDOWN_HTML_H

