#include <Error.h>
#include <TestFramework.h>

#include <string>
#include <string_view>
#include <utility>

// Covers every Result/Error path that does not abort. Misuse guards (GetValue on
// an error, GetError on an ok) abort via CHECK and so cannot be exercised by this
// framework — see the plan's death-test note.

TEST(Result, OkInt)
{
    Engine::Result<int> result = 42;
    EXPECT_TRUE(result.IsOk());
    EXPECT_TRUE(result.HasValue());
    EXPECT_TRUE(static_cast<bool>(result));
    EXPECT_EQUAL(result.GetValue(), 42);
    EXPECT_EQUAL(result.ValueOr(-1), 42);
}

TEST(Result, ErrorInt)
{
    Engine::Result<int> result = Engine::Error{Engine::ErrorCode::NotFound};
    EXPECT_FALSE(result.IsOk());
    EXPECT_FALSE(static_cast<bool>(result));
    EXPECT_EQUAL(result.GetError().GetCode(), Engine::ErrorCode::NotFound);
    EXPECT_EQUAL(result.ValueOr(-1), -1);
}

TEST(Result, InPlaceError)
{
    Engine::Result<int> result{Engine::ErrorInit, Engine::ErrorCode::InvalidArgument, "bad input"};
    EXPECT_FALSE(result.IsOk());
    EXPECT_EQUAL(result.GetError().GetCode(), Engine::ErrorCode::InvalidArgument);
    EXPECT_EQUAL(result.GetError().GetMessage(), std::string{"bad input"});
}

TEST(Result, MovesValueOut)
{
    Engine::Result<std::string> result = std::string{"hello"};
    EXPECT_TRUE(result.IsOk());
    std::string moved = std::move(result).GetValue();
    EXPECT_EQUAL(moved, std::string{"hello"});
}

TEST(Result, StringError)
{
    Engine::Result<std::string> result = Engine::Error{Engine::ErrorCode::IoFailure, "disk gone"};
    EXPECT_FALSE(result.IsOk());
    EXPECT_EQUAL(result.GetError().GetCode(), Engine::ErrorCode::IoFailure);
    EXPECT_EQUAL(result.ValueOr(std::string{"fallback"}), std::string{"fallback"});
}

TEST(Result, VoidOk)
{
    Engine::Result<void> result = Engine::Ok();
    EXPECT_TRUE(result.IsOk());
    EXPECT_TRUE(static_cast<bool>(result));
}

TEST(Result, VoidError)
{
    Engine::Result<void> result = Engine::Error{Engine::ErrorCode::Timeout};
    EXPECT_FALSE(result.IsOk());
    EXPECT_EQUAL(result.GetError().GetCode(), Engine::ErrorCode::Timeout);
}

TEST(Error, DescribeUsesMessageWhenSet)
{
    Engine::Error error{Engine::ErrorCode::NotFound, "file missing"};
    EXPECT_EQUAL(error.Describe(), std::string_view{"file missing"});
}

TEST(Error, DescribeFallsBackToCategory)
{
    Engine::Error error{Engine::ErrorCode::NotFound};
    EXPECT_TRUE(error.GetMessage().empty());
    EXPECT_EQUAL(error.Describe(), Engine::DescribeErrorCode(Engine::ErrorCode::NotFound));
}

TEST(Error, DescribeErrorCodeIsNeverEmpty)
{
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::None).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::Unknown).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::InvalidArgument).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::OutOfRange).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::NotFound).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::AlreadyExists).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::NotImplemented).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::IoFailure).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::OutOfMemory).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::InvalidState).empty());
    EXPECT_FALSE(Engine::DescribeErrorCode(Engine::ErrorCode::Timeout).empty());
}

TEST(Error, DescribeErrorCodeHandlesOutOfRange)
{
    EXPECT_EQUAL(Engine::DescribeErrorCode(static_cast<Engine::ErrorCode>(99999)), std::string_view{"Unrecognized error code"});
}
