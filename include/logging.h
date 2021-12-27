#ifndef _LIBMSG_LOGGING_H_
#define _LIBMSG_LOGGING_H_

#include <stdio.h>
#include <string.h>

#define LOG_NONE 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_TRACE 4
#define LOG_DEBUG 5

// compatibile with C, not only C++ can use log interface.
extern "C" {
int current_loglevel();
void set_current_loglevel(int);
}

#ifndef LOG_HEADER
#define LOG_HEADER " <== %s:%s|%s()"
#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_HEADVAR __FILENAME__, __LINE__, __FUNCTION__
#undef __FILENAME__
#endif

#ifndef LOG_DEV
#define LOG_DEV stderr
#endif

#ifndef LOGLEVEL
#define LOGLEVEL LOG_INFO
#endif

#define LOG_MSG(level, format, ...)               \
  do {                                            \
    if (level <= current_loglevel()) {            \
      fprintf(LOG_DEV, format "\n", __VA_ARGS__); \
    }                                             \
  } while (0)

#if LOGLEVEL >= LOG_DEBUG
#define DEBUG(msg, args...) \
  LOG_MSG(LOG_DEBUG, "[DEBUG] " msg LOG_HEADER, ##args, LOG_HEADVAR)
#else
#define DEBUG(msg, args...)
#endif

#if LOGLEVEL >= LOG_TRACE
#define TRACE(msg, args...) \
  LOG_MSG(LOG_TRACE, "[TRACE] " msg LOG_HEADER, ##args, LOG_HEADVAR)
#else
#define TRACE(msg, args...)
#endif

#if LOGLEVEL >= LOG_INFO
#undef INFO
#define INFO(msg, args...) \
  LOG_MSG(LOG_INFO, "[INFO] " msg LOG_HEADER, ##args, LOG_HEADVAR)
#else
#define INFO(msg, args...)
#endif

#if LOGLEVEL >= LOG_WARN
#undef WARN
#define WARN(msg, args...) \
  LOG_MSG(LOG_WARN, "[WARN] " msg LOG_HEADER, ##args, LOG_HEADVAR)
#else
#define WARN(msg, args...)
#endif

#if LOGLEVEL >= LOG_ERROR
#define ERROR(msg, args...) \
  LOG_MSG(LOG_ERROR, "[ERROR] " msg LOG_HEADER, ##args, LOG_HEADVAR)
#else
#define ERROR(msg, args...)
#endif

#endif  // _LIBMSG_LOGGING_H_