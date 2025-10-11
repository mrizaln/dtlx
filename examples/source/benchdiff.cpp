#include <dtl.hpp>
#include <dtl_modern/dtl_modern.hpp>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <chrono>
#include <random>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

using Clock = std::chrono::steady_clock;
using Ms    = std::chrono::milliseconds;

enum class Lib
{
    Dtl,
    DtlModern,
    Both,
};

struct DiffCase
{
    std::string left;
    std::string right;
};

struct BenchResult
{
    Clock::duration old_diff = {};
    Clock::duration old_dist = {};
    Clock::duration new_diff = {};
    Clock::duration new_dist = {};

    BenchResult operator/(std::size_t divisor) const
    {
        return {
            .old_diff = old_diff / divisor,
            .old_dist = old_dist / divisor,
            .new_diff = new_diff / divisor,
            .new_dist = new_dist / divisor,
        };
    }

    BenchResult& operator+=(const BenchResult& other)
    {
        old_diff += other.old_diff;
        old_dist += other.old_dist;
        new_diff += other.new_diff;
        new_dist += other.new_dist;

        return *this;
    }
};

constexpr auto hello1
    = "aaaaaaaaaaaaa>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      ">>>>>>>aaaadsafabcaaaaaaaaaaaaaaaaaaewaaabdaaaaaabbb"sv;

constexpr auto hello2
    = "aaaaaaaaaaaaaaadasfdsafsadasdafbaaaaaaaaaaaaaaaaaeaaaaaaaaaae&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
      "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&saaabcaaaaaaccc"sv;

void print(const BenchResult& result)
{
    const auto& [old_diff, old_dist, new_diff, new_dist] = result;

    using Ms   = std::chrono::duration<float, std::milli>;
    auto to_ms = [](Clock::duration dur) { return std::chrono::duration_cast<Ms>(dur); };

    auto print_border = []() { fmt::println("{:-^50}", ""); };
    auto print_header = []() { fmt::println("| {:>10} | {:>15} | {:>15} |", "name", "diff", "edit_dist"); };
    auto print_result = [](std::string_view name, float diff, float dist, long diff_d, long dist_d) {
        fmt::println(
            "| {:>10} | {:>7.2f} ({:>4}%) | {:>7.2f} ({:>4}%) |",
            name,
            diff,
            fmt::styled(diff_d, fmt::bg(diff_d > 0 ? fmt::color::orange_red : fmt::color::green)),
            dist,
            fmt::styled(dist_d, fmt::bg(dist_d > 0 ? fmt::color::orange_red : fmt::color::green))
        );
    };

    auto diff_delta = 100 * (new_diff - old_diff) / old_diff;
    auto dist_delta = 100 * (new_dist - old_dist) / old_dist;

    print_border();
    print_header();
    print_border();
    print_result("dtl", to_ms(old_diff).count(), to_ms(old_dist).count(), 0, 0);
    print_result("dtl-modern", to_ms(new_diff).count(), to_ms(new_dist).count(), diff_delta, dist_delta);
    print_border();
}

std::string mutate_string(std::mt19937& rng, std::string_view input, std::size_t mutation_count)
{
    enum class Mutation
    {
        Add,
        Delete,
    };

    auto mut_dist  = std::uniform_int_distribution<int>{ 0, 1 };
    auto char_dist = std::uniform_int_distribution<char>{ 32, 126 };

    auto string = std::string{ input };

    while (mutation_count-- > 0 and not string.empty()) {
        switch (static_cast<Mutation>(mut_dist(rng))) {
        case Mutation::Add: {
            auto len_dist = std::uniform_int_distribution<std::size_t>{ 0u, string.size() };
            string.insert(len_dist(rng), 1, char_dist(rng));
        } break;
        case Mutation::Delete:
            auto len_dist = std::uniform_int_distribution<std::size_t>{ 0u, string.size() - 1 };
            string.erase(len_dist(rng), 1);
        }
    }

    return string;
}

std::string generate_string(std::mt19937& rng, std::size_t max_len)
{
    auto dist   = std::uniform_int_distribution<char>{ 0 };
    auto string = std::string{};

    for (auto i = 0u; i < max_len; ++i) {
        if (auto ch = dist(rng); ch >= 32 and ch < 127) {
            string.push_back(ch);
        }
    }

    return string;
}

