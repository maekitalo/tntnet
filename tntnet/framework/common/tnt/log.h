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

#include <log4cplus/logger.h>
#include <tnt/logfwd.h>

/// logging-Makros
#define log_fatal(expr)  do { LOG4CPLUS_FATAL(getLogger(), expr); } while (false)
#define log_error(expr)  do { LOG4CPLUS_ERROR(getLogger(), expr); } while (false)
#define log_warn(expr)   do { LOG4CPLUS_WARN(getLogger(), expr); } while (false)
#define log_info(expr)   do { LOG4CPLUS_INFO(getLogger(), expr); } while (false)
#define log_debug(expr)  do { LOG4CPLUS_DEBUG(getLogger(), expr); } while (false)

#define log_trace(event)  LOG4CPLUS_TRACE_METHOD(getLogger(), event)

#define log_fatal_ns(ns, expr)  do { LOG4CPLUS_FATAL(ns::getLogger(), expr); } while (false)
#define log_error_ns(ns, expr)  do { LOG4CPLUS_ERROR(ns::getLogger(), expr); } while (false)
#define log_warn_ns(ns, expr)   do { LOG4CPLUS_WARN(ns::getLogger(), expr); } while (false)
#define log_info_ns(ns, expr)   do { LOG4CPLUS_INFO(ns::getLogger(), expr); } while (false)
#define log_debug_ns(ns, expr)  do { LOG4CPLUS_DEBUG(ns::getLogger(), expr); } while (false)

#define log_trace_ns(ns, event)  LOG4CPLUS_TRACE_METHOD(ns::getLogger(), event)

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

#define log_declare_class_inline()   \
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

#endif // LOG_H
