cmake_minimum_required(VERSION 3.16)
project(dtl-modern-test VERSION 0.0.0)

include(cmake/prelude.cmake)
include(GoogleTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(GTest REQUIRED)
find_package(ut REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(lib/dtl-modern)

enable_testing()

# ~~~
# old tests
# ~~~
add_library(dtl_test_common_old STATIC source/old/dtl_test_common.cpp)
target_include_directories(dtl_test_common_old PUBLIC source/old/)
target_link_libraries(dtl_test_common_old PRIVATE dtl-modern gtest::gtest)
target_compile_options(dtl_test_common_old PRIVATE -Wall -Wextra -Wconversion)

function(make_test_old NAME)
  add_executable(${NAME} source/old/${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtl_test_common_old dtl-modern GTest::gtest_main)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
  gtest_discover_tests(${NAME})
endfunction()

make_test_old(Intdifftest)
make_test_old(Objdifftest)
make_test_old(Patchtest)
make_test_old(Strdiff3test)
make_test_old(Strdifftest)
make_test_old(dtl_test)
# ~~~

# ~~~
# new tests
# ~~~

function(make_test_new NAME)
  add_executable(${NAME} source/new/${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtl-modern Boost::ut fmt::fmt)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
endfunction()

make_test_new(intdiff_test)
make_test_new(strdiff_test)
# ~~~

add_custom_command(
  TARGET strdiff_test
  POST_BUILD
  COMMENT "copying ./resources/ses/ directory to build directory"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/ses
          $<TARGET_FILE_DIR:strdiff_test>/ses)

add_custom_command(
  TARGET strdiff_test
  POST_BUILD
  COMMENT "copying ./resources/hunks/ directory to build directory"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/resources/hunks $<TARGET_FILE_DIR:strdiff_test>/hunks)
