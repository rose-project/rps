#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include "command.h"
#include "installcommand.h"

void show_usage()
{
    fprintf(stderr, "usage: \n"
        "  rps-client status\n"
        "  rps-client install [PACKAGE ...]\n"
        "  rps-client remove [PACKAGE ...]\n"
        "  rps-client get-release RELEASE\n"
        "  rps-client help\n"
        "  rps-client version\n"
    );
}

void show_version()
{
    std::cerr << "ROSE Package Service Client" << RPS_VERSION
        << std::endl;
}

int main(int argc, char *argv[])
{
    show_version();


    if (argc < 2) {
        show_usage();
        return 1;
    }
    
    std::vector<std::string> arguments(argv, argv + argc);
    

    // call the command handler

    std::unique_ptr<rose::Tools::Command> cmd;

    try {
        if (arguments[1] == std::string("status")) {
            //cmd = std::make_unique<RPS::Tools::StatusCommand>();
        } else if (arguments[1] == std::string("install")) {
            cmd = std::make_unique<rose::Tools::InstallCommand>();
        } else if (arguments[1] == std::string("remove")) {
            //cmd = std::make_unique<RPS::Tools::StatusCommand>();
        } else if (arguments[1] == std::string("get-release")) {
            //cmd = std::make_unique<RPS::Tools::GetReleaseCommand>();
        } else if (arguments[1] == std::string("help")) {
            show_usage();
            return 0;
        } else if (arguments[1] == std::string("version")) {
            show_version();
            return 0;
        } else {
            std::cerr << "unknown command. " << arguments[1] << std::endl;
            show_usage();
            return 1;

        }

        arguments.erase(arguments.begin());
        arguments.erase(arguments.begin());

        cmd->execute(arguments);
    } catch (const char *str) {
        std::cerr << "Error: " << str << std::endl;
        return 1;
    }


    return 0;
}
