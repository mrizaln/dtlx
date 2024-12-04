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

        Ses(bool move_del)
            : m_deletes_first{ move_del }
        {
        }

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

            auto ses_elem = SesElem{
                .m_elem = std::move(elem),
                .m_info = info,
            };

            if (not m_deletes_first) {
                m_sequence.push_back(ses_elem);
            }

            switch (type) {
            case SesEdit::Delete: {
                m_only_copy = false;
                m_only_add  = false;
                if (m_deletes_first) {
                    m_sequence.insert(m_sequence.begin() + m_next_delete_index, std::move(ses_elem));
                    ++m_next_delete_index;
                }
            } break;
            case SesEdit::Common: {
                m_only_add    = false;
                m_only_delete = false;
                if (m_deletes_first) {
                    m_sequence.push_back(std::move(ses_elem));
                    m_next_delete_index = m_sequence.size();
                }
            } break;
            case SesEdit::Add: {
                m_only_delete = false;
                m_only_copy   = false;
                if (m_deletes_first) {
                    m_sequence.push_back(std::move(ses_elem));
                }
            } break;
            }
        }

    private:
        std::vector<SesElem<Elem>> m_sequence;

        std::size_t m_next_delete_index = 0;

        bool m_only_add    = true;
        bool m_only_delete = true;
        bool m_only_copy   = true;

        bool m_deletes_first = false;
    };
}

#endif /* end of include guard: DTL_MODERN_SES_HPP */
