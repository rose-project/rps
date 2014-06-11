 /**
 * @file version.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <syslog.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "version.h"

static inline int digits_int(int i)
{
    if (i == 0) {
        return 1;
    } else if (i > 0) {
        return floor(log10(abs(i))) + 1;
    } else {
        return floor(log10(abs(i))) + 2;
    }
}

const struct mpk_version MPK_VERSION_DEFAULT = {
    -1, -1, -1, MPK_VERSION_BUILDTYPE_UNKNOWN, -1
};

/* has to be in sync with enum MPK_VERSION_OPERATOR */
const char *mpk_version_operator_strings[] = {
    "",
    "<",
    "<=",
    ">",
    ">=",
    "=="
};

int mpk_version_isempty(struct mpk_version *v)
{
    return (v->major == -1);
}

int mpk_version_serializedsize(struct mpk_version *v)
{
    int len = 0;

    if (!v) {
        return -1;
    }

    if (v->major < 0) {
        return len;
    }
    len += digits_int(v->major);

    if (v->minor < 0) {
        return len;
    }
    len += 1 + digits_int(v->minor);

    if (v->patch < 0) {
        return len;
    }
    len += 1 + digits_int(v->patch);

    if (v->buildtype != MPK_VERSION_BUILDTYPE_UNKNOWN) {
        if (v->buildid != MPK_VERSION_BUILDID_UNFEDINED) {
            /* buildid + buildtype */
            len += 2 + MPK_VERSION_BUILDID_SIZE;
        } else {
            /* buildtype without buildid */
            len += 2;
        }
    } else {
        if (v->buildid != MPK_VERSION_BUILDID_UNFEDINED) {
            len += 1 + MPK_VERSION_BUILDID_SIZE;
        }
    }

    return len;
}

mpk_ret_t mpk_version_serialize(char *dst, int *written, int len,
    struct mpk_version *v)
{
    int n;
    int w = 0;

    if (!dst || !v || len < 0)
        return MPK_FAILURE;

    if (v->major < 0) {
        if (written)
            *written = w;
        return MPK_SUCCESS;
    }
    n = snprintf(dst, len, "%d", v->major);
    w += n;
    dst += n;
    len -= n;

    if (v->minor < 0) {
        if (written)
            *written = w;
        return MPK_SUCCESS;
    }
    n = snprintf(dst, len, ".%d", v->minor);
    w += n;
    dst += n;
    len -= n;

    if (v->patch < 0) {
        if (written)
            *written = w;
        return MPK_SUCCESS;
    }
    n = snprintf(dst, len, ".%d", v->patch);
    w += n;
    dst += n;
    len -= n;

    if (v->buildtype != MPK_VERSION_BUILDTYPE_UNKNOWN ||
            v->buildid != MPK_VERSION_BUILDID_UNFEDINED) {
        n = snprintf(dst, len, "-");
        w += n;
        dst += n;
        len -= n;
    } else {
        if (written)
            *written = w;
        return MPK_SUCCESS;
    }

    switch (v->buildtype) {
    case MPK_VERSION_BUILDTYPE_TEST:
        n = snprintf(dst, len, "%c", 'T');
        break;
    case MPK_VERSION_BUILDTYPE_DEBUG:
        n = snprintf(dst, len, "%c", 'D');
        break;
    case MPK_VERSION_BUILDTYPE_ALPHA:
        n = snprintf(dst, len, "%c", 'A');
        break;
    case MPK_VERSION_BUILDTYPE_BETA:
        n = snprintf(dst, len, "%c", 'B');
        break;
    case MPK_VERSION_BUILDTYPE_PRERELEASE:
        n = snprintf(dst, len, "%c", 'P');
        break;
    case MPK_VERSION_BUILDTYPE_RELEASE:
        n = snprintf(dst, len, "%c", 'R');
        break;
    case MPK_VERSION_BUILDTYPE_UNKNOWN:
    default:
        n = 0;
        break;
    }
    w += n;
    dst += n;
    len -= n;

    if (v->buildid != MPK_VERSION_BUILDID_UNFEDINED) {
        if (len < MPK_VERSION_BUILDID_SIZE)
            return MPK_FAILURE;
        n = snprintf(dst, MPK_VERSION_BUILDID_SIZE + 1, "%.*lld",
            MPK_VERSION_BUILDID_SIZE, v->buildid);
        w += n;
        dst += n;
        len -= n;
    }

    if (written)
        *written = w;
    return MPK_SUCCESS;
}

