 /**
  * @file package.cpp
  */
#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <archive.h>
#include <archive_entry.h>
#include <boost/filesystem.hpp>
#include <rps/exception.h>
#include "rps/package.h"

/** The defaut size used for chunks of data used in for packing/unpacking and
 * signature generation generation
 */
#define CHUNKSIZE 512

namespace RPS {

const boost::filesystem::path Package::mWorkDir = "/tmp/rps";

Package::Package()
{
    setupWorkdir();
}

Package::Package(std::string package_file)
    : mPackagePath(package_file)
{
    setupWorkdir();
    readPackageFile(package_file);
}

Package::~Package()
{

}

void Package::readPackageFile(const std::string &package_path)
{
    if (!package_path.empty())
        mPackagePath = package_path;

    boost::filesystem::path unpacked_dir = mWorkDir;
    unpacked_dir /= mPackagePath.stem();
    mUnpackedDir = unpacked_dir;

    const void *buf;

    int r;

    struct archive *a = archive_read_new();
    struct archive_entry *entry;

    archive_read_support_compression_bzip2(a);
    archive_read_support_format_tar(a);


    struct archive *ext = archive_write_disk_new();
    int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    r = archive_read_open_filename(a, package_path.c_str(), 16384);
    if (r != ARCHIVE_OK) {
        archive_read_free(a);
        archive_write_free(ext);
        throw Exception(std::string("archive_read_open_filename() failed for file: ") + package_path);
    }

    while (true) {

        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF) {
            break;
        }
        if (r < ARCHIVE_OK) {
            std::cerr << archive_error_string(ext) << std::endl;
        }
        if (r < ARCHIVE_WARN) {
            archive_read_free(a);
            archive_write_free(ext);
            throw Exception(std::string("archive_read_next_header() failed:") + archive_error_string(a));
        }

        std::cout << std::string("extract: ") << archive_entry_pathname(entry) << std::endl;
        archive_entry_set_pathname(entry, (mUnpackedDir.string() + std::string("/") + archive_entry_pathname(entry)).c_str());

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK) {
            archive_read_free(a);
            archive_write_free(ext);
            throw Exception(std::string("archive_write_header() failed:") + archive_error_string(ext));
        } else if (archive_entry_size(entry) > 0) {

            size_t size;
            int64_t offset;
            while (true) {
                r = archive_read_data_block(a, &buf, &size, &offset);
                if (r == ARCHIVE_EOF)
                    break;
                if (r < ARCHIVE_WARN) {
                    archive_read_free(a);
                    archive_write_free(ext);
                    throw Exception(std::string(archive_error_string(ext)));
                }
                if (r < ARCHIVE_OK) {
                    std::cerr << archive_error_string(ext) << std::endl;
                    break;
                }

                r = archive_write_data_block(ext, buf, size, offset);
                if (r < ARCHIVE_WARN) {
                    archive_read_free(a);
                    archive_write_free(ext);
                    throw Exception(std::string(archive_error_string(ext)));
                }
                if (r < ARCHIVE_OK) {
                    std::cerr << archive_error_string(ext) << std::endl;
                    break;
                }
            }


            r = archive_write_finish_entry(ext);
            if (r < ARCHIVE_OK)
                std::cerr << archive_error_string(ext) << std::endl;
            if (r < ARCHIVE_WARN) {
                archive_read_free(a);
                archive_write_free(ext);
                throw Exception(std::string(archive_error_string(ext)));
            }
        }
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
}

void Package::readPackageDir(std::string package_dir)
{
    boost::filesystem::path pkgdir = package_dir;
    if (!boost::filesystem::exists(pkgdir))
        throw Exception("package directory '" + package_dir + "'does not exist");

    mManifest.readFromFile(package_dir + "/manifest.json");

    mUnpackedDir = package_dir;
}

void Package::signPackage(std::string priv_key)
{

}

void Package::verify(std::string pub_key)
{

}

void Package::writePackge(std::string dest_dir)
{
    if (mManifest.packageName().empty())
        throw Exception("package metatdata is not set");

    // create package workdir or delete old data
    boost::filesystem::path package_tmp_dir =
            std::string("/tmp/rps/") + mManifest.packageName();
    if (boost::filesystem::exists(package_tmp_dir))
        boost::filesystem::remove_all(package_tmp_dir);

    boost::filesystem::create_directory(package_tmp_dir);

    mManifest.writeManifestFile(package_tmp_dir.string() + "/manifest.json");

    // pack + compress
    pack();

    // TODO: copy to destination directory

}

void Package::setupWorkdir()
{
    // create rps working dir if not present
    boost::filesystem::path rps_tmp_dir = "/tmp/rps";
    if (!boost::filesystem::exists(rps_tmp_dir))
        boost::filesystem::create_directory(rps_tmp_dir);
}

void Package::pack()
{
    struct archive *a;
    char buf[8192];

    std::string package_base_filename(mManifest.packageName() + "-"
        + std::to_string(mManifest.packageVersion()) + "-"
        + mManifest.targetArch());

    std::string tbz2_path = mWorkDir.string() + "/"
        + package_base_filename + ".rps";

    a = archive_write_new();
    archive_write_add_filter_bzip2(a);
    archive_write_set_format_pax_restricted(a);
    archive_write_open_filename(a, tbz2_path.c_str());

    struct stat st;
    struct archive_entry *entry;

    for (auto &f: mManifest.files()) {
        std::string source = mUnpackedDir.string() + std::string("/data/") + f.name();
        std::string dest = std::string("data/") + f.name();
        stat(source.c_str(), &st);
        entry = archive_entry_new();
        archive_entry_set_pathname(entry, dest.c_str());
        archive_entry_set_size(entry, st.st_size);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_write_header(a, entry);
        int fd = open(source.c_str(), O_RDONLY);
        int len = read(fd, buf, sizeof(buf));
        while (len > 0) {
            archive_write_data(a, buf, len);
            len = read(fd, buf, sizeof(buf));
        }
        close(fd);
        archive_entry_free(entry);
    }

    archive_write_close(a);
    archive_write_free(a);
}

void Package::unpack()
{

}

} // namespace RPS
