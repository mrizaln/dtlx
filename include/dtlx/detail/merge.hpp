#ifndef DTLX_DETAIL_MERGE_HPP
#define DTLX_DETAIL_MERGE_HPP

#include "dtlx/concepts.hpp"
#include "dtlx/detail/diff.hpp"

#include <variant>

namespace dtlx::detail
{
    /**
     * @brief The result of merge algorithm.
     *
     * The type wraps a `std::variant`, should you visit them, use the member `visit` function or direclty use
     * std::visit on the underlying value.
     */
    template <Diffable E, template <typename... EInner> typename Cont>
    struct [[nodiscard]] MergeResult
    {
        // clang-format off
        struct Conflict {};
        struct Merge    { Cont<E> value ; };

        bool is_conflict() const { return std::holds_alternative<Conflict>(variant); }
        bool is_merge()    const { return not is_conflict(); }

        Merge&& as_merge() && { return std::get<Merge>(std::move(variant)); }

        decltype(auto) visit(auto&& v)       { return std::visit(std::forward<decltype(v)>(v), variant); }
        decltype(auto) visit(auto&& v) const { return std::visit(std::forward<decltype(v)>(v), variant); }
        // clang-format on

        using Variant = std::variant<Conflict, Merge>;

        Variant variant;
    };

    template <Diffable E, template <typename... EInner> typename Cont>
    static MergeResult<E, Cont> create_conflict()
    {
        return MergeResult<E, Cont>{ typename MergeResult<E, Cont>::Conflict{} };
    }

    template <Diffable E, template <typename... EInner> typename Cont>
    static MergeResult<E, Cont> create_merge(Cont<E>&& merged)
    {
        return MergeResult<E, Cont>{ typename MergeResult<E, Cont>::Merge{ std::move(merged) } };
    }

    enum class TrivialMergeKind
    {
        AEqualsBEqualsC,
        AEqualsB,
        BEqualsC
    };

    template <Diffable E>
    std::optional<TrivialMergeKind> trivially_mergeable(
        const DiffResult<E>& diff_ba,
        const DiffResult<E>& diff_bc
    )
    {
        auto ed_ba = diff_ba.edit_distance;
        auto ed_bc = diff_bc.edit_distance;

        // clang-format off
        if (ed_ba == 0 and ed_bc == 0)  return TrivialMergeKind::AEqualsBEqualsC;
        if (ed_ba == 0)                 return TrivialMergeKind::AEqualsB;
        if (ed_bc == 0)                 return TrivialMergeKind::BEqualsC;
        // clang-format on

        return std::nullopt;
    }

    template <Diffable E, template <typename... EInner> typename Container, Comparator<E> Comp>
    MergeResult<E, Container> merge(const DiffResult<E>& diff_ba, const DiffResult<E>& diff_bc, Comp comp)
    {
        // if a merge is trivial to begin with, this function should not be called
        assert(trivially_mergeable(diff_ba, diff_bc) == std::nullopt);

        auto ses_ba = diff_ba.ses.get();
        auto ses_bc = diff_bc.ses.get();

        auto ba_it = ses_ba.begin();
        auto bc_it = ses_bc.begin();

        auto ba_end = [&] { return ba_it == ses_ba.end(); };
        auto bc_end = [&] { return bc_it == ses_bc.end(); };

        auto merged = Container<E>{};

        while (not ba_end() or not bc_end()) {
            while (true) {
                const auto& [ba_elem, ba_info] = *ba_it;
                const auto& [bc_elem, bc_info] = *bc_it;

                if (ba_end() or bc_end()) {
                    break;
                }

                auto same_elem      = comp(ba_elem, bc_elem);
                auto edit_is_common = ba_info.type == SesEdit::Common and bc_info.type == SesEdit::Common;

                if (not same_elem or not edit_is_common) {
                    break;
                }

                if (not ba_end()) {
                    merged.push_back(ba_elem);
                } else if (not bc_end()) {
                    merged.push_back(bc_elem);
                }

                ++ba_it;
                ++bc_it;
            }

            if (ba_end() or bc_end()) {
                break;
            }

            const auto& [ba_elem, ba_info] = *ba_it;
            const auto& [bc_elem, bc_info] = *bc_it;

            switch (ba_info.type) {
            case SesEdit::Common: {
                switch (bc_info.type) {
                case SesEdit::Delete: {
                    ++ba_it;
                    ++bc_it;
                } break;
                case SesEdit::Add: {
                    merged.push_back(bc_elem);
                    ++bc_it;
                } break;
                case SesEdit::Common: {
                    [[unlikely]];
                    assert(false and "unreachable");
                } break;
                }
            } break;
            case SesEdit::Delete: {
                switch (bc_info.type) {
                case SesEdit::Common: {
                    ++ba_it;
                    ++bc_it;
                } break;
                case SesEdit::Delete: {
                    if (comp(ba_elem, bc_elem)) {
                        ++ba_it;
                        ++bc_it;
                    } else {
                        return create_conflict<E, Container>();
                    }
                } break;
                case SesEdit::Add: {
                    return create_conflict<E, Container>();
                } break;
                }
            } break;
            case SesEdit::Add: {
                switch (bc_info.type) {
                case SesEdit::Common: {
                    merged.push_back(ba_elem);
                    ++ba_it;
                } break;
                case SesEdit::Delete: {
                    return create_conflict<E, Container>();
                } break;
                case SesEdit::Add: {
                    if (comp(ba_elem, bc_elem)) {
                        merged.push_back(ba_elem);
                        ++ba_it;
                        ++bc_it;
                    } else {
                        return create_conflict<E, Container>();
                    }
                } break;
                }
            } break;
            }
        }

        if (ba_end()) {
            while (not bc_end()) {
                const auto& [bc_elem, bc_info] = *bc_it;
                if (bc_info.type == SesEdit::Add) {
                    merged.push_back(bc_elem);
                }
                ++bc_it;
            }
        } else if (bc_end()) {
            while (not ba_end()) {
                const auto& [ba_elem, ba_info] = *ba_it;
                if (ba_info.type == SesEdit::Add) {
                    merged.push_back(ba_elem);
                }
                ++ba_it;
            }
        }

        return create_merge<E, Container>(std::move(merged));
    }
}

#endif /* end of include guard: DTLX_DETAIL_MERGE_HPP */
