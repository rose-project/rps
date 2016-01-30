#ifndef RPS_TOOLS_CREATECOMMAND_H
#define RPS_TOOLS_CREATECOMMAND_H

#include "command.h"

namespace RPS {
namespace Tools {

class CreateCommand : public Command
{
public:
    CreateCommand();

    virtual void execute(std::vector<std::string> &arguments);

};

} // namespace Tools
} // namespace RPS

#endif // RPS_TOOLS_CREATECOMMAND_H
