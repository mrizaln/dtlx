#ifndef DTLX_TEST_FORMATTER_HPP
#define DTLX_TEST_FORMATTER_HPP

#include <dtl.hpp>
#include <dtlx/common.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

// from <dtl/dtl.hpp>
template <fmt::formattable T>
struct fmt::formatter<std::pair<T, dtl::eleminfo>>
{
    bool display_idx = false;

    constexpr auto parse(auto& ctx)
    {
        auto pos = ctx.begin();
        while (pos != ctx.end() and *pos != '}') {
            if (*pos == '?') {
                display_idx = true;
            }
            ++pos;
        }
        return pos;
    }

    auto format(const std::pair<T, dtl::eleminfo>& pair, auto& ctx) const
    {
        auto&& [elem, info] = pair;
        auto out            = ctx.out();

        if (not display_idx) {
            switch (info.type) {
            case dtl::SES_DELETE: fmt::format_to(out, "D{}", elem); break;
            case dtl::SES_ADD: fmt::format_to(out, "A{}", elem); break;
            case dtl::SES_COMMON: fmt::format_to(out, "C{}", elem); break;
            }
        } else {
            auto before = info.beforeIdx;
            auto after  = info.afterIdx;
            switch (info.type) {
            case dtl::SES_DELETE: fmt::format_to(out, "D<{}><{}>{}", before, after, elem); break;
            case dtl::SES_ADD: fmt::format_to(out, "A<{}><{}>{}", before, after, elem); break;
            case dtl::SES_COMMON: fmt::format_to(out, "C<{}><{}>{}", before, after, elem); break;
            }
        }

        return out;
    }
};

// from <dtlx/dtlx.hpp>
template <fmt::formattable T>
    requires dtlx::Diffable<T>
struct fmt::formatter<dtlx::SesElem<T>> : fmt::formatter<std::string_view>
{
    bool display_idx = false;

    constexpr auto parse(auto& ctx)
    {
        auto pos = ctx.begin();
        while (pos != ctx.end() and *pos != '}') {
            if (*pos == '?') {
                display_idx = true;
            }
            ++pos;
        }
        return pos;
    }

    auto format(const dtlx::SesElem<T>& ses_elem, auto& ctx) const
    {
        auto&& [elem, info] = ses_elem;
        auto out            = ctx.out();

        using E = dtlx::SesEdit;
        if (not display_idx) {
            switch (info.type) {
            case E::Delete: fmt::format_to(out, "D{}", elem); break;
            case E::Add: fmt::format_to(out, "A{}", elem); break;
            case E::Common: fmt::format_to(out, "C{}", elem); break;
            }
        } else {
            auto before = info.index_before;
            auto after  = info.index_after;
            switch (info.type) {
            case E::Delete: fmt::format_to(out, "D<{}><{}>{}", before, after, elem); break;
            case E::Add: fmt::format_to(out, "A<{}><{}>{}", before, after, elem); break;
            case E::Common: fmt::format_to(out, "C<{}><{}>{}", before, after, elem); break;
            }
        }

        return out;
    }
};

// from <dtl/dtl.hpp>
template <fmt::formattable T>
struct fmt::formatter<dtl::uniHunk<std::pair<T, dtl::elemInfo>>> : public fmt::formatter<std::string_view>
{
    auto format(const dtl::uniHunk<std::pair<T, dtl::elemInfo>>& hunk, auto& fmt) const
    {
        return fmt::format_to(
            fmt.out(),
            "Unified Format Hunk:{{\n"
            "\ta            : {}\n"
            "\tb            : {}\n"
            "\tc            : {}\n"
            "\td            : {}\n"
            "\tcommon[0]    : {::?}\n"
            "\tcommon[1]    : {::?}\n"
            "\tchange       : {::?}\n"
            "\tinc-dec count: {}\n}}",
            hunk.a,
            hunk.b,
            hunk.c,
            hunk.d,
            hunk.common[0],
            hunk.common[1],
            hunk.change,
            hunk.inc_dec_count
        );
    }
};

// from <dtlx/dtlx.hpp>
template <fmt::formattable T>
    requires dtlx::Diffable<T>
struct fmt::formatter<dtlx::UniHunk<T>> : public fmt::formatter<std::string_view>
{
    auto format(const dtlx::UniHunk<T>& hunk, auto& fmt) const
    {
        return fmt::format_to(
            fmt.out(),
            "Unified Format Hunk:{{\n"
            "\ta            : {}\n"
            "\tb            : {}\n"
            "\tc            : {}\n"
            "\td            : {}\n"
            "\tcommon[0]    : {::?}\n"
            "\tcommon[1]    : {::?}\n"
            "\tchange       : {::?}\n"
            "\tinc-dec count: {}\n}}",
            hunk.a,
            hunk.b,
            hunk.c,
            hunk.d,
            hunk.common_0,
            hunk.common_1,
            hunk.change,
            hunk.inc_dec_count
        );
    }
};

#endif /* end of include guard: DTLX_TEST_FORMATTER_HPP */
