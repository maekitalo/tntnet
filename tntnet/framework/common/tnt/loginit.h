////////////////////////////////////////////////////////////////////////
// loginit.h
//

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
