#ifndef DTL_MODERN_CONSTANTS_HPP
#define DTL_MODERN_CONSTANTS_HPP

#include <limits>
#include <string_view>

namespace dtl_modern::constants
{
    constexpr std::string_view library_version = "1.21";

    constexpr std::size_t dtl_separate_size = 3;
    constexpr std::size_t dtl_context_size  = 3;

    // binary aligned(?) is better instead of 2'000'000 I guess
    constexpr std::size_t default_limit = 1 << 21;
    constexpr std::size_t no_limit      = std::numeric_limits<std::size_t>::max();
}

#endif /* end of include guard: DTL_MODERN_CONSTANTS_HPP */