mpk_ret_t mpk_version_deserialize(struct mpk_version *v, int *len, char *data,
    int data_size)
{
    int i = 0, n = 0, c = 0, idx = 0;
    char buf[32];
    enum MPK_VERSION_BUILDTYPE type = MPK_VERSION_BUILDTYPE_UNKNOWN;

    if (!v || !data) {
        syslog(LOG_ERR, "illegal values for parameter 'v'' or 'data'");
        return MPK_FAILURE;
    }

    while (i < data_size && n < 32) {
        if (isdigit(data[i])) {
            buf[n++] = data[i];
            c++;
        } else if (data[i] == 'T'
                || data[i] == 'D'
                || data[i] == 'A'
                || data[i] == 'B'
                || data[i] == 'P'
                || data[i] == 'R') {
            if (idx != 3) {
                return MPK_FAILURE;
            }
            switch (data[i]) {
            case 'T':
                v->buildtype = MPK_VERSION_BUILDTYPE_TEST;
                break;
            case 'D':
                v->buildtype = MPK_VERSION_BUILDTYPE_DEBUG;
                break;
            case 'A':
                v->buildtype = MPK_VERSION_BUILDTYPE_ALPHA;
                break;
            case 'B':
                v->buildtype = MPK_VERSION_BUILDTYPE_BETA;
                break;
            case 'P':
                v->buildtype = MPK_VERSION_BUILDTYPE_PRERELEASE;
                break;
            case 'R':
                v->buildtype = MPK_VERSION_BUILDTYPE_RELEASE;
                break;
            }
            c++;
            idx++;
        } else if (data[i] == '.') {
            if (n == 0)
                return MPK_FAILURE;
            c++;
            buf[n] = 0; /* terminate */
            switch(idx) {
            case 0:
                v->major = atoi(buf);
                break;
            case 1:
                v->minor = atoi(buf);
                break;
            case 2:
                v->patch = atoi(buf);
                break;
            default:
                return MPK_FAILURE;
            }
            n = 0;
            idx++;
        } else if (data[i] == '-') {
            buf[n] = 0; /* terminate */
            if (idx != 2)
                return MPK_FAILURE; /* '-' not allowed here */
            v->patch = (n == 0) ? 0 : atoi(buf);
            n = 0;
            c++;
            idx++;
        } else if (isblank(data[i]) || (data[i] == '\0' && (idx > 0)))  {
            buf[n] = 0; /* terminate */
            switch(idx) {
            case 0:
                if (n == 0) {
                    c++;
                    break; /* just a whitespaces in front */
                } else {
                    v->major = atoi(buf);
                    if (len)
                        *len = c;
                    return MPK_SUCCESS;
                }
                break;
            case 1:
                v->minor = (n == 0) ? 0 : atoi(buf);
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 2:
                v->patch = (n == 0) ? 0 : atoi(buf);
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 3:
                v->buildtype = type;
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 4:
                v->buildid = (n == 0) ? 0 : atoll(buf);
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            default:
                return -1;
            }
        } else if (data[i] == '#' || data[i] == '\r' || data[i] == '\n'
                || data[i] == ')') {
            buf[n] = 0; /* terminate */
            switch(idx) {
            case 0:
                if (n == 0) {
                    return MPK_FAILURE;
                } else {
                    v->major = atoi(buf);
                }
                *len = c;
                return MPK_SUCCESS;
            case 1:
                v->minor = (n == 0) ? 0 : atoi(buf);
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 2:
                v->patch = (n == 0) ? 0 : atoi(buf);
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 3:
                v->buildtype = type;
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 4:
                v->buildid = (n == 0) ? 0 : atoi(buf);
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            default:
                return MPK_FAILURE;
            }
        } else {
            return MPK_FAILURE; /* illegal symbol */
        }
        i++;
    }

    /* finish, if all input data was read */
    if (i == data_size) {
        buf[n] = 0; /* terminate */
        switch(idx) {
        case 0:
            if (n == 0) {
                return MPK_FAILURE;
            } else {
                v->major = atoi(buf);
            }
            *len = c;
            return MPK_SUCCESS;
        case 1:
            v->minor = (n == 0) ? 0 : atoi(buf);
            if (len)
                *len = c;
            return MPK_SUCCESS;
        case 2:
            v->patch = (n == 0) ? 0 : atoi(buf);
            if (len)
                *len = c;
            return MPK_SUCCESS;
        case 3:
            v->buildtype = type;
            if (len)
                *len = c;
            return MPK_SUCCESS;
        case 4:
            v->buildid = (n == 0) ? 0 : atoll(buf);
            if (len)
                *len = c;
            return MPK_SUCCESS;
        default:
            return MPK_FAILURE;
        }
    }

    /* we should not get to the following code */

    if (i >= data_size) {
        /* reached end of input buffer but could not read valid version info */
        syslog(LOG_ERR, "canot read version info");
    }

    if (n >= 32) {
        /* due to some strange input, we reached end of our buffer */
        syslog(LOG_ERR, "invalid version info data");
    }

    return MPK_FAILURE;
}

