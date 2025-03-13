cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME Constant)

set(WINDOW_NAME "Constant")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 540)

# C++ 17
set(CMAKE_CXX_STANDARD 17)

if (EMSCRIPTEN)
  # 64-bit WASM memory model and multi-threading with pthread
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sMEMORY64=1 -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sMEMORY64=1 -pthread")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sMEMORY64=1 -pthread")
endif()

# Set project
project(${PROJECT_NAME})

# Add source files
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/logger_config.cpp    src/logger_config.h
)

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

# Declare variables for dependencies
set(DEP_INCLUDE_DIR)
set(DEP_LIB_DIR)
set(DEP_LIBS)

include(Dependency.cmake)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${DEP_INCLUDE_DIR}
)
target_link_directories(${PROJECT_NAME}
  PUBLIC
    ${DEP_LIB_DIR}
)
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ${DEP_LIBS}
)

if (EMSCRIPTEN)
  # Declare emscripten option variables
  set(EMSCRIPTEN_COMPILE_OPTIONS)
  set(EMSCRIPTEN_LINK_OPTIONS)
  set(EMSCRIPTEN_OPTIMIZATIONS)
  set(EMSCRIPTEN_DEBUG_OPTIONS)

  list(APPEND EMSCRIPTEN_LINK_OPTIONS
    "SHELL:-sWASM=1"
    "SHELL:-sENVIRONMENT=web,worker"
    "SHELL:-sUSE_WEBGL2=1"
    "SHELL:-sMODULARIZE=1"
    "SHELL:-sEXPORT_NAME='Viewer'"
    "SHELL:-s USE_GLFW=3"
    # "SHELL:--extern-post-js ${CMAKE_SOURCE_DIR}/src/post-js.js"
    # "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/webassembly/resources/@/resources/"
    "SHELL:-sALLOW_MEMORY_GROWTH=1"
    # "SHELL:-sTOTAL_MEMORY=256MB"
    "SHELL:-sMAXIMUM_MEMORY=16GB"
    "SHELL:-sUSE_PTHREADS=1" 
    "SHELL:-sPTHREAD_POOL_SIZE=5"
    # "SHELL:-sPROXY_TO_PTHREAD=1"
    "SHELL:-sNO_EXIT_RUNTIME=1"
    "SHELL:-Wno-pthreads-mem-growth"  # Suppress warning (-Wpthreads-mem-growth)
  )

  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type: Release mode")
    list(APPEND EMSCRIPTEN_OPTIMIZATIONS "-O3")
    list(APPEND EMSCRIPTEN_DEBUG_OPTIONS "-g0")
  elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type: Debug mode")
    list(APPEND EMSCRIPTEN_OPTIMIZATIONS "-O0")
    list(APPEND EMSCRIPTEN_DEBUG_OPTIONS "-g3")
    list(APPEND EMSCRIPTEN_LINK_OPTIONS
      "SHELL:-sASSERTIONS=1"
      "SHELL:--memoryprofiler"
    )
  endif()

  target_compile_options(${PROJECT_NAME}
    PRIVATE
      ${EMSCRIPTEN_COMPILE_OPTIONS}
      ${EMSCRIPTEN_OPTIMIZATIONS}
      ${EMSCRIPTEN_DEBUG_OPTIONS}
  )

  target_link_options(${PROJECT_NAME}
    PRIVATE
      ${EMSCRIPTEN_LINK_OPTIONS}
      ${EMSCRIPTEN_OPTIMIZATIONS}
      ${EMSCRIPTEN_DEBUG_OPTIONS}
  )

  install(
    FILES
      # ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.data
      ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.wasm
      ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.js
    DESTINATION "${CMAKE_SOURCE_DIR}/wasm"
  ) 
else()
  set(OPTIMIZATIONS)
  set(DEBUG_OPTIONS)
  if (NOT MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
      message(STATUS "Build type: Release mode")
      list(APPEND OPTIMIZATIONS "-O3")
      list(APPEND DEBUG_OPTIONS "-g0")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
      message(STATUS "Build type: Debug mode")
      list(APPEND OPTIMIZATIONS "-O0")
      list(APPEND DEBUG_OPTIONS "-g3")
    endif()
  endif()
endif()

# Add compile definitions
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      DEBUG_BUILD
  )
endif()

# if DEP_LIST is not empty, add dependencies
if (DEP_LIST)
  add_dependencies(${PROJECT_NAME} ${DEP_LIST})
endif()

if (APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreFoundation"
    "-framework CoreVideo"
    "-framework OpenGL"
  )
endif()