#include "common_modern.hpp"

#include <dtl-modern/dtl-modern.hpp>
#include <dtl/dtl.hpp>

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

void classic()
{
    fmt::println("\n>>> dtl classic:");

    auto a = std::vector{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    auto b = std::vector{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 };

    fmt::println("\ta: {}", a);
    fmt::println("\tb: {}\n", b);

    auto diff = dtl::Diff<int>{ a, b };
    diff.compose();

    fmt::println("\tedit_distance: {}", diff.getEditDistance());
    fmt::println("\tlcs          : {}", diff.getLcs().getSequence());
    fmt::println("\tses          : {}", diff.getSes().getSequence());
}

void modern()
{
    fmt::println("\n>>> dtl modern:");

    auto a = std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    auto b = std::array{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 };

    fmt::println("\ta: {}", a);
    fmt::println("\tb: {}\n", b);

    auto [lcs, ses, edit_distance] = dtl_modern::diff(a, b);

    fmt::println("\tedit_distance: {}", edit_distance);
    fmt::println("\tlcs          : {}", lcs.get());
    fmt::println("\tses          : {}", ses.get());
}

int main()
{
    classic();
    modern();
}
