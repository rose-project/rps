#include <iostream>
#include <rps/exception.h>
#include "command.h"
#include "createcommand.h"
#include "unpackcommand.h"

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

    std::vector<std::string> arguments(argv, argv + argc);


    // call the command handler

    std::unique_ptr<RPS::Tools::Command> cmd;

    try {
        if (arguments[1] == std::string("create")) {
            cmd = std::make_unique<RPS::Tools::CreateCommand>();
        } else if (arguments[1] == std::string("unpack")) {
            cmd = std::make_unique<RPS::Tools::UnpackCommand>();
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
    } catch (RPS::Exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }


    return 0;
}
