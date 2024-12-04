cmake_minimum_required(VERSION 3.16)
project(dtl-modern-test VERSION 0.0.0)

include(cmake/prelude.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ut REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(lib/dtl-modern)

# fetches the original dtl library as fetch::dtl
include(cmake/fetched-libs.cmake)

function(make_test NAME)
  add_executable(${NAME} source/${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtl-modern fetch::dtl Boost::ut fmt::fmt)
  target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)

  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
endfunction()

enable_testing()

make_test(intdiff_test)
make_test(strdiff_test)
