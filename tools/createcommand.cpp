#include "createcommand.h"
#include <rps/manifest.h>
#include <rps/package.h>
#include <libgen.h>
#include <sys/stat.h>
#include <cstddef>
#include <iostream>
#include <string>

namespace rose
{
namespace Tools
{

CreateCommand::CreateCommand() {}

void CreateCommand::execute(std::vector<std::string> &arguments)
{
    // parse command line

    std::string package_name, source_dir, out_dir;

    for (std::vector<std::string>::iterator it = arguments.begin(); arguments.end() - it >= 1;
         it += 2) {
        if (*it == std::string("-d")) {
            std::string path(*(it + 1));

            const auto last_slash_pos = path.find_last_of('/');
            package_name = path.substr(last_slash_pos + 1);
            source_dir = path.substr(0, last_slash_pos);
            continue;
        }

        if (*it == std::string("-o")) {
            out_dir = *(it + 1);
            continue;
        }
    }

    if (source_dir.empty() || package_name.empty())
        throw "source is not set";

    if (out_dir.empty())
        out_dir = ".";

    std::cout << "create package '" << package_name << "' from '" << source_dir << "' in '"
              << out_dir << "'." << std::endl;

    // pack the package

    rose::Package pkg;
    pkg.readPackageDir(source_dir + "/" + package_name);

    //    pkg.signPackage();

    pkg.writePackge(out_dir);
}

} // namespace Tools
} // namespace rose
