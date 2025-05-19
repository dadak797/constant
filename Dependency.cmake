include(ExternalProject)

set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

if (EMSCRIPTEN)
  # 64-bit WASM memory model and multi-threading with pthread
  # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sMEMORY64=1 -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sMEMORY64=1 -pthread")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sMEMORY64=1 -pthread")
endif()

# spdlog: fast logger library
ExternalProject_Add(
  dep-spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v1.x"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS
    # -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
    -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DSPDLOG_BUILD_EXAMPLE=OFF
    -DSPDLOG_BUILD_TESTS=OFF
    -DSPDLOG_BUILD_BENCH=OFF
    -DSPDLOG_USE_STD_FORMAT=OFF
  TEST_COMMAND ""
)
set(DEP_LIST ${DEP_LIST} dep-spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)

# Cereal: serialization library
ExternalProject_Add(
  dep_cereal
  GIT_REPOSITORY "https://github.com/USCiLab/cereal"
  GIT_TAG "v1.3.2"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${PROJECT_BINARY_DIR}/dep_cereal-prefix/src/dep_cereal/include/cereal
      ${DEP_INSTALL_DIR}/include/cereal
)
set(DEP_LIST ${DEP_LIST} dep_cereal)

if (NOT EMSCRIPTEN)  # For emscripten, '-sUSE_GLFW=3' is used.
  # glfw
  ExternalProject_Add(
    dep_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.4"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DGLFW_BUILD_EXAMPLES=OFF
      -DGLFW_BUILD_TESTS=OFF
      -DGLFW_BUILD_DOCS=OFF
    TEST_COMMAND ""
  )
  set(DEP_LIST ${DEP_LIST} dep_glfw)
  set(DEP_LIBS ${DEP_LIBS} glfw3)

  # glad
  ExternalProject_Add(
    dep_glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad.git"
    GIT_TAG "v0.1.36"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DGLAD_INSTALL=ON
      -DGLAD_API="gl=3.3"
      -DGLAD_PROFILE="core"
    TEST_COMMAND ""
  )
  set(DEP_LIST ${DEP_LIST} dep_glad)
  set(DEP_LIBS ${DEP_LIBS} glad)
endif()

# imgui
set(IMGUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/dependency/imgui-1.91.8-docking/)
add_library(imgui
  ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
  ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
  ${IMGUI_SOURCE_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_SOURCE_DIR}/imgui_impl_opengl3.cpp
  ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
  ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
  ${IMGUI_SOURCE_DIR}/imgui.cpp
)
target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})
add_dependencies(imgui ${DEP_LIST})  # imgui requires glfw and glad.
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${IMGUI_SOURCE_DIR})
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)

# stb image
ExternalProject_Add(
  dep_stb
  GIT_REPOSITORY "https://github.com/nothings/stb"
  GIT_TAG "master"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
    ${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h
    ${DEP_INSTALL_DIR}/include/stb/stb_image.h
)
set(DEP_LIST ${DEP_LIST} dep_stb)

# glm
ExternalProject_Add(
  dep_glm
  GIT_REPOSITORY "https://github.com/g-truc/glm"
  GIT_TAG "1.0.1"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
    ${DEP_INSTALL_DIR}/include/glm
)
set(DEP_LIST ${DEP_LIST} dep_glm)