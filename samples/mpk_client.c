#include <stdlib.h>
#include <stdio.h>
#include <mpk/mpk.h>

enum COMMAND {
    COMMAND_UNDEFINED = -1,
    COMMAND_UPDATE,
    COMMAND_INSTALL,
    COMMAND_UNINSTALL,
    COMMAND_CLEANUP,
    COMMAND_RESTORE,
    COMMAND_CREATE,
    COMMAND_SEARCH,
    COMMAND_INIT
};

int run_install(const char *package)
{
    
    return 0;
}

int main(int argc, char *argv[])
{
    printf("mpk-client");

    if (mpk_init() != MPK_SUCCESS) {
        return EXIT_FAILURE;
    }

    if (mpk_create(TESTDATA_DIR "testpackage", "/tmp",
            TESTDATA_DIR "privkey.pem") != MPK_SUCCESS) {
        return EXIT_FAILURE;
    }

    mpk_deinit();

    return EXIT_SUCCESS;
}

