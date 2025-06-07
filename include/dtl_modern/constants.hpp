#ifndef DTL_MODERN_CONSTANTS_HPP
#define DTL_MODERN_CONSTANTS_HPP

#include <compare>
#include <limits>

namespace dtl_modern::constants
{
    struct SemanticVersion
    {
        std::size_t m_major;
        std::size_t m_minor;
        std::size_t m_patch;

        std::strong_ordering operator<=>(const SemanticVersion&) const = default;
    };

    constexpr SemanticVersion version = { .m_major = 1, .m_minor = 0, .m_patch = 1 };

    constexpr std::size_t unidiff_separate_size = 3;
    constexpr std::size_t unidiff_context_size  = 3;

    // binary aligned(?) is better instead of 2'000'000 I guess
    constexpr std::size_t default_limit = 1 << 21;
    constexpr std::size_t no_limit      = std::numeric_limits<std::size_t>::max();
}

#endif /* end of include guard: DTL_MODERN_CONSTANTS_HPP */
