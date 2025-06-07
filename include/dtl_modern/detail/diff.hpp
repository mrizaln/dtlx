#ifndef DTL_MODERN_DETAIL_DIFF_HPP
#define DTL_MODERN_DETAIL_DIFF_HPP

#include "dtl_modern/common.hpp"
#include "dtl_modern/constants.hpp"
#include "dtl_modern/lcs.hpp"
#include "dtl_modern/ses.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <variant>

namespace dtl_modern::detail
{
    /**
     * @struct DiffResult
     *
     * @brief The result of the diff algorithm.
     */
    template <Diffable E>
    struct [[nodiscard]] DiffResult
    {
        Lcs<E> m_lcs;
        Ses<E> m_ses;
        i64    m_edit_distance = 0;

        bool operator==(const DiffResult&) const
            requires TriviallyComparable<E>
        = default;
    };

    // READ: https://publications.mpi-cbg.de/Wu_1990_6334.pdf
    template <Diffable E, Comparator<E> Comp, ComparableRange<Comp> R1, ComparableRange<Comp> R2, bool Swap>
    class Diff
    {
    public:
        using Subrange1 = std::ranges::subrange<std::ranges::iterator_t<R1>, std::ranges::sentinel_t<R1>>;
        using Subrange2 = std::ranges::subrange<std::ranges::iterator_t<R2>, std::ranges::sentinel_t<R2>>;

        struct RecordSequenceStatus
        {
            struct Complete
            {
            };

            struct Incomplete
            {
                Subrange1 m_new_A;
                Subrange2 m_new_B;
                i64       m_new_ox;
                i64       m_new_oy;
            };

            static RecordSequenceStatus complete() { return RecordSequenceStatus{ Complete{} }; }

            static RecordSequenceStatus incomplete(Subrange1 new_A, Subrange2 new_B, i64 new_ox, i64 new_oy)
            {
                return RecordSequenceStatus{ Incomplete{ new_A, new_B, new_ox, new_oy } };
            }

            bool is_complete() const noexcept { return std::holds_alternative<Complete>(m_inner); }
            bool is_incomplete() const noexcept { return not is_complete(); }

            Incomplete&& as_incomplete() && { return std::get<Incomplete>(std::move(m_inner)); }

            std::variant<Complete, Incomplete> m_inner;
        };

        struct RecordCoordinates
        {
            struct ShouldRecord
            {
                EditPath&                    m_path;
                EditPathCoordinates<KPoint>& m_path_coordinates;
                u64                          m_path_coordinates_limit;
            };

            struct DontRecord
            {
                const EditPath& m_path;
            };

            bool should_record() const noexcept { return std::holds_alternative<ShouldRecord>(m_inner); }
            bool dont_record() const noexcept { return not should_record(); }

            const EditPath& path() const
            {
                if (should_record()) {
                    return std::get<ShouldRecord>(m_inner).m_path;
                }
                return std::get<DontRecord>(m_inner).m_path;
            }

            void record(KPoint point, i64 k) const
            {
                if (should_record()) {
                    auto& [path, path_coordinates, path_coordinates_limit] = std::get<ShouldRecord>(m_inner);
                    path.at(k) = static_cast<i64>(path_coordinates.size());
                    path_coordinates.add(point);
                }
            }

            bool is_within_limit() const noexcept
            {
                if (should_record()) {
                    auto& [_, path_coordinates, path_coordinates_limit] = std::get<ShouldRecord>(m_inner);
                    return path_coordinates.size() < path_coordinates_limit;
                }

                return true;
            }

            std::variant<ShouldRecord, DontRecord> m_inner;
        };

        Diff(R1 lhs, R2 rhs, Comp comp)
            : m_comp{ comp }
        {
            init_state(lhs, rhs, 0, 0);
        }

