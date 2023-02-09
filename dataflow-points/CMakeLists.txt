cmake_minimum_required(VERSION 3.4.3)

project(csa-checker-plugin C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

set(CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_CMAKE_DIR}")

include(AddLLVM)

find_package(Clang REQUIRED CONFIG)

# Set checker sources:
set(csa-checker-sources print-variable-names.cpp)

message(STATUS "Sources: ${csa-checker-sources}")

add_llvm_library(csa-checker MODULE ${csa-checker-sources} PLUGIN_TOOL clang)

target_include_directories(csa-checker PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_compile_definitions(csa-checker PRIVATE ${LLVM_DEFINITIONS})

# list(TRANSFORM sources-headers PREPEND ${LLVM_INCLUDE_DIRS}/)
# execute_process(COMMAND find-includes "${csa-checker-sources}"
#   OUTPUT_VARIABLE sources-headers
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# list(TRANSFORM sources-headers PREPEND ${LLVM_INCLUDE_DIRS}/)

# message(STATUS "Compiling headers: ${sources-headers}")

# target_precompile_headers(csa-checker PRIVATE ${sources-headers})

message(STATUS "LLVM_DEFINITIONS:   ${LLVM_DEFINITIONS}")
message(STATUS "LLVM_INCLUDE_DIRS:  ${LLVM_INCLUDE_DIRS}")
message(STATUS "CLANG_INCLUDE_DIRS: ${CLANG_INCLUDE_DIRS}")
