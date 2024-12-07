#include "helper.hpp"

#include <dtl_modern/dtl_modern.hpp>
#include <dtl.hpp>

#include <boost/ut.hpp>

namespace ut = boost::ut;

using helper::SeqPair;

const auto g_seq_pairs = std::tuple{
    SeqPair{
        .m_s1 = std::array<int, 0>{},
        .m_s2 = std::array<int, 0>{},
    },
    SeqPair{
        .m_s1 = std::vector<int>{},
        .m_s2 = std::vector<int>{ 1 },
    },
    SeqPair{
        .m_s1 = std::vector<int>{ 1 },
        .m_s2 = std::vector<int>{},
    },
    SeqPair{
        .m_s1 = std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
        .m_s2 = std::vector{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 },
    },
    SeqPair{
        .m_s1 = std::vector{ 1, 2, 3, 4, 5 },
        .m_s2 = std::array{ 3, 5, 1, 4, 5 },
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
                expect(that % res_old.m_edit_dist == res_new.m_edit_dist) << "Edit distance not equal";
                helper::ut::ses_equals(res_old.m_ses, res_new.m_ses);
                helper::ut::lcs_equals(res_old.m_lcs, res_new.m_lcs);
                helper::ut::uni_hunks_equals(res_old.m_hunks, res_new.m_hunks);
            };

            "the formatted output of the diff should be the same"_test = [&] {
                auto hunks_str_old = helper::stringify_hunks_old(res_old.m_hunks);
                auto ses_str_old   = helper::stringify_ses_old(res_old.m_ses);

                auto hunks_str_new = fmt::to_string(dtl_modern::extra::display(res_new.m_hunks));
                auto ses_str_new   = fmt::to_string(dtl_modern::extra::display(res_new.m_ses));

                expect(hunks_str_old == hunks_str_new);
                expect(ses_str_old == ses_str_new);
            };

            "edit dist from calling edit_distance directly should be the same as from (uni)diff"_test = [&] {
                auto edit_distance = dtl_modern::edit_distance(s1, s2, comp);

                expect(that % edit_distance == res_new.m_edit_dist);
                expect(that % edit_distance == res_old.m_edit_dist);
            };

            if (flags.m_unified_format) {
                "constructing unified format hunks from ses should be correct"_test = [&] {
                    auto uni_hunks_from_ses = dtl_modern::ses_to_unidiff(res_new.m_ses);

                    expect(std::ranges::equal(uni_hunks_from_ses.m_inner, res_new.m_hunks.m_inner))
                        << fmt::format(
                               "expect: {}\ngot   :{}\n",    //
                               res_new.m_hunks.m_inner,
                               uni_hunks_from_ses.m_inner
                           );
                };
            }
        }
    });
}
