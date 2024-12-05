#ifndef DTL_MODERN_SES_HPP
#define DTL_MODERN_SES_HPP

#include "dtl_modern/common.hpp"
#include <span>

namespace dtl_modern
{
    /**
     * @brief Shortest Edit Script
     */
    template <Comparable E>
    class Ses
    {
    public:
        using Elem = E;

        Ses() = default;

        // clang-format off
        bool is_only_add()           const { return m_only_add; }
        bool is_only_delete()        const { return m_only_delete; }
        bool is_only_copy()          const { return m_only_copy; }
        bool is_change()             const { return not is_only_copy(); }
        bool is_only_one_operation() const { return is_only_add() or is_only_delete() or is_only_copy(); }
        // clang-format on

        std::span<const SesElem<Elem>> get() const { return m_sequence; }

        void add(Elem elem, i64 index_before, i64 index_after, SesEdit type)
        {
            auto info = ElemInfo{
                .m_index_before = index_before,
                .m_index_after  = index_after,
                .m_type         = type,
            };

            m_sequence.emplace_back(std::move(elem), std::move(info));

            switch (type) {
            case SesEdit::Delete: {
                m_only_copy = false;
                m_only_add  = false;
            } break;
            case SesEdit::Common: {
                m_only_add    = false;
                m_only_delete = false;
            } break;
            case SesEdit::Add: {
                m_only_delete = false;
                m_only_copy   = false;
            } break;
            }
        }

        bool operator==(const Ses&) const = default;

    private:
        std::vector<SesElem<Elem>> m_sequence;

        bool m_only_add    = true;
        bool m_only_delete = true;
        bool m_only_copy   = true;
    };
}

#endif /* end of include guard: DTL_MODERN_SES_HPP */
