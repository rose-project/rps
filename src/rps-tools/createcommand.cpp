#include <iostream>
#include <libgen.h>
#include "createcommand.h"

#include <string>

namespace RPS {
namespace Tools {

CreateCommand::CreateCommand()
{

}

void CreateCommand::execute(char *argv[])
{
    // parse command line

    std::string package_name, source_dir, out_dir;

    while (argv) {
        if (!argv[0] || !argv[1])
            break;

        if (argv[0] == std::string("-d")) {
            source_dir = dirname(argv[1]);
            package_name = basename(argv[1]);
            argv += 2;
            continue;
        }

        if (argv[0] == std::string("-o")) {
            out_dir = argv[1];
            argv += 2;
            continue;
        }
    }

    if (source_dir.empty() || package_name.empty())
        throw "source is not set";

    if (out_dir.empty())
        out_dir = ".";

    std::cout << "create package '" << package_name << "' from '" << source_dir
        << "' in '" << out_dir << "'." << std::endl;

    // pack the package


    // sign the package
    // TODO
}

} // namespace Tools
} // namespace RPS

