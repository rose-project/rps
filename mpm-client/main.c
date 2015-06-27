#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpk/stringlist.h>
#include <mpk/pkgref.h>
#include <mpk/defines.h>

#define MPM_CLIENT_VERSION "0.1.0"

enum COMMAND {
    COMMAND_UNDEFINED,
    COMMAND_VERSION,
    COMMAND_HELP,
    COMMAND_STATUS,
    COMMAND_INSTALL,
    COMMAND_REMOVE,
    COMMAND_RELEASE
};

void show_usage()
{
    fprintf(stderr, "usage: \n"
        "  mpm-client status\n"
        "  mpm-client install [PACKAGE ...]\n"
        "  mpm-client remove [PACKAGE ...]\n"
        "  mpm-client change-release RELEASE\n"
        "  mpm-client help\n"
        "  mpm-client version\n"
    );
}

void show_version()
{
    fprintf(stderr, "MPK Package Management Client Version %s\n",
        MPM_CLIENT_VERSION);
}

enum COMMAND parse_commandline(int argc, char *argv[],
    struct mpk_stringlist *pkgs, const char **param)
{
    enum COMMAND cmd = COMMAND_UNDEFINED;

    if (argc <= 1) {
        return COMMAND_UNDEFINED;
    }

    argv++;

    if (strcmp("help", *argv) == 0) {
        cmd = COMMAND_HELP;
    } else if (strcmp("version", *argv) == 0) {
        cmd = COMMAND_VERSION;
    } else if (strcmp("status", *argv) == 0) {
        cmd = COMMAND_STATUS;
    } else if (strcmp("install", *argv) == 0) {
        cmd = COMMAND_INSTALL;
        argv++;
        int cnt = 0;
        for(; *argv; argv++) {
            if (mpk_stringlist_add(pkgs, *argv) != MPK_SUCCESS) {
                cmd = COMMAND_UNDEFINED;
                mpk_stringlist_empty(pkgs);
                break;
            }
            cnt++;
        }
        if (!cnt) {
            cmd = COMMAND_UNDEFINED;
        }
    } else if (strcmp("remove", *argv) == 0) {
        cmd = COMMAND_REMOVE;
        argv++;
        int cnt = 0;
        for(; *argv; argv++) {
            if (mpk_stringlist_add(pkgs, *argv) != MPK_SUCCESS) {
                cmd = COMMAND_UNDEFINED;
                mpk_stringlist_empty(pkgs);
                break;
            }
            cnt++;
        }
        if (!cnt) {
            cmd = COMMAND_UNDEFINED;
        }
    } else if (strcmp("change-release", *argv) == 0) {
        cmd = COMMAND_RELEASE;
        argv++;
        if (!*argv || (*(argv + 1) != 0)) {
            cmd = COMMAND_UNDEFINED;
        } else {
            *param = *argv;
        }
    } else {
        cmd = COMMAND_UNDEFINED;
    }

    return cmd;
}

int install_packages(struct mpk_stringlist *packages)
{
    return MPK_FAILURE;
}

int remove_packages(struct mpk_stringlist *packages)
{
    return MPK_FAILURE;
}

int change_release(const char *release)
{
    return MPK_FAILURE;
}

int main(int argc, char *argv[])
{
    struct mpk_stringlist packages;
    const char *param = NULL;

    if (mpk_stringlist_init(&packages) != MPK_SUCCESS) {
        fprintf(stderr, "mpk_stringlist_init() failed\n");
        return EXIT_FAILURE;
    }

    switch (parse_commandline(argc, argv, &packages, &param)) {
    case COMMAND_STATUS:
        /* TODO: print status from database */
        fprintf(stderr, "command status is not yet implemented\n");
        break;
    case COMMAND_INSTALL:
        if (install_packages(&packages) != MPK_SUCCESS) {
            fprintf(stderr, "install_packages() failed\n");
        }
        break;
    case COMMAND_REMOVE:
        if (remove_packages(&packages) != MPK_SUCCESS) {
            fprintf(stderr, "remove_packages() failed\n");
        }
        break;
    case COMMAND_RELEASE:
        if (change_release(param) != MPK_SUCCESS) {
            fprintf(stderr, "change_release() failed\n");
        }
        break;
    case COMMAND_VERSION:
        show_version();
        break;
    case COMMAND_UNDEFINED:
    default:
        fprintf(stderr, "invalid arguments\n");
    case COMMAND_HELP:
        show_usage();
    }

    mpk_stringlist_empty(&packages);

    return EXIT_SUCCESS;
}
