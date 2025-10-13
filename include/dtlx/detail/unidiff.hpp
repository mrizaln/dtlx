#ifndef DTLX_DETAIL_UNIDIFF_HPP
#define DTLX_DETAIL_UNIDIFF_HPP

#include "dtlx/common.hpp"
#include "dtlx/constants.hpp"
#include "dtlx/lcs.hpp"
#include "dtlx/ses.hpp"

#include <algorithm>
#include <utility>
#include <vector>

namespace dtlx::detail
{
    /**
     * @brief The result of unidiff algorithm.
     */
    template <Diffable E>
    struct [[nodiscard]] UniDiffResult
    {
        UniHunkSeq<E> uni_hunks;
        Lcs<E>        lcs;
        Ses<E>        ses;
        i64           edit_distance = 0;

        bool operator==(const UniDiffResult&) const
            requires TriviallyComparable<E>
        = default;
    };

    template <Diffable E, std::ranges::range... Srcs>
        requires (std::same_as<RangeElem<Srcs>, SesElem<E>> and ...)
    void inline extend_ses_vec(std::vector<SesElem<E>>& dest, Srcs&&... srcs)
    {
        auto insert = [&dest](auto&& src) { dest.insert(dest.end(), src.begin(), src.end()); };
        (insert(srcs), ...);
    }

    template <Diffable E>
    UniHunkSeq<E> unidiff(const Ses<E>& ses)
    {
        auto hunks = UniHunkSeq<E>{};

        i64 l_cnt  = 1;
        i64 middle = 0;

        bool is_middle = false;
        bool is_after  = false;

        auto adds    = std::vector<SesElem<E>>{};
        auto deletes = std::vector<SesElem<E>>{};

        const auto ses_seq = ses.get();
        const i64  length  = ses_seq.size();

        // scratch hunk (will be modified and sometimes moved/reset in the for loop below)
        auto hunk = UniHunk<E>{
            .a             = 0,
            .b             = 0,
            .c             = 0,
            .d             = 0,
            .common_0      = {},
            .common_1      = {},
            .change        = {},
            .inc_dec_count = 0,
        };

        for (auto it = ses_seq.begin(); it != ses_seq.end(); ++it, ++l_cnt) {
            switch (it->info.type) {
            case SesEdit::Add: {
                middle = 0;
                ++hunk.inc_dec_count;

                adds.push_back(*it);

                if (not is_middle) {
                    is_middle = true;
                }
                ++hunk.d;

                if (l_cnt >= length) {
                    extend_ses_vec(hunk.change, deletes, adds);
                    is_after = true;
                }
            } break;
            case SesEdit::Delete: {
                middle = 0;
                --hunk.inc_dec_count;

                deletes.push_back(*it);

                if (not is_middle) {
                    is_middle = true;
                }
                ++hunk.b;

                if (l_cnt >= length) {
                    extend_ses_vec(hunk.change, deletes, adds);
                    is_after = true;
                }
            } break;
            case SesEdit::Common: {
                ++hunk.b;
                ++hunk.d;

                if (hunk.common_1.empty() and adds.empty() and deletes.empty() and hunk.change.empty()) {
                    if (hunk.common_0.size() < constants::unidiff_context_size) {
                        if (hunk.a == 0 and hunk.c == 0) {
                            const auto& info = it->info;
                            if (not ses.is_swapped()) {
                                hunk.a = info.index_before;
                                hunk.c = info.index_after;
                            } else {
                                hunk.a = info.index_after;
                                hunk.c = info.index_before;
                            }
                        }
                        hunk.common_0.push_back(*it);
                    } else {
                        std::ranges::rotate(hunk.common_0, hunk.common_0.begin() + 1);
                        hunk.common_0.pop_back();
                        hunk.common_0.push_back(*it);
                        ++hunk.a;
                        ++hunk.c;
                        --hunk.b;
                        --hunk.d;
                    }
                }

                if (is_middle and not is_after) {
                    ++middle;
                    extend_ses_vec(hunk.change, deletes, adds);
                    hunk.change.push_back(*it);

                    if (static_cast<u64>(middle) >= constants::unidiff_separate_size or l_cnt >= length) {
                        is_after = true;
                    }

                    adds.clear();
                    deletes.clear();
                }
            } break;
            default:
                [[unlikely]];
                // no through
            }

            // compose unified format hunk
            if (is_after and not hunk.change.empty()) {
                auto cit = it;
                u64  cnt = 0;

                for (u64 i = 0; i < constants::unidiff_separate_size and (cit != ses_seq.end()); ++i, ++cit) {
                    if (cit->info.type == SesEdit::Common) {
                        ++cnt;
                    }
                }

                if (cnt < constants::unidiff_separate_size and l_cnt < length) {
                    middle   = 0;
                    is_after = false;
                    continue;
                }

                if (auto c0_size = hunk.common_0.size(); c0_size >= constants::unidiff_separate_size) {
                    std::ranges::rotate(
                        hunk.common_0, hunk.common_0.begin() + c0_size - constants::unidiff_separate_size
                    );

                    for (u64 i = 0; i < c0_size - constants::unidiff_separate_size; ++i) {
                        hunk.common_0.pop_back();
                    }

                    hunk.a += c0_size - constants::unidiff_separate_size;
                    hunk.c += c0_size - constants::unidiff_separate_size;
                }

                if (hunk.a == 0) {
                    ++hunk.a;
                }
                if (hunk.c == 0) {
                    ++hunk.c;
                }

                if (ses.is_swapped()) {
                    std::swap(hunk.a, hunk.c);
                }

                hunks.inner.push_back(std::exchange(hunk, {}));

                is_middle = false;
                is_after  = false;

                adds.clear();
                deletes.clear();
            }
        }

        return hunks;
    }
}

#endif /* end of include guard: DTLX_DETAIL_UNIDIFF_HPP */
