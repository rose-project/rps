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
#include <rps/version.h>
#include <rps/file.h>


namespace rose {

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
        Files
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

    void setLocales(const std::list<std::string> &locales);

    std::list<Dependency> dependencies() const;
    void setDependencies(const std::list<Dependency> &dependencies);

    std::string source() const;
    void setSource(const std::string &source);

    std::string vendor() const;
    void setVendor(const std::string &vendor);

    std::string packageLabel() const;
    void setPackageLabel(const std::string &packageLabel);

    std::string versionLabal() const;
    void setVersionLabal(const std::string &versionLabal);

    std::string description() const;
    void setDescription(const std::string &description);

    std::string license() const;
    void setLicense(const std::string &license);

    std::list<File> &files();
    void setFiles(const std::list<File> &files);

private:
    void handleTag(const std::string &tag, json_t value);

    static std::string readStringTag(json_t *in);

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

    static std::map<std::string, Tag> ManifestTags;
    static std::list<std::string> ManifestTag;
    static std::map<Tag, std::function<void(Manifest &, json_t *)>> ReadTagFunctions;
};

}

#endif /* _MANIFEST_H */
