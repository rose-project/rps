#include "mpk/mpk.h"
#include "gtest/gtest.h"

TEST(MpkHighLevelApi, package_create) {

    EXPECT_EQ(MPK_SUCCESS, mpk_init());
    EXPECT_EQ(MPK_SUCCESS,
        mpk_create(TESTDATA_DIR "testpackage","/tmp",
        TESTDATA_DIR "privkey.pem"));
    mpk_deinit();
}

TEST(MpkHighLevelApi, package_install) {
    EXPECT_EQ(MPK_SUCCESS, mpk_install(TESTDATA_DIR"/testpackage.mpk"));
}
