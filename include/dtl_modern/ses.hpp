#ifndef DTL_MODERN_SES_HPP
#define DTL_MODERN_SES_HPP

#include "dtl_modern/common.hpp"
#include "dtl_modern/concepts.hpp"
#include <span>

namespace dtl_modern
{
    /**
     * @brief Shortest Edit Script
     */
    template <Diffable E>
    class Ses
    {
    public:
        using Elem = E;

        Ses(bool swapped)
            : m_swapped{ swapped }
        {
        }

        bool has_changes() const { return not is_only_copy(); }
        bool is_swapped() const { return m_swapped; }

        bool is_only_add() const { return m_only_add; }
        bool is_only_delete() const { return m_only_delete; }
        bool is_only_copy() const { return m_only_copy; }

        std::optional<SesEdit> is_only_one_operation() const
        {
            if (is_only_add()) {
                return SesEdit::Add;
            } else if (is_only_delete()) {
                return SesEdit::Delete;
            } else if (is_only_copy()) {
                return SesEdit::Common;
            } else {
                return std::nullopt;
            }
        }

        /**
         * @brief Get the view of the sequence.
         */
        std::span<const SesElem<Elem>> get() const { return m_sequence; }

        /**
         * @brief Add a new element to the sequence.
         */
        void add(Elem elem, i64 index_before, i64 index_after, SesEdit type)
        {
            auto info = ElemInfo{
                .index_before = index_before,
                .index_after  = index_after,
                .type         = type,
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

        bool operator==(const Ses&) const
            requires TriviallyComparable<Elem>
        = default;

    private:
        std::vector<SesElem<Elem>> m_sequence;

        bool m_only_add    = true;
        bool m_only_delete = true;
        bool m_only_copy   = true;

        bool m_swapped = false;
    };
}

#endif /* end of include guard: DTL_MODERN_SES_HPP */
