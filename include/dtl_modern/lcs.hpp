#ifndef DTL_MODERN_LCS_HPP
#define DTL_MODERN_LCS_HPP

#include "dtl_modern/concepts.hpp"

#include <span>
#include <vector>

namespace dtl_modern
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

        std::span<const Elem> get() const { return m_sequence; }

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

#endif /* end of include guard: DTL_MODERN_LCS_HPP */
