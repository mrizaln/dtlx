#include "common.hpp"    // fmt::formatter for dtlx::SesElem

#include <dtlx/dtlx.hpp>
#define DTLX_DISPLAY_FMTLIB
#include <dtlx/extra/ses_display_simple.hpp>

#include <dtl.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

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

    auto [lcs, ses, edit_distance] = dtlx::diff(hello1, hello2);

    fmt::println("\tedit_distance: {}", edit_distance);
    fmt::println("\tlcs          : {}", lcs.get());
    fmt::println("\tses          : {}", ses.get());

    fmt::print("{}", dtlx::extra::display(ses));
    // std::cout << dtlx::extra::display(ses);
}

int main()
{
    classic();
    modern();
}
