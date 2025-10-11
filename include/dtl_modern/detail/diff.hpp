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
        Lcs<E> lcs;
        Ses<E> ses;
        i64    edit_distance = 0;

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
                Subrange1 new_A;
                Subrange2 new_B;
                i64       new_ox;
                i64       new_oy;
            };

            static RecordSequenceStatus complete() { return RecordSequenceStatus{ Complete{} }; }

            static RecordSequenceStatus incomplete(Subrange1 new_A, Subrange2 new_B, i64 new_ox, i64 new_oy)
            {
                return RecordSequenceStatus{ Incomplete{ new_A, new_B, new_ox, new_oy } };
            }

            bool is_complete() const noexcept { return std::holds_alternative<Complete>(inner); }
            bool is_incomplete() const noexcept { return not is_complete(); }

            Incomplete&& as_incomplete() && { return std::get<Incomplete>(std::move(inner)); }

            std::variant<Complete, Incomplete> inner;
        };

        Diff(R1 lhs, R2 rhs, Comp comp)
            : m_comp{ comp }
        {
            init_state(lhs, rhs, 0, 0);
        }

        DiffResult<E> diff(u64 max_coords_size, bool reserve_first)
        {
            auto furthest_points = std::vector<i64>(static_cast<u64>(m_M + m_N + 3), -1);

            auto path                = EditPath{ static_cast<u64>(m_M + m_N + 3), -1 };
            auto path_coords         = EditPathCoords{};
            auto reduced_path_coords = EditPathCoords<Point>{};

            if (reserve_first and max_coords_size != constants::no_limit) {
                path_coords.inner.reserve(max_coords_size);
            }

            auto lcs           = Lcs<E>{};
            auto ses           = Ses<E>{ Swap };
            auto edit_distance = i64{ 0 };

            while (true) {
                edit_distance += record_edits(furthest_points, path, path_coords, max_coords_size);

                auto r = path.at(m_delta + m_offset);
                while (r != -1) {
                    auto [x, y, k] = path_coords.at(r);
                    reduced_path_coords.add(Point{ x, y });
                    r = k;
                }

                auto status = record_sequence(lcs, ses, reduced_path_coords);
                if (status.is_complete()) {
                    break;
                }

                auto [new_A, new_B, new_ox, new_oy] = std::move(status).as_incomplete();
                init_state(new_A, new_B, new_ox, new_oy);

                furthest_points.clear();
                furthest_points.resize(static_cast<u64>(m_M + m_N + 3), -1);
                std::ranges::fill(path.inner, -1);

                path_coords.clear();
                reduced_path_coords.clear();
            }

            return {
                .lcs           = std::move(lcs),
                .ses           = std::move(ses),
                .edit_distance = edit_distance,
            };
        }

        i64 edit_distance() const
        {
            auto furthest_points = std::vector<i64>(static_cast<u64>(m_M + m_N + 3), -1);
            return calculate_edit_distance(furthest_points);
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

        i64 snake(i64 k, i64 above, i64 below) const
        {
            auto y = std::max(above, below);
            auto x = y - k;

            using I1 = std::ranges::range_difference_t<Subrange1>;
            using I2 = std::ranges::range_difference_t<Subrange2>;

            while (x < m_M and y < m_N and compare(m_A[static_cast<I1>(x)], m_B[static_cast<I2>(y)])) {
                ++x;
                ++y;
            }

            return y;
        }

        i64 snake_record(EditPath& path, EditPathCoords<KPoint>& path_coords, i64 k, i64 above, i64 below)
            const
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

            path.at(k + m_offset) = static_cast<i64>(path_coords.size());
            path_coords.add({ x, y, r });

            return y;
        }

        i64 calculate_edit_distance(std::span<i64> furthest_points) const
        {
            auto fp = [&](i64 loc) -> i64& { return furthest_points[static_cast<u64>(loc + m_offset)]; };

            i64 p = -1;
            do {
                ++p;

                for (i64 k = -p; k <= m_delta - 1; ++k) {
                    fp(k) = snake(k, fp(k - 1) + 1, fp(k + 1));
                }
                for (i64 k = m_delta + p; k >= m_delta + 1; --k) {
                    fp(k) = snake(k, fp(k - 1) + 1, fp(k + 1));
                }
                fp(m_delta) = snake(m_delta, fp(m_delta - 1) + 1, fp(m_delta + 1));

            } while (fp(m_delta) != m_N);

            return m_delta + 2 * p;
        }

        i64 record_edits(
            std::span<i64>          furthest_points,
            EditPath&               path,
            EditPathCoords<KPoint>& path_coords,
            u64                     limit
        )
        {
            auto fp = [&](i64 loc) -> i64& { return furthest_points[static_cast<u64>(loc + m_offset)]; };

            i64 p = -1;
            do {
                ++p;

                for (i64 k = -p; k <= m_delta - 1; ++k) {
                    fp(k) = snake_record(path, path_coords, k, fp(k - 1) + 1, fp(k + 1));
                }
                for (i64 k = m_delta + p; k >= m_delta + 1; --k) {
                    fp(k) = snake_record(path, path_coords, k, fp(k - 1) + 1, fp(k + 1));
                }
                fp(m_delta) = snake_record(path, path_coords, m_delta, fp(m_delta - 1) + 1, fp(m_delta + 1));

            } while (fp(m_delta) != m_N and path_coords.size() < limit);

            return m_delta + 2 * p;
        }

        RecordSequenceStatus record_sequence(
            Lcs<E>&                      lcs,
            Ses<E>&                      ses,
            const EditPathCoords<Point>& path_coords
        ) const
        {
            auto x = m_A.begin();
            auto y = m_B.begin();

            i64 x_idx = 1;
            i64 y_idx = 1;

            i64 px_idx = 0;
            i64 py_idx = 0;

            for (size_t i = path_coords.size(); i > 0; --i) {
                auto idx = i - 1;

                while (px_idx < path_coords[idx].x or py_idx < path_coords[idx].y) {
                    auto lhs = path_coords[idx].y - path_coords[idx].x;
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
