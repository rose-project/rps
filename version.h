/**
 * @file version.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief API for package versions.
 */
#ifndef _VERSION_H
#define _VERSION_H

#include "err.h"

enum MPK_VERSION_BUILDTYPE {
    MPK_VERSION_BUILDTYPE_UNKNOWN = -1,
    MPK_VERSION_BUILDTYPE_TEST,       /* T */
    MPK_VERSION_BUILDTYPE_DEBUG,      /* D */
    MPK_VERSION_BUILDTYPE_ALPHA,      /* A */
    MPK_VERSION_BUILDTYPE_BETA,       /* B */
    MPK_VERSION_BUILDTYPE_PRERELEASE, /* P */
    MPK_VERSION_BUILDTYPE_RELEASE     /* R */
};

enum MPK_VERSION_OPERATOR {
    MPK_VERSION_OPERATOR_UNKNOWN,
    MPK_VERSION_OPERATOR_LESS,
    MPK_VERSION_OPERATOR_LESS_OR_EQUAL,
    MPK_VERSION_OPERATOR_GREATER,
    MPK_VERSION_OPERATOR_GREATER_OR_EQUAL,
    MPK_VERSION_OPERATOR_EQUAL
};

struct mpk_version {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
    enum MPK_VERSION_BUILDTYPE buildtype;
    unsigned long long int buildid; /* fixed length of 14 digits */
};
extern const struct mpk_version MPK_VERSION_DEFAULT;

#define MPK_VERSION_BUILDID_SIZE 14
#define MPK_VERSION_BUILDID_UNFEDINED 0xffffffffffffffff

mpk_ret_t mpk_version_deserialize(struct mpk_version *v, int *len, char *data, int data_size);

#endif /* _VERSION_H */
