#ifndef DTL_MODERN_CONCEPTS_HPP
#define DTL_MODERN_CONCEPTS_HPP

#include <concepts>
#include <ranges>

namespace dtl_modern
{
    template <std::ranges::range R>
    using RangeElem = std::ranges::range_value_t<R>;

    template <typename Elem>
    concept Diffable = std::copyable<Elem>;

    template <typename Comp, typename Elem>
    concept Comparator = requires {
        requires std::invocable<Comp, const Elem&, const Elem&>;
        requires std::same_as<std::invoke_result_t<Comp, const Elem&, const Elem&>, bool>;
    };

    template <typename Elem, typename Comp>
    concept Comparable = Comparator<Comp, Elem>;

    template <typename Elem>
    concept TriviallyComparable = requires (const Elem& e1, const Elem& e2) {
        { e1 == e2 } -> std::same_as<bool>;
    };

    template <typename... Rs>
    concept RandomAccessRange = (std::ranges::random_access_range<Rs> and ...);

    template <typename... Rs>
    concept SizedRange = (std::ranges::sized_range<Rs> and ...);

    template <typename Range, typename Comp>
    concept ComparableRange = requires {
        requires RandomAccessRange<Range>;
        requires SizedRange<Range>;

        requires Diffable<RangeElem<Range>>;
        requires Comparable<RangeElem<Range>, Comp>;
    };

    template <typename R1, typename R2, typename Comp>
    concept ComparableRanges = requires {
        requires RandomAccessRange<R1, R2>;
        requires SizedRange<R1, R2>;

        requires std::same_as<std::ranges::range_value_t<R1>, std::ranges::range_value_t<R2>>;

        requires Diffable<RangeElem<R1>>;
        requires Comparable<RangeElem<R1>, Comp>;
    };
}

#endif /* end of include guard: DTL_MODERN_CONCEPTS_HPP */
