#include "helper.hpp"

#include <boost/ut.hpp>
#include <dtl_modern/dtl_modern.hpp>
#include <dtl.hpp>

#include <string_view>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace ut = boost::ut;
namespace fs = std::filesystem;
using namespace std::string_view_literals;

using LineByLineView = std::vector<std::string_view>;

std::optional<std::string> load_file(const fs::path& path)
{
    if (not fs::exists(path)) {
        return {};
    }

    auto ifs = std::ifstream{ path };
    auto ss  = std::stringstream{};

    ss << ifs.rdbuf();

    return ss.str();
}

LineByLineView generate_line_by_line(const std::string_view string)
{
    auto result = LineByLineView{};

    std::size_t idx = 0;
    while (true) {
        auto next = string.find('\n', idx);
        if (next == std::string::npos) {
            break;
        }

        result.push_back(string.substr(idx, next - idx));
        idx = next + 1;
    }

    return result;
}

int main()
{
    using ut::expect, ut::that;
    using namespace ut::literals;
    using namespace ut::operators;

    const auto file1 = fs::current_path() / "resource" / "file1.txt";
    const auto file2 = fs::current_path() / "resource" / "file2.txt";

    auto file1_content = load_file(file1).value();    // Assume file exists, or throws then abort
    auto file2_content = load_file(file2).value();

    auto file1_lines = generate_line_by_line(file1_content);
    auto file2_lines = generate_line_by_line(file2_content);

    auto uni_format_huge_permut = {
        helper::DiffFlags{ true, true },
        helper::DiffFlags{ true, false },
        helper::DiffFlags{ false, true },
        helper::DiffFlags{ false, false },
    };

    for (auto flags : uni_format_huge_permut) {
        const auto& s1 = file1_lines;
        const auto& s2 = file2_lines;

        auto [res_old, res_new] = helper::do_diff(s1, s2, std::equal_to{}, flags);

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
            auto edit_distance = dtl_modern::edit_distance(s1, s2, {});

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
}
