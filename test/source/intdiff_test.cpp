#include "helper.hpp"

#include <dtl.hpp>
#include <dtlx/dtlx.hpp>

#include <boost/ut.hpp>

namespace ut = boost::ut;

using helper::SeqPair;

const auto g_seq_pairs = std::tuple{
    SeqPair{
        .s1 = std::array<int, 0>{},
        .s2 = std::array<int, 0>{},
    },
    SeqPair{
        .s1 = std::vector<int>{},
        .s2 = std::vector<int>{ 1 },
    },
    SeqPair{
        .s1 = std::vector<int>{ 1 },
        .s2 = std::vector<int>{},
    },
    SeqPair{
        .s1 = std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
        .s2 = std::vector{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 },
    },
    SeqPair{
        .s1 = std::vector{ 1, 2, 3, 4, 5 },
        .s2 = std::array{ 3, 5, 1, 4, 5 },
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
                helper::ut::ses_equals(res_old.ses, res_new.ses);
                helper::ut::lcs_equals(res_old.lcs, res_new.lcs);
                helper::ut::uni_hunks_equals(res_old.hunks, res_new.hunks);
            };

            "the formatted output of the diff should be the same"_test = [&] {
                auto hunks_str_old = helper::stringify_hunks_old(res_old.hunks);
                auto ses_str_old   = helper::stringify_ses_old(res_old.ses);

                auto hunks_str_new = fmt::to_string(dtlx::extra::display(res_new.hunks));
                auto ses_str_new   = fmt::to_string(dtlx::extra::display(res_new.ses));

                expect(hunks_str_old == hunks_str_new);
                expect(ses_str_old == ses_str_new);
            };

            "edit dist from calling edit_distance directly should be the same as from (uni)diff"_test = [&] {
                auto edit_distance = dtlx::edit_distance(s1, s2, comp);

                expect(that % edit_distance == res_new.edit_dist);
                expect(that % edit_distance == res_old.edit_dist);
            };

            if (flags.unified_format) {
                "constructing unified format hunks from ses should be correct"_test = [&] {
                    auto uni_hunks_from_ses = dtlx::ses_to_unidiff(res_new.ses);

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
