#pragma once

#include <string>
#include <vector>

namespace Testing
{
    using TestFunction = void (*)();

    // Metadata for a single registered test.
    struct TestCase
    {
        std::string suite;
        std::string name;
        TestFunction function;
    };

    // Global test registry (Meyers singleton — safe against static init order issues).
    std::vector<TestCase>& GetTestRegistry();

    // Registers a test in the registry. Called from the TEST macro's static initializer.
    // The return value only exists to drive static initialization; it is meaningless.
    int RegisterTest(const std::string& suite, const std::string& name, TestFunction function);

    // Reports an assertion failure (prints file/line/message and marks the current test failed).
    void ReportFailure(const char* file, int line, const std::string& message);

    // Runs all registered tests. Returns the number of failed tests (0 = all passed).
    int RunAllTests();
} // namespace Testing

// Test definition macro. Declares the body function, auto-registers it via a static
// object, then appends the body definition.
#define TEST(suiteName, testName)                                                                                                  \
    static void suiteName##testName##Body();                                                                                       \
    static const int suiteName##testName##Registered = ::Testing::RegisterTest(#suiteName, #testName, &suiteName##testName##Body); \
    static void suiteName##testName##Body()

#define EXPECT_TRUE(condition)                                                           \
    do                                                                                   \
    {                                                                                    \
        if (!(condition))                                                                \
        {                                                                                \
            ::Testing::ReportFailure(__FILE__, __LINE__, "EXPECT_TRUE(" #condition ")"); \
        }                                                                                \
    } while (false)

#define EXPECT_FALSE(condition)                                                           \
    do                                                                                    \
    {                                                                                     \
        if (condition)                                                                    \
        {                                                                                 \
            ::Testing::ReportFailure(__FILE__, __LINE__, "EXPECT_FALSE(" #condition ")"); \
        }                                                                                 \
    } while (false)

#define EXPECT_EQUAL(lhs, rhs)                                                                \
    do                                                                                        \
    {                                                                                         \
        if (!((lhs) == (rhs)))                                                                \
        {                                                                                     \
            ::Testing::ReportFailure(__FILE__, __LINE__, "EXPECT_EQUAL(" #lhs ", " #rhs ")"); \
        }                                                                                     \
    } while (false)

#define EXPECT_NOT_EQUAL(lhs, rhs)                                                                \
    do                                                                                            \
    {                                                                                             \
        if (!((lhs) != (rhs)))                                                                    \
        {                                                                                         \
            ::Testing::ReportFailure(__FILE__, __LINE__, "EXPECT_NOT_EQUAL(" #lhs ", " #rhs ")"); \
        }                                                                                         \
    } while (false)

#define REQUIRE_TRUE(condition)                                                           \
    do                                                                                    \
    {                                                                                     \
        if (!(condition))                                                                 \
        {                                                                                 \
            ::Testing::ReportFailure(__FILE__, __LINE__, "REQUIRE_TRUE(" #condition ")"); \
            return;                                                                       \
        }                                                                                 \
    } while (false)

#define REQUIRE_EQUAL(lhs, rhs)                                                                \
    do                                                                                         \
    {                                                                                          \
        if (!((lhs) == (rhs)))                                                                 \
        {                                                                                      \
            ::Testing::ReportFailure(__FILE__, __LINE__, "REQUIRE_EQUAL(" #lhs ", " #rhs ")"); \
            return;                                                                            \
        }                                                                                      \
    } while (false)
