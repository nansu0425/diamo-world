# diamo-world — Project Conventions (read this first)

A virtual world project written from scratch in C++. **AI writes the code.**

## Maintaining this file
This file records **only** what is **not** mechanically enforced by configuration **and not**
already detailed in another document. Do **not** duplicate enforced rules or content covered
elsewhere — point to the source instead.

## State rules generally
When writing a general rule (in docs, comments, anywhere), keep it general. Don't pin it to
specific instances — enumerating particular examples makes the rule look limited to those
cases. State the principle itself; keep any example clearly illustrative, never exhaustive.

## Language policy
- **All project artifacts are in English**: source, comments, identifiers, commit messages,
  docs, config. No other language anywhere.

## Core philosophy
- **Zero external code dependencies**: standard library only; no third-party library is
  linked into the binaries. (Dev tools — CMake/CTest/clang-tidy/clang-format — are fine.)
- **Tests use the self-made framework** `Tools/Testing/Framework` — never gtest/Catch2 or
  another external one. (How to write a test: see `README.md`.)

## Naming — only what the linters cannot enforce
`.clang-tidy` enforces all identifier **case** rules. The rest, which it cannot check:
- **Files and directories: PascalCase** (filesystem, not lintable) — e.g. `World.h`,
  `World/Engine/`.
- **Avoid abbreviations; spell words out** (`Src` -> `Source`, `mgr` -> `manager`).
  Dictionary-common abbreviations (`config`, `max`, `min`, `id`) are allowed; ad-hoc
  shortenings (`usr`, `ctx`, `mgr`) are not.
- **Part of speech** (a word's meaning, so not lintable):
  - Functions start with a **verb** (an action): `TickWorld`, `RunAllTests`.
  - Everything else (types, variables) starts with a **noun**, never a verb: `WorldConfig`,
    `failCount`.

## Headers / module layout (conventions, not linted)
- Header extension is **`.h`**.
- Includes **omit the prefix**: `#include <World.h>` (each library is the sole source).
- Libraries split into `Public/` (API headers, the only exposed include path) and `Private/`
  (implementation). Executables have no public API, so `Main.cpp` sits in the module root.

## Build conduct (for AI)
- **Always build inside Developer PowerShell for VS 2026**; never hardcode absolute paths in
  CMake — discover tools via environment variables / PATH.
- **Do not bypass** compiler warnings or linters (e.g. with `NOLINT`, pragmas, or disabling
  options) — fix the code instead.
