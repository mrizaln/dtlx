#include <dtl_modern/dtl_modern.hpp>

#include <fmt/core.h>
#include <boost/ut.hpp>

#include <string_view>
#include <array>

namespace ut = boost::ut;

struct MergeTestCase
{
    std::string_view m_a;
    std::string_view m_b;
    std::string_view m_c;
    std::string_view m_expected;
};

constexpr auto g_merge_success = std::array{
    MergeTestCase{ "ab", "b", "bc", "abc" },
    MergeTestCase{ "bc", "b", "ab", "abc" },
    MergeTestCase{ "qqqabc", "abc", "abcdef", "qqqabcdef" },
    MergeTestCase{ "abcdef", "abc", "qqqabc", "qqqabcdef" },
    MergeTestCase{ "aaacccbbb", "aaabbb", "aaabbbqqq", "aaacccbbbqqq" },
    MergeTestCase{ "aaabbbqqq", "aaabbb", "aaacccbbb", "aaacccbbbqqq" },
    MergeTestCase{ "aeaacccbbb", "aaabbb", "aaabbbqqq", "aeaacccbbbqqq" },
    MergeTestCase{ "aaabbbqqq", "aaabbb", "aeaacccbbb", "aeaacccbbbqqq" },
    MergeTestCase{ "aeaacccbbb", "aaabbb", "aaabebbqqq", "aeaacccbebbqqq" },
    MergeTestCase{ "aaabebbqqq", "aaabbb", "aeaacccbbb", "aeaacccbebbqqq" },
    MergeTestCase{ "aaacccbbb", "aaabbb", "aeaabbbqqq", "aeaacccbbbqqq" },
    MergeTestCase{ "aeaabbbqqq", "aaabbb", "aaacccbbb", "aeaacccbbbqqq" },
    MergeTestCase{ "aaacccbbb", "aaabbb", "aaabeebbeeqqq", "aaacccbeebbeeqqq" },
    MergeTestCase{ "aaabeebbeeqqq", "aaabbb", "aaacccbbb", "aaacccbeebbeeqqq" },
    MergeTestCase{ "aiueo", "aeo", "aeKokaki", "aiueKokaki" },
    MergeTestCase{ "aeKokaki", "aeo", "aiueo", "aiueKokaki" },
    MergeTestCase{ "1234567390", "1234567890", "1239567890", "1239567390" },
    MergeTestCase{ "1239567890", "1234567890", "1234567390", "1239567390" },
    MergeTestCase{ "qabcdef", "abcdef", "ab", "qab" },
    MergeTestCase{ "ab", "abcdef", "qabcdef", "qab" },
    MergeTestCase{ "abcdf", "abcdef", "acdef", "acdf" },
    MergeTestCase{ "acdef", "abcdef", "abcdf", "acdf" },
    MergeTestCase{ "acdef", "abcdef", "abcdfaa", "acdfaa" },
    MergeTestCase{ "abcdfaa", "abcdef", "acdef", "acdfaa" },

};

constexpr auto g_merge_conflict = std::array{
    MergeTestCase{ "adc", "abc", "aec", "" },
    MergeTestCase{ "abqdcf", "abcdef", "abqqef", "" },
};

constexpr auto g_custom_comp = std::array{
    MergeTestCase{ "abc", "abc", "abC", "abc" },
};

int main()
{
    using ut::expect, ut::fatal, ut::that;
    using namespace ut::literals;
    using namespace ut::operators;

    "merge function should successfully merge strings"_test = [] {
        for (const auto& [a, b, c, expected] : g_merge_success) {
            auto result = dtl_modern::merge<std::basic_string>(a, b, c);
            expect(result.is_merge() >> fatal) << fmt::format("merge fail: {} | {} | {}", a, b, c);

            auto merged = std::move(result).as_merge().m_value;
            expect(merged == expected) << fmt::format("merged string not the same");
        }
    };

    "merge function should fail on conflict"_test = [] {
        for (const auto& [a, b, c, expected] : g_merge_conflict) {
            auto result = dtl_modern::merge<std::basic_string>(a, b, c);

            expect(result.is_conflict())    //
                << fmt::format("merge somehow succeeded: {} | {} | {} ", a, b, c);

            if (result.is_merge()) {
                fmt::println("merge result: {}", std::move(result).as_merge().m_value);
            }
        }
    };

    "merge function should be able to work with custom comparison function"_test = [] {
        for (const auto& [a, b, c, expected] : g_custom_comp) {
            auto ignore_case = [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); };
            auto result      = dtl_modern::merge<std::basic_string>(a, b, c, ignore_case);
            expect(result.is_merge() >> fatal) << fmt::format("merge fail: {} | {} | {}", a, b, c);

            auto merged = std::move(result).as_merge().m_value;
            expect(merged == expected) << fmt::format("merged string not the same");
        }
    };
}
