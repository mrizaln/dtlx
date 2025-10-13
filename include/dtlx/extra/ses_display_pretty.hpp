#ifndef DTLX_EXTRA_SES_DISPLAY_PRETTY_HPP
#define DTLX_EXTRA_SES_DISPLAY_PRETTY_HPP

#include "dtlx/ses.hpp"

namespace dtlx::extra
{
    /**
     * @struct SesDisplayPretty
     *
     * @brief Wrapper type for displaying SES in pretty format.
     */
    template <Diffable E>
    struct SesDisplayPretty
    {
        const Ses<E>&    ses;
        std::string_view sep;

        // only checks whether it points to the same ses
        bool operator==(const SesDisplayPretty& other) const { return &ses == &other.ses; }
    };

    /**
     * @brief Create a wrapper for displaying SES in pretty format.
     *
     * @param ses The SES to display.
     * @param sep Separator between lhs sequence and rhs sequence.
     * @return The wrapper for displaying SES in pretty format.
     */
    template <Diffable E>
    SesDisplayPretty<E> display_pretty(const Ses<E>& ses, std::string_view sep = "\n")
    {
        return { ses, sep };
    }
}

#include <fmt/color.h>
#include <fmt/core.h>

template <fmt::formattable E>
    requires dtlx::Diffable<E>
struct fmt::formatter<dtlx::extra::SesDisplayPretty<E>>
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
            case 'l': display_line = DisplayLine::Left; break;
            case 'r': display_line = DisplayLine::Right; break;
            case 'f': {
                colorize_common = true;
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

    auto format(const dtlx::extra::SesDisplayPretty<E>& ses_display, auto& fmt) const
    {
        const auto& [ses, sep] = ses_display;

        const auto red        = fmt::bg(fmt::color::red);
        const auto green      = fmt::bg(fmt::color::green);
        const auto dark_red   = fmt::bg(fmt::color::dark_red);
        const auto dark_green = fmt::bg(fmt::color::dark_green);

        switch (display_line) {
        case DisplayLine::Left: {
            for (const auto& [elem, info] : ses.get()) {
                using Edit = dtlx::SesEdit;
                switch (info.type) {
                case Edit::Delete: fmt::format_to(fmt.out(), red, "{}", elem); break;
                case Edit::Add: /* do nothing */ break;
                case Edit::Common: {
                    if (colorize_common) {
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
                using Edit = dtlx::SesEdit;
                switch (info.type) {
                case Edit::Delete: /* do nothing */ break;
                case Edit::Add: fmt::format_to(fmt.out(), green, "{}", elem); break;
                case Edit::Common: {
                    if (colorize_common) {
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
                using Edit = dtlx::SesEdit;
                switch (info.type) {
                case Edit::Delete: fmt::format_to(lhs_it, red, "{}", elem); break;
                case Edit::Add: fmt::format_to(rhs_it, green, "{}", elem); break;
                case Edit::Common: {
                    if (colorize_common) {
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

    bool        colorize_common = false;
    DisplayLine display_line    = DisplayLine::Both;
};

#endif /* end of include guard: DTLX_EXTRA_SES_DISPLAY_PRETTY_HPP */
