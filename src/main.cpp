#include "logger_config.h"
#include <thread>
#include <vector>
#include <fstream>
#ifdef EMSCRIPTEN
    #include <emscripten.h>
#endif

#define NUM_THREADS 4


#ifdef EMSCRIPTEN
    std::vector<uint8_t> shared_buffer;
    std::atomic<bool> buffer_ready { false };
#endif

void print_hello(int thread_id) {
    SPDLOG_INFO("Save binary file from thread {}", thread_id);
    
    std::stringstream ss;

    // Add header
    unsigned char header[] = {0xAB, 0xCD, 0xEF, 0x12};
    ss.write(reinterpret_cast<const char*>(header), sizeof(header));
    
    std::string text = "Hello, World";
    ss.write(text.c_str(), text.size());
    
    // Add footer
    unsigned char footer[] = {0xFF, 0xFE, 0xFD, 0xFC};
    ss.write(reinterpret_cast<const char*>(footer), sizeof(footer));
    
#ifdef EMSCRIPTEN
    // Copy data on shared buffer (shared between threads)
    shared_buffer.assign(
        std::istreambuf_iterator<char>(ss), 
        std::istreambuf_iterator<char>()
    );
    
    // Release memory order is used to ensure that the data is written before the flag is set
    buffer_ready.store(true, std::memory_order_release);
#else
    // Save ss to file
    std::ofstream ofs("hello.bin", std::ios::binary);
    ofs << ss.rdbuf();
    ofs.close();
#endif
}

void worker_function() {
    std::thread thread(print_hello, 0);
    thread.join();
}

#ifdef EMSCRIPTEN
void main_loop() {
    // This function will be called repeatedly by the browser.
    // You can process asynchronous tasks here.

    if (buffer_ready.load(std::memory_order_acquire)) {
        buffer_ready.store(false, std::memory_order_relaxed);
        
        // Write shared buffer to a binary file
        std::ofstream outfile("hello.bin", std::ios::binary);
        if (outfile.is_open()) {
            outfile.write(reinterpret_cast<const char*>(shared_buffer.data()), shared_buffer.size());
            outfile.close();
            
            // Download the binary file
            EM_ASM({
                const data = FS.readFile('hello.bin');
                const blob = new Blob([data], { type: 'application/octet-stream' });
                const url = URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.href = url;
                a.download = 'hello.bin';
                document.body.appendChild(a);
                a.click();
                document.body.removeChild(a);
                URL.revokeObjectURL(url);
            });
        }
    }
}
#endif

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