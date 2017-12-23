Chunked encoding
================

Why chunked encoding
--------------------

By default tntnet uses normal http. The http reply consists of http headers and
a http body. The header specifies the number of bytes of the body. For tntnet
that implies, that it has to know the size of the reply before starting to send
the reply.

Since tntnet components are dynamic the only way to know that is to collect the
output somewhere. This of course needs some memory and delays the answer on the
client side. This is ok for most of the requests since the reply is normally
quite small.

For bigger replies this may not be the best way. Therefore the designers of http
invented another operation mode. Chunked encoding do not send the full size of
the reply at start but sends the reply in smaller portions. The server do not
need to know how many bytes will be sent when starting and it can start sending
before processing is finished.

Starting with tntnet 2.3 supports chunked encoding.

How to use chunked encoding
---------------------------

Using chunked encoding in tntnet is so easy that it do not really fill a whole
document. The component must enable it by calling the method:

    reply.setChunkedEncoding();

After that output is sent in chunks of 8192 bytes to the client.

The application can flush the output stream in which case all data collected so
far is sent in a possibly smaller chunk to the client. This is done with the
normal flush operation of the output stream. In C++ sections the flush method
can be used:

    reply.out().flush();

Alternatively in ecpp the `std::flush` manipulator may be sent:

    <$ std::flush $>

It makes no difference whether the flush is done on `reply.out()` or
`reply.sout()`. The former is the "raw" output stream and the latter is the
output stream which escapes html output.

Limitations of chunked encoding
-------------------------------

Unfortunately there are some limitations in tntnet when using chunked encoding.

The first request of a session must not be chunked encoding since tntnet checks
after the request processing whether session variables were used and when so and
no session cookie was received before, a cookie is sent. Since enabling chunked
encoding sends the headers already and starts sending the body, tntnet can't
send the session cookie any more.

Also the request is not allowed to fail after enabling chunked encoding.
Normally a request can throw a exception and tntnet will reply with a internal
server error to the client. Tntnet can't do that when it has already send the
http ok code. Throwing an exception in the component will just close the
connection, which results in a incomplete reply.

The component may set http headers like setting the content type. The component
has to do that before enabling chunked encoding.
