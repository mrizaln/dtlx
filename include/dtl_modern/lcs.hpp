#ifndef DTL_MODERN_LCS_HPP
#define DTL_MODERN_LCS_HPP

#include "dtl_modern/common.hpp"

#include <span>

namespace dtl_modern
{
    /**
     * @brief Longest Common Subsequence class
     */
    template <Comparable E>
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

    private:
        std::vector<Elem> m_sequence;
    };
};

#endif /* end of include guard: DTL_MODERN_LCS_HPP */
