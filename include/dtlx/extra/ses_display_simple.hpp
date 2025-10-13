#ifndef DTLX_EXTRA_SES_DISPLAY_SIMPLE_HPP
#define DTLX_EXTRA_SES_DISPLAY_SIMPLE_HPP

#include "dtlx/ses.hpp"

namespace dtlx::extra
{
    /**
     * @struct SesDisplaySimple
     *
     * @brief Wrapper type for displaying SES in a simple format.
     */
    template <Diffable E>
    struct SesDisplaySimple
    {
        const Ses<E>& ses;

        // only checks whether it points to the same ses
        bool operator==(const SesDisplaySimple& other) const { return &ses == &other.ses; }
    };

    /**
     * @brief Create a wrapper for displaying SES in a simple format.
     *
     * @param ses The SES to display.
     * @return The wrapper for displaying SES in a simple format.
     */
    template <Diffable E>
    SesDisplaySimple<E> display(const Ses<E>& ses)
    {
        return { ses };
    }
}

#ifdef DTLX_DISPLAY_FMTLIB
#    define DTLX_FMT fmt
#    include <fmt/core.h>
#else
#    define DTLX_FMT std
#    include <format>
#endif

template <dtlx::Diffable E>
struct DTLX_FMT::formatter<dtlx::extra::SesDisplaySimple<E>>
    : public DTLX_FMT::formatter<std::string_view>
{
    auto format(const dtlx::extra::SesDisplaySimple<E>& ses_display, auto& fmt) const
    {
        for (const dtlx::SesElem<E>& ses_elem : ses_display.ses.get()) {
            const auto& [elem, info] = ses_elem;
            DTLX_FMT::format_to(fmt.out(), "{}{}\n", dtlx::ses_mark(info.type), elem);
        }
        return fmt.out();
    }
};

#include <ostream>

namespace dtlx::extra
{
    template <Diffable E>
    std::ostream& operator<<(std::ostream& os, const SesDisplaySimple<E>& ses)
    {
        os << DTLX_FMT::format("{}", ses);
        return os;
    }
}

#undef DTLX_FMT

#endif /* end of include guard: DTLX_EXTRA_SES_DISPLAY_SIMPLE_HPP */
