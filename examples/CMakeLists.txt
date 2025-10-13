cmake_minimum_required(VERSION 3.16)
project(dtlx-example VERSION 0.0.0)

include(cmake/prelude.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt REQUIRED)

add_subdirectory(lib/dtlx) # dtlx library

# fetches the original dtl library as fetch::dtl
include(cmake/fetched-libs.cmake)

function(make_exe NAME)
  add_executable(${NAME} source/${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtlx fetch::dtl fmt::fmt)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  # sanitizer
  target_compile_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
  target_link_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
endfunction()

make_exe(intdiff)
make_exe(strdiff)
make_exe(objdiff)
make_exe(benchdiff)
make_exe(pretty_print)
make_exe(filediff)
