#ifndef DTLX_LCS_HPP
#define DTLX_LCS_HPP

#include "dtlx/concepts.hpp"

#include <span>
#include <vector>

namespace dtlx
{
    /**
     * @brief Longest Common Subsequence class
     */
    template <Diffable E>
    class Lcs
    {
    public:
        using Elem = E;

        Lcs() = default;

        /**
         * @brief Get the view of the sequence.
         */
        std::span<const Elem> get() const { return m_sequence; }

        /**
         * @brief Add a new element to the sequence.
         */
        template <typename... Args>
        void add(Args&&... args)
        {
            m_sequence.emplace_back(std::forward<Args>(args)...);
        }

        bool operator==(const Lcs&) const
            requires TriviallyComparable<Elem>
        = default;

    private:
        std::vector<Elem> m_sequence;
    };
};

#endif /* end of include guard: DTLX_LCS_HPP */
