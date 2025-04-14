// Export to Javascript funtion
// Only for emscripten

#ifdef __EMSCRIPTEN__
#include "app.h"
#include "file_loader.h"

#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(Constant) {
    emscripten::function("initIdbfs", &App::InitIdbfs);
    emscripten::function("saveImGuiIniFile", &App::SaveImGuiIniFile);
    emscripten::function("loadImGuiIniFile", &App::LoadImGuiIniFile);
    emscripten::function("loadArrayBuffer", &FileLoader::LoadArrayBuffer);
}
#endif