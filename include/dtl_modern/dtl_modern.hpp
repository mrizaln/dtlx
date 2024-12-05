#ifndef DTL_MODERN_DTL_MODERN_HPP
#define DTL_MODERN_DTL_MODERN_HPP

#include "concepts.hpp"
#include "dtl_modern/common.hpp"
#include "dtl_modern/detail/diff.hpp"
#include "dtl_modern/detail/unidiff.hpp"

#include <cassert>
#include <ranges>

namespace dtl_modern
{
    using detail::DiffResult;

    template <Diffable E>
    struct UniDiffResult
    {
        UniHunkSeq<E> m_uni_hunks;
        Lcs<E>        m_lcs;
        Ses<E>        m_ses;
        i64           m_edit_distance = 0;

        bool operator==(const UniDiffResult&) const
            requires TriviallyComparable<E>
        = default;
    };

    inline bool should_swap(std::ranges::sized_range auto&& r1, std::ranges::sized_range auto&& r2)
    {
        auto lhs_size = std::ranges::size(r1);
        auto rhs_size = std::ranges::size(r2);

        return lhs_size >= rhs_size;
    }

    template <typename R1, typename R2, typename Comp = std::equal_to<>>
        requires ComparableRanges<R1, R2, Comp>
    DiffResult<std::ranges::range_value_t<R1>> diff(R1&& lhs, R2&& rhs, Comp comp = {}, bool huge = false)
    {
        using E = std::ranges::range_value_t<R1>;

        if (should_swap(lhs, rhs)) {
            auto diff_impl = detail::Diff<E, Comp, R2, R1, true>{ rhs, lhs, comp };
            return diff_impl.diff(huge);
        } else {
            auto diff_impl = detail::Diff<E, Comp, R1, R2, false>{ lhs, rhs, comp };
            return diff_impl.diff(huge);
        }
    }

    template <typename R1, typename R2, typename Comp = std::equal_to<>>
        requires ComparableRanges<R1, R2, Comp>
    i64 edit_distance(R1&& lhs, R2&& rhs, Comp comp = {})
    {
        using E = std::ranges::range_value_t<R1>;

        if (should_swap(lhs, rhs)) {
            auto diff_impl = detail::Diff<E, Comp, R2, R1, true>{ rhs, lhs, comp };
            return diff_impl.edit_distance();
        } else {
            auto diff_impl = detail::Diff<E, Comp, R1, R2, false>{ lhs, rhs, comp };
            return diff_impl.edit_distance();
        }
    }

    template <Diffable E>
    UniHunkSeq<E> ses_to_unidiff(const Ses<E>& ses)
    {
        return detail::unidiff<E>(ses);
    }

    template <typename R1, typename R2, typename Comp = std::equal_to<>>
        requires ComparableRanges<R1, R2, Comp>
    UniDiffResult<std::ranges::range_value_t<R1>> unidiff(
        R1&& lhs,
        R2&& rhs,
        Comp comp = {},
        bool huge = false
    )
    {
        auto [lcs, ses, edit_dist] = diff(lhs, rhs, comp, huge);

        return {
            .m_uni_hunks     = ses_to_unidiff(ses),
            .m_lcs           = std::move(lcs),
            .m_ses           = std::move(ses),
            .m_edit_distance = edit_dist,
        };
    }
}

#endif /* end of include guard: DTL_MODERN_DTL_MODERN_HPP */
