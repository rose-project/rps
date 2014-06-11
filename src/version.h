/**
 * @file version.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief API for package versions.
 */
#ifndef _VERSION_H
#define _VERSION_H

#include <stdio.h>
#include <stdint.h>
#include "err.h"

enum MPK_VERSION_BUILDTYPE {
    MPK_VERSION_BUILDTYPE_UNKNOWN = -1,
    MPK_VERSION_BUILDTYPE_TEST,       /* T */
    MPK_VERSION_BUILDTYPE_DEBUG,      /* D */
    MPK_VERSION_BUILDTYPE_ALPHA,      /* A */
    MPK_VERSION_BUILDTYPE_BETA,       /* B */
    MPK_VERSION_BUILDTYPE_PRERELEASE, /* P */
    MPK_VERSION_BUILDTYPE_RELEASE,    /* R */
    MPK_VERSION_BUILDTYPE_COUNT
};

enum MPK_VERSION_OPERATOR {
    MPK_VERSION_OPERATOR_UNKNOWN,
    MPK_VERSION_OPERATOR_LESS,
    MPK_VERSION_OPERATOR_LESS_OR_EQUAL,
    MPK_VERSION_OPERATOR_GREATER,
    MPK_VERSION_OPERATOR_GREATER_OR_EQUAL,
    MPK_VERSION_OPERATOR_EQUAL,
    MPK_VERSION_OPERATOR_COUNT
};

extern const char *mpk_version_operator_strings[];

struct mpk_version {
    int32_t major;
    int32_t minor;
    int32_t patch;
    enum MPK_VERSION_BUILDTYPE buildtype;
    int64_t buildid; /* fixed length of 16 digits */
};
extern const struct mpk_version MPK_VERSION_DEFAULT;

#define MPK_VERSION_BUILDID_SIZE 16
#define MPK_VERSION_BUILDID_UNFEDINED -1

int mpk_version_isempty(struct mpk_version *v);

int mpk_version_serializedsize(struct mpk_version *v);

mpk_ret_t mpk_version_serialize(char *dst, int *written, int len, struct mpk_version *v);

mpk_ret_t mpk_version_deserialize(struct mpk_version *v, int *len, char *data,
    int data_size);

mpk_ret_t mpk_version_operator_deserialize(enum MPK_VERSION_OPERATOR *op,
    int *len, char *data, int data_size);

mpk_ret_t mpk_version_operator_print(FILE *f, enum MPK_VERSION_OPERATOR);

mpk_ret_t mpk_version_print(FILE *f, struct mpk_version *v);

#endif /* _VERSION_H */
