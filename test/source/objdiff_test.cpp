#include "helper.hpp"

#include <dtl.hpp>
#include <dtlx/dtlx.hpp>

#include <boost/ut.hpp>

namespace ut = boost::ut;

using helper::SeqPair;

struct Obj
{
    std::string_view str;

    // for fmt
    friend auto format_as(const Obj& obj) { return fmt::format("Obj{{ {:?} }}", obj.str); }

    // for ut debug output
    friend auto& operator<<(std::ostream& os, const Obj& obj) { return os << format_as(obj); }

    // for debugging purpose
    bool operator==(const Obj&) const = default;
};

bool obj_the_same(const Obj& obj1, const Obj& obj2)
{
    return std::ranges::equal(obj1.str, obj2.str, [](char a, char b) {
        return std::tolower(a) == std::tolower(b);
    });
}

const auto g_seq_pairs = std::tuple{
    SeqPair{
        .s1   = std::vector{ Obj{ "the" }, Obj{ "quick" }, Obj{ "brown" } },
        .s2   = std::vector{ Obj{ "The" }, Obj{ "Quick" }, Obj{ "Fox" } },
        .comp = obj_the_same,
    },
    SeqPair{
        .s1   = std::vector{ Obj{ "b" }, Obj{ "c" }, Obj{ "e" }, Obj{ "g" } },
        .s2   = std::vector{ Obj{ "a" }, Obj{ "d" }, Obj{ "e" }, Obj{ "f" }, Obj{ "h" } },
        .comp = obj_the_same,
    },
    SeqPair{
        .s1   = std::vector{ Obj{ "a" }, Obj{ "d" }, Obj{ "e" }, Obj{ "f" }, Obj{ "h" } },
        .s2   = std::vector{ Obj{ "b" }, Obj{ "c" }, Obj{ "e" }, Obj{ "g" } },
        .comp = obj_the_same,
    },
};

int main()
{
    using ut::expect, ut::that, ut::test;
    using namespace ut::literals;
    using namespace ut::operators;

    helper::for_each_tuple(g_seq_pairs, [&](std::size_t i, const auto& pair) {
        const auto uni_format_huge_permut = {
            helper::DiffFlags{ true, true },
            helper::DiffFlags{ true, false },
            helper::DiffFlags{ false, true },
            helper::DiffFlags{ false, false },
        };

        for (auto flags : uni_format_huge_permut) {
            auto&& [s1, s2, comp] = pair;

            const auto [res_old, res_new] = helper::do_diff(s1, s2, comp, flags);
            const auto name = std::format("[i={}, uni_fmt={}, huge={}]", i, flags.unified_format, flags.huge);

            test("the raw output of the diff should have the same values" + name) = [&] {
                expect(that % res_old.edit_dist == res_new.edit_dist) << "Edit distance not equal";

                helper::ut::ses_equals(res_old.ses, res_new.ses)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                helper::ut::lcs_equals(res_old.lcs, res_new.lcs)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                helper::ut::uni_hunks_equals(res_old.hunks, res_new.hunks)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);
            };

            test("the formatted output of the diff should be the same" + name) = [&] {
                auto hunks_str_old = helper::stringify_hunks_old(res_old.hunks);
                auto ses_str_old   = helper::stringify_ses_old(res_old.ses);

                auto hunks_str_new = fmt::to_string(dtlx::extra::display(res_new.hunks));
                auto ses_str_new   = fmt::to_string(dtlx::extra::display(res_new.ses));

                expect(hunks_str_old == hunks_str_new)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);

                expect(ses_str_old == ses_str_new)
                    << fmt::format("\n>>> lhs seq: {}\n>>> rhs seq: {}", s1, s2);
            };

            test("edit distance should same from calling edit_distance() or diff()" + name) = [&] {
                auto edit_distance = dtlx::edit_distance(s1, s2, comp);

                expect(that % edit_distance == res_new.edit_dist);
                expect(that % edit_distance == res_old.edit_dist);
            };

            if (flags.unified_format) {
                test("constructing unified format hunks from ses should be correct" + name) = [&] {
                    auto uni_hunks_from_ses = dtlx::ses_to_unidiff(res_new.ses);

                    expect(std::ranges::equal(uni_hunks_from_ses.inner, res_new.hunks.inner)) << fmt::format(
                        "expect: {}\ngot   :{}\n", res_new.hunks.inner, uni_hunks_from_ses.inner
                    );
                };
            }
        }
    });
}
