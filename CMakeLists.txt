cmake_minimum_required(VERSION 3.12)

project(
  dtlx
  VERSION 1.0.1
  LANGUAGES CXX
  DESCRIPTION "diff template library written in C++20")

add_library(dtlx INTERFACE)
target_include_directories(dtlx INTERFACE include)
target_compile_features(dtlx INTERFACE cxx_std_20)
set_target_properties(dtlx PROPERTIES CXX_EXTENSIONS OFF)
