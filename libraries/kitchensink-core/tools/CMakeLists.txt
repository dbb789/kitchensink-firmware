cmake_minimum_required(VERSION 3.14)

# Set compilers before project() so they take effect
if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

project(kitchensink-core-tools CXX C)

# Path to Arduino libraries (mirrors ARDUINO_LIBRARIES in the original Makefile)
if(NOT DEFINED ARDUINO_LIBRARIES)
    set(ARDUINO_LIBRARIES "$ENV{HOME}/Arduino")
endif()
set(TF_PSA_CRYPTO_SRC "${ARDUINO_LIBRARIES}/libraries/kitchensink-tf-psa-crypto/src")

# Compiler flags
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -g -Wall")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS ON)  # produces -std=gnu++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall '-DKS_PASSWORD_SUFFIX=\"_kitchenSink\""')

# Include directories shared by all targets
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${TF_PSA_CRYPTO_SRC}
)

# C sources from tf-psa-crypto
file(GLOB C_FILES "${TF_PSA_CRYPTO_SRC}/*.c")

# C++ library sources (mirrors the ../src/… globs in the original Makefile)
file(GLOB LIB_CPP_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/types/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/crypto/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/data/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/event/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/serialize/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/log/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/hardware/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/timer/*.cpp"
)

# Static library (mirrors libkitchensink.a)
add_library(kitchensink STATIC
    ${C_FILES}
    ${LIB_CPP_FILES}
)

# aesencrypt tool (mirrors: aesencrypt.o fileoutstream.o libkitchensink.a)
add_executable(aesencrypt
    aesencrypt.cpp
    fileoutstream.cpp
)
target_link_libraries(aesencrypt kitchensink)

# aesdecrypt tool (mirrors: aesdecrypt.o fileinstream.o fileoutstream.o libkitchensink.a)
add_executable(aesdecrypt
    aesdecrypt.cpp
    fileinstream.cpp
    fileoutstream.cpp
)
target_link_libraries(aesdecrypt kitchensink)
