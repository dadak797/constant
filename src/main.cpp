#include "logger_config.h"


int main() {
    app::init_logger();

    SPDLOG_TRACE("hello, trace");
    SPDLOG_DEBUG("hello, debug");
    SPDLOG_INFO("hello, info");
    SPDLOG_WARN("hello, warn");
    SPDLOG_ERROR("hello, error");

    spdlog::trace("hello, trace");
    spdlog::debug("hello, debug");
    spdlog::info("hello, info");
    spdlog::warn("hello, warn");
    spdlog::error("hello, error");

    app::shutdown_logger();
    return 0;
}