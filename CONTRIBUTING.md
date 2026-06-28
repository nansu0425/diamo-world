# Contributing to diamo-world

Conventions that apply to everyone working on this project — whether the code is written by a
person, by Claude Code, or by any other tool. Guidance that only matters when working through
Claude Code lives in `CLAUDE.md`.

These record **only** what is **not** mechanically enforced. Identifier **case**, formatting,
and warnings are enforced at build time by `.clang-tidy`, `.clang-format`, and `/WX` (see
`README.md`); the rules below are the rest, which tooling cannot check.

## State rules generally
When writing a general rule (in docs, comments, anywhere), keep it general. Don't pin it to
specific instances — enumerating particular examples makes the rule look limited to those
cases. State the principle itself; keep any example clearly illustrative, never exhaustive.

## Language
- **All project artifacts are in English**: source, comments, identifiers, commit messages,
  docs, config. No other language anywhere.

## Core philosophy
The thing built from scratch is **World** (`World/` — the engine, server, and client).
**Tools** (`Tools/` — tooling to develop World: testing, profiling, debugging) merely serve
that work. The dependency rule follows that split.
- **World is zero third-party**: World's source and binaries (Engine/Server/Client) use the
  standard library plus the host platform's native APIs (graphics, windowing, input,
  networking, audio) only — no third-party code appears in World's source, and no third-party
  library is linked into World's binaries. The platform boundary is allowed because it is the
  OS/hardware itself, not someone else's code — but talk to it directly, never through a
  third-party abstraction layer.
- **Tools may use third-party**: development tooling is free to pull in third-party tools and
  libraries, provided they never reach World. The boundary: a tool's third-party code is not
  linked into any World binary, and a tool touches World only across its public API (e.g.
  `#include <World.h>`, the way the test executable does) — adopting a tool never means
  editing World's source to accommodate it. (Dev tools the build itself relies on —
  CMake/CTest/clang-tidy/clang-format — are likewise fine.) The test framework under
  `Tools/Testing/Framework` is self-made today, but that is its current state, not a mandate.
  (How to write a test: see `README.md`.)

## Naming — only what the linters cannot enforce
`.clang-tidy` enforces all identifier **case** rules. The rest, which it cannot check:
- **Files and directories: PascalCase** (filesystem, not lintable) — e.g. `World.h`,
  `World/Engine/`. A name a tool mandates (e.g. `CMakeLists.txt`, `.clang-tidy`, git hook
  filenames) keeps the form the tool requires.
- **Avoid abbreviations; spell words out** (`Src` -> `Source`, `mgr` -> `manager`).
  Dictionary-common abbreviations (`config`, `max`, `min`, `id`) are allowed; ad-hoc
  shortenings (`usr`, `ctx`, `mgr`) are not.
- **Part of speech** (a word's meaning, so not lintable):
  - Functions start with a **verb** (an action): `TickWorld`, `RunAllTests`.
  - Everything else (types, variables) starts with a **noun**, never a verb: `WorldConfig`,
    `failCount`.
- **Program-scope compile-time constants go in an anonymous namespace, not behind the
  `static` keyword.** The case split is: program-scope constants (namespace/global/
  class-static) are PascalCase; everything inside a function (locals, and function-`static`)
  is camelCase. clang-tidy classifies a file-scope `static` constant the same as a
  function-`static` one, so `static constexpr Foo` at file scope gets forced to camelCase —
  declare it in an anonymous namespace instead and it reads as a global constant (PascalCase).

## Headers / module layout
- Header extension is **`.h`**.
- Includes **omit the prefix**: `#include <World.h>` (each library is the sole source).
- Libraries split into `Public/` (API headers, the only exposed include path) and `Private/`
  (implementation). Executables have no public API, so `Main.cpp` sits in the module root.

## Building
- **Always build inside Developer PowerShell for VS 2026** (see `README.md`); never hardcode
  absolute paths in CMake — discover tools via environment variables / PATH.
- **Do not bypass** compiler warnings or linters (e.g. with `NOLINT`, pragmas, or disabling
  options) — fix the code instead.

## Error handling
World is exception-free: do not use C++ exceptions for control flow. Engine's
error system (`Assert.h`, `Error.h`) is the standard toolkit, and new systems
should adopt it from the start rather than inventing ad-hoc sentinels or
logging-and-continuing.
- **Programming errors / invariants → assertions.** Validate what correct code
  must guarantee. `CHECK_*` stays in every build (guard invariants that must hold
  in release, especially at public API boundaries and on external input);
  `ASSERT_*` is debug-only for internal sanity with no release cost. A failed
  assertion reports and aborts — use it where continuing would mean running on
  corrupt state.
- **Recoverable / expected failures → `Result<T>`.** Return a `Result` carrying
  an `Error` (a code plus an optional message) and let the caller decide, instead
  of returning a sentinel value or only logging.
- The error system depends on nothing else, so any system may use it freely.

The standard library is exception-based. Rather than disable that, handle it at
our boundary so the rules above still hold:
- **Prefer the non-throwing API.** Where the standard library offers a
  non-throwing form, use it and translate the outcome into the model above — an
  `std::error_code` overload or a checked parse becomes a `Result`; a misuse that
  would otherwise throw (out-of-bounds, wrong variant alternative, empty optional)
  is prevented with a `CHECK` rather than left to the throwing accessor.
- **Let the unrecoverable terminate.** A genuinely unrecoverable failure (e.g. an
  exhausted allocation) is allowed to propagate to `std::terminate`. A single
  process-wide terminate handler turns that into a Fatal diagnostic and aborts,
  consistent with a failed assertion.
- **Never catch.** World is catch-free: even the terminate handler reports and
  aborts without catching. This is not only convention — clang-tidy parses World
  with exceptions disabled, so a stray `try`/`catch` fails the build.

## Commit messages
Use **Conventional Commits**. It is chosen for AI-agent-written history: the format is one
models reproduce consistently, it is machine-readable for tooling and for a future agent
rebuilding context from `git log`, its single-type rule keeps commits atomic, and it is the
only commit convention with a grammar a hook can enforce.

```
<type>(<scope>): <subject>

<body>

<footer>
```

- **type** — one of `feat`, `fix`, `refactor`, `perf`, `docs`, `test`, `build`, `ci`,
  `chore`, `style`.
- **scope** (optional) — the area touched, e.g. `engine`, `client`, `server`, `testing`,
  `cmake`, `build`.
- **subject** — describe the change concisely, no trailing period; keep the whole first line
  within 72 characters. Append `!` after the type/scope to mark a breaking change.
- **body** (optional) — explain *why*, not the *what* the diff already shows; a `-` bullet
  list is fine. Separate it from the subject with a blank line.
- **footer** (optional) — `BREAKING CHANGE: <detail>` or issue references (`Refs: #12`).
  Never add a `Co-Authored-By` or any other authorship trailer.
- **Atomic** — one logical change, one type. If the subject needs an "and", or two types
  would fit, split the commit.

The repo's pre-convention history (the first commits) predates this rule — do not mirror it.

Enforced by the self-made `.githooks/commit-msg` hook (no third-party dependency); it checks
the structural rules above, the rest hold by convention. CMake activates it during configure
(`core.hooksPath=.githooks`), so a normal build picks it up with no manual step. To enable it
without configuring — e.g. before the first build — run once:

```
git config core.hooksPath .githooks
```
