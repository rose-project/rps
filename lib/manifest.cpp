/**
 * @file manifest.c
 */
#include "rps/manifest.h"
#include "rps/defines.h"
#include "stringhelper.h"
#include <jansson.h>
#include <memory.h>
#include <syslog.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace rose
{

std::map<std::string, Manifest::Tag> Manifest::ManifestTags{{"manifest", Manifest::Tag::Manifest},
    {"name", Manifest::Tag::Name}, {"version", Manifest::Tag::Version}, {"api", Manifest::Tag::API},
    {"arch", Manifest::Tag::Arch}, {"localization", Manifest::Tag::Localization},
    {"depends", Manifest::Tag::Depends}, {"source", Manifest::Tag::Source},
    {"vendor", Manifest::Tag::Vendor}, {"label", Manifest::Tag::Label},
    {"version-label", Manifest::Tag::VersionLabel}, {"description", Manifest::Tag::Description},
    {"license", Manifest::Tag::License}, {"files", Manifest::Tag::Files}};

std::map<Manifest::Tag, std::function<void(Manifest &, json_t *)>> Manifest::ReadTagFunctions{
    {Manifest::Tag::Manifest, Manifest::readTagManifest},
    {Manifest::Tag::Name, Manifest::readTagName},
    {Manifest::Tag::Version, Manifest::readTagVersion}, {Manifest::Tag::API, Manifest::readTagAPI},
    {Manifest::Tag::Arch, Manifest::readTagArch},
    {Manifest::Tag::Localization, Manifest::readTagLocalization},
    {Manifest::Tag::Depends, Manifest::readTagDepends},
    {Manifest::Tag::Source, Manifest::readTagSource},
    {Manifest::Tag::Vendor, Manifest::readTagVendor},
    {Manifest::Tag::Label, Manifest::readTagLabel},
    {Manifest::Tag::VersionLabel, Manifest::readTagVersionLabel},
    {Manifest::Tag::Description, Manifest::readTagDescription},
    {Manifest::Tag::License, Manifest::readTagLicense},
    {Manifest::Tag::Files, Manifest::readTagFiles}};

Manifest::Manifest() {}

Manifest::~Manifest() {}

void Manifest::readFromFile(std::string filename)
{
    json_t *root = json_load_file(filename.c_str(), 0, NULL);

    std::cout << "read manifest: " << filename << std::endl;

    if (!root)
        throw "cannot read manifest";

    const char *key;
    json_t *value;
    json_object_foreach(root, key, value)
    {
        Tag t = ManifestTags[key];
        if (t == Tag::Undefined)
            throw "invalid key in manifest";

        ReadTagFunctions[t](*this, value);
    }

    json_decref(root);
}

void Manifest::writeManifestFile(std::string filename)
{
    json_t *root;

    root = json_object();

    // manifest
    if (json_object_set_new(root, "manifest", json_string("1.0")) != 0) {
        json_decref(root);
        throw "cannot write manifest version tag";
    }

    // name
    if (json_object_set_new(root, "name", json_string(mPackageName.c_str())) != 0) {
        json_decref(root);
        throw "cannot write package name";
    }

    // version
    if (json_object_set_new(root, "version", json_integer(mPackageVersion)) != 0) {
        json_decref(root);
        throw "cannot write package version";
    }

    // API level

    json_t *api_item = json_object();
    if (!api_item) {
        json_decref(root);
        throw "json_object() failed";
    }
    if (json_object_set_new(api_item, "min", json_integer(mApiMin)) != 0) {
        json_decref(api_item);
        json_decref(root);
        throw "cannot write api version: mininum";
    }
    if (json_object_set_new(api_item, "target", json_integer(mApiTarget)) != 0) {
        json_decref(api_item);
        json_decref(root);
        throw "cannot write api version: target";
    }
    if (json_object_set_new(api_item, "max", json_integer(mApiMax)) != 0) {
        json_decref(api_item);
        json_decref(root);
        throw "cannot write api version: maximum";
    }
    if (json_object_set_new(root, "api", api_item) != 0) {
        json_decref(root);
        throw "cannot add api info";
    }

    // arch

    if (json_object_set_new(root, "arch", json_string(mTargetArch.c_str())) != 0) {
        json_decref(root);
        throw "cannot write arch";
    }

    // Localization
    json_t *localization_item = json_array();
    if (!localization_item) {
        json_decref(root);
        throw "json_object() failed";
    }
    if (json_object_set_new(root, "localization", localization_item) != 0) {
        json_decref(localization_item);
        json_decref(root);
        throw "cannot add locales";
    }
    for (auto &i : mLocales) {
        if (json_array_append_new(localization_item, json_string(i.c_str())) != 0) {
            json_decref(root);
            throw "cannot add locale";
        }
    }

    // depends
    json_t *depends_item = json_array();
    if (!depends_item) {
        json_decref(root);
        throw "json_object() failed";
    }
    if (json_object_set_new(root, "depends", depends_item) != 0) {
        json_decref(depends_item);
        json_decref(root);
        throw "cannot add 'depends'";
    }
    for (auto &i : mDependencies) {
        // TODO
    }

    // source

    if (json_object_set_new(root, "source", json_string(mSource.c_str())) != 0) {
        json_decref(root);
        throw "cannot write source";
    }

    // vendor

    if (json_object_set_new(root, "vendor", json_string(mVendor.c_str())) != 0) {
        json_decref(root);
        throw "cannot write vendor";
    }

    // label

    if (json_object_set_new(root, "label", json_string(mPackageLabel.c_str())) != 0) {
        json_decref(root);
        throw "cannot write label";
    }

    // version label

    if (json_object_set_new(root, "version-label", json_string(mVersionLabal.c_str())) != 0) {
        json_decref(root);
        throw "cannot write version-label";
    }

    // description

    if (json_object_set_new(root, "description", json_string(mDescription.c_str())) != 0) {
        json_decref(root);
        throw "cannot write description";
    }

    // license

    if (json_object_set_new(root, "license", json_string(mLicense.c_str())) != 0) {
        json_decref(root);
        throw "cannot write license";
    }

    // files
    json_t *files_item = json_array();
    if (!files_item) {
        json_decref(root);
        throw "json_object() failed";
    }
    if (json_object_set_new(root, "files", files_item) != 0) {
        json_decref(files_item);
        json_decref(root);
        throw "cannot write files section";
    }
    for (auto &i : mFiles) {
        json_t *file_item = json_object();
        if (!file_item) {
            json_decref(root);
            throw "json_object() failed";
        }
        if (json_array_append_new(files_item, file_item) != 0) {
            json_decref(file_item);
            json_decref(root);
            throw "cannot write files section";
        }
        if (json_object_set_new(file_item, "name", json_string(i.name().c_str())) != 0) {
            json_decref(root);
            throw "cannot add file";
        }
        std::cerr << "added file: " << i.name().c_str() << std::endl;
    }

    if (json_dump_file(root, filename.c_str(), JSON_INDENT(4) | JSON_PRESERVE_ORDER) != 0) {
        json_decref(root);
        throw "json_dump_file failed";
    }

    json_decref(root);
}

std::string Manifest::packageName() const { return mPackageName; }

void Manifest::setPackageName(const std::string &packageName) { mPackageName = packageName; }

std::string Manifest::readStringTag(json_t *in)
{
    const char *s = json_string_value(in);
    if (!s)
        throw "connot source section";

    return std::string(s);
}

void Manifest::readTagManifest(Manifest &mfst, json_t *in)
{
    std::cout << "read tag manifest" << std::endl;

    const char *str = json_string_value(in);
    if (!str)
        throw "connot read manifest version";

    if (std::string(str) == "1.0") {
        mfst.setManifestVersion(ManifestVersion::Version1_0);
    } else {
        throw "invalid manifest version";
    }
}

void Manifest::readTagName(Manifest &mfst, json_t *in)
{
    std::cout << "read tag name" << std::endl;

    const char *str = json_string_value(in);
    if (!str)
        throw "connot read package name";

    mfst.setPackageName(str);
}

void Manifest::readTagVersion(Manifest &mfst, json_t *in)
{
    std::cout << "read tag version" << std::endl;

    if (json_typeof(in) != JSON_INTEGER)
        throw "connot read version";

    int v = json_integer_value(in);

    mfst.setPackageVersion(v);
}

void Manifest::readTagAPI(Manifest &mfst, json_t *in)
{
    std::cout << "read tag API" << std::endl;

    if (!in || json_typeof(in) != JSON_OBJECT)
        throw "invalid arguments";

    const char *key;
    json_t *val;
    json_object_foreach(in, key, val)
    {
        if (!key)
            throw "invalid data in section 'api'";
        int v = json_integer_value(val);

        std::cout << key << ": " << v << std::endl;

        if (std::string("min") == key)
            mfst.setApiMin(v);
        else if (std::string("max") == key)
            mfst.setApiMax(v);
        else if (std::string("target") == key)
            mfst.setApiTarget(v);
        else
            throw "invalid data";
    }
}

void Manifest::readTagArch(Manifest &mfst, json_t *in)
{
    std::cout << "read tag ABI" << std::endl;

    if (json_typeof(in) != JSON_STRING)
        throw "connot read tag 'abi'";

    const char *str = json_string_value(in);

    mfst.setTargetArch(str);
}

void Manifest::readTagLocalization(Manifest &mfst, json_t *in)
{
    std::cout << "read tag localization" << std::endl;

    if (!in || json_typeof(in) != JSON_ARRAY)
        throw "invalid arguments";

    int i;
    json_t *value;
    json_array_foreach(in, i, value)
    {
        const char *str = json_string_value(value);
        if (!str)
            throw "invalid data";

        mfst.locales().push_back(std::string(str));
    }
}

void Manifest::readTagDepends(Manifest &mfst, json_t *in)
{
    std::cout << "read tag depends" << std::endl;

    if (!in || json_typeof(in) != JSON_ARRAY)
        throw "invalid arguments";

    int i;
    json_t *pkg;
    json_array_foreach(in, i, pkg)
    {
        if (json_typeof(pkg) != JSON_OBJECT)
            throw "invalid data in section 'depends'";

        json_t *name = json_object_get(pkg, "name");
        if (!name || json_typeof(name) != JSON_STRING)
            throw "no package name for dependency";

        const char *name_str = json_string_value(name);

        std::cout << "depends: " << name_str << std::endl;

        json_t *requires = json_object_get(pkg, "requires");
        if (requires) {
            // TODO: read versions
        }

        json_t *conflicts = json_object_get(pkg, "conflicts");
        if (requires) {
            // TODO: read versions
        }
    }
}

void Manifest::readTagSource(Manifest &mfst, json_t *in)
{
    std::cout << "read tag source" << std::endl;

    const char *str = json_string_value(in);
    if (!str)
        throw "connot source section";

    mfst.setSource(str);
}

void Manifest::readTagVendor(Manifest &mfst, json_t *in)
{
    std::cout << "read tag vendor" << std::endl;

    const char *str = json_string_value(in);
    if (!str)
        throw "connot vendor section";

    mfst.setVendor(str);
}

void Manifest::readTagLabel(Manifest &mfst, json_t *in)
{
    std::cout << "read tag label" << std::endl;

    mfst.setPackageLabel(readStringTag(in));
}

void Manifest::readTagVersionLabel(Manifest &mfst, json_t *in)
{
    std::cout << "read tag version label" << std::endl;

    mfst.setVersionLabal(readStringTag(in));
}

void Manifest::readTagDescription(Manifest &mfst, json_t *in)
{
    std::cout << "read tag description" << std::endl;

    mfst.setDescription(readStringTag(in));
}

void Manifest::readTagLicense(Manifest &mfst, json_t *in)
{
    std::cout << "read tag license" << std::endl;

    mfst.setLicense(readStringTag(in));
}

void Manifest::readTagFiles(Manifest &mfst, json_t *in)
{
    std::cout << "read tag files" << std::endl;

    if (!in || json_typeof(in) != JSON_ARRAY)
        throw "invalid arguments";

    int i;
    json_t *pkg;
    json_array_foreach(in, i, pkg)
    {
        if (json_typeof(pkg) != JSON_OBJECT)
            throw "invalid data in section 'depends'";

        json_t *file_name = json_object_get(pkg, "name");
        if (!file_name || json_typeof(file_name) != JSON_STRING)
            throw "no package name";

        File f;

        const char *name_str = json_string_value(file_name);
        f.setName(name_str);

        std::cout << "file: " << name_str << std::endl;

        json_t *file_type = json_object_get(pkg, "type");
        if (file_type) {
            // TODO
        }

        json_t *file_hash = json_object_get(pkg, "hash");
        if (file_hash) {
            // TODO
        }

        mfst.files().push_back(f);
    }
}

std::list<File> &Manifest::files() { return mFiles; }

void Manifest::setFiles(const std::list<File> &files) { mFiles = files; }

std::string Manifest::license() const { return mLicense; }

void Manifest::setLicense(const std::string &license) { mLicense = license; }

std::string Manifest::description() const { return mDescription; }

void Manifest::setDescription(const std::string &description) { mDescription = description; }

std::string Manifest::versionLabal() const { return mVersionLabal; }

void Manifest::setVersionLabal(const std::string &versionLabal) { mVersionLabal = versionLabal; }

std::string Manifest::packageLabel() const { return mPackageLabel; }

void Manifest::setPackageLabel(const std::string &packageLabel) { mPackageLabel = packageLabel; }

std::string Manifest::vendor() const { return mVendor; }

void Manifest::setVendor(const std::string &vendor) { mVendor = vendor; }

std::string Manifest::source() const { return mSource; }

void Manifest::setSource(const std::string &source) { mSource = source; }

std::list<Dependency> Manifest::dependencies() const { return mDependencies; }

void Manifest::setDependencies(const std::list<Dependency> &dependencies)
{
    mDependencies = dependencies;
}

void Manifest::setLocales(const std::list<std::string> &locales) { mLocales = locales; }

std::string Manifest::targetArch() const { return mTargetArch; }

void Manifest::setTargetArch(const std::string &targetArch) { mTargetArch = targetArch; }

std::list<std::string> &Manifest::locales() { return mLocales; }

int32_t Manifest::apiMax() const { return mApiMax; }

void Manifest::setApiMax(const int32_t apiMax) { mApiMax = apiMax; }

int32_t Manifest::apiTarget() const { return mApiTarget; }

void Manifest::setApiTarget(const int32_t apiTarget) { mApiTarget = apiTarget; }

int32_t Manifest::apiMin() const { return mApiMin; }

void Manifest::setApiMin(const int32_t apiMin) { mApiMin = apiMin; }

int32_t Manifest::packageVersion() const { return mPackageVersion; }

void Manifest::setPackageVersion(const int32_t packageVersion) { mPackageVersion = packageVersion; }

Manifest::ManifestVersion Manifest::manifestVersion() const { return mManifestVersion; }

void Manifest::setManifestVersion(const ManifestVersion version) { mManifestVersion = version; }

} // namespace rose
