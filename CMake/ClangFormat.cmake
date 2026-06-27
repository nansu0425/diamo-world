# clang-format based formatting check — runs at build time, fails the build on violation.
# Works at the same "build time" as clang-tidy and only re-checks changed files (incremental).
#
# Implementation: CMake has no clang-format compile hook, so for each checked file we add a
# custom command that runs "clang-format --dry-run --Werror" and leaves a stamp file. If the
# stamp is newer than the source it is not re-checked, giving incremental behavior. A
# violation makes the custom command fail, which stops the whole build.

find_program(CLANG_FORMAT
    NAMES clang-format
    HINTS "$ENV{VCINSTALLDIR}Tools/Llvm/x64/bin"
          "$ENV{VCINSTALLDIR}Tools/Llvm/bin")

# Add the formatting-check target (ClangFormatCheck, ALL) over the project sources/headers.
function(AddClangFormatCheck)
    if(NOT CLANG_FORMAT)
        message(WARNING "clang-format not found — format check disabled.")
        return()
    endif()

    # Check project code only (exclude system / standard headers). CONFIGURE_DEPENDS so new
    # files trigger a re-configure.
    file(GLOB_RECURSE formatFiles CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/World/*.h"
        "${CMAKE_SOURCE_DIR}/World/*.cpp"
        "${CMAKE_SOURCE_DIR}/Tools/*.h"
        "${CMAKE_SOURCE_DIR}/Tools/*.cpp")

    set(stamps "")
    foreach(sourceFile ${formatFiles})
        file(RELATIVE_PATH relativePath "${CMAKE_SOURCE_DIR}" "${sourceFile}")
        set(stampFile "${CMAKE_BINARY_DIR}/ClangFormat/${relativePath}.stamp")
        get_filename_component(stampDir "${stampFile}" DIRECTORY)

        add_custom_command(
            OUTPUT "${stampFile}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${stampDir}"
            COMMAND "${CLANG_FORMAT}" --dry-run --Werror "${sourceFile}"
            COMMAND "${CMAKE_COMMAND}" -E touch "${stampFile}"
            # Re-check everything when .clang-format changes.
            DEPENDS "${sourceFile}" "${CMAKE_SOURCE_DIR}/.clang-format"
            COMMENT "clang-format check: ${relativePath}"
            VERBATIM)

        list(APPEND stamps "${stampFile}")
    endforeach()

    add_custom_target(ClangFormatCheck ALL DEPENDS ${stamps})
endfunction()
