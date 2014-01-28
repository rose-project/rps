 /**
 * @file version.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <syslog.h>
#include <ctype.h>
#include <stdlib.h>
#include "version.h"

const struct mpk_version MPK_VERSION_DEFAULT = {
    0, 0, 0, MPK_VERSION_BUILDTYPE_UNKNOWN, 0
};

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
                    v->minor = 0;
                    v->patch = 0;
                    v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
                    v->buildid = 0;
                    if (len)
                        *len = c;
                    return MPK_SUCCESS;
                }
                break;
            case 1:
                v->minor = (n == 0) ? 0 : atoi(buf);
                v->patch = 0;
                v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
                v->buildid = 0;
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 2:
                v->patch = (n == 0) ? 0 : atoi(buf);
                v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
                v->buildid = 0;
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 3:
                v->buildtype = type;
                v->buildid = 0;
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
                    v->minor = 0;
                    v->patch = 0;
                    v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
                    v->buildid = 0;
                }
                *len = c;
                return MPK_SUCCESS;
            case 1:
                v->minor = (n == 0) ? 0 : atoi(buf);
                v->patch = 0;
                v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
                v->buildid = 0;
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 2:
                v->patch = (n == 0) ? 0 : atoi(buf);
                v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
                v->buildid = 0;
                if (len)
                    *len = c;
                return MPK_SUCCESS;
            case 3:
                v->buildtype = type;
                v->buildid = 0;
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
                v->minor = 0;
                v->patch = 0;
                v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
                v->buildid = 0;
            }
            *len = c;
            return MPK_SUCCESS;
        case 1:
            v->minor = (n == 0) ? 0 : atoi(buf);
            v->patch = 0;
            v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
            v->buildid = 0;
            if (len)
                *len = c;
            return MPK_SUCCESS;
        case 2:
            v->patch = (n == 0) ? 0 : atoi(buf);
            v->buildtype = MPK_VERSION_BUILDTYPE_UNKNOWN;
            v->buildid = 0;
            if (len)
                *len = c;
            return MPK_SUCCESS;
        case 3:
            v->buildtype = type;
            v->buildid = 0;
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
