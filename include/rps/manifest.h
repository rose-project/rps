/**
 * @file manifest.h
 * @brief API for parsing and writing of manifest files.
 */
#ifndef _MANIFEST_H
#define _MANIFEST_H

#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <jansson.h>
#include <rps/pkginfo.h>
#include <rps/version.h>
#include <rps/file.h>

#define MANIFEST_VERSION "1.0"

/**
 * @brief mpk_manifest_read reads manifest file (or template) into pkginfo
 *        object
 *
 * @param pkginfo the object to write to; this has to be empty without any
 *      preallocated data
 * @param filename the file to read from
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_manifest_read(struct mpk_pkginfo *pkginfo, const char *filename);

/**
 * @brief mpk_manifest_write Writes the contents of the manifest object to a
 * file.
 *
 * @param filename
 * @param pkg
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_manifest_write(const char *filename, struct mpk_pkginfo *pkg);

namespace RPS {

class Manifest {
public:
    enum class ManifestVersion {
        VersionUnknown = -1,
        Version1_0 = 100
    };

    enum class Tag {
        Undefined = 0,
        Manifest,
        Name,
        Version,
        API,
        Arch,
        Localization,
        Depends,
        Source,
        Vendor,
        Label,
        VersionLabel,
        Description,
        License,
        Files,
        Signatures,
    };

public:
    Manifest();
    virtual ~Manifest();

    void readFromFile(std::string filename);

    void writeManifestFile(std::string filename);

    std::string packageName() const;
    void setPackageName(const std::string &packageName);

    ManifestVersion manifestVersion() const;
    void setManifestVersion(const ManifestVersion version);

    int32_t packageVersion() const;
    void setPackageVersion(const int32_t packageVersion);

    int32_t apiMin() const;
    void setApiMin(const int32_t apiMin);

    int32_t apiTarget() const;
    void setApiTarget(const int32_t apiTarget);

    int32_t apiMax() const;
    void setApiMax(const int32_t apiMax);

    std::list<std::string> &locales();

    std::string targetArch() const;
    void setTargetArch(const std::string &targetArch);

private:
    void handleTag(const std::string &tag, json_t value);

    static void readTagManifest(Manifest &mfst, json_t *in);
    static void readTagName(Manifest &mfst, json_t *in);
    static void readTagVersion(Manifest &mfst, json_t *in);
    static void readTagAPI(Manifest &mfst, json_t *in);
    static void readTagArch(Manifest &mfst, json_t *in);
    static void readTagLocalization(Manifest &mfst, json_t *in);
    static void readTagDepends(Manifest &mfst, json_t *in);
    static void readTagSource(Manifest &mfst, json_t *in);
    static void readTagVendor(Manifest &mfst, json_t *in);
    static void readTagLabel(Manifest &mfst, json_t *in);
    static void readTagVersionLabel(Manifest &mfst, json_t *in);
    static void readTagDescription(Manifest &mfst, json_t *in);
    static void readTagLicense(Manifest &mfst, json_t *in);
    static void readTagFiles(Manifest &mfst, json_t *in);
    static void readTagSignatures(Manifest &mfst, json_t *in);

private:
    ManifestVersion mManifestVersion;
    std::string mPackageName;
    int32_t mPackageVersion;
    int32_t mApiMin;
    int32_t mApiTarget;
    int32_t mApiMax;
    std::string mTargetArch;
    std::list<std::string> mLocales;
    std::list<Dependency> mDependencies;
    std::string mSource;
    std::string mVendor;
    std::string mPackageLabel; // human-readable package name
    std::string mVersionLabal; // version as shown to the user
    std::string mDescription;
    std::string mLicense;
    std::list<File> mFiles;
    std::vector<uint8_t> mSignature;

    static std::map<std::string, Tag> ManifestTags;
    static std::list<std::string> ManifestTag;
    static std::map<Tag, std::function<void(Manifest &, json_t *)>> ReadTagFunctions;
};

}

#endif /* _MANIFEST_H */
