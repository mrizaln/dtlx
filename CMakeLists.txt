cmake_minimum_required(VERSION 3.12)

project(
  dtl-modern
  VERSION 1.0.0
  LANGUAGES CXX
  DESCRIPTION "diff template library written in C++20")

add_library(dtl-modern INTERFACE)
target_include_directories(dtl-modern INTERFACE include)
target_compile_features(dtl-modern INTERFACE cxx_std_20)
set_target_properties(dtl-modern PROPERTIES CXX_EXTENSIONS OFF)
