#ifndef RPS_TOOLS_COMMAND_H
#define RPS_TOOLS_COMMAND_H

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace rose
{
namespace Tools
{

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
} // namespace rose

#endif // RPS_TOOLS_COMMAND_H
