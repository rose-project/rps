#include <stdlib.h>
#include <sys/stat.h>
#include "mpk/mpk.h"
#include "gtest/gtest.h"

TEST(MpkHighLevelApi, package_create) {

    EXPECT_EQ(MPK_SUCCESS, mpk_init());
    EXPECT_EQ(MPK_SUCCESS,
        mpk_create(TESTDATA_DIR "testpackage","/tmp",
        TESTDATA_DIR "privkey.pem"));
    mpk_deinit();

    struct stat st;
    EXPECT_EQ(0,  stat("/tmp/testpackage.mpk", &st));
    unlink("/tmp/testpackage.mpk");
}

TEST(MpkHighLevelApi, package_unpack) {
    EXPECT_EQ(MPK_SUCCESS, mpk_init());
    EXPECT_EQ(MPK_SUCCESS, mpk_unpack(TESTDATA_DIR"/testpackage.mpk", "/tmp"));
    mpk_deinit();
    system("rm -rf /tmp/testpackage");
}
