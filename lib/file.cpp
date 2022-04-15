/**
 * @file file.c
 */
#include "rps/file.h"
#include "rps/defines.h"
#include "stringhelper.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace rose
{

File::File() {}

File::~File() {}

std::string File::name() const { return mName; }

void File::setName(const std::string &name) { mName = name; }

const std::vector<uint8_t> &File::hash() const { return mHash; }

void File::setHash(const std::vector<uint8_t> &hash) { mHash = hash; }

} // namespace rose
