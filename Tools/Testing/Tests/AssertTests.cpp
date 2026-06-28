#include <Assert.h>
#include <TestFramework.h>

// Only the passing path is testable: a failing CHECK/ASSERT aborts the whole
// process (no death tests in this framework — see the plan). Reaching the end of
// each test without aborting is the check that a true condition never fires.
//
// Do not put a local read only inside an ASSERT here: ASSERT is elided in release,
// so that local would go unused and /W4 /WX would (correctly) reject the build.

TEST(Assert, CheckPassesDoesNotAbort)
{
    CHECK(1 + 1 == 2);
    CHECK_MESSAGE(true, "should not fire");
    EXPECT_TRUE(true);
}

TEST(Assert, AssertPassesDoesNotAbort)
{
    ASSERT(1 + 1 == 2);
    ASSERT_MESSAGE(true, "should not fire");
    EXPECT_TRUE(true);
}
