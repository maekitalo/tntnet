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

#include <tnt/config.h>

#ifdef TNTNET_USE_LOG4CXX

#include <log4cxx/logger.h>

#define log_init_fatal()   log_init(::log4cxx::Level::FATAL)
#define log_init_error()   log_init(::log4cxx::Level::ERROR)
#define log_init_warn()    log_init(::log4cxx::Level::WARN)
#define log_init_info()    log_init(::log4cxx::Level::INFO)
#define log_init_debug()   log_init(::log4cxx::Level::DEBUG)
#define log_init_trace()   log_init(::log4cxx::Level::TRACE)

void log_init();
void log_init(::log4cxx::LevelPtr level);
void log_init(const std::string& filename);
void log_init(const char* filename)
{ log_init(std::string(filename)); }

#endif

#ifdef TNTNET_USE_LOG4CPLUS

#include <log4cplus/loglevel.h>

#define log_init_fatal()   log_init(log4cplus::FATAL_LOG_LEVEL)
#define log_init_error()   log_init(log4cplus::ERROR_LOG_LEVEL)
#define log_init_warn()    log_init(log4cplus::WARN_LOG_LEVEL)
#define log_init_info()    log_init(log4cplus::INFO_LOG_LEVEL)
#define log_init_debug()   log_init(log4cplus::DEBUG_LOG_LEVEL)
#define log_init_trace()   log_init(log4cplus::TRACE_LOG_LEVEL)

void log_init(log4cplus::LogLevel level = log4cplus::ERROR_LOG_LEVEL);
void log_init(const std::string& propertyfilename);

#endif

#ifdef TNTNET_USE_LOGSTDOUT

#include <string>

enum log_level_type {
  FATAL_LOG_LEVEL = 0,
  ERROR_LOG_LEVEL = 100,
  WARN_LOG_LEVEL  = 200,
  INFO_LOG_LEVEL  = 300,
  DEBUG_LOG_LEVEL = 400,
  TRACE_LOG_LEVEL = 500
}

#define log_init_fatal()   log_init(FATAL_LOG_LEVEL)
#define log_init_error()   log_init(ERROR_LOG_LEVEL)
#define log_init_warn()    log_init(WARN_LOG_LEVEL)
#define log_init_info()    log_init(INFO_LOG_LEVEL)
#define log_init_debug()   log_init(DEBUG_LOG_LEVEL)
#define log_init_trace()   log_init(TRACE_LOG_LEVEL)

void log_init(log_level_type level = ERROR_LOG_LEVEL);
inline void log_init(const std::string& propertyfilename)
{ }

#endif

#endif
