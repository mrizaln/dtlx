#include "common.hpp"

#include <dtlx/dtlx.hpp>
#define DTLX_DISPLAY_FMTLIB
#include <dtlx/extra/ses_display_simple.hpp>
#include <dtlx/extra/uni_hunk_display_simple.hpp>

#include <dtl.hpp>

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <sstream>

void classic()
{
    fmt::println("\n>>> dtl classic:");

    auto a = std::vector{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto b = std::vector{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 };

    fmt::println("\ta: {}", a);
    fmt::println("\tb: {}\n", b);

    auto diff = dtl::Diff<int>{ a, b };
    diff.compose();
    diff.composeUnifiedHunks();

    fmt::println("\tedit_distance: {}", diff.getEditDistance());
    fmt::println("\tlcs          : {}", diff.getLcs().getSequence());
    fmt::println("\tses          : {::?}", diff.getSes().getSequence());
    fmt::println("\thunks        : {}", diff.getUniHunks().size());

    std::stringstream sstream;
    diff.printSES(sstream);
    fmt::println("\tses text     : {:?}", sstream.str());
    std::stringstream{}.swap(sstream);

    diff.printUnifiedFormat(sstream);
    fmt::println("\thunks text   : {:?}", sstream.str());
}

void modern()
{
    fmt::println("\n>>> dtl modern:");

    auto a = std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto b = std::array{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 };

    fmt::println("\ta: {}", a);
    fmt::println("\tb: {}\n", b);

    auto [hunks, lcs, ses, edit_distance] = dtlx::unidiff(a, b);

    fmt::println("\tedit_distance: {}", edit_distance);
    fmt::println("\tlcs          : {}", lcs.get());
    fmt::println("\tses          : {::?}", ses.get());
    fmt::println("\thunks        : {}", hunks.get().size());

    auto ses_str   = fmt::format("{}", dtlx::extra::display(ses));
    auto hunks_str = fmt::format("{}", dtlx::extra::display(hunks));

    fmt::println("\tses text     : {:?}", ses_str);
    fmt::println("\thunks text   : {:?}", hunks_str);
}

int main()
{
    classic();
    modern();
}
