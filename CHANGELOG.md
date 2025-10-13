# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- New changelog file.

### Changed

- Project name is renamed to `dtlx` (including filenames and namespaces).
- Remove `m_` prefix on names from public member of structs.
- Set default limit to `2'000'000`, same as `dtl` library.
- Rename `DiffFlags::max_coordinates_size` to `DiffFlags::limit`.

## [1.0.1] - 2025-06-07

### Fixed

- Fix integer conversion warning on subrange `operator[]`.

## [1.0.0] - 2024-12-11

### Added

- CMake support.
- New implementation: complete rewrite of `dtl` library emphasizing on ranges interface.
- `dtlx::diff()` reimplement `dtl::Diff` (`compose()`).
- `dtlx::edit_distance()` reimplement `dtl::Diff` (`onOnlyEditDistance()` + `compose()`).
- `dtlx::unidiff()` reimplement `dtl::Diff` (`compose()` + `composeUnifiedHunk`).
- `dtlx::ses_to_unidiff()` reimplement `dtl::Diff` (`compose()` + `composeUnifiedHunk`).
- `dtlx::patch()` reimplement `dtl::Diff` (`patch()`).
- `dtlx::merge()` reimplement `dtl::Diff3` (`merge()`).
- Extra pretty printing functionality: `dtlx::extra::display` and `dtlx::extra::display::pretty`.

### Changed

- Explicitly support C++20 above only.
- Use simple function for all the interface instead of OOP-based builder-pattern.
- Make the comparison function a callable object that takes two parameters that returns bool.
- Make the diff parameters an optional struct that gets passed into diff functions.

[unreleased]: https://github.com/mrizaln/dtlx/compare/v1.0.1...HEAD
[1.0.1]: https://github.com/mrizaln/dtlx/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/mrizaln/dtlx/releases/tag/v1.0.0
