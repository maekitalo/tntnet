////////////////////////////////////////////////////////////////////////
// http.h
//

#ifndef HTTP_H
#define HTTP_H

#include <sstream>
#include <cxxtools/query_params.h>
#include <netinet/in.h>
#include <tnt/messageheader.h>
#include <tnt/contenttype.h>
#include <tnt/multipart.h>

static const unsigned DECLINED = 0;
static const unsigned HTTP_CONTINUE = 100;
static const unsigned HTTP_SWITCHING_PROTOCOLS = 101;
static const unsigned HTTP_PROCESSING = 102;
static const unsigned HTTP_OK = 200;
static const unsigned HTTP_CREATED = 201;
static const unsigned HTTP_ACCEPTED = 202;
static const unsigned HTTP_NON_AUTHORITATIVE = 203;
static const unsigned HTTP_NO_CONTENT = 204;
static const unsigned HTTP_RESET_CONTENT = 205;
static const unsigned HTTP_PARTIAL_CONTENT = 206;
static const unsigned HTTP_MULTI_STATUS = 207;
static const unsigned HTTP_MULTIPLE_CHOICES = 300;
static const unsigned HTTP_MOVED_PERMANENTLY = 301;
static const unsigned HTTP_MOVED_TEMPORARILY = 302;
static const unsigned HTTP_SEE_OTHER = 303;
static const unsigned HTTP_NOT_MODIFIED = 304;
static const unsigned HTTP_USE_PROXY = 305;
static const unsigned HTTP_TEMPORARY_REDIRECT = 307;
static const unsigned HTTP_BAD_REQUEST = 400;
static const unsigned HTTP_UNAUTHORIZED = 401;
static const unsigned HTTP_PAYMENT_REQUIRED = 402;
static const unsigned HTTP_FORBIDDEN = 403;
static const unsigned HTTP_NOT_FOUND = 404;
static const unsigned HTTP_METHOD_NOT_ALLOWED = 405;
static const unsigned HTTP_NOT_ACCEPTABLE = 406;
static const unsigned HTTP_PROXY_AUTHENTICATION_REQUIRED = 407;
static const unsigned HTTP_REQUEST_TIME_OUT = 408;
static const unsigned HTTP_CONFLICT = 409;
static const unsigned HTTP_GONE = 410;
static const unsigned HTTP_LENGTH_REQUIRED = 411;
static const unsigned HTTP_PRECONDITION_FAILED = 412;
static const unsigned HTTP_REQUEST_ENTITY_TOO_LARGE = 413;
static const unsigned HTTP_REQUEST_URI_TOO_LARGE = 414;
static const unsigned HTTP_UNSUPPORTED_MEDIA_TYPE = 415;
static const unsigned HTTP_RANGE_NOT_SATISFIABLE = 416;
static const unsigned HTTP_EXPECTATION_FAILED = 417;
static const unsigned HTTP_UNPROCESSABLE_ENTITY = 422;
static const unsigned HTTP_LOCKED = 423;
static const unsigned HTTP_FAILED_DEPENDENCY = 424;
static const unsigned HTTP_UPGRADE_REQUIRED = 426;
static const unsigned HTTP_INTERNAL_SERVER_ERROR = 500;
static const unsigned HTTP_NOT_IMPLEMENTED = 501;
static const unsigned HTTP_BAD_GATEWAY = 502;
static const unsigned HTTP_SERVICE_UNAVAILABLE = 503;
static const unsigned HTTP_GATEWAY_TIME_OUT = 504;
static const unsigned HTTP_VERSION_NOT_SUPPORTED = 505;
static const unsigned HTTP_VARIANT_ALSO_VARIES = 506;
static const unsigned HTTP_INSUFFICIENT_STORAGE = 507;
static const unsigned HTTP_NOT_EXTENDED = 510;

namespace tnt
{
  /// Basisklasse für HTTP-Messages
  class httpMessage
  {
    public:
      typedef messageheader header_type;

      static const std::string Content_Type;
      static const std::string Content_Length;
      static const std::string Connection;
      static const std::string Connection_close;
      static const std::string Connection_Keep_Alive;
      static const std::string Last_Modified;
      static const std::string Server;
      static const std::string ServerName;

    private:
      std::string method;
      std::string url;
      std::string query_string;
      std::string body;
      unsigned short major_version;
      unsigned short minor_version;

      size_t content_size;

    protected:
      header_type header;

    public:
      httpMessage()
        : major_version(0),
          minor_version(0)
        { }
      virtual ~httpMessage()
      { }

      void clear();
      virtual void parse(std::istream& in);

      const std::string& getMethod() const      { return method; }
      const std::string& getUrl() const         { return url; }
      const std::string& getQueryString() const { return query_string; }
      std::string getHeader(const std::string& key,
        const std::string& def = std::string()) const;
      const std::string& getBody() const        { return body; }

