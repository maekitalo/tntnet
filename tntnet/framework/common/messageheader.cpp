////////////////////////////////////////////////////////////////////////
// messageheader - Standard-header like rfc822
//

#include <tnt/messageheader.h>
#include <iostream>

namespace tnt
{
  std::istream& operator>> (std::istream& in, messageheader& data)
  {
    data.parse(in);
    return in;
  }

  void messageheader::parse(std::istream& in)
  {
    enum state_type
    {
      state_0,
      state_fieldname,
      state_fieldnamespace,
      state_fieldbody0,
      state_fieldbody,
      state_fieldbody_cr,
      state_fieldbody_crlf,
      state_end_cr,
      state_end
    };

    state_type state = state_0;
    std::streambuf* buf = in.rdbuf();

    std::string fieldname, fieldbody;

    while (state != state_end
        && buf->sgetc() != std::ios::traits_type::eof())
    {
      char ch = buf->sbumpc();
      switch (state)
      {
        case state_0:
          if (ch >= 33 && ch <= 126 && ch != ':')
          {
            fieldname = ch;
            state = state_fieldname;
          }
          else if (!std::isspace(ch))
          {
            in.setstate(std::ios::failbit);
            state = state_end;
          }
          break;

        case state_fieldname:
          if (ch == ':')            // Field-name:
          {
            fieldname += ch;
            state = state_fieldbody0;
          }
          else if (ch >= 33 && ch <= 126)
            fieldname += ch;
          else if (std::isspace(ch))
            state = state_fieldnamespace;
          else
          {
            in.setstate(std::ios::failbit);
            state = state_end;
          }
          break;

        case state_fieldnamespace:
          if (ch == ':')                   // "Field-name :"
            state = state_fieldbody0;
          else if (ch >= 33 && ch <= 126)  // "Field-name blah..."
          {
            fieldbody = ch;
            state = state_fieldbody;
          }
          else if (!std::isspace(ch))
          {
            in.setstate(std::ios::failbit);
            state = state_end;
          }
          break;

        case state_fieldbody0:
          if (ch == '\r')
            state = state_fieldbody_cr;
          else if (ch == '\n')
            state = state_fieldbody_crlf;
          else if (!std::isspace(ch))
          {
            fieldbody = ch;
            state = state_fieldbody;
          }
          break;

        case state_fieldbody:
          if (ch == '\r')
            state = state_fieldbody_cr;
          else if (ch == '\n')
            state = state_fieldbody_crlf;
          else
            fieldbody += ch;
          break;

        case state_fieldbody_cr:
          if (ch == '\n')
            state = state_fieldbody_crlf;
          else
          {
            in.setstate(std::ios::failbit);
            state = state_end;
          }
          break;

        case state_fieldbody_crlf:
          if (ch == '\r')
            state = state_end_cr;
          else if (ch == '\n')
          {
            switch (onField(fieldname, fieldbody))
            {
              case OK:
              case END:  state = state_end; break;
              case FAIL: in.setstate(std::ios::failbit); break;
            }
            fieldname.clear();
            fieldbody.clear();
          }
          else if (std::isspace(ch))
          {
            fieldbody += ch;
            state = state_fieldbody;
          }
          else if (ch >= 33 && ch <= 126)
          {
            switch (onField(fieldname, fieldbody))
            {
              case OK:   state = state_fieldname; break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end; break;
            }
            fieldbody.clear();
            fieldname = ch;
          }
          break;

        case state_end_cr:
          if (ch == '\n')
          {
            if (onField(fieldname, fieldbody) == FAIL)
              in.setstate(std::ios::failbit);

            fieldname.clear();
            fieldbody.clear();
            state = state_end;
          }
          else
          {
            in.setstate(std::ios::failbit);
            state = state_end;
          }
          break;

        case state_end:
          break;
      }
    }
  }

  messageheader::return_type messageheader::onField(const std::string& name, const std::string& value)
  {
    insert(value_type(name, value));
    return OK;
  }
}
