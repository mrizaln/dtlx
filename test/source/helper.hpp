#ifndef DTL_MODERN_TEST_HELPER_HPP
#define DTL_MODERN_TEST_HELPER_HPP

// formatter for
// - std::pair<T, dtl::elemInfo>
// - dtl_modern::SesElem<T>
// - dtl::uniHunk<std::pair<T, dtl::elemInfo>>
// - dtl_modern::UniHunk<T>
#include "formatter.hpp"

#include <dtl_modern/dtl_modern.hpp>
#define DTL_MODERN_DISPLAY_FMTLIB
#include <dtl_modern/extra/ses_display_simple.hpp>
#include <dtl_modern/extra/uni_hunk_display_simple.hpp>

#include <dtl.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <boost/ut.hpp>

#include <sstream>

namespace helper
{
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
    bool ses_eq(
        const std::pair<Elem, dtl::elemInfo>& ses_elem_old,
        const dtl_modern::SesElem<Elem>&      ses_elem_new
    )
    {
        const auto& [elem_old, info_old] = ses_elem_old;
        const auto& [elem_new, info_new] = ses_elem_new;

        const auto& [before_old, after_old, type_old] = info_old;
        const auto& [before_new, after_new, type_new] = info_new;

        auto elem_eq = elem_old == elem_new;
        auto idx_eq  = before_old == before_new and after_old == after_new;
        auto type_eq = static_cast<dtl_modern::SesEdit>(type_old) == type_new;

        return elem_eq and idx_eq and type_eq;
    };

    template <typename Elem>
    bool uni_hunks_eq(
        const dtl::uniHunk<std::pair<Elem, dtl::elemInfo>>& hunk_old,
        const dtl_modern::UniHunk<Elem>&                    hunk_new
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
            (fn(std::get<I>(tuple)), ...);
        };
        handler(std::make_index_sequence<std::tuple_size_v<Tuple>>());
    }
}

namespace helper::ut
{
    using boost::ut::expect;
    using boost::ut::reflection::source_location;

    template <typename Elem>
    auto&& ses_equals(
        const dtl::Ses<Elem>&        ses_old,
        const dtl_modern::Ses<Elem>& ses_new,
        source_location              loc = source_location::current()
    )
    {
        const auto& ses_seq_old = ses_old.getSequence();
        const auto& ses_seq_new = ses_new.get();

        return expect(std::ranges::equal(ses_seq_old, ses_seq_new, ses_eq<Elem>), loc)
            << fmt::format("SES not equals:\nold: {::?}\nnew: {::?}", ses_seq_old, ses_seq_new);
    }

    template <typename Elem>
    auto&& lcs_equals(
        const dtl::Lcs<Elem>&        lcs_old,
        const dtl_modern::Lcs<Elem>& lcs_new,
        source_location              loc = source_location::current()
    )
    {
        const auto& lcs_seq_old = lcs_old.getSequence();
        const auto& lcs_seq_new = lcs_new.get();

        return expect(std::ranges::equal(lcs_seq_old, lcs_seq_new, std::equal_to{}), loc)
            << fmt::format("LCS not equals:\nold: {}\nnew: {}", lcs_seq_old, lcs_seq_new);
    }

    template <typename Elem>
    auto&& uni_hunks_equals(
        const std::vector<dtl::uniHunk<std::pair<Elem, dtl::elemInfo>>>& hunks_old,
        const dtl_modern::UniHunkSeq<Elem>&                              hunks_new,
        source_location                                                  loc = source_location::current()
    )
    {
        const auto& hunks_seq_old = hunks_old;
        const auto& hunks_seq_new = hunks_new.get();

        return expect(std::ranges::equal(hunks_seq_old, hunks_seq_new, uni_hunks_eq<Elem>), loc)
            << fmt::format("UniHunks not equals:\nold: {}\nnew: {}", hunks_seq_old, hunks_seq_new);
    }
}

#endif /* end of include guard: DTL_MODERN_TEST_HELPER_HPP */
