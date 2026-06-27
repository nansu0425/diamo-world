cmake_minimum_required(VERSION 3.21)

# Disallow in-source builds — always build into a separate directory such as out/build.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed. Configure into a separate directory, e.g.\n"
        "  cmake --preset ninja-msvc")
endif()

project(DiamoWorld VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Interpret sources as UTF-8. MSVC defaults to the system code page, so make it explicit.
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Generate the compile database used by clang-tidy / clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

# Common compiler-warning helper (ApplyCompilerWarnings).
include(${CMAKE_SOURCE_DIR}/CMake/CompilerWarnings.cmake)

# clang-format check helper (AddClangFormatCheck).
include(${CMAKE_SOURCE_DIR}/CMake/ClangFormat.cmake)

# --- AI coding harness: enforce naming rules with clang-tidy ------------------
# Machine independent: no hardcoded absolute paths; discover via the VS Developer Shell
# environment variable (VCINSTALLDIR) plus PATH.
find_program(CLANG_TIDY
    NAMES clang-tidy
    HINTS "$ENV{VCINSTALLDIR}Tools/Llvm/x64/bin"
          "$ENV{VCINSTALLDIR}Tools/Llvm/bin")

if(CLANG_TIDY)
    # Treat naming violations (readability-identifier-naming in .clang-tidy) as build errors.
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY};--warnings-as-errors=*")
    message(STATUS "clang-tidy enabled (naming enforced as errors): ${CLANG_TIDY}")
else()
    message(WARNING
        "clang-tidy not found — building WITHOUT naming enforcement. "
        "Run inside Developer PowerShell for VS 2026 to enable it.")
endif()

add_subdirectory(World/Engine)
add_subdirectory(World/Server)
add_subdirectory(World/Client)
add_subdirectory(Tools/Testing)

# --- AI coding harness: enforce formatting with clang-format (checked at build time) ---
AddClangFormatCheck()
