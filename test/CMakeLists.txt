cmake_minimum_required(VERSION 3.16)
project(dtlx-test VERSION 0.0.0)

include(cmake/prelude.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ut REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(lib/dtlx)

# fetches the original dtl library as fetch::dtl
include(cmake/fetched-libs.cmake)

option(DTLX_TEST_RUN_AFTER_BUILD "immediately run test after build" ON)

function(make_test NAME)
  add_executable(${NAME} source/${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtlx fetch::dtl Boost::ut fmt::fmt)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  target_compile_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
  target_link_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)

  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)

  if(DTLX_TEST_RUN_AFTER_BUILD)
    add_custom_command(
      TARGET ${NAME}
      POST_BUILD
      COMMAND $<TARGET_FILE:${NAME}>)
  endif()
endfunction()

enable_testing()

make_test(intdiff_test)
make_test(strdiff_test)
make_test(objdiff_test)
make_test(strmerge_test)
make_test(strpatch_test)
make_test(filediff_test)

add_custom_command(
  TARGET filediff_test
  COMMENT "Linking resource directory to build directory"
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/resource/
          $<TARGET_FILE_DIR:filediff_test>/resource)
