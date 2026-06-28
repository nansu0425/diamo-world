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
