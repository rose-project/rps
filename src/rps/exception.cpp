#include "rps/exception.h"

namespace RPS {

Exception::Exception(std::string reason)
    : mReason(reason)
{
}

} // namespace RPS

