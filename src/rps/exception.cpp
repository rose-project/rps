#include "rps/exception.h"

namespace RPS {

Exception::Exception(std::string reason) noexcept
    : mReason(reason)
{
}

Exception::~Exception() noexcept
{

}

const char *Exception::what() const noexcept
{
    return mReason.c_str();
}

} // namespace RPS

