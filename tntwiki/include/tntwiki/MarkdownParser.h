/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef TNTWIKI_MARKDOWNPARSER_H
#define TNTWIKI_MARKDOWNPARSER_H

#include <string>
#include <cxxtools/smartptr.h>
#include <cxxtools/refcounted.h>

namespace tntwiki
{
  namespace markdown
  {
    struct ParserEvent
    {
      // data
      virtual void onData(const std::string& data) { }

      // span elements
      virtual void onEmphasisBegin() { }
      virtual void onEmphasisEnd() { }

      virtual void onStrongEmphasisBegin() { }
      virtual void onStrongEmphasisEnd() { }

      virtual void onUnderscoreBegin() { }
      virtual void onUnderscoreEnd() { }

      virtual void onDoubleUnderscoreBegin() { }
      virtual void onDoubleUnderscoreEnd() { }

      virtual void onLink(const std::string& target) { }

      // block elements
      virtual void onLineBreak() { }
      virtual void onHorizontalRule() { }

      virtual void onHeaderBegin(unsigned short level) { }
      virtual void onHeaderEnd(unsigned short level) { }

      virtual void onParaBegin() { }
      virtual void onParaEnd() { }
      virtual void onParaEndHeader(unsigned short level) { }

      virtual void onCodeBegin() { }
      virtual void onCodeEnd() { }

      virtual void onBlockQuoteBegin() { }
      virtual void onBlockQuoteEnd() { }

      virtual void onListBegin(char ch) { }
      virtual void onListEntryBegin() { }
      virtual void onListEntryEnd() { }
      virtual void onListEnd() { }
      virtual void onOrderedListBegin() { }
      virtual void onOrderedListEntryBegin() { }
      virtual void onOrderedListEntryEnd() { }
      virtual void onOrderedListEnd() { }

      // finalize
      virtual void finalize() { }
    };

    class SpanElementParser
    {
        ParserEvent& _event;

        enum {
          state_0,
          state_esc,
          state_em0,
          state_em,
          state_emesc,
          state_em2,
          state_em2esc,
          state_em2e,
          state_underscore0,
          state_underscore,
          state_underscoreesc,
          state_underscore2,
          state_underscore2esc,
          state_underscore2e,
          state_code0,
          state_code,
          state_codeesc,
          state_code2,
          state_code2esc,
          state_code2e,
        } _state;

        std::string _data;

      public:
        explicit SpanElementParser(ParserEvent& event)
          : _event(event),
            _state(state_0)
          { }

        void parse(char ch);
        void finalize();
        void parse(const std::string& data, bool doFinalize = true)
        {
          for (std::string::const_iterator it = data.begin(); it != data.end(); ++it)
            parse(*it);

          if (doFinalize)
            finalize();
        }

        void reset()
        {
          _state = state_0;
          _data.clear();
        }

        bool empty() const
        {
          return _data.empty();
        }

        const std::string& str() const
        {
          return _data;
        }
    };

    class Parser : public cxxtools::RefCounted
    {
        ParserEvent& _event;
        enum {
          state_0,
          state_hr0,
          state_hr,
          state_sp,
          state_list1,
          state_list,
          state_listend,
          state_orderedList_innumber0,
          state_orderedList_startentry,
          state_orderedList_inentry,
          state_orderedList_afterentry,
          state_orderedList_innumber,
          state_blockquote,
          state_blockquote_end,
          state_para,
          state_paraend,
          state_code,
          state_codeend,
          state_h1,
          state_h2,
          state_header,
          state_headerp
        } _state;

        unsigned _lineNo;
        bool _para;
        cxxtools::SmartPtr<Parser> _sub;
        std::string _token;
        SpanElementParser _data;
        char _listChar;

        // disable copy and assignment
        Parser(Parser&);
        Parser& operator=(Parser&);

      public:
        explicit Parser(ParserEvent& event, bool para = true)
          : _event(event),
            _state(state_0),
            _lineNo(1),
            _para(para),
            _sub(0),
            _data(event)
        { }

        void reset()
        { _state = state_0; _lineNo = 1; _data.reset(); }

        unsigned lineNo() const   { return _lineNo; }

        void parse(char ch);
        void finalize();

        void parse(const std::string& data, bool doFinalize = true)
        {
          for (std::string::const_iterator it = data.begin(); it != data.end(); ++it)
            parse(*it);

          if (doFinalize)
            finalize();
        }
    };

  }
}

#endif // TNTWIKI_MARKDOWNPARSER_H

