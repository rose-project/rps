/**
 * @file file.c
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include "stringhelper.h"
#include "rps/defines.h"
#include "rps/file.h"

namespace RPS {

File::File()
{

}

File::~File()
{

}

std::string File::name() const
{
    return mName;
}

void File::setName(const std::string &name)
{
    mName = name;
}

const std::vector<uint8_t> &File::hash() const
{
    return mHash;
}

void File::setHash(const std::vector<uint8_t> &hash)
{
    mHash = hash;
}

} // namespace RPS
