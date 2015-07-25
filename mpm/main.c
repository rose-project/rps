#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mpk/mpk.h>

static char *param0 = NULL, *param1 = NULL, *param2 = NULL;

enum COMMAND {
    COMMAND_UNDEFINED = -1,
    COMMAND_INSTALL,
    COMMAND_REMOVE,
    COMMAND_CREATE,
    COMMAND_UNPACK,
    COMMAND_HELP,
    COMMAND_VERSION
};

void show_banner()
{
    printf("MPK Package Manager %d.%d.%d\n", 0, 0, 0);
}

void show_usage()
{
    printf("Usage: \n"
           "    mpm <command> [<args>]\n"
           "    mpm {-h | --help}\n"
           "    mpm {-v | --version}\n"
           "Commands: \n"
           "    install [-p prefix] pkg\n"
           "    remove pkg\n"
           "    create [-k private_key] [-o output_dir] source_dir\n"
           "    unpack [-o output_dir] pkg\n");
}

enum COMMAND parse_commandline(int argc, char *argv[])
{
    if (argc < 2)
        return COMMAND_UNDEFINED;

    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
        return COMMAND_HELP;

    if (argc == 2 && (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")))
        return COMMAND_VERSION;

    if (!strcmp(argv[1], "install")) {
        int i = 2;
        param1 = NULL;
        while (i < argc) {
            if (i + 1 == argc) {
                param0 = argv[i];
                return COMMAND_INSTALL;
            } else if (i + 1 < argc && !strcmp(argv[i], "-p")) {
                param1 = argv[i + 1];
                i += 2;
            } else {
                return COMMAND_UNDEFINED;
            }
        }
        return COMMAND_UNDEFINED;
    }

    if (!strcmp(argv[1], "remove")) {
        int i = 1;
        while (i < argc) {
            if (i + 1 == argc) {
                param0 = argv[i];
                return COMMAND_REMOVE;
            } else {
                return COMMAND_UNDEFINED;
            }
        }
        return COMMAND_UNDEFINED;
    }

    if (!strcmp(argv[1], "create")) {
        int i = 2;
        while (i < argc) {
            if (i + 1 == argc) {
                param0 = argv[i];
                return COMMAND_CREATE;
            } else if (i + 1 < argc && !strcmp(argv[i], "-k")) {
                param1 = argv[i + 1];
                i += 2;
            } else if (i + 1 < argc && !strcmp(argv[i], "-o")) {
                param2 = argv[i + 1];
                i += 2;
            } else {
                return COMMAND_UNDEFINED;
            }
        }
    }

    if (!strcmp(argv[1], "unpack")) {
        int i = 2;
        while (i < argc) {
            if (i + 1 == argc) {
                param0 = argv[i];
                return COMMAND_UNPACK;
            } else if (i + 1 < argc && !strcmp(argv[i], "-o")) {
                param1 = argv[i + 1];
                i += 2;
            } else {
                return COMMAND_UNDEFINED;
            }
        }
    }

    return COMMAND_UNDEFINED;
}

int run_install(const char *package_fpath, const char *prefix)
{
    /* TODO:
     * - request package install from backend > dependency resoltion on backend
     * - receive package url and signature from backend
     * - download package
     * - verify package signature
     */

    /* install */
    if (mpk_install(package_fpath, prefix) != MPK_SUCCESS)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int run_remove(const char *package_name)
{

    return 0;
}

int run_create(const char *src_dir, const char *dst_dir, const char *key_fpath)
{
    if (mpk_init() != MPK_SUCCESS)
        return 1;

    printf("create package from: %s\n", src_dir);

    if (mpk_create(src_dir, dst_dir, key_fpath) != MPK_SUCCESS)
        return 1;

    mpk_deinit();

    return 0;
}

int run_unpack(const char *package_fpath, const char *dst_dir)
{
    /* extract */
    if (mpk_unpack(package_fpath, dst_dir) != MPK_SUCCESS)
        return 1;

    return 0;
}

int main(int argc, char *argv[])
{
    enum COMMAND cmd;

    show_banner();

    switch (parse_commandline(argc, argv)) {
    case COMMAND_INSTALL:
        return run_install(param0, param1);
    case COMMAND_REMOVE:
        return run_remove(param0);
    case COMMAND_CREATE:
        return run_create(param0, param2, param1);
    case COMMAND_UNPACK:
        return run_unpack(param0, param1);
    case COMMAND_UNDEFINED:
    default:
        show_usage();
        return 1;
    }

    return EXIT_SUCCESS;
}

