cmake_minimum_required(VERSION 3.14)

# Set compilers before project() so they take effect
if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

project(kitchensink-core-tests CXX C)

# Path to Arduino libraries (mirrors ARDUINO_LIBRARIES in the original Makefile)
if(NOT DEFINED ARDUINO_LIBRARIES)
    set(ARDUINO_LIBRARIES "$ENV{HOME}/Arduino")
endif()
set(TF_PSA_CRYPTO_SRC "${ARDUINO_LIBRARIES}/libraries/kitchensink-tf-psa-crypto/src")

# Compiler flags
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -g -Wall")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS ON)  # produces -std=gnu++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fsanitize=address '-DKS_PASSWORD_SUFFIX=\"\"'")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${TF_PSA_CRYPTO_SRC}
)

# C sources from tf-psa-crypto
file(GLOB C_FILES "${TF_PSA_CRYPTO_SRC}/*.c")

# C++ test sources (one level deep under tests/, e.g. crypto/, hardware/, types/…)
file(GLOB CPP_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp")

# C++ library sources (mirrors the ../src/… globs in the original Makefile)
file(GLOB CPP_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/types/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/crypto/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/data/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/event/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/serialize/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/log/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/hardware/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/timer/*.cpp"
)

# Build GTest from source (mirrors the manual gtest_main.a target)
set(GTEST_DIR /usr/src/gtest)
add_library(gtest_main_lib STATIC
    ${GTEST_DIR}/src/gtest-all.cc
    ${GTEST_DIR}/src/gtest_main.cc
)
target_include_directories(gtest_main_lib PRIVATE ${GTEST_DIR})

# Test executable
add_executable(tests
    ${C_FILES}
    ${CPP_TEST_FILES}
    ${CPP_SRC_FILES}
)
target_link_libraries(tests
    gtest_main_lib
    pthread
)
