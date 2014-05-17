#include "mpk.h"
extern "C" {
#include "manifest.h"
}
#include "gtest/gtest.h"

TEST(ManifestTest, TestTest) {
    EXPECT_EQ (MPK_SUCCESS, mpk_manifest_test());
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}