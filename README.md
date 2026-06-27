# diamo-world

A virtual world project built from scratch in C++. It builds a server, a client, and a
shared engine with no external code dependencies (standard library only).

## Requirements

- **Visual Studio 2026 (= version 18)** — bundles MSVC, CMake, Ninja, clang-tidy and
  clang-format
- Builds must run inside **Developer PowerShell for VS 2026** (`cl`/`cmake`/`ninja` on PATH,
  `VCINSTALLDIR` set)

No separate system-wide install of the tools is needed. Enter the Developer Shell with:

```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1"
# or cmd: "...\VC\Auxiliary\Build\vcvars64.bat"
```

> The absolute path differs per machine. CMake never hardcodes absolute paths; tools are
> discovered via environment variables / PATH (independent of edition, version, or install
> location).

## Build / test / run

Inside Developer PowerShell, from the project root:

```powershell
cmake --preset ninja-msvc          # configure (Ninja Multi-Config, generates compile_commands.json)
cmake --build --preset debug       # build (clang-tidy + clang-format run and enforce conventions)
ctest --preset debug               # test
```

Executables:

```powershell
out\build\ninja-msvc\World\Server\Debug\Server.exe
out\build\ninja-msvc\World\Client\Debug\Client.exe
```

Release works the same with `--preset release`.

## Layout

```
World/                  the virtual world itself
  Engine/               shared static library (engine shared by Server/Client)
    Public/             public API headers  -> #include <World.h>
    Private/            internal implementation
  Server/  Client/      executables (link Engine)
Tools/                  development tooling
  Testing/
    Framework/          self-made test framework (zero external deps)
    Tests/              actual tests (registered with CTest)
CMake/                  CMake helper modules
```

## AI coding harness

This repo assumes **AI writes the code**, so conventions are enforced mechanically at build
time. The build is strict: **all compiler warnings are treated as errors** (`/WX`), so any
warning (shadowing, unused values, narrowing conversions, ...) fails the build.

- **`.clang-tidy`** — encodes naming rules (`readability-identifier-naming`). Runs **at build
  time** alongside each source; violations are **build errors**.
- **`.clang-format`** — formatting rules. At build time the `ClangFormatCheck` target checks
  changed files (`--dry-run --Werror`); violations are **build errors** (same time and
  incremental, like clang-tidy). Apply formatting manually with `clang-format -i <files>`.
- **`CLAUDE.md`** — rules the linters cannot enforce (file names, directories, includes,
  abbreviation policy, etc.).

> Both checks run automatically on `cmake --build` and stop the build on violation. Tool
> discovery is environment-variable / PATH based, in `CMake/ClangFormat.cmake` (clang-format)
> and the root `CMakeLists.txt` (clang-tidy).

## Writing tests

Add a source under `Tools/Testing/Tests/` and register an executable + `add_test` in
`Tools/Testing/CMakeLists.txt`.

```cpp
#include <TestFramework.h>
#include <World.h>

TEST(World, Tick)
{
    EXPECT_EQUAL(Engine::TickWorld(0), 1);
}
```

Defining `TEST(Suite, Name)` auto-registers it, and the `main()` provided by
`TestingFramework` runs them all. Assertions: `EXPECT_TRUE/FALSE`,
`EXPECT_EQUAL/NOT_EQUAL`, `REQUIRE_TRUE/EQUAL`.
