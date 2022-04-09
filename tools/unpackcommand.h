#ifndef RPS_TOOLS_UNPACKCOMMAND_H
#define RPS_TOOLS_UNPACKCOMMAND_H

#include "command.h"

namespace rose {
namespace Tools {

class UnpackCommand : public rose::Tools::Command
{
public:
    UnpackCommand();

    virtual void execute(std::vector<std::string> &arguments);
};

} // namespace Tools
} // namespace RPS

#endif // RPS_TOOLS_UNPACKCOMMAND_H
