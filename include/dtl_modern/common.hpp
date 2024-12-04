#ifndef DTL_MODERN_COMMON_HPP
#define DTL_MODERN_COMMON_HPP

#include <cassert>
#include <concepts>
#include <cstdint>
#include <ranges>
#include <vector>

namespace dtl_modern
{
    template <typename Elem>
    concept Comparable = requires (const Elem& e1, const Elem& e2) {
        { e1 == e2 } -> std::same_as<bool>;
    };

    template <typename Range, typename Elem>
    concept ComparableRange = requires {
        requires std::ranges::random_access_range<Range>;
        requires std::ranges::sized_range<Range>;

        requires std::same_as<std::ranges::range_value_t<Range>, Elem>;

        requires Comparable<Elem>;
    };

    template <typename R1, typename R2>
    concept ComparableRanges = requires {
        requires ComparableRange<R1, std::ranges::range_value_t<R1>>;
        requires ComparableRange<R2, std::ranges::range_value_t<R2>>;

        requires std::same_as<std::ranges::range_value_t<R1>, std::ranges::range_value_t<R2>>;
    };

    using i64 = std::int64_t;
    using u64 = std::uint64_t;

    /**
     * @brief Type of edit for SES
     */
    enum class SesEdit : std::int32_t
    {
        Delete = -1,
        Common = 0,
        Add    = 1,
    };

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
     * @brief Info for Unified Format
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
     * @struct Point
     * @brief Coordinate for registering route
     */
    struct KPoint
    {
        i64 m_x;
        i64 m_y;
        i64 m_k;    // vertex
    };

    struct Point
    {
        i64 m_x;
        i64 m_y;
    };

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
    };

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
    };

    template <Comparable E>
    struct SesElem
    {
        using Elem = E;

        Elem     m_elem;
        ElemInfo m_info;
    };

    /**
     * @brief Structure of Unified Format Hunk
     */
    template <Comparable E>
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
    };

    template <Comparable E>
    struct UniHunkSeq
    {
        using Elem = E;

        std::span<const UniHunk<E>> get() const { return m_inner; }

        std::vector<UniHunk<E>> m_inner;
    };
}

#endif /* end of include guard: DTL_MODERN_COMMON_HPP */
