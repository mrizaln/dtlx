set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

# dtl
# ---
FetchContent_Declare(
  dtl
  GIT_REPOSITORY https://github.com/cubicdaiya/dtl
  GIT_TAG 32567bb9ec704f09040fb1ed7431a3d967e3df03
)
FetchContent_MakeAvailable(dtl)

add_library(dtl INTERFACE)
target_include_directories(dtl INTERFACE ${dtl_SOURCE_DIR}/dtl)

# ut
# --
FetchContent_Declare(
  ut
  ULR_HASH SHA256=e51bf1873705819730c3f9d2d397268d1c26128565478e2e65b7d0abb45ea9b1
  URL https://github.com/boost-ext/ut/archive/refs/tags/v2.3.1.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(ut)

# fmt
# ---
FetchContent_Declare(
  fmt
  ULR_HASH SHA256=sha256:695fd197fa5aff8fc67b5f2bbc110490a875cdf7a41686ac8512fb480fa8ada7
  URL https://github.com/fmtlib/fmt/releases/download/12.1.0/fmt-12.1.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(fmt)
