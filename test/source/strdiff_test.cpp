#include "helper.hpp"

#include <dtl.hpp>
#include <dtl_modern/dtl_modern.hpp>

#include <boost/ut.hpp>

namespace ut = boost::ut;

using helper::SeqPair;

using std::string_view_literals::operator""sv;
using std::string_literals::operator""s;

const auto g_seq_pairs = std::tuple{
    SeqPair{
        .s1 = "abc"sv,
        .s2 = "abd"sv,
    },
    SeqPair{
        .s1 = "acbdeacbed"s,
        .s2 = "acebdabbabed"s,
    },
    SeqPair{
        .s1 = "abcdef"sv,
        .s2 = "dacfea"s,
    },
    SeqPair{
        .s1 = "abcbda"s,
        .s2 = "bdcaba"sv,
    },
    SeqPair{
        .s1 = "bokko"sv,
        .s2 = "bokkko"sv,
    },
    SeqPair{
        .s1 = ""sv,
        .s2 = ""sv,
    },
    SeqPair{
        .s1 = "a"sv,
        .s2 = ""sv,
    },
    SeqPair{
        .s1 = ""sv,
        .s2 = "b"sv,
    },
    SeqPair{
        .s1 = "acbdeaqqqqqqqcbed"sv,
        .s2 = "acebdabbqqqqqqqabed"sv,
    },
    SeqPair{
        .s1   = "abc"sv,
        .s2   = "Abc"sv,
        .comp = [](char l, char r) { return std::tolower(l) == std::tolower(r); },
    },

    // dtl and dtl_modern disagrees on the ses output if the path coordinates limit is set
    SeqPair{
        .s1
        = "aaaaaaaaaaaaa>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
          "aaaadsafabcaaaaaaaaaaaaaaaaaaewaaabdaaaaaabbb"sv,
        .s2
        = "aaaaaaaaaaaaaaadasfdsafsadasdafbaaaaaaaaaaaaaaaaaeaaaaaaaaaae&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
          "saaabcaaaaaaccc"sv,
    },
};

int main()
{
    using ut::expect, ut::that;
    using namespace ut::literals;
    using namespace ut::operators;

    helper::for_each_tuple(g_seq_pairs, [&](const auto& pair) {
        // permutations between unified_format flag and huge flag
        auto uni_format_huge_permut = {
            helper::DiffFlags{ true, true },
            helper::DiffFlags{ true, false },
            helper::DiffFlags{ false, true },
            helper::DiffFlags{ false, false },
        };

        for (auto flags : uni_format_huge_permut) {
            auto&& [s1, s2, comp]   = pair;
            auto [res_old, res_new] = helper::do_diff(s1, s2, comp, flags);

            "the raw output of the diff should have the same values"_test = [&] {
                expect(that % res_old.edit_dist == res_new.edit_dist) << "Edit distance not equal";

                helper::ut::ses_equals(res_old.ses, res_new.ses)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                helper::ut::lcs_equals(res_old.lcs, res_new.lcs)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                helper::ut::uni_hunks_equals(res_old.hunks, res_new.hunks)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);
            };

            "the formatted output of the diff should be the same"_test = [&] {
                auto hunks_str_old = helper::stringify_hunks_old(res_old.hunks);
                auto ses_str_old   = helper::stringify_ses_old(res_old.ses);

                auto hunks_str_new = fmt::to_string(dtl_modern::extra::display(res_new.hunks));
                auto ses_str_new   = fmt::to_string(dtl_modern::extra::display(res_new.ses));

                expect(hunks_str_old == hunks_str_new)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                expect(ses_str_old == ses_str_new)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);
            };

            "edit dist from calling edit_distance directly should be the same as from (uni)diff"_test = [&] {
                auto [old_edit_dist, new_edit_dist] = helper::calc_edit_dist(s1, s2, comp);

                expect(that % new_edit_dist == old_edit_dist) << "edit distance not the same";

                // NOTE: "edit distance only" operation may result in different value with "diff"
                ut::log << fmt::format("new: [with diff={}, only={}]", res_new.edit_dist, new_edit_dist)
                        << fmt::format("old: [with diff={}, only={}]", res_old.edit_dist, old_edit_dist);
            };

            if (flags.unified_format) {
                "constructing unified format hunks from ses should be correct"_test = [&] {
                    auto uni_hunks_from_ses = dtl_modern::ses_to_unidiff(res_new.ses);

                    expect(std::ranges::equal(uni_hunks_from_ses.inner, res_new.hunks.inner)) << fmt::format(
                        "expect: {}\ngot   :{}\n",    //
                        res_new.hunks.inner,
                        uni_hunks_from_ses.inner
                    );
                };
            }
        }
    });
}