mpk_ret_t mpk_version_operator_deserialize(enum MPK_VERSION_OPERATOR *op,
    int *len, char *data, int data_size)
{
    if (!op || !data || data_size < 1)
        return MPK_FAILURE;

    if (data_size == 1) {
        switch (data[0]) {
        case '<':
            *op = MPK_VERSION_OPERATOR_LESS;
            if (len)
                *len = 1;
            return MPK_SUCCESS;
        case '>':
            *op = MPK_VERSION_OPERATOR_GREATER;
            if (len)
                *len = 1;
            return MPK_SUCCESS;
        default:
            return MPK_FAILURE;
        }
    }

    if (data_size == 2) {
        switch (data[0]) {
        case '<':
            switch (data[1]) {
            case 0:
                *op = MPK_VERSION_OPERATOR_LESS;
                if (len)
                    *len = 2;
                return MPK_SUCCESS;
            case '=':
                *op = MPK_VERSION_OPERATOR_LESS_OR_EQUAL;
                if (len)
                    *len = 2;
                return MPK_SUCCESS;
            default:
                return MPK_FAILURE;
            }
        case '>':
            switch (data[1]) {
            case 0:
                *op = MPK_VERSION_OPERATOR_GREATER;
                if (len)
                    *len = 2;
                return MPK_SUCCESS;
            case '=':
                *op = MPK_VERSION_OPERATOR_GREATER_OR_EQUAL;
                if (len)
                    *len = 2;
                return MPK_SUCCESS;
            default:
                return MPK_FAILURE;
            }
        case '=':
             switch (data[1]) {
            case 0:
                *op = MPK_VERSION_OPERATOR_EQUAL;
                if (len)
                    *len = 2;
                return MPK_SUCCESS;
            case '=':
                 *op = MPK_VERSION_OPERATOR_EQUAL;
                 if (len)
                     *len = 2;
                 return MPK_SUCCESS;
            default:
                return MPK_FAILURE;
             }
        default:
            return MPK_FAILURE;
        }
    }

    if (data_size == 3) {
        if (data[2] != 0)
            return MPK_FAILURE;
        switch (data[0]) {
        case '<':
            if (data[1] == '=') {
                *op = MPK_VERSION_OPERATOR_LESS_OR_EQUAL;
                if (len)
                    *len = 3;
                return MPK_SUCCESS;
            } else {
                return MPK_FAILURE;
            }
        case '>':
            if (data[1] == '=') {
                *op = MPK_VERSION_OPERATOR_GREATER_OR_EQUAL;
                if (len)
                    *len = 3;
                return MPK_SUCCESS;
            } else {
                return MPK_FAILURE;
            }
        case '=':
            if (data[1] == '=') {
                *op = MPK_VERSION_OPERATOR_EQUAL;
                if (len)
                    *len = 3;
                return MPK_SUCCESS;
            } else {
                return MPK_FAILURE;
            }
        default:
            return MPK_FAILURE;
        }
    }

    return MPK_FAILURE;
}

mpk_ret_t mpk_version_operator_print(FILE *f, enum MPK_VERSION_OPERATOR op)
{
    if (!f || op < 0 || op >= MPK_VERSION_OPERATOR_COUNT) {
        return MPK_FAILURE;
    }

    fprintf(f, "'%s'", mpk_version_operator_strings[op]);

    return MPK_SUCCESS;
}

mpk_ret_t mpk_version_print(FILE *f, struct mpk_version *v)
{
    if (!f || !v) {
        syslog(LOG_ERR, "ivalid parameter for mpk_version_print()");
        return MPK_FAILURE;
    }

    fprintf(f, "%d.%d.%d", v->major, v->minor, v->patch);

    switch (v->buildtype) {
    case MPK_VERSION_BUILDTYPE_TEST:
        fprintf(f, "-%c", 'T');
        break;
    case MPK_VERSION_BUILDTYPE_DEBUG:
        fprintf(f, "-%c", 'D');
        break;
    case MPK_VERSION_BUILDTYPE_ALPHA:
        fprintf(f, "-%c", 'A');
        break;
    case MPK_VERSION_BUILDTYPE_BETA:
        fprintf(f, "-%c", 'B');
        break;
    case MPK_VERSION_BUILDTYPE_PRERELEASE:
        fprintf(f, "-%c", 'P');
        break;
    case MPK_VERSION_BUILDTYPE_RELEASE:
        fprintf(f, "-%c", 'R');
        break;
    case MPK_VERSION_BUILDTYPE_UNKNOWN:
    default:
        return MPK_SUCCESS;
    }

    /* TODO make sure string length is always MPK_VERSION_BUILDID_SIZE */
    if (v->buildid != MPK_VERSION_BUILDID_UNFEDINED) {
        fprintf(f, "%llu", v->buildid);
    }

    return MPK_SUCCESS;
}

