#ifndef DTL_MODERN_CONSTANTS_HPP
#define DTL_MODERN_CONSTANTS_HPP

#include <string_view>

namespace dtl_modern::constants
{
    constexpr std::string_view library_version = "1.21";

    constexpr std::size_t dtl_separate_size = 3;
    constexpr std::size_t dtl_context_size  = 3;

    // binary aligned(?) is better instead of 2'000'000 I guess
    constexpr std::size_t max_coordinates_size = 1 << 21;
}

#endif /* end of include guard: DTL_MODERN_CONSTANTS_HPP */