      unsigned short getMajorVersion() const
        { return major_version; }
      unsigned short getMinorVersion() const
        { return minor_version; }
      void setVersion(unsigned short major, unsigned short minor)
        { major_version = major; minor_version = minor; }

      size_t getContentSize() const
        { return content_size; }

      header_type::const_iterator header_begin() const
        { return header.begin(); }
      header_type::const_iterator header_end() const
        { return header.end(); }

      void setHeader(const std::string& key, const std::string& value);
      void setContentLengthHeader(size_t size);

      std::string dumpHeader() const;
      void dumpHeader(std::ostream& out) const;

      bool keepAlive() const;

      static std::string htdate(time_t t);
      static std::string htdate(struct tm* tm);

      static bool checkUrl(const std::string& url);

    private:
      void parseStartline(std::istream& in);
      void parseBody(std::istream& in);
  };

  inline std::istream& operator>> (std::istream& in, httpMessage& msg)
  { msg.parse(in); return in; }

  /// eine HTTP-Request-message
  class httpRequest : public httpMessage
  {
    public:
      typedef std::vector<std::string> args_type;

    private:
      std::string pathinfo;
      args_type args;
      query_params qparam;
      struct sockaddr_in peerAddr;
      struct sockaddr_in serverAddr;
      contenttype ct;
      multipart mp;
      bool ssl;
      unsigned serial;
      static unsigned serial_;

    public:
      httpRequest() : ssl(false) { }

      void setPathInfo(const std::string& p)       { pathinfo = p; }
      const std::string& getPathInfo() const       { return pathinfo; }

      void setArgs(const args_type& a)             { args = a; }
      const args_type& getArgs() const             { return args; }
      args_type::const_reference getArg(args_type::size_type n) const
                                                   { return args[n]; }
      args_type::size_type getArgsCount() const    { return args.size(); }

      virtual void parse(std::istream& in);

      query_params& getQueryParams()              { return qparam; }
      const query_params& getQueryParams() const  { return qparam; }

      void setPeerAddr(const struct sockaddr_in& p)
        { memcpy(&peerAddr, &p, sizeof(struct sockaddr_in)); }
      const struct sockaddr_in& getPeerAddr() const
        { return peerAddr; }
      std::string getPeerIp() const;

      void setServerAddr(const struct sockaddr_in& p)
        { memcpy(&serverAddr, &p, sizeof(struct sockaddr_in)); }
      const struct sockaddr_in& getServerAddr() const
        { return serverAddr; }
      std::string getServerIp() const;
      unsigned short int getServerPort() const
        { return ntohs(serverAddr.sin_port); }

      void setSsl(bool sw = true)
        { ssl = sw; }
      bool isSsl() const
        { return ssl;  }
      const contenttype& getContentType() const  { return ct; }
      bool isMultipart() const               { return ct.isMultipart(); }
      const multipart& getMultipart() const  { return mp; }

      unsigned getSerial() const             { return serial; }
  };

  /// eine HTTP-Reply-message
  class httpReply : public httpMessage
  {
      std::string contentType;
      std::ostream& socket;
      std::ostringstream outstream;
      std::ostream* current_outstream;

    public:
      httpReply(std::ostream& s)
        : contentType("text/html"),
          socket(s),
          current_outstream(&outstream)
      { }

      void setContentType(const std::string& t)    { contentType = t; }
      const std::string& getContentType() const    { return contentType; }

      virtual void throwError(unsigned errorCode, const std::string& errorMessage) const;
      virtual void throwError(const std::string& errorMessage) const;
      virtual void throwNotFound(const std::string& errorMessage) const;
      unsigned redirect(const std::string& newLocation)
      {
        setHeader("Location", newLocation);
        return HTTP_MOVED_TEMPORARILY;
      }

      void sendReply(unsigned ret);

      std::ostream& out()   { return *current_outstream; }

      virtual void setDirectMode();
      virtual void setDirectModeNoFlush();
      virtual bool isDirectMode() const
        { return current_outstream == &socket; }
      std::string::size_type getContentSize() const
        { return outstream.str().size(); }
  };

  /// HTTP-Fehler-Klasse
  class httpError : public std::exception
  {
      std::string msg;

    public:
      httpError(const std::string& m)
        : msg(m)
        { }

      httpError(unsigned errcode, const std::string& msg);
      ~httpError() throw ()
        { }

      const char* what() const throw ()
      { return msg.c_str(); }
  };

  /// HTTP-Fehler 404
  class notFoundException : public httpError
  {
    public:
      notFoundException(const std::string& url)
        : httpError(404, "Not Found (" + url + ')')
        { }
  };
}

#endif // HTTP_H