std::vector<DiffCase> generate_diff_cases(std::mt19937& rng, std::size_t count)
{
    constexpr std::size_t max_len = 20000;

    auto cases = std::vector<DiffCase>{};

    while (count-- > 0) {
        auto& diff = cases.emplace_back();
        diff.left  = generate_string(rng, max_len);

        auto mutation = std::uniform_int_distribution<std::size_t>{ 0, diff.left.size() }(rng);
        diff.right    = mutate_string(rng, diff.left, mutation);
    }

    return cases;
}

Clock::duration time(std::invocable auto fn)
{
    auto start = Clock::now();
    std::move(fn)();
    return std::chrono::duration_cast<Ms>(Clock::now() - start);
}

long run_old(const DiffCase& diff, bool dist_only)
{
    auto&& [left, right] = diff;
    if (dist_only) {
        auto diff = dtl::Diff<char, std::string>(left, right);
        diff.onOnlyEditDistance();
        diff.compose();
        return diff.getEditDistance();
    } else {
        auto diff = dtl::Diff<char, std::string>(left, right);
        diff.onHuge();
        diff.compose();
        return diff.getEditDistance();
    }
}

long run_new(const DiffCase& diff, bool dist_only)
{
    auto&& [left, right] = diff;
    if (dist_only) {
        return dtl_modern::edit_distance(left, right);
    } else {
        auto [lcs, ses, dist] = dtl_modern::diff(left, right);
        return dist;
    }
}

BenchResult bench(std::span<const DiffCase> cases, std::size_t run_count)
{
    auto total = BenchResult{};

    auto run = [&](auto fn, const DiffCase& diff, bool dist_only) {
        auto duration = Clock::duration::zero();
        for (auto j = 0u; j < run_count; ++j) {
            duration += time([&] { fn(diff, dist_only); });
        }
        duration = duration / run_count;
        return duration.count() > 0 ? duration : Clock::duration{ 1 };    // prevent floating point exception
    };

    for (auto i = 0u; i < cases.size(); ++i) {
        const auto& diff = cases[i];

        auto dist_old = run_old(diff, true);
        auto dist_new = run_new(diff, true);

        auto result = BenchResult{
            run(run_old, diff, false),
            run(run_old, diff, true),
            run(run_new, diff, false),
            run(run_new, diff, true),
        };

        total += result;

        fmt::println("\ncase: {} (edit: {} vs {})", i, dist_old, dist_new);
        print(result);
    }

    return total / cases.size();
}

template <std::integral I>
std::optional<I> to_int(std::string_view str)
{
    auto value     = I{};
    auto [ptr, ec] = std::from_chars(str.begin(), str.end(), value);
    if (ec != std::errc{}) {
        fmt::println("failed to parse string: {}", std::make_error_code(ec).message());
        return std::nullopt;
    } else if (ptr != str.end()) {
        fmt::println("runs constains non-digit: {}", ptr);
        return std::nullopt;
    }
    return value;
}

int main(int argc, char* argv[])
{
    if (argc != 2 and argc != 3 and argc != 4) {
        fmt::println(stderr, "Usage: {} <runs> [cases] [seed]", argv[0]);
        fmt::println(stderr, "");
        fmt::println(stderr, "Options:");
        fmt::println(stderr, "\truns        must be positive integer (required)");
        fmt::println(stderr, "\tcases       the value is 10 by default");
        fmt::println(stderr, "\tseed        the value will be generated randomly if not provided");
        return 1;
    }

    auto runs = to_int<std::size_t>(argv[1]);
    if (not runs) {
        return 1;
    }

    auto num_cases = std::optional<std::size_t>{};
    if (argc >= 3) {
        num_cases = to_int<std::size_t>(argv[2]);
        if (not num_cases) {
            return 1;
        }
    } else {
        num_cases = 10;
    }

    auto seed = std::optional<std::mt19937::result_type>{};
    if (argc >= 4) {
        seed = to_int<std::mt19937::result_type>(argv[3]);
        if (not seed) {
            return 1;
        }
    } else {
        seed = std::random_device{}();
    }

    auto rng = std::mt19937{ *seed };
    fmt::println("seed: {}", *seed);

    auto cases = generate_diff_cases(rng, *num_cases);
    cases.emplace_back(std::string{ hello1 }, std::string{ hello2 });

    fmt::println("\ncases:");
    for (auto i = 0u; i < cases.size(); ++i) {
        const auto& [left, right] = cases[i];
        fmt::println("{:>2}: [{}] {:?}...", i, left.size(), std::string_view{ left.data(), 60 });
        fmt::println("{:>2}  [{}] {:?}...", "", right.size(), std::string_view{ right.data(), 60 });
    }

    auto result = bench(cases, *runs);

    fmt::println("\naverage:");
    print(result);

    return 0;
}
