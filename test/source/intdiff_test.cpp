#include "helper.hpp"

#include <dtl_modern/dtl_modern.hpp>
#include <dtl.hpp>

#include <boost/ut.hpp>

namespace ut = boost::ut;

template <typename E>
struct DtlOldResult
{
    std::vector<dtl::uniHunk<std::pair<E, dtl::elemInfo>>> m_hunks;
    dtl::Lcs<E>                                            m_lcs;
    dtl::Ses<E>                                            m_ses;
    std::ptrdiff_t                                         m_edit_dist;
};

template <typename E>
struct DtlNewResult
{
    dtl_modern::UniHunkSeq<E> m_hunks;
    dtl_modern::Lcs<E>        m_lcs;
    dtl_modern::Ses<E>        m_ses;
    std::ptrdiff_t            m_edit_dist;
};

template <typename S1, dtl_modern::ComparableRanges<S1> S2>
struct SeqPair
{
    S1 s1;
    S2 s2;
};

const auto g_seq_pairs = std::tuple{
    SeqPair{
        .s1 = std::array<int, 0>{},
        .s2 = std::array<int, 0>{},
    },
    SeqPair{
        .s1 = std::vector<int>{},
        .s2 = std::vector<int>{ 1 },
    },
    SeqPair{
        .s1 = std::vector<int>{ 1 },
        .s2 = std::vector<int>{},
    },
    SeqPair{
        .s1 = std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
        .s2 = std::vector{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 },
    },
    SeqPair{
        .s1 = std::vector{ 1, 2, 3, 4, 5 },
        .s2 = std::array{ 3, 5, 1, 4, 5 },
    },
};

template <typename R1, typename R2>
std::pair<DtlOldResult<int>, DtlNewResult<int>> do_diff(R1&& r1, R2&& r2)
{
    // old
    // ---
    // both seq needs to be the same type
    auto vec1 = std::vector(std::begin(r1), std::end(r1));
    auto vec2 = std::vector(std::begin(r2), std::end(r2));

    auto diff = dtl::Diff<int>{ vec1, vec2 };

    diff.compose();
    diff.composeUnifiedHunks();
    // ---

    // new
    // ---
    auto [hunks_new, lcs_new, ses_new, edit_dist_new] = dtl_modern::unidiff(r1, r2);
    // ---

    return {
        DtlOldResult<int>{
            .m_hunks     = diff.getUniHunks(),
            .m_lcs       = diff.getLcs(),
            .m_ses       = diff.getSes(),
            .m_edit_dist = diff.getEditDistance(),
        },
        DtlNewResult<int>{
            .m_hunks     = std::move(hunks_new),
            .m_lcs       = std::move(lcs_new),
            .m_ses       = std::move(ses_new),
            .m_edit_dist = edit_dist_new,
        },
    };
}

int main()
{
    using ut::expect, ut::that;
    using namespace ut::literals;
    using namespace ut::operators;

    helper::for_each_tuple(g_seq_pairs, [&]<typename T1, typename T2>(const SeqPair<T1, T2>& pair) {
        auto&& [p1, p2]         = pair;
        auto [res_old, res_new] = do_diff(p1, p2);

        "intdiff test the raw output of the diff"_test = [&] {
            expect(that % res_old.m_edit_dist == res_new.m_edit_dist) << "Edit distance not equal";
            helper::ut::ses_equals(res_old.m_ses, res_new.m_ses);
            helper::ut::lcs_equals(res_old.m_lcs, res_new.m_lcs);
            helper::ut::uni_hunks_equals(res_old.m_hunks, res_new.m_hunks);
        };

        "intdiff test the formatted output of the diff"_test = [&] {
            auto hunks_str_old = helper::stringify_hunks_old(res_old.m_hunks);
            auto ses_str_old   = helper::stringify_ses_old(res_old.m_ses);

            auto hunks_str_new = fmt::to_string(dtl_modern::extra::display(res_new.m_hunks));
            auto ses_str_new   = fmt::to_string(dtl_modern::extra::display(res_new.m_ses));

            expect(hunks_str_old == hunks_str_new);
            expect(ses_str_old == ses_str_new);
        };
    });
}
