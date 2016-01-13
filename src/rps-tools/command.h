#ifndef RPS_TOOLS_COMMAND_H
#define RPS_TOOLS_COMMAND_H

#include <string>
#include <list>
#include <memory>

namespace RPS {
namespace Tools {

class Command
{
public:
    Command();

    enum class Type {
        Status,
        Install,
        Remove,
        ChangeRelease,
        Create,
        Help,
        Version,
    };

    virtual void execute(char *argv[]) = 0;
};


} // namespace Tools
} // namespace RPS

#endif // RPS_TOOLS_COMMAND_H