        // FIXME: currently diff can't be segmented for now since it outputs different result for edit
        // distance
        DiffResult<E> diff(u64 max_coordinates_size, bool reserve_first)
        {
            auto furthest_points = std::vector<i64>(static_cast<u64>(m_M + m_N + 3), -1);

            auto path                     = EditPath{ static_cast<u64>(m_M + m_N + 3), -1 };
            auto path_coordinates         = EditPathCoordinates{};
            auto reduced_path_coordinates = EditPathCoordinates<Point>{};

            if (reserve_first and max_coordinates_size != constants::no_limit) {
                path_coordinates.m_inner.reserve(max_coordinates_size);
            }

            auto diff_result = DiffResult<E>{
                .m_lcs           = Lcs<E>{},
                .m_ses           = Ses<E>{ Swap },
                .m_edit_distance = 0,
            };

            auto original_A = m_A;
            auto original_B = m_B;

            auto record_coordinates = RecordCoordinates{ typename RecordCoordinates::ShouldRecord{
                .m_path                   = path,
                .m_path_coordinates       = path_coordinates,
                .m_path_coordinates_limit = max_coordinates_size,
            } };

            while (true) {
                auto edit_distance           = calculate_edit_distance(furthest_points, record_coordinates);
                diff_result.m_edit_distance += edit_distance;

                auto r = path.at(m_delta + m_offset);
                while (r != -1) {
                    auto [x, y, k] = path_coordinates.at(r);
                    reduced_path_coordinates.add(Point{ x, y });
                    r = k;
                }

                auto& [lcs, ses, _] = diff_result;
                auto status         = record_sequence(lcs, ses, reduced_path_coordinates);

                if (status.is_complete()) {
                    break;
                }

                auto [new_A, new_B, new_ox, new_oy] = std::move(status).as_incomplete();
                init_state(new_A, new_B, new_ox, new_oy);

                furthest_points.resize(static_cast<u64>(m_M + m_N + 3));
                std::ranges::fill(furthest_points, -1);
                std::ranges::fill(path.m_inner, -1);

                path_coordinates.clear();
                reduced_path_coordinates.clear();
            }

            init_state(original_A, original_B, 0, 0);

            return diff_result;
        }

        i64 edit_distance() const
        {
            auto furthest_points    = std::vector<i64>(static_cast<u64>(m_M + m_N + 3), -1);
            auto path               = EditPath{ static_cast<u64>(m_M + m_N + 3), -1 };
            auto record_coordinates = RecordCoordinates{ typename RecordCoordinates::DontRecord{
                .m_path = path,
            } };

            return calculate_edit_distance(furthest_points, record_coordinates);
        }

    private:
        bool compare(const E& lhs, const E& rhs) const noexcept
        {
            if constexpr (Swap) {
                return m_comp(rhs, lhs);
            } else {
                return m_comp(lhs, rhs);
            }
        }

        KPoint snake(const EditPath& path, i64 k, i64 above, i64 below) const
        {
            auto r = above > below ? path.at(k - 1 + m_offset) : path.at(k + 1 + m_offset);
            auto y = std::max(above, below);
            auto x = y - k;

            using I1 = std::ranges::range_difference_t<Subrange1>;
            using I2 = std::ranges::range_difference_t<Subrange2>;

            while (x < m_M and y < m_N and compare(m_A[static_cast<I1>(x)], m_B[static_cast<I2>(y)])) {
                ++x;
                ++y;
            }

            return {
                .m_x = x,
                .m_y = y,
                .m_k = r,
            };
        }

        i64 calculate_edit_distance(
            std::span<i64>    furthest_points,    // caching
            RecordCoordinates record_coordinates
        ) const
        {
            const auto& path      = record_coordinates.path();
            auto record_edit_path = [&](KPoint p, i64 k) { record_coordinates.record(p, k + m_offset); };

            auto fp = [&](i64 loc) -> i64& {
                loc += m_offset;
                assert(loc >= 0);
                return furthest_points[static_cast<u64>(loc)];
            };

            i64 p = -1;

            do {
                ++p;

                for (i64 k = -p; k <= m_delta - 1; ++k) {
                    auto point = snake(path, k, fp(k - 1) + 1, fp(k + 1));
                    fp(k)      = point.m_y;
                    record_edit_path(point, k);
                }

                for (i64 k = m_delta + p; k >= m_delta + 1; --k) {
                    auto point = snake(path, k, fp(k - 1) + 1, fp(k + 1));
                    fp(k)      = point.m_y;
                    record_edit_path(point, k);
                }

                auto point  = snake(path, m_delta, fp(m_delta - 1) + 1, fp(m_delta + 1));
                fp(m_delta) = point.m_y;
                record_edit_path(point, m_delta);

                // FIXME: path coordinate limit makes the algorithm outputs different output when used in the
                // diff function even in the original dtl library. I believe this is a bug in the diff
                // function but since I don't understand fully the diff algorithm itself I decided to just
                // disable this check entirely for now thus this might uses much more memory than the original
                // library on long string of data.
                //                                             ↓↓↓↓ this check
            } while (fp(m_delta) != m_N and record_coordinates.is_within_limit());

            return m_delta + 2 * p;
        }

