#include <libgen.h>
#include <iostream>
#include <cstddef>
#include <string>
#include <rps/manifest.h>
#include "createcommand.h"

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
            std::string path(argv[1]);

            const auto last_slash_pos = path.find_last_of('/');
            package_name = path.substr(last_slash_pos + 1);
            source_dir =  path.substr(0, last_slash_pos);
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

    RPS::Manifest mfst;
    mfst.readFromFile(source_dir + "/" + package_name + "/manifest.json");



    // sign the package
    // TODO
}

} // namespace Tools
} // namespace RPS

