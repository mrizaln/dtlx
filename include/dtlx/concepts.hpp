#ifndef DTLX_CONCEPTS_HPP
#define DTLX_CONCEPTS_HPP

#include <concepts>
#include <ranges>

namespace dtlx
{
    /**
     * @brief Template alias helper for getting the element type of a range.
     */
    template <std::ranges::range R>
    using RangeElem = std::ranges::range_value_t<R>;

    /**
     * @brief Fundamental constraint for types that can be diffed.
     */
    template <typename Elem>
    concept Diffable = std::copyable<Elem>;

    /**
     * @brief Constraint for function or function-like object that can be used for comparison.
     *
     * @tparam Comp The comparison function type.
     * @tparam Elem The element type to be compared.
     */
    template <typename Comp, typename Elem>
    concept Comparator = requires (Comp comp, const Elem e1, const Elem e2) {
        { comp(e1, e2) } -> std::same_as<bool>;
    };

    /**
     * @brief Constraint for types that can be compared using the `Comp` function.
     *
     * @tparam Elem The element type to be compared.
     * @tparam Comp The comparison function type.
     *
     * This concept is the swapped argument version of `Comparator`: `Comparator<Comp, Elem>`.
     */
    template <typename Elem, typename Comp>
    concept Comparable = Comparator<Comp, Elem>;

    /**
     * @brief Specifies that type type can be compared using `operator==`.
     */
    template <typename Elem>
    concept TriviallyComparable = std::equality_comparable<Elem>;

    /**
     * @brief Specifies that the types is a range that can be randomly accessed.
     */
    template <typename... Rs>
    concept RandomAccessRange = (std::ranges::random_access_range<Rs> and ...);

    /**
     * @brief Ranges that has a known size.
     */
    template <typename... Rs>
    concept SizedRange = (std::ranges::sized_range<Rs> and ...);

    /**
     * @brief Ranges that can be compared with a `Comp` function and have `Diffable` elements.
     *
     * @tparam Range The range type.
     * @tparam Comp The comparison function type.
     */
    template <typename Range, typename Comp>
    concept ComparableRange = requires {
        requires RandomAccessRange<Range>;
        requires SizedRange<Range>;

        requires Diffable<RangeElem<Range>>;
        requires Comparable<RangeElem<Range>, Comp>;
    };

    /**
     * @brief Ranges that can be compared with a `Comp` function and have `Diffable` elements.
     *
     * @tparam R1 The first range type.
     * @tparam R2 The second range type.
     * @tparam Comp The comparison function type.
     */
    template <typename R1, typename R2, typename Comp>
    concept ComparableRanges = requires {
        requires RandomAccessRange<R1, R2>;
        requires SizedRange<R1, R2>;

        requires std::same_as<RangeElem<R1>, RangeElem<R2>>;

        requires Diffable<RangeElem<R1>>;
        requires Comparable<RangeElem<R1>, Comp>;
    };
}

#endif /* end of include guard: DTLX_CONCEPTS_HPP */
