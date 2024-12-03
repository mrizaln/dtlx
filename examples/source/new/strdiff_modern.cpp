#include "common_modern.hpp"    // fmt::formatter for dtl_modern::SesElem

#include <dtl_modern/dtl_modern.hpp>

#define DTL_MODERN_DISPLAY_FMTLIB
#include <dtl_modern/extra/ses_display_simple.hpp>

#include <fmt/core.h>
#include <fmt/std.h>
#include <fmt/ranges.h>

void classic()
{
    using namespace std::string_literals;    // s UDL
    using Diff = dtl::Diff<char, std::string>;

    fmt::println("\n>>> dtl classic:");

    auto hello1 = "hello World!"s;
    auto hello2 = "Hell word"s;

    fmt::println("\thello1       : {:?}", hello1);
    fmt::println("\thello2       : {:?}\n", hello2);

    auto diff = Diff{ hello1, hello2 };
    diff.compose();

    fmt::println("\tedit_distance: {}", diff.getEditDistance());
    fmt::println("\tlcs          : {}", diff.getLcs().getSequence());
    fmt::println("\tses          : {}", diff.getSes().getSequence());

    diff.printSES();
}

void modern()
{
    using namespace std::literals;    // s and sv UDL

    fmt::println("\n>>> dtl modern:");

    auto hello1 = "hello World!"sv;    // notice the different type, this one is std::string_view
    auto hello2 = "Hell word"s;        // this one is std::string

    fmt::println("\thello1       : {:?}", hello1);
    fmt::println("\thello2       : {:?}\n", hello2);

    auto [lcs, ses, edit_distance] = dtl_modern::diff(hello1, hello2);

    fmt::println("\tedit_distance: {}", edit_distance);
    fmt::println("\tlcs          : {}", lcs.get());
    fmt::println("\tses          : {}", ses.get());

    fmt::print("{}", dtl_modern::extra::display(ses));
    // std::cout << dtl_modern::extra::display(ses);
}

int main()
{
    classic();
    modern();
}
