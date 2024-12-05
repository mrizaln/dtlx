#ifndef DTL_MODERN_DTL_MODERN_HPP
#define DTL_MODERN_DTL_MODERN_HPP

#include "dtl_modern/common.hpp"
#include "dtl_modern/detail/diff.hpp"
#include "dtl_modern/detail/unidiff.hpp"

#include <cassert>
#include <ranges>

namespace dtl_modern
{
    using detail::DiffResult;

    template <Comparable E>
    struct UniDiffResult
    {
        UniHunkSeq<E> m_uni_hunks;
        Lcs<E>        m_lcs;
        Ses<E>        m_ses;
        i64           m_edit_distance = 0;

        bool operator==(const UniDiffResult&) const = default;
    };

    inline bool should_swap(std::ranges::sized_range auto&& r1, std::ranges::sized_range auto&& r2)
    {
        auto lhs_size = std::ranges::size(r1);
        auto rhs_size = std::ranges::size(r2);

        return lhs_size >= rhs_size;
    }

    template <typename R1, ComparableRanges<R1> R2>
    DiffResult<std::ranges::range_value_t<R1>> diff(R1&& lhs, R2&& rhs, bool huge = false)
    {
        using E = std::ranges::range_value_t<R1>;

        if (should_swap(lhs, rhs)) {
            auto diff_impl = detail::Diff<E, R2, R1, true>{ rhs, lhs };
            return diff_impl.diff(huge);
        } else {
            auto diff_impl = detail::Diff<E, R1, R2, false>{ lhs, rhs };
            return diff_impl.diff(huge);
        }
    }

    template <typename R1, ComparableRanges<R1> R2>
    i64 edit_distance(R1&& lhs, R2&& rhs)
    {
        using E = std::ranges::range_value_t<R1>;

        if (should_swap(lhs, rhs)) {
            auto diff_impl = detail::Diff<E, R2, R1, true>{ rhs, lhs };
            return diff_impl.edit_distance();
        } else {
            auto diff_impl = detail::Diff<E, R1, R2, false>{ lhs, rhs };
            return diff_impl.edit_distance();
        }
    }

    template <Comparable E>
    UniHunkSeq<E> ses_to_unidiff(const Ses<E>& ses, bool swap)
    {
        return detail::unidiff<E>(ses, swap);
    }

    template <typename R1, ComparableRanges<R1> R2>
    UniDiffResult<std::ranges::range_value_t<R1>> unidiff(R1&& lhs, R2&& rhs, bool huge = false)
    {
        auto [lcs, ses, edit_dist] = diff(lhs, rhs, huge);

        return {
            .m_uni_hunks     = ses_to_unidiff(ses, should_swap(lhs, rhs)),
            .m_lcs           = std::move(lcs),
            .m_ses           = std::move(ses),
            .m_edit_distance = edit_dist,
        };
    }
}

#endif /* end of include guard: DTL_MODERN_DTL_MODERN_HPP */
