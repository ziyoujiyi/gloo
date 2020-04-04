#ifndef PRINT_LOGGING_H
#define PRINT_LOGGING_H

#include <sstream>
#include <string>

namespace gloo {

enum class GlooLogLevel {
  TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
};

#define GLOO_LOG_LEVELS "TDIWEF"

class GlooLogMessage : public std::basic_ostringstream<char> {
 public:
  GlooLogMessage(const char* fname, int line, GlooLogLevel severity);
  ~GlooLogMessage();

 protected:
  void GenerateLogMessage(bool log_time);

 private:
  const char* fname_;
  int line_;
  GlooLogLevel severity_;
};

// LogMessageFatal ensures the process will exit in failure after
// logging this message.
class GlooLogMessageFatal : public GlooLogMessage {
 public:
  GlooLogMessageFatal(const char* file, int line);
  ~GlooLogMessageFatal();
};

#define GLOO_HVD_LOG_TRACE \
  GlooLogMessage(__FILE__, __LINE__, GlooLogLevel::TRACE)
#define GLOO_HVD_LOG_DEBUG \
  GlooLogMessage(__FILE__, __LINE__, GlooLogLevel::DEBUG)
#define GLOO_HVD_LOG_INFO \
  GlooLogMessage(__FILE__, __LINE__, GlooLogLevel::INFO)
#define GLOO_HVD_LOG_WARNING \
  GlooLogMessage(__FILE__, __LINE__, GlooLogLevel::WARNING)
#define GLOO_HVD_LOG_ERROR \
  GlooLogMessage(__FILE__, __LINE__, GlooLogLevel::ERROR)
#define GLOO_HVD_LOG_FATAL \
  GlooLogMessageFatal(__FILE__, __LINE__)

#define _GLOO_LOG(severity) GLOO_HVD_LOG_##severity

#define GLOO_LOG_RANK(severity, rank) GLOO_HVD_LOG_##severity << "[" << rank << "]: "

#define GLOO_GET_LOG(_1, _2, NAME, ...) NAME
#define GLOO_LOG(...) GLOO_GET_LOG(__VA_ARGS__, _GLOO_LOG_RANK, _GLOO_LOG)(__VA_ARGS__)

GlooLogLevel GlooMinLogLevelFromEnv();
bool GlooLogTimeFromEnv();

}

#endif // PRINT_LOGGING_H
