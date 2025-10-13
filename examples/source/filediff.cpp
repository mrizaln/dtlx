#include <dtlx/dtlx.hpp>
#define DTLX_DISPLAY_FMTLIB
#include <dtlx/extra/ses_display_pretty.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>

namespace fs = std::filesystem;

struct LineByLineRef
{
    std::vector<std::string_view> list;
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

        result.list.push_back(string.substr(idx, next - idx));
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

    auto diff = dtlx::diff(file1_lines.list, file2_lines.list);

    for (auto&& [elem, info] : diff.ses.get()) {
        const auto red   = fmt::bg(fmt::color::red);
        const auto green = fmt::bg(fmt::color::green);

        switch (info.type) {
        case dtlx::SesEdit::Common: fmt::println("{}", elem); break;
        case dtlx::SesEdit::Delete: fmt::println("{}", fmt::styled(elem, red)); break;
        case dtlx::SesEdit::Add: fmt::println("{}", fmt::styled(elem, green)); break;
        }
    }

    return 0;
}
