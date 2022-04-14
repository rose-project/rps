#include "unpackcommand.h"
#include <rps/package.h>
#include <string>

namespace rose
{
namespace Tools
{

UnpackCommand::UnpackCommand() {}

void UnpackCommand::execute(std::vector<std::string> &arguments)
{
    // parse command line

    std::string package_path, out_dir;

    for (std::vector<std::string>::iterator it = arguments.begin(); arguments.end() - it >= 1;
         it += 2) {

        if (*it == std::string("-f")) {
            package_path = *(it + 1);
            continue;
        }

        if (*it == std::string("-o")) {
            out_dir = *(it + 1);
            continue;
        }
    }

    // unpack the package
    Package pkg(package_path);
}

} // namespace Tools
} // namespace rose
