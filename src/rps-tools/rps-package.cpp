#include <iostream>
#include "command.h"
#include "createcommand.h"

#define RPS_PKGTOOL_VERSION "0.1.0"

void show_usage()
{
    fprintf(stderr, "usage: \n"
        "  mpm-client create -d DIRECTORY [-o OUTPUT]\n"
        "  mpm-client unpack -f PACKAGE\n"
        "  mpm-client help\n"
        "  mpm-client version\n"
    );
}

void show_version()
{
    std::cerr << "ROSE Package Service - Package Tool " << RPS_PKGTOOL_VERSION << std::endl;
}

int main(int argc, char *argv[])
{
    show_version();


    if (argc < 2) {
        show_usage();
        return EXIT_FAILURE;
    }

    // call the command handler

    std::unique_ptr<RPS::Tools::Command> cmd;

    try {
        if (argv[1] == std::string("create")) {
            cmd = std::make_unique<RPS::Tools::CreateCommand>();
        } else if (argv[1] == std::string("unpack")) {
            //cmd = std::make_unique<RPS::Tools::UnpackCommand>();
        } else if (argv[1] == std::string("help")) {
            show_usage();
            return 0;
        } else if (argv[1] == std::string("version")) {
            show_version();
            return 0;
        } else {
            std::cerr << "unknown command. " << argv[1] << std::endl;
            show_usage();
            return EXIT_FAILURE;
        }

        cmd->execute(&argv[2]);

    } catch (const char *str) {
        std::cerr << "Error: " << str << std::endl;
        return EXIT_FAILURE;
    }


    return 0;
}
