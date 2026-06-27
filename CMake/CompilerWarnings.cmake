# Common compiler-warning settings applied to every target.
# All warnings are treated as errors (/WX, -Werror): any compiler warning fails the build,
# mirroring how clang-tidy diagnostics are enforced.
# Usage: ApplyCompilerWarnings(<target>)
function(ApplyCompilerWarnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()
