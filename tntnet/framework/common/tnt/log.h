/* tnt/log.h
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

#ifndef TNT_LOG_H
#define TNT_LOG_H

#include <tnt/logfwd.h>

#ifdef TNTNET_USE_LOG4CXX

#include <log4cxx/logger.h>

#define log_fatal(expr)   do { LOG4CXX_FATAL(getLogger(), expr); } while(false)
#define log_error(expr)   do { LOG4CXX_ERROR(getLogger(), expr); } while(false)
#define log_warn(expr)    do { LOG4CXX_WARN(getLogger(), expr); } while(false)
#define log_info(expr)    do { LOG4CXX_INFO(getLogger(), expr); } while(false)
#define log_debug(expr)   do { LOG4CXX_DEBUG(getLogger(), expr); } while(false)

namespace tnt
{
  class log4cxx_tracer
  {
      ::log4cxx::LoggerPtr logger;
      std::string event;

    public:
      log4cxx_tracer(::log4cxx::LoggerPtr _logger, const std::string& _event)
        : logger(_logger),
          event(_event)
        { LOG4CXX_DEBUG(logger, "ENTER " + event); }
      ~log4cxx_tracer()
        { LOG4CXX_DEBUG(logger, "EXIT " + event); }
  };
}

#define log_trace(event)  ::tnt::log4cxx_tracer log4cxx_trace_logger(getLogger(), event)

#define log_fatal_ns(ns, expr)   do { LOG4CXX_FATAL(ns::getLogger(), expr); } while(false)
#define log_error_ns(ns, expr)   do { LOG4CXX_ERROR(ns::getLogger(), expr); } while(false)
#define log_warn_ns(ns, expr)    do { LOG4CXX_WARN(ns::getLogger(), expr); } while(false)
#define log_info_ns(ns, expr)    do { LOG4CXX_INFO(ns::getLogger(), expr); } while(false)
#define log_debug_ns(ns, expr)   do { LOG4CXX_DEBUG(ns::getLogger(), expr); } while(false)

#define log_trace_ns(ns, event)  ::tnt::log4cxx_tracer log4cxx_trace_logger(ns::getLogger(), event)

/// Makros zum definieren von Loggingkategorien pro Namespace

#define log_define_namespace(ns, category) \
  namespace ns \
  { \
    ::log4cxx::LoggerPtr getLogger()  \
    {  \
      static const ::std::string log_category = category;  \
      return ::log4cxx::Logger::getLogger(log_category); \
    } \
  }
#define log_declare_class_inline(category) \
  static log4cxx::LoggerPtr getLogger()   \
  {  \
    static const std::string log_category = category;  \
    return ::log4cxx::Logger::getLogger(log_category); \
  }
#define log_define_class(classname, category) \
  log4cxx::LoggerPtr classname::getLogger()   \
  {  \
    static const std::string log_category = category;  \
    return ::log4cxx::Logger::getLogger(log_category); \
  }

#endif

#ifdef TNTNET_USE_LOG4CPLUS

#include <log4cplus/logger.h>

/// logging-Makros
#define log_fatal(expr)  do { LOG4CPLUS_FATAL(getLogger(), expr) } while(false)
#define log_error(expr)  do { LOG4CPLUS_ERROR(getLogger(), expr) } while(false)
#define log_warn(expr)   do { LOG4CPLUS_WARN(getLogger(), expr) } while(false)
#define log_info(expr)   do { LOG4CPLUS_INFO(getLogger(), expr) } while(false)
#define log_debug(expr)  do { LOG4CPLUS_DEBUG(getLogger(), expr) } while(false)

#define log_trace(event)  do { LOG4CPLUS_TRACE_METHOD(getLogger(), event) } while(false)

#define log_fatal_ns(ns, expr)  do { LOG4CPLUS_FATAL(ns::getLogger(), expr) } while(false)
#define log_error_ns(ns, expr)  do { LOG4CPLUS_ERROR(ns::getLogger(), expr) } while(false)
#define log_warn_ns(ns, expr)   do { LOG4CPLUS_WARN(ns::getLogger(), expr) } while(false)
#define log_info_ns(ns, expr)   do { LOG4CPLUS_INFO(ns::getLogger(), expr) } while(false)
#define log_debug_ns(ns, expr)  do { LOG4CPLUS_DEBUG(ns::getLogger(), expr) } while(false)

#define log_trace_ns(ns, event)  do { LOG4CPLUS_TRACE_METHOD(ns::getLogger(), event) } while(false)

/// Makros zum definieren von Loggingkategorien pro Namespace

#define log_define_namespace(ns, category)  \
  namespace ns \
  { \
    log4cplus::Logger getLogger()  \
    {  \
      static const std::string log_category = category;  \
      log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(log_category);  \
      return tntlogger;  \
    } \
  }

#define log_declare_class_inline(category)   \
  static log4cplus::Logger getLogger()   \
  {  \
    static const std::string log_category = category;  \
    log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(log_category);  \
    return tntlogger;  \
  }

#define log_define_class(classname, category)   \
  log4cplus::Logger classname::getLogger()   \
  {  \
    static const std::string log_category = category;  \
    log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(log_category);  \
    return tntlogger;  \
  }

#endif

#ifdef TNTNET_USE_LOGSTDOUT

#define log_fatal(expr)   do { std::cout << expr; } while (false)
#define log_error(expr)   do { std::cout << expr; } while (false)
#define log_warn(expr)    do { std::cout << expr; } while (false)
#define log_info(expr)    do { std::cout << expr; } while (false)
#define log_debug(expr)

#define log_trace(event)

#define log_fatal_ns(ns, expr)  log_fatal(expr)
#define log_error_ns(ns, expr)  log_error(expr)
#define log_warn_ns(ns, expr)   log_warn(expr)
#define log_info_ns(ns, expr)   log_info(expr)
#define log_debug_ns(ns, expr)  log_debug(expr)

#define log_trace_ns(ns, event)  log_trace(event)

/// Makros zum definieren von Loggingkategorien pro Namespace

#define log_define_namespace(ns, category)
#define log_declare_class_inline(category)
#define log_define_class(classname, category)

#endif

#endif // LOG_H
