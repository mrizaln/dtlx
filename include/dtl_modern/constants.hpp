#ifndef DTL_MODERN_CONSTANTS_HPP
#define DTL_MODERN_CONSTANTS_HPP

#include <compare>
#include <limits>

namespace dtl_modern::constants
{
    struct SemanticVersion
    {
        std::size_t major;
        std::size_t minor;
        std::size_t patch;

        std::strong_ordering operator<=>(const SemanticVersion&) const = default;
    };

    constexpr SemanticVersion version = { .major = 1, .minor = 0, .patch = 1 };

    constexpr std::size_t unidiff_separate_size = 3;
    constexpr std::size_t unidiff_context_size  = 3;

    // limit of coordinate size, default value is the same as one used in dtl
    constexpr std::size_t default_limit = 2'000'000;
    constexpr std::size_t no_limit      = std::numeric_limits<std::size_t>::max();
}

#endif /* end of include guard: DTL_MODERN_CONSTANTS_HPP */
