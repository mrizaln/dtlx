cmake_minimum_required(VERSION 3.16)
project(dtl-modern-test VERSION 0.0.0)

include(cmake/prelude.cmake)
include(GoogleTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(GTest REQUIRED)

add_subdirectory(lib/dtl-modern)

add_library(dtl_test_common STATIC dtl_test_common.cpp)
target_link_libraries(dtl_test_common PRIVATE dtl-modern gtest::gtest)
target_compile_options(dtl_test_common PRIVATE -Wall -Wextra -Wconversion)

function(make_test NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtl_test_common dtl-modern GTest::gtest_main)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
  gtest_discover_tests(${NAME})
endfunction()

enable_testing()

make_test(Intdifftest)
make_test(Objdifftest)
make_test(Patchtest)
make_test(Strdiff3test)
make_test(Strdifftest)
make_test(dtl_test)

add_custom_command(
  TARGET dtl_test
  POST_BUILD
  COMMENT "copying ./resources/ses/ directory to build directory"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/ses
          $<TARGET_FILE_DIR:dtl_test>/ses)

add_custom_command(
  TARGET dtl_test
  POST_BUILD
  COMMENT "copying ./resources/hunks/ directory to build directory"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/hunks
          $<TARGET_FILE_DIR:dtl_test>/hunks)
