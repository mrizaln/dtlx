#ifndef DTLX_TEST_HELPER_HPP
#define DTLX_TEST_HELPER_HPP

// formatter for
// - std::pair<T, dtl::elemInfo>
// - dtlx::SesElem<T>
// - dtl::uniHunk<std::pair<T, dtl::elemInfo>>
// - dtlx::UniHunk<T>
#include "formatter.hpp"

#include <dtlx/dtlx.hpp>
#define DTLX_DISPLAY_FMTLIB
#include <dtlx/extra/ses_display_simple.hpp>
#include <dtlx/extra/uni_hunk_display_simple.hpp>

#include <dtl.hpp>

#include <boost/ut.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <ranges>
#include <sstream>

namespace helper
{
    template <std::ranges::range R>
    using RangeElem = std::ranges::range_value_t<R>;

    struct DiffFlags
    {
        bool unified_format;
        bool huge;
    };

    template <typename E>
    struct DtlOldResult
    {
        std::vector<dtl::uniHunk<std::pair<E, dtl::elemInfo>>> hunks;
        dtl::Lcs<E>                                            lcs;
        dtl::Ses<E>                                            ses;
        std::ptrdiff_t                                         edit_dist;
    };

    template <typename E>
    struct DtlNewResult
    {
        dtlx::UniHunkSeq<E> hunks;
        dtlx::Lcs<E>        lcs;
        dtlx::Ses<E>        ses;
        std::ptrdiff_t      edit_dist;
    };

    template <typename S1, typename S2, typename Comp = std::equal_to<>>
        requires dtlx::ComparableRanges<S1, S2, Comp>
    struct SeqPair
    {
        S1   s1;
        S2   s2;
        Comp comp = {};
    };

    template <dtlx::Diffable E, dtlx::Comparator<E> Comp>
    struct CompareImpl
    {
        Comp comp;
        bool impl(const E& e1, const E& e2) const { return comp(e1, e2); }
    };

    // allow partially specify template type
    template <dtlx::Diffable E, dtlx::Comparator<E> Comp>
    CompareImpl<E, Comp> wrap_comp(Comp comp)
    {
        return { comp };
    }

    template <typename R1, typename R2, typename Comp>
        requires dtlx::ComparableRanges<R1, R2, Comp>
    std::pair<DtlOldResult<RangeElem<R1>>, DtlNewResult<RangeElem<R1>>> do_diff(
        R1&&      r1,
        R2&&      r2,
        Comp      comp,
        DiffFlags flags
    )
    {
        using E = RangeElem<R1>;

        // old
        // ---
        // both seq needs to be the same type
        auto vec1 = std::vector(std::begin(r1), std::end(r1));
        auto vec2 = std::vector(std::begin(r2), std::end(r2));

        auto comp_impl = wrap_comp<E>(comp);
        auto diff      = dtl::Diff<E, std::vector<E>, decltype(comp_impl)>{ vec1, vec2, comp_impl };

        diff.compose();
        if (flags.huge) {
            diff.onHuge();
        }
        if (flags.unified_format) {
            diff.composeUnifiedHunks();
        }
        // ---

        // new
        if (flags.unified_format) {
            auto [hunks_new, lcs_new, ses_new, edit_dist_new] = dtlx::unidiff(
                r1, r2, comp, { .huge = flags.huge }
            );
            return {
                DtlOldResult{ diff.getUniHunks(), diff.getLcs(), diff.getSes(), diff.getEditDistance() },
                DtlNewResult{ std::move(hunks_new), std::move(lcs_new), std::move(ses_new), edit_dist_new },
            };
        } else {
            auto [lcs_new, ses_new, edit_dist_new] = dtlx::diff(r1, r2, comp, { .huge = flags.huge });

            return {
                DtlOldResult{ diff.getUniHunks(), diff.getLcs(), diff.getSes(), diff.getEditDistance() },
                DtlNewResult{ {}, std::move(lcs_new), std::move(ses_new), edit_dist_new },
            };
        }
    }

    template <typename R1, typename R2, typename Comp>
        requires dtlx::ComparableRanges<R1, R2, Comp>
    std::pair<std::ptrdiff_t, std::ptrdiff_t> calc_edit_dist(R1&& r1, R2&& r2, Comp comp)
    {
        using E = RangeElem<R1>;

        // old
        // ---
        auto vec1 = std::vector(std::begin(r1), std::end(r1));
        auto vec2 = std::vector(std::begin(r2), std::end(r2));

        auto comp_impl = wrap_comp<E>(comp);
        auto diff      = dtl::Diff<E, std::vector<E>, decltype(comp_impl)>{ vec1, vec2, comp_impl };

        diff.onOnlyEditDistance();
        diff.compose();
        // ---

        // new
        // ---
        auto edit_distance = dtlx::edit_distance(r1, r2, comp);
        // ---

        return { diff.getEditDistance(), edit_distance };
    }

