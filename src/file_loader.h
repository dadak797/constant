#pragma once

#include "macro/singleton_macro.h"

// Standard library
#include <string>


class FileLoader {
    DECLARE_SINGLETON(FileLoader)

public:
    void OpenFileBrowser();

    // fileName: File name in MemFS
    // deleteFile: Delete file in MemFS in this function
    static void LoadArrayBuffer(const std::string& fileName, bool deleteFile);
};