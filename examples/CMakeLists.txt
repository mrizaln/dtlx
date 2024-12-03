cmake_minimum_required(VERSION 3.16)
project(dtl-modern-example VERSION 0.0.0)

include(cmake/prelude.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt REQUIRED)

add_subdirectory(lib/dtl-modern)

function(make_exe DIR NAME)
  add_executable(${NAME} source/${DIR}/${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtl-modern fmt::fmt)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  # sanitizer
  target_compile_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
  target_link_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
endfunction()

make_exe(old bdiff)
make_exe(old fpatch)
make_exe(old intdiff3)
make_exe(old intdiff)
make_exe(old patch)
make_exe(old st2ses)
make_exe(old strdiff3)
make_exe(old strdiff_cp)
make_exe(old strdiff)
make_exe(old strdiff_storage)
make_exe(old unidiff)
make_exe(old unistrdiff)

make_exe(new intdiff_modern)
make_exe(new strdiff_modern)
