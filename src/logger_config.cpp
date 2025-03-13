#include "logger_config.h"

#include <iostream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>


namespace App {
    void InitLogger(bool useConsole, bool newFile) {
        try {
            if (useConsole) {
                auto console = spdlog::stdout_color_mt("console");
                spdlog::set_default_logger(console);
            }
            else {
                auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/spdlog.log", newFile);
                spdlog::set_default_logger(file_logger);
            }
        #ifdef DEBUG_BUILD
            spdlog::set_level(spdlog::level::debug);
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
        #else
            spdlog::set_level(spdlog::level::info);
            spdlog::set_pattern("[%^%l%$] %v");
        #endif
        }
        catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "spdlog initialization error: " << ex.what() << std::endl;
        }
    }

    void ShutdownLogger() {
        spdlog::shutdown();
    }
}