        // FIXME: this function also have different output when the sequence is segemented
        RecordSequenceStatus record_sequence(
            Lcs<E>&                           lcs,
            Ses<E>&                           ses,
            const EditPathCoordinates<Point>& path_coordinates
        ) const
        {
            auto x = m_A.begin();
            auto y = m_B.begin();

            i64 x_idx = 1;
            i64 y_idx = 1;

            i64 px_idx = 0;
            i64 py_idx = 0;

            for (size_t i = path_coordinates.size(); i > 0; --i) {
                auto idx = i - 1;

                while (px_idx < path_coordinates[idx].m_x or py_idx < path_coordinates[idx].m_y) {
                    auto lhs = path_coordinates[idx].m_y - path_coordinates[idx].m_x;
                    auto rhs = py_idx - px_idx;

                    auto cmp = lhs <=> rhs;

                    if (cmp == std::strong_ordering::greater) {
                        if constexpr (not Swap) {
                            ses.add(*y, 0, y_idx + m_oy, SesEdit::Add);
                        } else {
                            ses.add(*y, y_idx + m_oy, 0, SesEdit::Delete);
                        }
                        ++y;
                        ++y_idx;
                        ++py_idx;
                    } else if (cmp == std::strong_ordering::less) {
                        if constexpr (not Swap) {
                            ses.add(*x, x_idx + m_ox, 0, SesEdit::Delete);
                        } else {
                            ses.add(*x, 0, x_idx + m_ox, SesEdit::Add);
                        }
                        ++x;
                        ++x_idx;
                        ++px_idx;
                    } else {
                        if constexpr (not Swap) {
                            lcs.add(*x);
                            ses.add(*x, x_idx + m_ox, y_idx + m_oy, SesEdit::Common);
                        } else {
                            lcs.add(*y);
                            ses.add(*y, y_idx + m_oy, x_idx + m_ox, SesEdit::Common);
                        }
                        ++x;
                        ++y;
                        ++x_idx;
                        ++y_idx;
                        ++px_idx;
                        ++py_idx;
                    }
                }
            }

            if (x_idx > m_M and y_idx > m_N) {
                return RecordSequenceStatus::complete();
            }

            // TODO: implement trivial difference

            auto new_A = Subrange1{ m_A.begin() + x_idx - 1, m_A.end() };
            auto new_B = Subrange2{ m_B.begin() + y_idx - 1, m_B.end() };

            return RecordSequenceStatus::incomplete(new_A, new_B, x_idx - 1, y_idx - 1);
        }

        void init_state(Subrange1 A, Subrange2 B, i64 ox, i64 oy)
        {
            m_A = A;
            m_B = B;

            m_M = static_cast<i64>(std::ranges::size(A));
            m_N = static_cast<i64>(std::ranges::size(B));

            assert(m_M <= m_N);

            m_delta  = m_N - m_M;
            m_offset = m_M + 1;

            m_ox = ox;
            m_oy = oy;
        }

        Subrange1 m_A;
        Subrange2 m_B;

        i64 m_M = 0;
        i64 m_N = 0;

        i64 m_delta  = 0;
        i64 m_offset = 0;

        i64 m_ox = 0;
        i64 m_oy = 0;

        [[no_unique_address]] Comp m_comp;
    };
}

#endif /* end of include guard: DTL_MODERN_DETAIL_DIFF_HPP */
