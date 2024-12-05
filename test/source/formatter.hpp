#ifndef DTL_MODERN_TEST_FORMATTER_HPP
#define DTL_MODERN_TEST_FORMATTER_HPP

#include <dtl_modern/common.hpp>
#include <dtl.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

// from <dtl/dtl.hpp>
template <fmt::formattable T>
struct fmt::formatter<std::pair<T, dtl::eleminfo>>
{
    bool m_display_idx = false;

    constexpr auto parse(auto& ctx)
    {
        auto pos = ctx.begin();
        while (pos != ctx.end() and *pos != '}') {
            if (*pos == '?') {
                m_display_idx = true;
            }
            ++pos;
        }
        return pos;
    }

    auto format(const std::pair<T, dtl::eleminfo>& pair, auto& ctx) const
    {
        auto&& [elem, info] = pair;
        auto out            = ctx.out();

        if (not m_display_idx) {
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

// from <dtl_modern/dtl_modern.hpp>
template <fmt::formattable T>
    requires dtl_modern::Diffable<T>
struct fmt::formatter<dtl_modern::SesElem<T>> : fmt::formatter<std::string_view>
{
    bool m_display_idx = false;

    constexpr auto parse(auto& ctx)
    {
        auto pos = ctx.begin();
        while (pos != ctx.end() and *pos != '}') {
            if (*pos == '?') {
                m_display_idx = true;
            }
            ++pos;
        }
        return pos;
    }

    auto format(const dtl_modern::SesElem<T>& ses_elem, auto& ctx) const
    {
        auto&& [elem, info] = ses_elem;
        auto out            = ctx.out();

        using E = dtl_modern::SesEdit;
        if (not m_display_idx) {
            switch (info.m_type) {
            case E::Delete: fmt::format_to(out, "D{}", elem); break;
            case E::Add: fmt::format_to(out, "A{}", elem); break;
            case E::Common: fmt::format_to(out, "C{}", elem); break;
            }
        } else {
            auto before = info.m_index_before;
            auto after  = info.m_index_after;
            switch (info.m_type) {
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

// from <dtl_modern/dtl_modern.hpp>
template <fmt::formattable T>
    requires dtl_modern::Diffable<T>
struct fmt::formatter<dtl_modern::UniHunk<T>> : public fmt::formatter<std::string_view>
{
    auto format(const dtl_modern::UniHunk<T>& hunk, auto& fmt) const
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
            hunk.m_a,
            hunk.m_b,
            hunk.m_c,
            hunk.m_d,
            hunk.m_common_0,
            hunk.m_common_1,
            hunk.m_change,
            hunk.m_inc_dec_count
        );
    }
};

#endif /* end of include guard: DTL_MODERN_TEST_FORMATTER_HPP */
