/**
 * @file version.h
 * @brief API for package versions.
 */
#ifndef _VERSION_H
#define _VERSION_H

#include <stdio.h>
#include <stdint.h>

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

/**
 * @brief Contains the version information of a package.
 *
 * A version consistst of a #major, #minor and #patch number, the #buildtype
 * and a unique build ID (#buildid). The build ID consist of 16 decimal digits
 * which include a timestamp an some more digits which can be used to
 * differentiate builds with the same timestamp.
 */
struct mpk_version {
    int32_t major; /**< The major version number. */
    int32_t minor; /**< The minor version number. */
    int32_t patch; /**< The patch level number. */
    enum MPK_VERSION_BUILDTYPE buildtype; /**< The build type. */
    int64_t buildid; /**< Fixed unique build ID (16 decimal digits) */
};
extern const struct mpk_version MPK_VERSION_DEFAULT;

#define MPK_VERSION_BUILDID_SIZE 16
#define MPK_VERSION_BUILDID_UNFEDINED -1

int mpk_version_isempty(struct mpk_version *v);

int mpk_version_serializedsize(struct mpk_version *v);

int mpk_version_serialize(char *dst, int *written, int len,
    struct mpk_version *v);

int mpk_version_deserialize(struct mpk_version *v, int *len, const char *data,
    int data_size);

int mpk_version_operator_deserialize(enum MPK_VERSION_OPERATOR *op,
    int *len, char *data, int data_size);

int mpk_version_operator_print(FILE *f, enum MPK_VERSION_OPERATOR);

int mpk_version_print(FILE *f, struct mpk_version *v);

#endif /* _VERSION_H */
