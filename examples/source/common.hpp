#ifndef DTL_MODERN_EXAMPLE_COMMON_HPP
#define DTL_MODERN_EXAMPLE_COMMON_HPP

#include <dtl_modern/common.hpp>

#include <dtl.hpp>
#include <fmt/core.h>

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

template <fmt::formattable T>
    requires dtl_modern::Diffable<T>
struct fmt::formatter<dtl_modern::SesElem<T>> : fmt::formatter<std::string_view>
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

    auto format(const dtl_modern::SesElem<T>& ses_elem, auto& ctx) const
    {
        auto&& [elem, info] = ses_elem;
        auto out            = ctx.out();

        using E = dtl_modern::SesEdit;
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

#endif /* end of include guard: DTL_MODERN_EXAMPLE_COMMON_HPP */
