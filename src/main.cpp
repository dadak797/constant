#include "logger_config.h"
#include <thread>
#include <vector>

#define NUM_THREADS 4


void print_hello(int thread_id) {
    SPDLOG_INFO("Hello from thread {}", thread_id);
}

int main() {
    app::init_logger();

    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(print_hello, i);
    }

    for (auto& th: threads) {
        th.join();
    }

    app::shutdown_logger();
    return 0;
}
