#include "helper.hpp"

#include <boost/ut.hpp>
#include <dtl.hpp>
#include <dtlx/dtlx.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>

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

int main(int /* argc */, char** argv)
{
    using ut::expect, ut::that, ut::test;
    using namespace ut::literals;
    using namespace ut::operators;

    namespace fs = std::filesystem;

    const auto test_dir = fs::path{ argv[0] }.parent_path();    // assume first argument points to binary dir
    const auto file1    = test_dir / "resource" / "file1.txt";
    const auto file2    = test_dir / "resource" / "file2.txt";

    auto file1_content = load_file(file1).value();    // Assume file exists, or throws then abort
    auto file2_content = load_file(file2).value();

    auto file1_lines = generate_line_by_line(file1_content);
    auto file2_lines = generate_line_by_line(file2_content);

    const auto uni_format_huge_permut = {
        helper::DiffFlags{ true, true },
        helper::DiffFlags{ true, false },
        helper::DiffFlags{ false, true },
        helper::DiffFlags{ false, false },
    };

    for (auto flags : uni_format_huge_permut) {
        const auto& s1 = file1_lines;
        const auto& s2 = file2_lines;

        const auto [res_old, res_new] = helper::do_diff(s1, s2, std::equal_to{}, flags);
        const auto name = std::format("[uni_fmt={}, huge={}]", flags.unified_format, flags.huge);

        test("the raw output of the diff should have the same values" + name) = [&] {
            expect(that % res_old.edit_dist == res_new.edit_dist) << "Edit distance not equal";
            helper::ut::ses_equals(res_old.ses, res_new.ses);
            helper::ut::lcs_equals(res_old.lcs, res_new.lcs);
            helper::ut::uni_hunks_equals(res_old.hunks, res_new.hunks);
        };

        test("the formatted output of the diff should be the same" + name) = [&] {
            auto hunks_str_old = helper::stringify_hunks_old(res_old.hunks);
            auto ses_str_old   = helper::stringify_ses_old(res_old.ses);

            auto hunks_str_new = fmt::to_string(dtlx::extra::display(res_new.hunks));
            auto ses_str_new   = fmt::to_string(dtlx::extra::display(res_new.ses));

            expect(hunks_str_old == hunks_str_new);
            expect(ses_str_old == ses_str_new);
        };

        test("edit distance should same from calling edit_distance() or diff()" + name) = [&] {
            auto edit_distance = dtlx::edit_distance(s1, s2, {});

            expect(that % edit_distance == res_new.edit_dist);
            expect(that % edit_distance == res_old.edit_dist);
        };

        if (flags.unified_format) {
            test("constructing unified format hunks from ses should be correct" + name) = [&] {
                auto uni_hunks_from_ses = dtlx::ses_to_unidiff(res_new.ses);

                expect(std::ranges::equal(uni_hunks_from_ses.inner, res_new.hunks.inner))
                    << fmt::format("expect: {}\ngot   :{}\n", res_new.hunks.inner, uni_hunks_from_ses.inner);
            };
        }
    }
}
