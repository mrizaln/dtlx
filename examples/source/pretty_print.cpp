#include <dtl_modern/dtl_modern.hpp>
#include <dtl_modern/extra/ses_display_pretty.hpp>    // requires fmt

#include <fmt/color.h>
#include <fmt/core.h>

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
        switch (info.type) {
        case Edit::Delete: fmt::format_to(hello1_out, red, "{}", elem); break;
        case Edit::Add: fmt::format_to(hello2_out, green, "{}", elem); break;
        case Edit::Common: {
            fmt::format_to(hello1_out, dark_red, "{}", elem);
            fmt::format_to(hello2_out, dark_green, "{}", elem);
        } break;
        }
    }

    fmt::println("\n--- diy:");
    fmt::println("{}", hello1_color);
    fmt::println("{}", hello2_color);
    // ---

    // extra dtl_modern functionality
    // ---
    using dtl_modern::extra::display_pretty;

    fmt::println("\n--- extra:");
    fmt::println("\n> <nothing> :\n{}", display_pretty(ses));
    fmt::println("\n> l         :\n{:l}", display_pretty(ses));
    fmt::println("\n> r         :\n{:r}", display_pretty(ses));
    fmt::println("\n> f         :\n{:f}", display_pretty(ses));
    fmt::println("\n> lf        :\n{:lf}", display_pretty(ses));
    fmt::println("\n> rf        :\n{:rf}", display_pretty(ses));
    // ---
}
