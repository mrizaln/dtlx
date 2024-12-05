#ifndef DTL_MODERN_EXTRA_SES_DISPLAY_SIMPLE_HPP
#define DTL_MODERN_EXTRA_SES_DISPLAY_SIMPLE_HPP

#include "dtl_modern/ses.hpp"

namespace dtl_modern::extra
{
    template <Diffable E>
    struct SesDisplaySimple
    {
        const Ses<E>& m_ses;

        // only checks whether it points to the same ses
        bool operator==(const SesDisplaySimple& other) const { return &m_ses == &other.m_ses; }
    };

    template <Diffable E>
    SesDisplaySimple<E> display(const Ses<E>& ses)
    {
        return { ses };
    }
}

#ifdef DTL_MODERN_DISPLAY_FMTLIB
#    define DTL_MODERN_FMT fmt
#    include <fmt/core.h>
#else
#    define DTL_MODERN_FMT std
#    include <format>
#endif

template <dtl_modern::Diffable E>
struct DTL_MODERN_FMT::formatter<dtl_modern::extra::SesDisplaySimple<E>>
    : public DTL_MODERN_FMT::formatter<std::string_view>
{
    auto format(const dtl_modern::extra::SesDisplaySimple<E>& ses_display, auto& fmt) const
    {
        for (const dtl_modern::SesElem<E>& ses_elem : ses_display.m_ses.get()) {
            const auto& [elem, info] = ses_elem;
            DTL_MODERN_FMT::format_to(fmt.out(), "{}{}\n", dtl_modern::ses_mark(info.m_type), elem);
        }
        return fmt.out();
    }
};

#include <ostream>

namespace dtl_modern::extra
{
    template <Diffable E>
    std::ostream& operator<<(std::ostream& os, const SesDisplaySimple<E>& ses)
    {
        os << DTL_MODERN_FMT::format("{}", ses);
        return os;
    }
}

#undef DTL_MODERN_FMT

#endif /* end of include guard: DTL_MODERN_EXTRA_SES_DISPLAY_SIMPLE_HPP */
