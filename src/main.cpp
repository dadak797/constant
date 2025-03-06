#include "logger_config.h"
#include <thread>
#include <vector>
#ifdef EMSCRIPTEN
    #include <emscripten.h>
#endif

#define NUM_THREADS 4

void print_hello(int thread_id) {
    SPDLOG_INFO("Hello from thread {}", thread_id);
}

void worker_function() {
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(print_hello, i);
    }

    for (auto& th: threads) {
        th.join();
    }
}

void main_loop() {
    // This function will be called repeatedly by the browser.
    // You can process asynchronous tasks here.
}

int main() {
    app::init_logger();
    
    // Create worker thread
    std::thread worker(worker_function);
#ifdef EMSCRIPTEN
    // Detach the worker thread from the main thread
    worker.detach();
    emscripten_set_main_loop(main_loop, 0, true);
#else
    // Join the worker thread with the main thread on native environment
    worker.join();
#endif

    app::shutdown_logger();
    return 0;
}