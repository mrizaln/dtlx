# dtl-modern

The `dtl-modern` library is the _**modern**_ Diff Template Library written in C++20 as an improvement of the [`dtl`](https://github.com/cubicdaiya/dtl) library written by [`cubicdaiya`](https://github.com/cubicdaiya).

This library provides the functionality to compare two sequences of arbitrary type by employing [An O(NP) Sequence Comparison Algorithm](<https://doi.org/10.1016/0020-0190(90)90035-V>). This algorithm works by calculating the difference between two sequences as **Edit Distance**, **LCS**, and **SES**.

| Term          | Description                                                                                    |
| ------------- | ---------------------------------------------------------------------------------------------- |
| Edit Distance | Numerical value for declaring a difference between two sequences.                              |
| LCS           | Longest Common Subsequence.                                                                    |
| SES           | Shortest Edit Script; the shortest course of action for translating one sequence into another. |

If one sequence is `"abc"` and another sequence is `"abd"`, the **Edit Distance**, **LCS**, and **SES** are below.

| Metric        | Value             |
| ------------- | ----------------- |
| Edit Distance | `2`               |
| LCS           | `ab`              |
| SES           | `C a C b D c A d` |

With

- `C` for `Common`,
- `D` for `Delete`, and
- `A` for `Add`.

## Table of contents

- [Features](#features)
- [Constraints](#constraints)
- [Getting started](#getting-started)
  - [Comparing two strings](#comparing-two-strings)
  - [Comparing two arbitrary sequence](#comparing-two-arbitrary-sequences)
  - [Calculate edit distance only](#calculate-only-edit-distance)
  - [Difference as Unified Format](#difference-as-unified-format)
  - [Comparing two large sequences](#comparing-two-large-sequences)
  - [Merge three sequences](#merge-three-sequences)
  - [Patch a sequence](#patch-a-sequence)
  - [Displaying diff](#displaying-diff)
  - [Unserious difference](#unserious-difference)
- [Algorithm](#algorithm)
  - [Computational complexity](#computational-complexity)
  - [Comparison when difference between two sequences is very large](#comparison-when-difference-between-two-sequences-is-very-large)
- [Examples](#examples)
- [Tests](#tests)
- [License](#license)

## Features

This library provides these functions:

- Main functionality:

  - `dtl_modern::edit_distance `: calculates Edit Distance between two sequence
  - `dtl_modern::diff          `: produces LCS, SES, and Edit Distance at the same time
  - `dtl_modern::unidiff       `: produces Unified Format hunks, LCS, SES, and Edit Distance
  - `dtl_modern::ses_to_unidiff`: transforms SES into Unified Format
  - `dtl_modern::merge         `: merges three sequences, or not if there is a conflict
  - `dtl_modern::patch         `: patch a sequence given an SES

- Extra functionality:

  - `dtl_modern::extra::display`:
    - displays SES via `fmt::format` or `std::format`
      > - see [`<dtl_modern/extra/ses_display_simple.hpp>`](include/dtl_modern/extra/ses_display_simple.hpp) header
    - displays Unified Format hunks
      > - see [`<dtl_modern/extra/uni_hunk_display_simple.hpp>`](include/dtl_modern/extra/uni_hunk_display_simple.hpp) header
  - `dtl_modern::extra::display_pretty`
    - displays SES via `fmt::format` with pretty colors
      > - see [`<dtl_modern/extra/ses_display_pretty.hpp>`](include/dtl_modern/extra/ses_display_pretty.hpp) header

## Constraints

The sequences to be compared must support **`std::random_access_iterator`** / **`std::ranges::random_access_range`** and the contained type must be **`std::copyable`**. The full requirement of the sequence is defined in the [common.hpp](include/dtl_modern/common.hpp) header.

## Getting started

You can use any method of your choice to fetch this library, I usually just use CMake FetchContent. Then, all you need to do is include `dtl_modern.hpp`.

```cpp
#include <dtl_modern/dtl_modern.hpp>
```

### Comparing two strings

The most common usage of diff algorithm is to compare two strings. Here's how you do it:

```cpp
#include <dtl_modern/dtl_modern.hpp>

#include <string>
#include <string_view>

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    auto hello1 = "hello World!"sv;    // notice the different type, this one is std::string_view
    auto hello2 = "Hell word"s;        // this one is std::string

    // NOTE: passing const char[N] will work, but const char* won't (wrap it in std::string_view instead)

If one sequence is "abc" and another sequence is "abd", Edit Distance and LCS and SES is below.

    // ...

    // ...or you can provide a custom one yourself at call site
    auto ignore_case = [](char a, char b) { return std::tolower(a) == std::tolower(b); };
    auto [ilcs, ises, iedit_distance] = dtl_modern::diff(hello1, hello2, ignore_case);
}
```

> You can read the source file to see the difference in usage between `dtl` and `dtl-modern`: [strdiff.cpp](examples/source/strdiff.cpp).

### Comparing two arbitrary sequences

You can compare two sequences with arbitrary type as long as both of the sequences contain the same type. For example, You can compare two `int` ranges like in the example below.

> Virtually no difference just like how you compare two strings.

```cpp
#include <dtl_modern/dtl_modern.hpp>

#include <array>
#include <vector>

int main()
{
    auto a = std::array { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto b = std::vector{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 };

    auto [lcs, ses, edit_distance] = dtl_modern::diff(a, b);

    // ...
}
```

> You can read the source file to see the difference in usage between `dtl` and `dtl-modern`: [intdiff.cpp](examples/source/intdiff.cpp), [objdiff.cpp](examples/source/objdiff.cpp), or [filediff.cpp](examples/source/filediff.cpp).

### Calculate edit distance only

If you need edit distance only, using this function might be beneficial since the calculation of edit distance is lighter than the calculation of LCS and SES.

```cpp
#include <dtl_modern/dtl_modern.hpp>

#include <array>
#include <vector>

int main()
{
    auto a = std::array { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto b = std::vector{ 3, 5, 1, 4, 5, 1, 7, 9, 6, 10 };

    auto edit_distance = dtl_modern::edit_distance(a, b);

    // ...
}
```

### Difference as Unified Format

You can generate [Unified Format](http://www.gnu.org/s/diffutils/manual/html_node/Unified-Format.html) using `dtl_modern::unidiff` and/or `dtl_modern::ses_to_unidiff` function.

```cpp
#include <dtl_modern/dtl_modern.hpp>

int main()
{
    using namespace std::string_view_literals;

    auto a = "acbdeaqqqqqqqcbed"sv;   // const char* won't work for diff and unidiff
    auto b = "acebdabbqqqqqqqabed"sv;

    // directly generate unified format hunks, lcs, ses, and edit distance
    auto [uni_hunks, lcs, ses, edit_distance] = dtl_modern::unidiff(a, b);

    // ...

    // or generate unified format from ses generated beforehand
    auto uni_hunks = dtl_modern::ses_to_unidiff(ses);

    // ...
}
```

> Previous example of [intdiff.cpp](examples/source/intdiff.cpp) used `unidiff`, you can read it directly.

### Comparing two large sequences

When Comparing two large sequences, you can enable the `huge` flag on `dtl_modern::diff` and `dtl_modern::unidiff` function (`false` by default).
You can also limit the size of Edit Path Coordinates vector that is used internally to divide the original sequence into subsequences.

> - What the flag does is reserve a memory of size of your choice (or use default `dtl_modern::constants::default_limit`) at the start of diff-ing.
> - Setting the limit of the size of Edit Path Coordinates to `dtl_modern::constants::no_limit` makes the internal algorithm simply not reserve the memory at the start regardless the huge flag value.
> - Setting a limit to the maximum size of the Edit Path Coordinates may result in less accurate edit distance and SES though still usable.

```cpp
#include <dtl_modern/dtl_modern.hpp>

int main ()
{
    // ...

    {
        auto flags = dtl_modern::DiffFlags{
            .m_huge                 = true,
            .m_max_coordinates_size = dtl_modern::constants::default_limit,
        }:
        auto [lcs, ses, edit_distance] = dtl_modern::diff(a, b, {}, flags);

        // ...
    }

    {
        // set the huge flag to true, use the default comparison function (operator==)
        auto flags = dtl_modern::DiffFlags{
            .m_huge                 = true,
            .m_max_coordinates_size = dtl_modern::constants::default_limit,
        }:
        auto [uni_hunks, lcs, ses, edit_distance] = dtl_modern::unidiff(a, b, {}, true);

        // ...
    }

    // ...
}
```

### Merge three sequences

To merge three sequences, you can use the `dtl_modern::merge` function. It takes three ranges then you provide a template as the first template argument that will become the type of the returned new sequence. The returned value is not immediately the actual type but a variant that either holds the new sequence or a conflict.

```cpp
#include <dtl_modern/dtl_modern.hpp>

int main()
{
    using namespace std::string_view_literals;

    auto a = "qqqabc"sv;
    auto b = "abc"sv;
    auto c = "abcdef"sv;

    // pass a template std::basic_string to get std::basic_string<char> as the result
    auto maybe_result = dtl_modern::merge<std::basic_string>(a, b, c);
    if (maybe_result.is_conflict()) {
        // well, conflict happen, what to do ...
        return 1;
    }

    auto result = std::move(maybe_result).as_merge().m_value;

    // do something with the result ...
}
```

### Patch a sequence

The `dtl_modern::patch` function can apply patch in the form of SES to a sequence transforming it into other sequence. Just like the `dtl_modern::merge`, this function also takes a template as the first template argument, Here is some kind of a useless example:

```cpp
#include <dtl_modern/dtl_modern.hpp>

int main()
{
    using namespace std::string_view_literals;

    auto a = "abc"sv;
    auto b = "abd"sv;

    auto [_lcs, ses, _edit_dist] = dtl_modern::diff(a, b);

    // now, we've got an SES, we can apply patches to sequence a
    auto new_seq = dtl_modern::patch<std::basic_string>(a, ses);

    assert(b == new_seq);
}
```

> - TODO: implement unipatch that patch a sequence given a unified format hunks

### Displaying diff

Unlike `dtl`, `dtl-modern` consider this feature to be optional thus separating this feature into its own directory, [extra](include/dtl_modern/extra).

No additional flag is needed to use this feature, you just need to `#include` the required header to use it.

```cpp
// for displaying SES
#include <dtl_modern/extra/ses_display_simple.hpp>

// pretty print SES: colorize the addition and deletion
// must use fmt, the macro DTL_MODERN_DISPLAY_FMTLIB won't affect this header
#include <dtl_modern/extra/ses_display_pretty.hpp>

// for displaying Unified Format hunks
#include <dtl_modern/extra/uni_hunk_display_simple.hpp>
```

To display the SES/Unified Format hunks, you need to call `dtl_modern::extra::display` function. This function will return a type that wraps the argument into a type that can be displayed using `std::format` (or `fmt::format`; instruction below ) and/or `std::ostream&` using the `operator<<` overload.

The display functionality relies on `std::formatter` specialization for `std::format`. If you are using `fmt` library instead of the standard library, you can define macro `DTL_MODERN_DISPLAY_FMTLIB` to use `fmt::formatter` implementation instead. This action will replace the `std::formatter` specialization thus disabling `std::format` functionality and vice versa. There is plan to add an option to enable both.

> NOTE: the `fmt` library itself is not provided by this library. To enable `fmt` feature you need to have `fmt` available and linked to your project where this library is used.

```cpp
#include <dtl_modern/dtl_modern.hpp>

// uncomment this define to use the standard library <format> instead
#define DTL_MODERN_DISPLAY_FMTLIB
#include <dtl_modern/extra/ses_display_simple.hpp>
#include <dtl_modern/extra/uni_hunk_display_simple.hpp>

// must use fmt, the macro DTL_MODERN_DISPLAY_FMTLIB won't affect this header since it will use
// fmt regardless
#include <dtl_modern/extra/ses_display_pretty.hpp>

// using fmt
#include <fmt/core.h>

#include <string_view>

int main()
{
    using namespace std::string_view_literals;

    auto a = "hello World!"sv;
    auto b = "Hell word"s;

    auto [uni_hunks, lcs, ses, edit_distance] = dtl_modern::unidiff(a, b);

    fmt::println("\nSES:\n");
    fmt::println("{}", dtl_modern::extra::display(ses));

    fmt::println("\nUnified Format:\n");
    fmt::println("{}", dtl_modern::extra::display(uni_hunks));

    fmt::println("\nSES pretty:\n");
    fmt::println("{}", dtl_modern::extra::display_pretty(ses));
}
```

> Try running the [`pretty_print.cpp`](examples/source/pretty_print.cpp) example code to see the displayed output of SES's `display_pretty`.

### Unserious difference

> - NOTE: may or may not be implemented in the future

## Algorithm

The algorithm `dtl` (in turns `dtl-modern`) uses is based on [An O(NP) Sequence Comparison Algorithm](<https://doi.org/10.1016/0020-0190(90)90035-V>) as described by Sun Wu, Udi Manber and Gene Myers. This algorithm is an efficient algorithm for comparing two sequences.

### Computational complexity

The computational complexity of Wu's O(NP) Algorithm is averagely O(N+PD), in the worst case, is O(NP).

### Comparison when difference between two sequences is very large

Calculating LCS and SES efficiently at any time is a difficult and the calculation of LCS and SES needs massive amount of memory when a difference between two sequences is very large.

The `dtl` (in turns `dtl-modern`) avoids the above problem by dividing each sequence into plural sub-sequences and joining the difference of each sub-sequence at the end.

The program uses that algorithm don't consider that will burst in the worst case.

`dtl` avoids above-described problem by dividing each sequence into plural subsequences and joining the difference of each subsequence finally. (This feature is supported after version 0.04)

## Implementations with various programming languages

There are the Wu's O(NP) Algorithm implementations with various programming languages below.

https://github.com/cubicdaiya/onp

# Examples

There are examples in the [examples](examples) directory. I used Conan to be able to use external libraries for the examples. Make sure it is installed and configured in your system.

To build the examples, you first must change your current working directory to `examples`, then run these commands

```sh
conan install . --build missing -s build_type=Debug   # or Release
cmake --preset conan-debug                            # conan-release if Release mode, conan-default if you're on windows regardless
cmake --build --preset conan-debug                    # adjust the preset like above
```

## Tests

Just like examples, the tests requires Conan to be able to be built and ran.

Make sure you are in the `test` directory first, then run these commands to build the tests

```sh
conan install . --build missing -s build_type=Debug   # or Release
cmake --preset conan-debug                            # conan-release if Release mode, conan-default if you're on windows regardless
cmake --build --preset conan-debug                    # adjust the preset like above
```

## unidiff

```sh
ctest --preset conan-debug --output-on-failure        # adjust the preset like above
```
