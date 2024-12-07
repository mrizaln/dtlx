#include <dtl_modern/dtl_modern.hpp>
#define DTL_MODERN_DISPLAY_FMTLIB
#include <dtl_modern/extra/ses_display_simple.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>

namespace fs = std::filesystem;

struct LineByLineRef
{
    std::vector<std::string_view> m_list;
};

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

LineByLineRef generate_line_by_line(const std::string_view string)
{
    auto result = LineByLineRef{};

    std::size_t idx = 0;
    while (true) {
        auto next = string.find('\n', idx);
        if (next == std::string::npos) {
            break;
        }

        result.m_list.push_back(string.substr(idx, next - idx));
        idx = next + 1;
    }

    return result;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fmt::println(stderr, "Please provide two arguments");
        fmt::println(stderr, "{} <file1> <file2>", argv[0]);
        return 1;
    }

    auto file1 = fs::path{ argv[1] };
    auto file2 = fs::path{ argv[2] };

    if (not fs::exists(file1) or not fs::is_regular_file(file1)) {
        fmt::println(stderr, "No such file or directory: {}", file1.c_str());
        return 1;
    }
    if (not fs::exists(file2) or not fs::is_regular_file(file2)) {
        fmt::println(stderr, "No such file or directory: {}", file1.c_str());
        return 1;
    }

    auto file1_content = load_file(file1).value();
    auto file2_content = load_file(file2).value();

    auto file1_lines = generate_line_by_line(file1_content);
    auto file2_lines = generate_line_by_line(file2_content);

    auto diff = dtl_modern::diff(file1_lines.m_list, file2_lines.m_list);
    fmt::println("{}", dtl_modern::extra::display(diff.m_ses));

    return 0;
}
