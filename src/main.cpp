#include "logger_config.h"
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

struct MyRecord
{
    uint8_t x, y;
    float z;
    
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( x, y, z );
    }
};
    
struct SomeData
{
    int32_t id;
    std::shared_ptr<std::unordered_map<uint32_t, MyRecord>> data;

    SomeData() : data(std::make_shared<std::unordered_map<uint32_t, MyRecord>>()) {}

    template <class Archive>
    void save( Archive & ar ) const
    {
        ar( data );
    }
        
    template <class Archive>
    void load( Archive & ar )
    {
        static int32_t idGen = 0;
        id = idGen++;
        ar( data );
    }
};

void printData(const SomeData& data) {
    std::cout << "Records count: " << data.data->size() << std::endl;
    
    for (const auto& pair : *(data.data)) {
        std::cout << "Key: " << pair.first 
                  << ", Record: x=" << static_cast<int>(pair.second.x) 
                  << ", y=" << static_cast<int>(pair.second.y)
                  << ", z=" << pair.second.z << std::endl;
    }
}

int main()
{
    app::init_logger();

    std::ofstream os("out.cereal", std::ios::binary);
    cereal::BinaryOutputArchive archive( os );

    SomeData myData;
    myData.id = 42;
    
    // Add data
    (*(myData.data))[1] = {10, 20, 1.5f};
    (*(myData.data))[2] = {30, 40, 2.5f};
    (*(myData.data))[3] = {50, 60, 3.5f};
    
    std::cout << "Original data:" << std::endl;
    printData(myData);

    // Save data
    {
        std::ofstream os("out.cereal", std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(myData);
        std::cout << "Data saved to out.cereal" << std::endl;
    }

    // Load data
    SomeData loadedData;
    {
        std::ifstream is("out.cereal", std::ios::binary);
        cereal::BinaryInputArchive archive(is);
        archive(loadedData);
        std::cout << "\nLoaded data:" << std::endl;
        printData(loadedData);
    }

#ifdef __EMSCRIPTEN__
    // Download the binary file
    EM_ASM({
        const data = FS.readFile('out.cereal');
        const blob = new Blob([data], { type: 'application/octet-stream' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'out.cereal';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
    });
#endif

    app::shutdown_logger();
    return 0;
}
