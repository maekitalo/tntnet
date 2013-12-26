Releasenotes tntnet 2.2
=======================

Cxxtools 2.2 is needed
----------------------
Cxxtools version 2.2 is needed now in tntnet.

New proxy module
----------------
The new module proxy@tntnet allows forwarding request to other http servers.

Secure session scope for variables only valid in ssl sessions
-------------------------------------------------------------
A new scope type `<%securesession>` allows users to define variables which are only valid in ssl connections. Also a separate cookie is sent which a attribute to advise the browser to use it only when ssl is enables.

Configure tntnet with xml (or json) including logging
-----------------------------------------------------
Previously a tntnet was configured with a text file with a simple but proprietary format. Logging was configured in a separate file with a different format. Both is combined now in a single xml file.

Remove cgi library
------------------
There used to be a library, which allows to run ecpp components as cgi processes. The use was limited and the performance in cgi is always bad. As far as I know no one has used it (for good reason). Hence it is removed now.

Extend url mapping with mapping by method or ssl
------------------------------------------------
Mapping requests to components is extended. Not only the url can be used as a condition but also the http method or ssl state. The previous separate mapping per virtual host is now also defined as a mapping condition.

Use serialization framework to read %config variables from tntnet configuration to support complex configurations
-----------------------------------------------------------------------------------------------------------------
The ecpp tag `<%config>` allows web applications to read values from the tntnet configuration file. Formerly only a scalar variable could be read. Now using the cxxtools serialization complex objects can be configured and passed to a web application.

ecpp compiler do not stop on first error any more but tries to find more problems
---------------------------------------------------------------------------------
The ecpp compiler `ecppc` stopped compiling on the first error. Now it tries to recover and continues compiling and reporting errors.

Allow applications to clear the current session
-----------------------------------------------
Calling the new method `request.clearSession()` clears all session variables after the current request.

Read filenames for multibinary component optionally from file using option -i in ecppc
--------------------------------------------------------------------------------------
Multibinary components contain a collection of static files, which is compiled as a single tntnet component. This helps saving overhead since each component needs some code, which makes it callable. Since the list of static files used in a web application may be quite long, it is often convenient to maintain the list in a separate file. Also a very long list may extend the maximum allowed length of a shell command. Now `ecppc` can read the list from a file to solve that.

Arguments in url mapping have now names
---------------------------------------
When mapping a request to a component, arguments can be passed from the configuration to the component. Previously it was just a list of values. Now the values has names, which makes handling easier and more readable.

Allow distinquishing between GET and POST arguments
---------------------------------------------------
Ecpp allows defining query parameters using a `<%args>` tag. Tntnet passed here all GET and POST parameters. For developers it was difficult to find out, if arguments were passed via GET or POST. Now arguments can be defined in a `<%get>` or `<%post>` section to get only the GET or POST parameters.

Bugfixes and optimizations
--------------------------
Some minor bugfixes and optimizations are done in tntnet.
