#ifndef RPS_TOOLS_INSTALLCOMMAND_H
#define RPS_TOOLS_INSTALLCOMMAND_H

#include "command.h"

namespace rose {
namespace Tools {

class InstallCommand : public rose::Tools::Command
{
public:
    InstallCommand();

    virtual void execute(std::vector<std::string> &arguments);
};

} // namespace Tools
} // namespace RPS

#endif // RPS_TOOLS_INSTALLCOMMAND_H
