#ifndef RPS_TOOLS_COMMAND_H
#define RPS_TOOLS_COMMAND_H

#include <string>
#include <vector>
#include <list>
#include <memory>

namespace rose {
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

    virtual void execute(std::vector<std::string> &arguments) = 0;
};


} // namespace Tools
} // namespace RPS

#endif // RPS_TOOLS_COMMAND_H
