#include <Error.h>

namespace Engine
{
    std::string_view DescribeErrorCode(ErrorCode code) noexcept
    {
        // No default case: every enumerator is handled explicitly, so adding a
        // code without a string here is a compiler warning, not a silent gap. The
        // trailing return covers an out-of-range cast and satisfies /W4 C4715.
        switch (code)
        {
        case ErrorCode::None:
            return "No error";
        case ErrorCode::Unknown:
            return "Unknown error";
        case ErrorCode::InvalidArgument:
            return "Invalid argument";
        case ErrorCode::OutOfRange:
            return "Out of range";
        case ErrorCode::NotFound:
            return "Not found";
        case ErrorCode::AlreadyExists:
            return "Already exists";
        case ErrorCode::NotImplemented:
            return "Not implemented";
        case ErrorCode::IoFailure:
            return "I/O failure";
        case ErrorCode::OutOfMemory:
            return "Out of memory";
        case ErrorCode::InvalidState:
            return "Invalid state";
        case ErrorCode::Timeout:
            return "Timeout";
        }

        return "Unrecognized error code";
    }
} // namespace Engine
