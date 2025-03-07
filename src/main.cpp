#include "logger_config.h"
#include <thread>
#include <vector>
#include <fstream>
#ifdef EMSCRIPTEN
    #include <emscripten.h>
#endif
#include <iostream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

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

    int32_t value = 123456789;
    ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
    
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

bool read_binary_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return false;
    }
    
    // Check file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // header(4bytes) + 문자열 + integer(4bytes) + footer(4bytes)
    if (size < 12) { 
        SPDLOG_ERROR("File size is too small: {}", size);
        return false;
    }
    
    // Read file content
    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        SPDLOG_ERROR("Failed to read file: {}", filename);
        return false;
    }
    
    // Check header (first 4 bytes)
    unsigned char expected_header[] = {0xAB, 0xCD, 0xEF, 0x12};
    if (memcmp(buffer.data(), expected_header, 4) != 0) {
        SPDLOG_ERROR("Header is invalid");
        return false;
    }
    
    // Check footer (last 4 bytes)
    unsigned char expected_footer[] = {0xFF, 0xFE, 0xFD, 0xFC};
    if (memcmp(buffer.data() + size - 4, expected_footer, 4) != 0) {
        SPDLOG_ERROR("Footer is invalid");
        return false;
    }
    
    size_t text_length = 12;  // Length of "Hello, World"
    std::string text(reinterpret_cast<char*>(buffer.data() + 4), text_length);
    
    int32_t value;
    memcpy(&value, buffer.data() + 4 + text_length, sizeof(int32_t));
    
    // 결과 출력
    SPDLOG_INFO("Text from file: {}", text);
    SPDLOG_INFO("Integer value from file: {}", value);
    
    return true;
}

void saveData(const std::vector<int>& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    boost::archive::binary_oarchive oa(file);
    oa << data;
    
    std::cout << "Data stored in " << filename << std::endl;
}

std::vector<int> loadData(const std::string& filename) {
    std::vector<int> data;
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }
    
    boost::archive::binary_iarchive ia(file);
    ia >> data;
    
    std::cout << "Data loaded from " << filename << std::endl;
    return data;
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

        // Read binary file
        if (!read_binary_file("hello.bin")) {
            return;
        }
    }
}
#endif

int main() {
    app::init_logger();

    std::vector<int> myData = {1, 3, 5};
    const std::string filename = "data.bin";   
    saveData(myData, filename);
    
    std::cout << "Saved data: ";
    for (const auto& value : myData) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
    
    std::vector<int> loadedData = loadData(filename);
    
    std::cout << "Loaded data: ";
    for (const auto& value : loadedData) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    app::shutdown_logger();
    return 0;
}