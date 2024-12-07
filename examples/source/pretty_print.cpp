#include <dtl_modern/dtl_modern.hpp>
#include <dtl_modern/extra/ses_display_pretty.hpp>    // requires fmt

#include <fmt/core.h>
#include <fmt/color.h>

#include <iterator>

int main()
{
    using namespace std::literals;    // s and sv UDL

    auto hello1 = "hello World!"sv;    // notice the different type, this one is std::string_view
    auto hello2 = "Hell word"s;        // this one is std::string

    auto [lcs, ses, edit_distance] = dtl_modern::diff(hello1, hello2);

    // diy
    // ---
    auto hello1_color = std::string{};
    auto hello2_color = std::string{};

    auto hello1_out = std::back_inserter(hello1_color);
    auto hello2_out = std::back_inserter(hello2_color);

    const auto red        = fmt::bg(fmt::color::red);
    const auto green      = fmt::bg(fmt::color::green);
    const auto dark_red   = fmt::bg(fmt::color::dark_red);
    const auto dark_green = fmt::bg(fmt::color::dark_green);

    for (const auto& [elem, info] : ses.get()) {
        using Edit = dtl_modern::SesEdit;
        switch (info.m_type) {
        case Edit::Delete: fmt::format_to(hello1_out, red, "{}", elem); break;
        case Edit::Add: fmt::format_to(hello2_out, green, "{}", elem); break;
        case Edit::Common: {
            fmt::format_to(hello1_out, dark_red, "{}", elem);
            fmt::format_to(hello2_out, dark_green, "{}", elem);
        } break;
        }
    }

    fmt::println("\ndiy:");
    fmt::println("{}", fmt::styled(hello1_color, fmt::bg(fmt::color::dark_red)));
    fmt::println("{}", fmt::styled(hello2_color, fmt::bg(fmt::color::dark_red)));
    // ---

    // extra dtl_modern functionality
    // ---
    fmt::println("\nextra:");
    fmt::println("\n> common not colored:");
    fmt::println("{}", dtl_modern::extra::display_pretty(ses));
    fmt::println("\n> full color:");
    fmt::println("{:f}", dtl_modern::extra::display_pretty(ses));
    // ---
}
