/* tnt/loginit.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TNT_LOGINIT_H
#define TNT_LOGINIT_H

#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>

#define log_init_fatal()   log_init(log4cplus::FATAL_LOG_LEVEL)
#define log_init_error()   log_init(log4cplus::ERROR_LOG_LEVEL)
#define log_init_warn()    log_init(log4cplus::WARN_LOG_LEVEL)
#define log_init_info()    log_init(log4cplus::INFO_LOG_LEVEL)
#define log_init_debug()   log_init(log4cplus::DEBUG_LOG_LEVEL)
#define log_init_trace()   log_init(log4cplus::TRACE_LOG_LEVEL)

inline void log_init(log4cplus::LogLevel level = log4cplus::ERROR_LOG_LEVEL)
{
  log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender(true, true));
  appender->setName("Main");

  log4cplus::Logger root = log4cplus::Logger::getRoot();
  root.addAppender(appender);
  root.setLogLevel(level);
}

inline void log_init(const std::string& propertyfilename)
{
  log4cplus::PropertyConfigurator logconfig(propertyfilename);
  logconfig.configure();
}

#endif
