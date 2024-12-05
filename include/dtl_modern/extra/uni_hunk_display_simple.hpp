#ifndef DTL_MODERN_EXTRA_UNI_HUNK_DISPLAY_SIMPLE_HPP
#define DTL_MODERN_EXTRA_UNI_HUNK_DISPLAY_SIMPLE_HPP

#include "dtl_modern/common.hpp"

namespace dtl_modern::extra
{
    template <Diffable E>
    struct UniHunkDisplaySimple
    {
        const UniHunk<E>& m_hunk;

        // only checks whether it points to the same unihunk
        bool operator==(const UniHunkDisplaySimple& other) const { return &m_hunk == &other.m_hunk; }
    };

    template <Diffable E>
    struct UniHunkSeqDisplaySimple
    {
        const UniHunkSeq<E>& m_hunks;

        // only checks whether it points to the same unihunk
        bool operator==(const UniHunkSeqDisplaySimple& other) const { return &m_hunks == &other.m_hunks; }
    };

    template <Diffable E>
    UniHunkDisplaySimple<E> display(const UniHunk<E>& hunk)
    {
        return { hunk };
    }

    template <Diffable E>
    UniHunkSeqDisplaySimple<E> display(const UniHunkSeq<E>& hunks)
    {
        return { hunks };
    }
}

#ifdef DTL_MODERN_DISPLAY_FMTLIB
#    define DTL_MODERN_FMT fmt
#    include <fmt/core.h>
#else
#    define DTL_MODERN_FMT std
#    include <format>
#endif

#include <algorithm>

template <dtl_modern::Diffable E>
struct DTL_MODERN_FMT::formatter<dtl_modern::extra::UniHunkDisplaySimple<E>>
    : public DTL_MODERN_FMT::formatter<std::string_view>
{
    using UniHunk = dtl_modern::extra::UniHunkDisplaySimple<E>;

    auto format(const UniHunk& uni_hunk, auto& fmt) const
    {
        using dtl_modern::ses_mark, dtl_modern::SesEdit;
        using std::ranges::for_each;

        const auto& [a, b, c, d, common_0, common_1, change, inc_dec_count] = uni_hunk.m_hunk;

        auto f_ses_elem_change = [&](auto&& ses_elem) {
            const auto& [elem, info] = ses_elem;
            DTL_MODERN_FMT::format_to(fmt.out(), "{}{}\n", ses_mark(info.m_type), elem);
        };
        auto f_ses_elem_common = [&](auto&& ses_elem) {
            DTL_MODERN_FMT::format_to(fmt.out(), "{}{}\n", ses_mark(SesEdit::Common), ses_elem.m_elem);
        };

        DTL_MODERN_FMT::format_to(fmt.out(), "@@ -{},{} +{},{} @@\n", a, b, c, d);
        for_each(common_0, f_ses_elem_common);
        for_each(change, f_ses_elem_change);
        for_each(common_1, f_ses_elem_common);

        return fmt.out();
    }
};

template <dtl_modern::Diffable E>
struct DTL_MODERN_FMT::formatter<dtl_modern::extra::UniHunkSeqDisplaySimple<E>>
    : public DTL_MODERN_FMT::formatter<std::string_view>
{
    using UniHunks = dtl_modern::extra::UniHunkSeqDisplaySimple<E>;

    auto format(const UniHunks& uni_hunks, auto& fmt) const
    {
        using dtl_modern::extra::display;
        using dtl_modern::extra::UniHunkDisplaySimple;

        for (const auto& uni_hunk : uni_hunks.m_hunks.m_inner) {
            DTL_MODERN_FMT::format_to(fmt.out(), "{}", display(uni_hunk));
        }

        return fmt.out();
    }
};

#include <ostream>

namespace dtl_modern::extra
{
    template <Diffable E>
    std::ostream& operator<<(std::ostream& os, const UniHunkDisplaySimple<E>& uni_hunk)
    {
        os << DTL_MODERN_FMT::format("{}", uni_hunk);
        return os;
    }

    template <Diffable E>
    std::ostream& operator<<(std::ostream& os, const std::vector<UniHunkDisplaySimple<E>>& hunks)
    {
        for (const auto& hunk : hunks) {
            os << hunk;
        }
        return os;
    }
}

#undef DTL_MODERN_FMT

#endif /* end of include guard: DTL_MODERN_EXTRA_UNI_HUNK_DISPLAY_SIMPLE_HPP */
