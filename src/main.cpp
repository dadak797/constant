#include "logger_config.h"
#include <pthread.h>

#define NUM_THREADS 4


void* print_hello(void* arg) {
    int thread_id = *(int*)arg;
    SPDLOG_INFO("Hello from thread {}", thread_id);
    return nullptr;
}

int main() {
    app::init_logger();

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], nullptr, print_hello, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], nullptr);
    }

    while(true) {
    }

    app::shutdown_logger();
    return 0;
}
