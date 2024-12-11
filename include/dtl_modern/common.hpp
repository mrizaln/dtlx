#ifndef DTL_MODERN_COMMON_HPP
#define DTL_MODERN_COMMON_HPP

#include "dtl_modern/concepts.hpp"

#include <cassert>
#include <cstdint>
#include <vector>

namespace dtl_modern
{
    using i64 = std::int64_t;
    using u64 = std::uint64_t;

    /**
     * @enum SesEdit
     *
     * @brief Type of edit for SES.
     */
    enum class SesEdit : std::int32_t
    {
        Delete = -1,
        Common = 0,
        Add    = 1,
    };

    /**
     * @brief Get the mark of the edit type.
     *
     * @param edit Type of edit.
     * @return A character representing the edit type.
     */
    constexpr char ses_mark(SesEdit edit) noexcept
    {
        switch (edit) {
        case SesEdit::Delete: return '-';
        case SesEdit::Common: return ' ';
        case SesEdit::Add: return '+';
        default: [[unlikely]] return '?';
        }
    }

    /**
     * @struct ElemInfo
     *
     * @brief Info for Unified Format.
     */
    struct ElemInfo
    {
        i64     m_index_before;    // index of prev sequence
        i64     m_index_after;     // index of after sequence
        SesEdit m_type;            // type of edit

        auto operator<=>(const ElemInfo&) const = default;
        bool operator==(const ElemInfo&) const  = default;
    };

    /**
     * @struct KPoint
     *
     * @brief Edit graph/grid coordinate.
     */
    struct KPoint
    {
        i64 m_x;
        i64 m_y;
        i64 m_k;    // diagonal

        auto operator<=>(const KPoint&) const = default;
        bool operator==(const KPoint&) const  = default;
    };

    /**
     * @struct Point
     *
     * @brief Edit graph/grid coordinate without diagonal.
     */
    struct Point
    {
        i64 m_x;
        i64 m_y;

        auto operator<=>(const Point&) const = default;
        bool operator==(const Point&) const  = default;
    };

    /**
     * @struct EditPath
     *
     * @brief Edit path for a sequence.
     */
    struct EditPath
    {
        EditPath() = default;

        EditPath(u64 size, i64 value)
            : m_inner(size, value)
        {
        }

        // clang-format off
        i64&       operator[](u64 index)       { return m_inner[index]; }
        const i64& operator[](u64 index) const { return m_inner[index]; }

        i64&       at(i64 index)       { assert(index >= 0); return m_inner[static_cast<u64>(index)]; }
        const i64& at(i64 index) const { assert(index >= 0); return m_inner[static_cast<u64>(index)]; }

        void        add(i64 value)        { m_inner.push_back(value); }
        std::size_t size() const noexcept { return m_inner.size(); }
        void        clear() noexcept      { m_inner.clear(); }
        // clang-format on

        std::vector<i64> m_inner;

        bool operator==(const EditPath&) const = default;
    };

    /**
     * @struct EditPathCoordinates
     *
     * @brief Edit path coordinates for a sequence.
     */
    template <typename P = KPoint>
    struct EditPathCoordinates
    {
        EditPathCoordinates() = default;

        EditPathCoordinates(u64 size, P value)
            : m_inner(size, value)
        {
        }

        // clang-format off
        P&       operator[](u64 index)       { return m_inner[index]; }
        const P& operator[](u64 index) const { return m_inner[index]; }

        P&       at(i64 index)       { assert(index >= 0); return m_inner[static_cast<u64>(index)]; }
        const P& at(i64 index) const { assert(index >= 0); return m_inner[static_cast<u64>(index)]; }

        void        add(P value)          { m_inner.push_back(value); }
        std::size_t size() const noexcept { return m_inner.size(); }
        void        clear() noexcept      { m_inner.clear(); }
        // clang-format on

        std::vector<P> m_inner;

        bool operator==(const EditPathCoordinates&) const = default;
    };

    /**
     * @struct SesElem
     *
     * @brief SES element; contains the element and its edit type.
     */
    template <Diffable E>
    struct SesElem
    {
        using Elem = E;

        Elem     m_elem;
        ElemInfo m_info;

        bool operator==(const SesElem&) const
            requires TriviallyComparable<E>
        = default;
    };

    /**
     * @struct UniHunk
     *
     * @brief Structure of Unified Format Hunk.
     */
    template <Diffable E>
    struct UniHunk
    {
        using Elem = E;

        i64 m_a;    // @@ -a,b +c,d @@
        i64 m_b;
        i64 m_c;
        i64 m_d;

        std::vector<SesElem<Elem>> m_common_0;    // anteroposterior commons on changes
        std::vector<SesElem<Elem>> m_common_1;

        std::vector<SesElem<Elem>> m_change;    // changes

        i64 m_inc_dec_count;    // count of increase and decrease

        bool operator==(const UniHunk&) const
            requires TriviallyComparable<E>
        = default;
    };

    /**
     * @struct UniHunkSeq
     *
     * @brief Sequence of Unified Format Hunks.
     */
    template <Diffable E>
    struct UniHunkSeq
    {
        using Elem = E;

        std::span<const UniHunk<E>> get() const { return m_inner; }

        std::vector<UniHunk<E>> m_inner;

        bool operator==(const UniHunkSeq&) const
            requires TriviallyComparable<E>
        = default;
    };
}

#endif /* end of include guard: DTL_MODERN_COMMON_HPP */
