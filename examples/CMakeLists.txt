cmake_minimum_required(VERSION 3.16)
project(dtl-modern-example VERSION 0.0.0)

include(cmake/prelude.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib/dtl-modern)

add_library(example-common STATIC common.cpp)
target_compile_options(example-common PRIVATE -Wall -Wextra -Wconversion)

function(make_exe NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE example-common dtl-modern)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  # sanitizer
  target_compile_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
  target_link_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
endfunction()

make_exe(bdiff)
make_exe(fpatch)
make_exe(intdiff3)
make_exe(intdiff)
make_exe(patch)
make_exe(st2ses)
make_exe(strdiff3)
make_exe(strdiff_cp)
make_exe(strdiff)
make_exe(strdiff_storage)
make_exe(unidiff)
make_exe(unistrdiff)
