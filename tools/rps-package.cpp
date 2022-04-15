#include "command.h"
#include "createcommand.h"
#include "unpackcommand.h"
#include <rps/exception.h>
#include <iostream>

void show_usage()
{
    fprintf(stderr, "usage: \n"
                    "  rps-package create -d DIRECTORY [-o OUTPUT]\n"
                    "  rps-package unpack -f PACKAGE\n"
                    "  rps-package help\n"
                    "  rps-package version\n");
}

void show_version()
{
    std::cerr << "ROSE Package Service - Package Tool " << RPS_VERSION << std::endl;
}

int main(int argc, char *argv[])
{
    show_version();

    if (argc < 2) {
        show_usage();
        return EXIT_FAILURE;
    }

    std::vector<std::string> arguments(argv, argv + argc);

    // call the command handler

    std::unique_ptr<rose::Tools::Command> cmd;

    try {
        if (arguments[1] == std::string("create")) {
            cmd = std::make_unique<rose::Tools::CreateCommand>();
        } else if (arguments[1] == std::string("unpack")) {
            cmd = std::make_unique<rose::Tools::UnpackCommand>();
        } else if (arguments[1] == std::string("help")) {
            show_usage();
            return 0;
        } else if (arguments[1] == std::string("version")) {
            show_version();
            return 0;
        } else {
            std::cerr << "unknown command. " << arguments[1] << std::endl;
            show_usage();
            return EXIT_FAILURE;
        }

        arguments.erase(arguments.begin());
        arguments.erase(arguments.begin());

        cmd->execute(arguments);
    } catch (const char *str) {
        std::cerr << "Error: " << str << std::endl;
        return EXIT_FAILURE;
    } catch (rose::Exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
