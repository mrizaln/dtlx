#ifndef DTLX_DETAIL_PATCH_HPP
#define DTLX_DETAIL_PATCH_HPP

#include "dtlx/common.hpp"
#include "dtlx/ses.hpp"

#include <list>

namespace dtlx::detail
{
    template <template <typename... Inner> typename Container, Diffable E, std::ranges::range R>
        requires std::same_as<RangeElem<R>, E>
    Container<E> patch(R&& range, const Ses<E>& ses)
    {
        // auto result  = Container<E>(std::ranges::begin(range), std::ranges::end(range));    // copies
        // original u64  res_idx = 0;

        auto list    = std::list<E>(std::ranges::begin(range), std::ranges::end(range));
        auto list_it = list.begin();

        for (const auto& [elem, info] : ses.get()) {
            switch (info.type) {
            case SesEdit::Add: {
                list.insert(list_it, elem);
                // ++res_idx;
            } break;
            case SesEdit::Delete: {
                list_it = list.erase(list_it);
            } break;
            case SesEdit::Common: {
                ++list_it;
            } break;
            }
        }

        return Container<E>(list.begin(), list.end());
    }
}

#endif /* end of include guard: DTLX_DETAIL_PATCH_HPP */
