cmake_minimum_required(VERSION 3.12)

project(
  dtlx
  VERSION 2.0.0
  LANGUAGES CXX
  DESCRIPTION "diff template library written in C++20"
)

string(
  COMPARE EQUAL
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}"
  DTLX_STANDALONE
)

option(DTLX_BUILD_EXAMPLES "Build examples" ${DTLX_STANDALONE})
option(DTLX_BUILD_TESTS "Build tests" ${DTLX_STANDALONE})

add_library(dtlx INTERFACE)
target_include_directories(dtlx INTERFACE include)
target_compile_features(dtlx INTERFACE cxx_std_20)
set_target_properties(dtlx PROPERTIES CXX_EXTENSIONS OFF)

if(DTLX_BUILD_TESTS OR DTLX_BUILD_EXAMPLES)
  include(cmake/fetched-libs.cmake)
endif()

if(DTLX_BUILD_TESTS)
  add_subdirectory(test)
endif()

if(DTLX_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()
