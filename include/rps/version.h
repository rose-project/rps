/**
 * @file version.h
 * @brief API for package versions.
 */
#ifndef _VERSION_H
#define _VERSION_H

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <list>

namespace rose {

struct VersionInterval {
    int32_t start;
    int32_t end;
};

struct Dependency {
    std::string name;
    std::list<VersionInterval> requires;
    std::list<VersionInterval> conflicts;
};

}

#endif /* _VERSION_H */
