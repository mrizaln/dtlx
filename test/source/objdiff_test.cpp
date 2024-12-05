#include "helper.hpp"

#include <dtl_modern/dtl_modern.hpp>
#include <dtl.hpp>

#include <boost/ut.hpp>

namespace ut = boost::ut;

using helper::SeqPair;

struct Obj
{
    std::string_view m_str;

    // for fmt
    friend auto format_as(const Obj& obj) { return fmt::format("Obj{{ {:?} }}", obj.m_str); }

    // for ut debug output
    friend auto& operator<<(std::ostream& os, const Obj& obj) { return os << format_as(obj); }

    // for debugging purpose
    bool operator==(const Obj&) const = default;
};

bool obj_the_same(const Obj& obj1, const Obj& obj2)
{
    return std::ranges::equal(obj1.m_str, obj2.m_str, [](char a, char b) {
        return std::tolower(a) == std::tolower(b);
    });
}

const auto g_seq_pairs = std::tuple{
    SeqPair{
        .m_s1   = std::vector{ Obj{ "the" }, Obj{ "quick" }, Obj{ "brown" } },
        .m_s2   = std::vector{ Obj{ "The" }, Obj{ "Quick" }, Obj{ "Fox" } },
        .m_comp = obj_the_same,
    },
    SeqPair{
        .m_s1   = std::vector{ Obj{ "b" }, Obj{ "c" }, Obj{ "e" }, Obj{ "g" } },
        .m_s2   = std::vector{ Obj{ "a" }, Obj{ "d" }, Obj{ "e" }, Obj{ "f" }, Obj{ "h" } },
        .m_comp = obj_the_same,
    },
    SeqPair{
        .m_s1   = std::vector{ Obj{ "a" }, Obj{ "d" }, Obj{ "e" }, Obj{ "f" }, Obj{ "h" } },
        .m_s2   = std::vector{ Obj{ "b" }, Obj{ "c" }, Obj{ "e" }, Obj{ "g" } },
        .m_comp = obj_the_same,
    },
};

int main()
{
    using ut::expect, ut::that;
    using namespace ut::literals;
    using namespace ut::operators;

    helper::for_each_tuple(g_seq_pairs, [&](const auto& pair) {
        for (auto unified_format : { false, true }) {
            auto&& [s1, s2, comp]   = pair;
            auto [res_old, res_new] = helper::do_diff(s1, s2, comp, unified_format);

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
                auto edit_distance = dtl_modern::edit_distance(s1, s2, comp);

                expect(that % edit_distance == res_new.m_edit_dist);
                expect(that % edit_distance == res_old.m_edit_dist);
            };

            if (unified_format) {
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
