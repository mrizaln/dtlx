#ifndef DTL_MODERN_DETAIL_UNIDIFF_HPP
#define DTL_MODERN_DETAIL_UNIDIFF_HPP

#include "dtl_modern/common.hpp"
#include "dtl_modern/constants.hpp"
#include "dtl_modern/ses.hpp"

#include <algorithm>
#include <utility>
#include <vector>

namespace dtl_modern::detail
{
    template <Diffable E, std::ranges::range... Srcs>
        requires (std::same_as<std::ranges::range_value_t<Srcs>, SesElem<E>> and ...)
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
            .m_a             = 0,
            .m_b             = 0,
            .m_c             = 0,
            .m_d             = 0,
            .m_common_0      = {},
            .m_common_1      = {},
            .m_change        = {},
            .m_inc_dec_count = 0,
        };

        for (auto it = ses_seq.begin(); it != ses_seq.end(); ++it, ++l_cnt) {
            switch (it->m_info.m_type) {
            case SesEdit::Add: {
                middle = 0;
                ++hunk.m_inc_dec_count;

                adds.push_back(*it);

                if (not is_middle) {
                    is_middle = true;
                }
                ++hunk.m_d;

                if (l_cnt >= length) {
                    extend_ses_vec(hunk.m_change, deletes, adds);
                    is_after = true;
                }
            } break;
            case SesEdit::Delete: {
                middle = 0;
                --hunk.m_inc_dec_count;

                deletes.push_back(*it);

                if (not is_middle) {
                    is_middle = true;
                }
                ++hunk.m_b;

                if (l_cnt >= length) {
                    extend_ses_vec(hunk.m_change, deletes, adds);
                    is_after = true;
                }
            } break;
            case SesEdit::Common: {
                ++hunk.m_b;
                ++hunk.m_d;

                if (hunk.m_common_1.empty() and adds.empty() and deletes.empty() and hunk.m_change.empty()) {
                    if (hunk.m_common_0.size() < constants::dtl_context_size) {
                        if (hunk.m_a == 0 and hunk.m_c == 0) {
                            const auto& info = it->m_info;
                            if (not ses.is_swapped()) {
                                hunk.m_a = info.m_index_before;
                                hunk.m_c = info.m_index_after;
                            } else {
                                hunk.m_a = info.m_index_after;
                                hunk.m_c = info.m_index_before;
                            }
                        }
                        hunk.m_common_0.push_back(*it);
                    } else {
                        std::ranges::rotate(hunk.m_common_0, hunk.m_common_0.begin() + 1);
                        hunk.m_common_0.pop_back();
                        hunk.m_common_0.push_back(*it);
                        ++hunk.m_a;
                        ++hunk.m_c;
                        --hunk.m_b;
                        --hunk.m_d;
                    }
                }

                if (is_middle and not is_after) {
                    ++middle;
                    extend_ses_vec(hunk.m_change, deletes, adds);
                    hunk.m_change.push_back(*it);

                    if (static_cast<u64>(middle) >= constants::dtl_separate_size or l_cnt >= length) {
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
            if (is_after and not hunk.m_change.empty()) {
                auto cit = it;
                u64  cnt = 0;

                for (u64 i = 0; i < constants::dtl_separate_size and (cit != ses_seq.end()); ++i, ++cit) {
                    if (cit->m_info.m_type == SesEdit::Common) {
                        ++cnt;
                    }
                }

                if (cnt < constants::dtl_separate_size and l_cnt < length) {
                    middle   = 0;
                    is_after = false;
                    continue;
                }

                if (auto c0_size = hunk.m_common_0.size(); c0_size >= constants::dtl_separate_size) {
                    std::ranges::rotate(
                        hunk.m_common_0, hunk.m_common_0.begin() + c0_size - constants::dtl_separate_size
                    );

                    for (u64 i = 0; i < c0_size - constants::dtl_separate_size; ++i) {
                        hunk.m_common_0.pop_back();
                    }

                    hunk.m_a += c0_size - constants::dtl_separate_size;
                    hunk.m_c += c0_size - constants::dtl_separate_size;
                }

                if (hunk.m_a == 0) {
                    ++hunk.m_a;
                }
                if (hunk.m_c == 0) {
                    ++hunk.m_c;
                }

                if (ses.is_swapped()) {
                    std::swap(hunk.m_a, hunk.m_c);
                }

                hunks.m_inner.push_back(std::exchange(hunk, {}));

                is_middle = false;
                is_after  = false;

                adds.clear();
                deletes.clear();
            }
        }

        return hunks;
    }
}

#endif /* end of include guard: DTL_MODERN_DETAIL_UNIDIFF_HPP */
