#ifndef DTL_MODERN_DTL_MODERN_HPP
#define DTL_MODERN_DTL_MODERN_HPP

#include "dtl-modern/common.hpp"
#include "dtl-modern/detail/diff.hpp"

#include <cassert>
#include <ranges>

namespace dtl_modern
{
    using detail::DiffResult;

    template <typename R1, ComparableRanges<R1> R2>
    DiffResult<std::ranges::range_value_t<R1>> diff(R1&& lhs, R2&& rhs, bool huge = false)
    {
        using E = std::ranges::range_value_t<R1>;

        auto lhs_size = std::ranges::size(lhs);
        auto rhs_size = std::ranges::size(rhs);

        if (lhs_size < rhs_size) {
            auto diff_impl = detail::Diff<E, R1, R2, false>{ lhs, rhs };
            return diff_impl.diff(huge);
        } else {
            auto diff_impl = detail::Diff<E, R2, R1, true>{ rhs, lhs };
            return diff_impl.diff(huge);
        }
    }

    template <typename R1, ComparableRanges<R1> R2>
    i64 edit_distance(R1&& lhs, R2&& rhs)
    {
        using E = std::ranges::range_value_t<R1>;

        auto lhs_size = std::ranges::size(lhs);
        auto rhs_size = std::ranges::size(rhs);

        if (lhs_size < rhs_size) {
            auto diff_impl = detail::Diff<E, R1, R2, false>{ lhs, rhs };
            return diff_impl.edit_distance();
        } else {
            auto diff_impl = detail::Diff<E, R2, R1, true>{ rhs, lhs };
            return diff_impl.edit_distance();
        }
    }
}

#endif /* end of include guard: DTL_MODERN_DTL_MODERN_HPP */
