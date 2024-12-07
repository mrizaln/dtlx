#ifndef DTL_MODERN_DTL_MODERN_HPP
#define DTL_MODERN_DTL_MODERN_HPP

#include "dtl_modern/common.hpp"
#include "dtl_modern/concepts.hpp"
#include "dtl_modern/constants.hpp"
#include "dtl_modern/detail/diff.hpp"
#include "dtl_modern/detail/merge.hpp"
#include "dtl_modern/detail/patch.hpp"
#include "dtl_modern/detail/unidiff.hpp"

#include <cassert>
#include <ranges>

namespace dtl_modern
{
    using detail::DiffResult;
    using detail::MergeResult;

    template <Diffable E>
    struct [[nodiscard]] UniDiffResult
    {
        UniHunkSeq<E> m_uni_hunks;
        Lcs<E>        m_lcs;
        Ses<E>        m_ses;
        i64           m_edit_distance = 0;

        bool operator==(const UniDiffResult&) const
            requires TriviallyComparable<E>
        = default;
    };

    // TODO: add trivial
    struct DiffFlags
    {
        bool m_huge                 = false;
        u64  m_max_coordinates_size = constants::default_limit;
    };

    template <typename R1, typename R2, typename Comp = std::equal_to<>>
        requires ComparableRanges<R1, R2, Comp>
    DiffResult<std::ranges::range_value_t<R1>> diff(R1&& lhs, R2&& rhs, Comp comp = {}, DiffFlags flags = {})
    {
        using E = std::ranges::range_value_t<R1>;

        if (std::ranges::size(lhs) >= std::ranges::size(rhs)) {
            auto diff_impl = detail::Diff<E, Comp, R2, R1, true>{ rhs, lhs, comp };
            return diff_impl.diff(flags.m_max_coordinates_size, flags.m_huge);
        } else {
            auto diff_impl = detail::Diff<E, Comp, R1, R2, false>{ lhs, rhs, comp };
            return diff_impl.diff(flags.m_max_coordinates_size, flags.m_huge);
        }
    }

    template <typename R1, typename R2, typename Comp = std::equal_to<>>
        requires ComparableRanges<R1, R2, Comp>
    [[nodiscard]] i64 edit_distance(R1&& lhs, R2&& rhs, Comp comp = {})
    {
        using E = std::ranges::range_value_t<R1>;

        if (std::ranges::size(lhs) >= std::ranges::size(rhs)) {
            auto diff_impl = detail::Diff<E, Comp, R2, R1, true>{ rhs, lhs, comp };
            return diff_impl.edit_distance();
        } else {
            auto diff_impl = detail::Diff<E, Comp, R1, R2, false>{ lhs, rhs, comp };
            return diff_impl.edit_distance();
        }
    }

    template <Diffable E>
    [[nodiscard]] UniHunkSeq<E> ses_to_unidiff(const Ses<E>& ses)
    {
        return detail::unidiff<E>(ses);
    }

    template <typename R1, typename R2, typename Comp = std::equal_to<>>
        requires ComparableRanges<R1, R2, Comp>
    UniDiffResult<std::ranges::range_value_t<R1>> unidiff(
        R1&&      lhs,
        R2&&      rhs,
        Comp      comp  = {},
        DiffFlags flags = {}
    )
    {
        auto [lcs, ses, edit_dist] = diff(lhs, rhs, comp, flags);

        return {
            .m_uni_hunks     = ses_to_unidiff(ses),
            .m_lcs           = std::move(lcs),
            .m_ses           = std::move(ses),
            .m_edit_distance = edit_dist,
        };
    }

    template <
        template <typename...> typename Container,
        typename R1,
        typename R2,
        typename R3,
        typename Comp = std::equal_to<>>
        requires ComparableRanges<R1, R2, Comp> and ComparableRanges<R2, R3, Comp>
    MergeResult<RangeElem<R1>, Container> merge(
        R1&&      r1,
        R2&&      r2,
        R3&&      r3,
        Comp      comp  = {},
        DiffFlags flags = {}
    )
    {
        using Elem = RangeElem<R1>;

        auto diff_ba = diff(r2, r1, comp, flags);
        auto diff_bc = diff(r2, r3, comp, flags);

        auto maybe_trivial = detail::trivially_mergeable(diff_ba, diff_bc);
        if (not maybe_trivial) {
            return detail::merge<Elem, Container>(diff_ba, diff_bc, comp);
        }

        auto result_from = [](auto&& r) {
            return detail::create_merge<Elem, Container>(
                Container<Elem>(std::ranges::begin(r), std::ranges::end(r))
            );
        };

        using Kind = detail::TrivialMergeKind;

        switch (maybe_trivial.value()) {
        case Kind::AEqualsBEqualsC: return result_from(r2);
        case Kind::AEqualsB: return result_from(r3);
        case Kind::BEqualsC: return result_from(r1);
        default: [[unlikely]] assert(false and "unreachable");
        }
    }

    template <template <typename... Inner> typename Container, Diffable E, std::ranges::range R>
        requires std::same_as<RangeElem<R>, E>
    Container<E> patch(R&& range, const Ses<E>& ses)
    {
        return detail::patch<Container>(std::forward<R>(range), ses);
    }

}

#endif /* end of include guard: DTL_MODERN_DTL_MODERN_HPP */
