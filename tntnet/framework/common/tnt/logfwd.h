////////////////////////////////////////////////////////////////////////
// tnt/logfwd.h
//

#ifndef TNT_LOGFWD_H
#define TNT_LOGFWD_H

namespace log4cplus
{
  class Logger;
  Logger getLogger();
};

/// Makros zum definieren von Loggingkategorien pro Namespace

#define log_declare_namespace(ns)   \
  namespace ns { log4cplus::Logger getLogger(); }

#define log_declare_class()   \
  static log4cplus::Logger getLogger()

log_declare_namespace(tnt);
log_declare_namespace(tntcomp);
log_declare_namespace(ecpp_component);
log_declare_namespace(compcall);

#endif // TNT_LOGFWD_H

