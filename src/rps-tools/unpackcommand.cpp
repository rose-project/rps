#include <string>
#include <rps/package.h>
#include "unpackcommand.h"

namespace RPS {
namespace Tools {

UnpackCommand::UnpackCommand()
{

}

void UnpackCommand::execute(char *argv[])
{
    // parse command line

    std::string package_path, out_dir;

    while (argv) {
        if (!argv[0] || !argv[1])
            break;

        if (argv[0] == std::string("-f")) {
            package_path = std::string(argv[1]);
            argv += 2;
            continue;
        }

        if (argv[0] == std::string("-o")) {
            out_dir = argv[1];
            argv += 2;
            continue;
        }
    }

    // unpack the package
    Package pkg(package_path);

}

} // namespace Tools
} // namespace RPS

