#pragma once

#ifdef __EMSCRIPTEN__
#define GLFW_INCLUDE_ES3    // Include OpenGL ES 3.0 headers
#define GLFW_INCLUDE_GLEXT  // Include to OpenGL ES extension headers
#else
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
