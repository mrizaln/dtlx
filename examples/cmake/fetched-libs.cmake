set(FETCHCONTENT_QUIET FALSE)

include(FetchContent)

FetchContent_Declare(
  dtl
  GIT_REPOSITORY https://github.com/cubicdaiya/dtl
  GIT_TAG 32567bb9ec704f09040fb1ed7431a3d967e3df03)
FetchContent_MakeAvailable(dtl)

add_library(dtl INTERFACE)
target_include_directories(dtl INTERFACE ${dtl_SOURCE_DIR}/dtl)

add_library(fetch::dtl ALIAS dtl)