    template <typename Elem>
    std::string stringify_hunks_old(std::vector<dtl::uniHunk<std::pair<Elem, dtl::elemInfo>>> hunks)
    {
        using Diff = dtl::Diff<Elem>;

        auto ss = std::stringstream{};
        Diff::printUnifiedFormat(hunks, ss);

        return ss.str();
    }

    template <typename Elem>
    std::string stringify_ses_old(dtl::Ses<Elem> ses)
    {
        using Diff = dtl::Diff<Elem>;

        auto ss = std::stringstream{};
        Diff::printSES(ses, ss);

        return ss.str();
    }

    template <typename Elem>
    bool ses_eq(const std::pair<Elem, dtl::elemInfo>& ses_elem_old, const dtlx::SesElem<Elem>& ses_elem_new)
    {
        const auto& [elem_old, info_old] = ses_elem_old;
        const auto& [elem_new, info_new] = ses_elem_new;

        const auto& [before_old, after_old, type_old] = info_old;
        const auto& [before_new, after_new, type_new] = info_new;

        auto elem_eq = elem_old == elem_new;
        auto idx_eq  = before_old == before_new and after_old == after_new;
        auto type_eq = static_cast<dtlx::SesEdit>(type_old) == type_new;

        return elem_eq and idx_eq and type_eq;
    };

    template <typename Elem>
    bool uni_hunks_eq(
        const dtl::uniHunk<std::pair<Elem, dtl::elemInfo>>& hunk_old,
        const dtlx::UniHunk<Elem>&                          hunk_new
    )
    {
        const auto& [a_o, b_o, c_o, d_o, commons_o, change_o, inc_dec_count_o]              = hunk_old;
        const auto& [common_0_o, common_1_o]                                                = commons_o;
        const auto& [a_n, b_n, c_n, d_n, common_0_n, common_1_n, change_n, inc_dec_count_n] = hunk_new;

        auto eq = [&](auto&& l, auto&& r) { return ses_eq(l, r); };

        auto abcd_eq     = a_o == a_n and b_o == b_n and c_o == c_n and d_o == d_n;
        auto common_0_eq = std::ranges::equal(common_0_o, common_0_n, eq);
        auto common_1_eq = std::ranges::equal(common_1_o, common_1_n, eq);
        auto change_eq   = std::ranges::equal(change_o, change_n, eq);
        auto inc_dec_eq  = inc_dec_count_o == inc_dec_count_n;

        return abcd_eq and common_0_eq and common_1_eq and change_eq and inc_dec_eq;
    };

    template <typename Tuple, typename Fn>
    constexpr void for_each_tuple(Tuple& tuple, Fn&& fn)
    {
        const auto handler = [&]<std::size_t... I>(std::index_sequence<I...>) {
            (fn(I, std::get<I>(tuple)), ...);
        };
        handler(std::make_index_sequence<std::tuple_size_v<Tuple>>());
    }
}    // namespace helper

namespace helper::ut
{
    using boost::ut::expect;
    using boost::ut::reflection::source_location;

    template <typename Elem>
    auto ses_equals(
        const dtl::Ses<Elem>&  ses_old,
        const dtlx::Ses<Elem>& ses_new,
        source_location        loc = source_location::current()
    )
    {
        const auto& ses_seq_old = ses_old.getSequence();
        const auto& ses_seq_new = ses_new.get();

        return expect(std::ranges::equal(ses_seq_old, ses_seq_new, ses_eq<Elem>), loc)
            << fmt::format("SES not equals:\n>>> old: {::?}\n>>> new: {::?}", ses_seq_old, ses_seq_new);
    }

    template <typename Elem>
    auto lcs_equals(
        const dtl::Lcs<Elem>&  lcs_old,
        const dtlx::Lcs<Elem>& lcs_new,
        source_location        loc = source_location::current()
    )
    {
        const auto& lcs_seq_old = lcs_old.getSequence();
        const auto& lcs_seq_new = lcs_new.get();

        return expect(std::ranges::equal(lcs_seq_old, lcs_seq_new, std::equal_to{}), loc)
            << fmt::format("LCS not equals:\n>>> old: {}\n>>> new: {}", lcs_seq_old, lcs_seq_new);
    }

    template <typename Elem>
    auto uni_hunks_equals(
        const std::vector<dtl::uniHunk<std::pair<Elem, dtl::elemInfo>>>& hunks_old,
        const dtlx::UniHunkSeq<Elem>&                                    hunks_new,
        source_location                                                  loc = source_location::current()
    )
    {
        const auto& hunks_seq_old = hunks_old;
        const auto& hunks_seq_new = hunks_new.get();

        return expect(std::ranges::equal(hunks_seq_old, hunks_seq_new, uni_hunks_eq<Elem>), loc)
            << fmt::format("UniHunks not equals:\n>>> old: {}\n>>> new: {}", hunks_seq_old, hunks_seq_new);
    }
}

#endif /* end of include guard: DTLX_TEST_HELPER_HPP */
