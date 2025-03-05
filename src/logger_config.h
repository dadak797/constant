#pragma once

// SPDLOG_ACTIVE_LEVEL should be defined before first including <spdlog.h>
#ifdef DEBUG_BUILD
  #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
  #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#include <spdlog/spdlog.h>

// Log level (trace -> debug -> info -> warn -> error -> critical)
// - SPDLOG_TRACE: Trace message
// - SPDLOG_DEBUG: Debug message
// - SPDLOG_INFO: Information message
// - SPDLOG_WARN: Warning message
// - SPDLOG_ERROR: Error message
// - SPDLOG_CRITICAL: Critical message

// For debug build (Format: [timestamp] [level] [source:line] message)
// - SPDLOG_DEBUG, SPDLOG_INFO, SPDLOG_WARN, SPDLOG_ERROR and SPDLOG_CRITICAL can be used.
// - SPDLOG_TRACE is not used in debug build.

// For release build (Format: [timestamp] [level] message)
// - SPDLOG_INFO, SPDLOG_WARN, SPDLOG_ERROR and SPDLOG_CRITICAL can be used.
// - SPDLOG_DEBUG and SPDLOG_TRACE are not used in release build.

namespace app {
    // use_console: true - log to console, false - log to file
    // new_file: true - create new log file, false - append to the previous log file
    // If use_console is true, new_file is ignored.
    void init_logger(bool use_console = true, bool new_file = true);

    void shutdown_logger();
}