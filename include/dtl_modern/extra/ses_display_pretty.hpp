#ifndef DTL_MODERN_EXTRA_SES_DISPLAY_PRETTY_HPP
#define DTL_MODERN_EXTRA_SES_DISPLAY_PRETTY_HPP

#include "dtl_modern/ses.hpp"

namespace dtl_modern::extra
{
    template <Diffable E>
    struct SesDisplayPretty
    {
        const Ses<E>&    m_ses;
        std::string_view m_sep;

        // only checks whether it points to the same ses
        bool operator==(const SesDisplayPretty& other) const { return &m_ses == &other.m_ses; }
    };

    template <Diffable E>
    SesDisplayPretty<E> display_pretty(const Ses<E>& ses, std::string_view sep = "\n")
    {
        return { ses, sep };
    }
}

#include <fmt/core.h>
#include <fmt/color.h>

template <fmt::formattable E>
    requires dtl_modern::Diffable<E>
struct fmt::formatter<dtl_modern::extra::SesDisplayPretty<E>>
{
    enum class DisplayLine
    {
        Both,
        Left,
        Right
    };

    constexpr auto parse(format_parse_context& ctx)
    {
        auto pos = ctx.begin();
        while (pos != ctx.end() and *pos != '}') {
            switch (*pos) {
            case 'l': m_display_line = DisplayLine::Left; break;
            case 'r': m_display_line = DisplayLine::Right; break;
            case 'f': {
                m_colorize_common = true;
                ++pos;
                if (*pos != '}') {
                    throw fmt::format_error{ "Specifier 'f' must be the last specifier" };
                }
                return pos;
            }
            default:
                throw fmt::format_error{
                    "Invalid format specifier; expected 'l', 'r', 'f', 'lf', 'rf', or nothing."
                };
            }
            ++pos;
        }
        return pos;
    }

    auto format(const dtl_modern::extra::SesDisplayPretty<E>& ses_display, auto& fmt) const
    {
        const auto& [ses, sep] = ses_display;

        const auto red        = fmt::bg(fmt::color::red);
        const auto green      = fmt::bg(fmt::color::green);
        const auto dark_red   = fmt::bg(fmt::color::dark_red);
        const auto dark_green = fmt::bg(fmt::color::dark_green);

        switch (m_display_line) {
        case DisplayLine::Left: {
            for (const auto& [elem, info] : ses.get()) {
                using Edit = dtl_modern::SesEdit;
                switch (info.m_type) {
                case Edit::Delete: fmt::format_to(fmt.out(), red, "{}", elem); break;
                case Edit::Add: /* do nothing */ break;
                case Edit::Common: {
                    if (m_colorize_common) {
                        fmt::format_to(fmt.out(), dark_red, "{}", elem);
                    } else {
                        fmt::format_to(fmt.out(), "{}", elem);
                    }
                } break;
                }
            }
        } break;
        case DisplayLine::Right: {
            for (const auto& [elem, info] : ses.get()) {
                using Edit = dtl_modern::SesEdit;
                switch (info.m_type) {
                case Edit::Delete: /* do nothing */ break;
                case Edit::Add: fmt::format_to(fmt.out(), green, "{}", elem); break;
                case Edit::Common: {
                    if (m_colorize_common) {
                        fmt::format_to(fmt.out(), dark_green, "{}", elem);
                    } else {
                        fmt::format_to(fmt.out(), "{}", elem);
                    }
                } break;
                }
            }
        } break;
        default: {
            auto lhs = fmt::memory_buffer{};
            auto rhs = fmt::memory_buffer{};

            auto lhs_it = std::back_inserter(lhs);
            auto rhs_it = std::back_inserter(rhs);

            for (const auto& [elem, info] : ses.get()) {
                using Edit = dtl_modern::SesEdit;
                switch (info.m_type) {
                case Edit::Delete: fmt::format_to(lhs_it, red, "{}", elem); break;
                case Edit::Add: fmt::format_to(rhs_it, green, "{}", elem); break;
                case Edit::Common: {
                    if (m_colorize_common) {
                        fmt::format_to(lhs_it, dark_red, "{}", elem);
                        fmt::format_to(rhs_it, dark_green, "{}", elem);
                    } else {
                        fmt::format_to(lhs_it, "{}", elem);
                        fmt::format_to(rhs_it, "{}", elem);
                    }
                } break;
                }
            }

            fmt::format_to(fmt.out(), "{}{}{}", fmt::to_string(lhs), sep, fmt::to_string(rhs));
        } break;
        }

        return fmt.out();
    }

    bool        m_colorize_common = false;
    DisplayLine m_display_line    = DisplayLine::Both;
};

#endif /* end of include guard: DTL_MODERN_EXTRA_SES_DISPLAY_PRETTY_HPP */
