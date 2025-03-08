include(ExternalProject)

set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

if (EMSCRIPTEN)
  # 64-bit WASM memory model and multi-threading with pthread
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sMEMORY64=1 -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sMEMORY64=1 -pthread")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sMEMORY64=1 -pthread")
  set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -sMEMORY64=1 -pthread")
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
    # -DCMAKE_STATIC_LINKER_FLAGS=${CMAKE_STATIC_LINKER_FLAGS}
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

# Boost
if(WIN32)
  set(BOOST_BOOTSTRAP_COMMAND bootstrap.bat)
  set(BOOST_B2_COMMAND b2.exe)
  set(BOOST_EXTRA_OPTIONS "toolset=msvc" "cxxstd=17")
  message("Windows")
elseif(APPLE)
  set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
  set(BOOST_B2_COMMAND ./b2)
  set(BOOST_EXTRA_OPTIONS "toolset=clang" "cxxstd=17")
  message("MacOS")
elseif(EMSCRIPTEN)
  set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
  set(BOOST_B2_COMMAND ./b2)
  set(BOOST_EXTRA_OPTIONS "toolset=emscripten" "cxxstd=17" "cxxflags=-std=c++17 -sMEMORY64=1 -pthread")
  message("Emscripten")
else()
  set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
  set(BOOST_B2_COMMAND ./b2)
  set(BOOST_EXTRA_OPTIONS "toolset=clang" "cxxstd=17")
  message("Linux")
endif()

ExternalProject_Add(
  dep-boost
  GIT_REPOSITORY "https://github.com/boostorg/boost.git"
  GIT_TAG "boost-1.87.0"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND cd <SOURCE_DIR> && ${BOOST_BOOTSTRAP_COMMAND}
  BUILD_COMMAND cd <SOURCE_DIR> && ${BOOST_B2_COMMAND} install 
    --prefix=${DEP_INSTALL_DIR}
    --with-system 
    --with-filesystem 
    # --with-thread 
    --with-date_time
    --with-serialization
    variant=release
    link=static
    # threading=multi
    runtime-link=shared
    ${BOOST_EXTRA_OPTIONS}
    install
  INSTALL_COMMAND ""
)
set(DEP_LIST ${DEP_LIST} dep-boost)
set(DEP_LIBS ${DEP_LIBS}
  boost_system
  boost_filesystem
  # boost_thread
  boost_date_time
  boost_serialization
)
