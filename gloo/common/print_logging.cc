#include "gloo/common/print_logging.h"
#include <chrono>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace gloo {

GlooLogMessage::GlooLogMessage(const char* fname, int line, GlooLogLevel severity)
    : fname_(fname), line_(line), severity_(severity) {}

void GlooLogMessage::GenerateLogMessage(bool log_time) {
  bool use_cout = static_cast<int>(severity_) <= static_cast<int>(GlooLogLevel::INFO);
  std::ostream& os = use_cout ? std::cout : std::cerr;
  if (log_time) {
    auto now = std::chrono::system_clock::now();
    auto as_time_t = std::chrono::system_clock::to_time_t(now);

    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto micros_remainder = std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds);

    const size_t time_buffer_size = 30;
    char time_buffer[time_buffer_size];
    strftime(time_buffer, time_buffer_size, "%Y-%m-%d %H:%M:%S",
             localtime(&as_time_t));
    os << "[" << time_buffer << "." << std::setw(6) << micros_remainder.count() 
              << ": " << GLOO_LOG_LEVELS[static_cast<int>(severity_)] << " " 
              << fname_ << ":" << line_ << "] " << str() << std::endl;
  } else {
    os << "[" << GLOO_LOG_LEVELS[static_cast<int>(severity_)] << " " 
              << fname_ << ":" << line_ << "] " << str() << std::endl;
  }
}

GlooLogMessage::~GlooLogMessage() {
  static GlooLogLevel min_log_level = GlooMinLogLevelFromEnv();
  static bool log_time = GlooLogTimeFromEnv();
  if (severity_ >= min_log_level) {
    GenerateLogMessage(log_time);
  }
}

GlooLogMessageFatal::GlooLogMessageFatal(const char* file, int line)
    : GlooLogMessage(file, line, GlooLogLevel::FATAL) {}

GlooLogMessageFatal::~GlooLogMessageFatal() {
  static bool log_time = GlooLogTimeFromEnv();
  GenerateLogMessage(log_time);
  abort();
}

GlooLogLevel ParseLogLevelStr(const char* env_var_val) {
  std::string min_log_level(env_var_val);
  std::transform(min_log_level.begin(), min_log_level.end(), min_log_level.begin(), ::tolower);
  if (min_log_level == "trace") {
    return GlooLogLevel::TRACE;
  } else if (min_log_level == "debug") {
    return GlooLogLevel::DEBUG;
  } else if (min_log_level == "info") {
    return GlooLogLevel::INFO;
  } else if (min_log_level == "warning") {
    return GlooLogLevel::WARNING;
  } else if (min_log_level == "error") {
    return GlooLogLevel::ERROR;
  } else if (min_log_level == "fatal") {
    return GlooLogLevel::FATAL;
  } else {
    return GlooLogLevel::WARNING;
  }
}

GlooLogLevel GlooMinLogLevelFromEnv() {
  const char* env_var_val = getenv("GLOO_LOG_LEVEL");
  if (env_var_val == nullptr) {
    // default to WARNING
    return GlooLogLevel::WARNING;
  }
  return ParseLogLevelStr(env_var_val);
}

bool GlooLogTimeFromEnv() {
  const char* env_var_val = getenv("GLOO_LOG_HIDE_TIME");
  if (env_var_val != nullptr &&
      std::strtol(env_var_val, nullptr, 10) > 0) {
    return false;
  } else {
    return true;
  }
}

}
