#include "mpk.h"
extern "C" {
#include "manifest.h"
}
#include "gtest/gtest.h"

TEST(MpkHighLevelApi, package_create) {

    EXPECT_EQ(MPK_SUCCESS, mpk_init());
    EXPECT_EQ(MPK_SUCCESS,
        mpk_create(TESTDATA_DIR "testpackage","/tmp",
        TESTDATA_DIR "privkey.pem"));
    mpk_deinit();
}
