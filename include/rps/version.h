/**
 * @file version.h
 * @brief API for package versions.
 */
#ifndef _VERSION_H
#define _VERSION_H

#include <stdint.h>
#include <stdio.h>
#include <list>
#include <string>

namespace rose
{

struct VersionInterval {
    int32_t start;
    int32_t end;
};

struct Dependency {
    std::string name;
    std::list<VersionInterval>
    requires;
    std::list<VersionInterval> conflicts;
};

} // namespace rose

#endif /* _VERSION_H */
