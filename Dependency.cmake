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
# Need to check the latest version and the download URL from https://www.boost.org/users/download/
set(BOOST_VERSION "1.87.0")
set(BOOST_VERSION_UNDERSCORE "1_87_0")
set(BOOST_URL "https://archives.boost.io/release/${BOOST_VERSION}/source/boost_${BOOST_VERSION_UNDERSCORE}.tar.gz")
set(BOOST_SHA256 "f55c340aa49763b1925ccf02b2e83f35fdcf634c9d5164a2acb87540173c741d")

if(PLATFORM_WINDOWS)
  set(BOOST_BOOTSTRAP_COMMAND bootstrap.bat)
  set(BOOST_B2_COMMAND b2.exe)
  set(BOOST_EXTRA_OPTIONS "toolset=msvc" "cxxstd=17")
elseif(PLATFORM_MACOS)
  set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
  set(BOOST_B2_COMMAND ./b2)
  set(BOOST_EXTRA_OPTIONS "toolset=clang" "cxxstd=17")
elseif(EMSCRIPTEN)
  set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
  set(BOOST_B2_COMMAND ./b2)
  set(BOOST_EXTRA_OPTIONS "cxxstd=17" "cxxflags=-std=c++17 -sMEMORY64=1 -pthread")
  message("boost options: ${BOOST_EXTRA_OPTIONS}")
else()
  # Linux
  set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
  set(BOOST_B2_COMMAND ./b2)
  set(BOOST_EXTRA_OPTIONS "cxxstd=17")
endif()

set(BOOST_COMPONENTS
  # --with-system
  --with-filesystem
  # --with-thread
  # --with-date_time
  --with-serialization
)

if(EMSCRIPTEN)
  set(BOOST_TOOLSET_OPTION "toolset=emscripten")
else()
  set(BOOST_TOOLSET_OPTION "")
endif()

ExternalProject_Add(
  dep_boost
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  URL ${BOOST_URL}
  URL_HASH SHA256=${BOOST_SHA256}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND cd <SOURCE_DIR> && ${BOOST_BOOTSTRAP_COMMAND}
  BUILD_COMMAND cd <SOURCE_DIR> && ${BOOST_B2_COMMAND} 
    ${BOOST_COMPONENTS}
    --prefix=${DEP_INSTALL_DIR}
    --layout=system
    ${BOOST_TOOLSET_OPTION}
    variant=release
    link=static
    # threading=multi
    runtime-link=shared
    ${BOOST_EXTRA_OPTIONS}
    install
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE 1
)

set(DEP_LIST ${DEP_LIST} dep_boost)
set(DEP_LIBS ${DEP_LIBS}
  # boost_system
  boost_filesystem
  # boost_thread
  # boost_date_time
  boost_serialization
)