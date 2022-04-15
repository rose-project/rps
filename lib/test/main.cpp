#include <rps/manifest.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <cstdlib>
#include <list>
#include <string>

TEST(Manifest, ReadManifestFile)
{
    auto m = rose::Manifest();
    m.readFromFile(TESTDATA_DIR "/testpackage/manifest.json");

    std::cout << TESTDATA_DIR << std::endl;
    EXPECT_EQ(m.packageName(), std::string("testpackage"));
    EXPECT_EQ(m.manifestVersion(), rose::Manifest::ManifestVersion::Version1_0);
    EXPECT_EQ(m.packageVersion(), 12);
    EXPECT_EQ(m.apiMin(), 12);
    EXPECT_EQ(m.apiTarget(), 27);
    EXPECT_EQ(m.apiMax(), 4096);

    std::list<std::string> locales({"de:de", "en:us", "en:gb"});
    EXPECT_EQ(locales, m.locales());

    // TODO
    // EXPECT_EQ(m.dependencies().front().name, std::string("testb"));
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
