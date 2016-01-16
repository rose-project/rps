#ifndef RPS_EXCEPTION_H
#define RPS_EXCEPTION_H

#include <exception>
#include <string>

namespace RPS {

class Exception : public std::exception
{
public:
    Exception(std::string reason);

private:
    std::string mReason;
};

} // namespace RPS

#endif // RPS_EXCEPTION_H