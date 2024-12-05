#include "helper.hpp"

#include <dtl_modern/dtl_modern.hpp>
#include <dtl.hpp>

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
};

int main()
{
    using ut::expect, ut::that;
    using namespace ut::literals;
    using namespace ut::operators;

    helper::for_each_tuple(g_seq_pairs, [&]<typename T1, typename T2>(const SeqPair<T1, T2>& pair) {
        for (auto unified_format : { false, true }) {
            auto&& [s1, s2]         = pair;
            auto [res_old, res_new] = helper::do_diff(s1, s2, unified_format);

            "the raw output of the diff should have the same values"_test = [&] {
                expect(that % res_old.m_edit_dist == res_new.m_edit_dist) << "Edit distance not equal";

                helper::ut::ses_equals(res_old.m_ses, res_new.m_ses)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                helper::ut::lcs_equals(res_old.m_lcs, res_new.m_lcs)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                helper::ut::uni_hunks_equals(res_old.m_hunks, res_new.m_hunks)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);
            };

            "the formatted output of the diff should be the same"_test = [&] {
                auto hunks_str_old = helper::stringify_hunks_old(res_old.m_hunks);
                auto ses_str_old   = helper::stringify_ses_old(res_old.m_ses);

                auto hunks_str_new = fmt::to_string(dtl_modern::extra::display(res_new.m_hunks));
                auto ses_str_new   = fmt::to_string(dtl_modern::extra::display(res_new.m_ses));

                expect(hunks_str_old == hunks_str_new)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                expect(ses_str_old == ses_str_new)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);
            };

            "edit dist from calling edit_distance directly should be the same as from (uni)diff"_test = [&] {
                auto edit_distance = dtl_modern::edit_distance(s1, s2);

                expect(that % edit_distance == res_new.m_edit_dist);
                expect(that % edit_distance == res_old.m_edit_dist);
            };

            if (unified_format) {
                "constructing unified format hunks from ses should be correct"_test = [&] {
                    auto should_swap        = dtl_modern::should_swap(s1, s2);
                    auto uni_hunks_from_ses = dtl_modern::ses_to_unidiff(res_new.m_ses, should_swap);

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
