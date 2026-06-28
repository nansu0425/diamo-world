#pragma once

#include <Assert.h>

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace Engine
{
    // Stable, categorized failure codes. Numbered in groups with gaps so a
    // category gains members without renumbering. None is the no-error sentinel
    // and never rides inside a failing Result.
    enum class ErrorCode : int
    {
        None = 0,

        Unknown = 100,
        InvalidArgument = 101,
        OutOfRange = 102,
        NotFound = 103,
        AlreadyExists = 104,
        NotImplemented = 105,

        IoFailure = 200,
        OutOfMemory = 201,

        InvalidState = 300,
        Timeout = 301,
    };

    // Maps a code to a stable, human-readable category string. Defined in
    // Error.cpp; never returns an empty view.
    std::string_view DescribeErrorCode(ErrorCode code) noexcept;

    // A failure: a code plus an optional context message.
    class Error
    {
    public:
        Error() = default;
        explicit Error(ErrorCode code) : code_{code} {}
        Error(ErrorCode code, std::string message) : code_{code}, message_{std::move(message)} {}

        ErrorCode GetCode() const noexcept
        {
            return code_;
        }
        const std::string& GetMessage() const noexcept
        {
            return message_;
        }

        // The context message when one was given, otherwise the code's category
        // string. Always non-empty for a real failure.
        std::string_view Describe() const noexcept
        {
            return message_.empty() ? DescribeErrorCode(code_) : std::string_view{message_};
        }

    private:
        ErrorCode code_ = ErrorCode::None;
        std::string message_;
    };

    // Tag that selects a Result's error-constructing overloads, so building an
    // error in place is never confused with building a value.
    struct ErrorTag
    {
        explicit ErrorTag() = default;
    };
    inline constexpr ErrorTag ErrorInit{};

    // Holds either a value of T or an Error — a minimal stand-in for std::expected
    // (C++23, unavailable here). Reading the value of a failed Result, or the
    // error of an ok one, is a programming bug and trips a CHECK in every config.
    template <typename T>
    class Result
    {
    public:
        Result(const T& value) : storage_{value} {}
        Result(T&& value) : storage_{std::move(value)} {}

        Result(Error error) : storage_{std::move(error)} {}
        Result(ErrorTag, ErrorCode code) : storage_{Error{code}} {}
        Result(ErrorTag, ErrorCode code, std::string message) : storage_{Error{code, std::move(message)}} {}

        bool IsOk() const noexcept
        {
            return std::holds_alternative<T>(storage_);
        }
        bool HasValue() const noexcept
        {
            return IsOk();
        }
        explicit operator bool() const noexcept
        {
            return IsOk();
        }

        const T& GetValue() const&
        {
            CHECK_MESSAGE(IsOk(), "Result::GetValue() called on an error result");
            return std::get<T>(storage_);
        }

        T& GetValue() &
        {
            CHECK_MESSAGE(IsOk(), "Result::GetValue() called on an error result");
            return std::get<T>(storage_);
        }

        T&& GetValue() &&
        {
            CHECK_MESSAGE(IsOk(), "Result::GetValue() called on an error result");
            return std::get<T>(std::move(storage_));
        }

        const Error& GetError() const&
        {
            CHECK_MESSAGE(!IsOk(), "Result::GetError() called on an ok result");
            return std::get<Error>(storage_);
        }

        // The value when ok, otherwise fallback. Total: never asserts.
        T ValueOr(T fallback) const&
        {
            return IsOk() ? std::get<T>(storage_) : fallback;
        }

    private:
        std::variant<T, Error> storage_;
    };

    // Success-or-error with no value payload.
    template <>
    class Result<void>
    {
    public:
        Result() = default; // ok
        Result(Error error) : error_{std::move(error)} {}
        Result(ErrorTag, ErrorCode code) : error_{Error{code}} {}
        Result(ErrorTag, ErrorCode code, std::string message) : error_{Error{code, std::move(message)}} {}

        bool IsOk() const noexcept
        {
            return !error_.has_value();
        }
        explicit operator bool() const noexcept
        {
            return IsOk();
        }

        const Error& GetError() const&
        {
            CHECK_MESSAGE(!IsOk(), "Result::GetError() called on an ok result");
            return *error_;
        }

    private:
        std::optional<Error> error_;
    };

    // Reads better than `return {};` at a Result<void> success site.
    inline Result<void> Ok()
    {
        return Result<void>{};
    }
} // namespace Engine
