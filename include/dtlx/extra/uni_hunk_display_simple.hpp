#ifndef DTLX_EXTRA_UNI_HUNK_DISPLAY_SIMPLE_HPP
#define DTLX_EXTRA_UNI_HUNK_DISPLAY_SIMPLE_HPP

#include "dtlx/common.hpp"

namespace dtlx::extra
{
    /**
     * @struct UniHunkDisplaySimple
     *
     * @brief Wrapper type for displaying UniHunk in a simple format.
     */
    template <Diffable E>
    struct UniHunkDisplaySimple
    {
        const UniHunk<E>& hunk;

        // only checks whether it points to the same unihunk
        bool operator==(const UniHunkDisplaySimple& other) const { return &hunk == &other.hunk; }
    };

    /**
     * @struct UniHunkSeqDisplaySimple
     *
     * @brief Wrapper type for displaying UniHunkSeq in a simple format.
     */
    template <Diffable E>
    struct UniHunkSeqDisplaySimple
    {
        const UniHunkSeq<E>& hunks;

        // only checks whether it points to the same unihunk
        bool operator==(const UniHunkSeqDisplaySimple& other) const { return &hunks == &other.hunks; }
    };

    /**
     * @brief Create a wrapper for displaying UniHunk in a simple format.
     *
     * @param hunk The UniHunk to display.
     * @return The wrapper for displaying UniHunk in a simple format.
     */
    template <Diffable E>
    UniHunkDisplaySimple<E> display(const UniHunk<E>& hunk)
    {
        return { hunk };
    }

    /**
     * @brief Create a wrapper for displaying UniHunkSeq in a simple format.
     *
     * @param hunks The UniHunkSeq to display.
     * @return The wrapper for displaying UniHunkSeq in a simple format.
     */
    template <Diffable E>
    UniHunkSeqDisplaySimple<E> display(const UniHunkSeq<E>& hunks)
    {
        return { hunks };
    }
}

#ifdef DTLX_DISPLAY_FMTLIB
#    define DTLX_FMT fmt
#    include <fmt/core.h>
#else
#    define DTLX_FMT std
#    include <format>
#endif

#include <algorithm>

template <dtlx::Diffable E>
struct DTLX_FMT::formatter<dtlx::extra::UniHunkDisplaySimple<E>>
    : public DTLX_FMT::formatter<std::string_view>
{
    using UniHunk = dtlx::extra::UniHunkDisplaySimple<E>;

    auto format(const UniHunk& uni_hunk, auto& fmt) const
    {
        using dtlx::ses_mark, dtlx::SesEdit;
        using std::ranges::for_each;

        const auto& [a, b, c, d, common_0, common_1, change, inc_dec_count] = uni_hunk.hunk;

        auto f_ses_elem_change = [&](auto&& ses_elem) {
            const auto& [elem, info] = ses_elem;
            DTLX_FMT::format_to(fmt.out(), "{}{}\n", ses_mark(info.type), elem);
        };
        auto f_ses_elem_common = [&](auto&& ses_elem) {
            DTLX_FMT::format_to(fmt.out(), "{}{}\n", ses_mark(SesEdit::Common), ses_elem.elem);
        };

        DTLX_FMT::format_to(fmt.out(), "@@ -{},{} +{},{} @@\n", a, b, c, d);
        for_each(common_0, f_ses_elem_common);
        for_each(change, f_ses_elem_change);
        for_each(common_1, f_ses_elem_common);

        return fmt.out();
    }
};

template <dtlx::Diffable E>
struct DTLX_FMT::formatter<dtlx::extra::UniHunkSeqDisplaySimple<E>>
    : public DTLX_FMT::formatter<std::string_view>
{
    using UniHunks = dtlx::extra::UniHunkSeqDisplaySimple<E>;

    auto format(const UniHunks& uni_hunks, auto& fmt) const
    {
        using dtlx::extra::display;
        using dtlx::extra::UniHunkDisplaySimple;

        for (const auto& uni_hunk : uni_hunks.hunks.inner) {
            DTLX_FMT::format_to(fmt.out(), "{}", display(uni_hunk));
        }

        return fmt.out();
    }
};

#include <ostream>

namespace dtlx::extra
{
    template <Diffable E>
    std::ostream& operator<<(std::ostream& os, const UniHunkDisplaySimple<E>& uni_hunk)
    {
        os << DTLX_FMT::format("{}", uni_hunk);
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

#undef DTLX_FMT

#endif /* end of include guard: DTLX_EXTRA_UNI_HUNK_DISPLAY_SIMPLE_HPP */
