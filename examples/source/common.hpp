#ifndef DTL_MODERN_EXAMPLE_COMMON_HPP
#define DTL_MODERN_EXAMPLE_COMMON_HPP

#include <dtl_modern/common.hpp>

#include <dtl.hpp>
#include <fmt/core.h>

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

#endif /* end of include guard: DTL_MODERN_EXAMPLE_COMMON_HPP */
