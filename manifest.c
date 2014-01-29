/**
 * @file manifest.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdio.h>
#include <syslog.h>
#include <yaml.h>
#include "manifest.h"

enum PARSE_STATE {
    PARSE_STATE_ERROR = -1,
    PARSE_STATE_START,
    PARSE_STATE_STOP,
    PARSE_STATE_PARSING,
    PARSE_STATE_MANIFEST,
    PARSE_STATE_NAME,
    PARSE_STATE_VERSION,
    PARSE_STATE_ARCH,
    PARSE_STATE_REGIONS,
    PARSE_STATE_REGIONS_LIST,
    PARSE_STATE_DEPENDS,
    PARSE_STATE_DEPENDS_NAME,
    PARSE_STATE_DEPENDS_VERSION,
    PARSE_STATE_DEPENDS_OPERATOR,
    PARSE_STATE_CONFLICTS,
    PARSE_STATE_CONFLICTS_NAME,
    PARSE_STATE_CONFLICTS_VERSION,
    PARSE_STATE_CONFLICTS_OPERATOR,
    PARSE_STATE_PRIORITY,
    PARSE_STATE_SOURCE,
    PARSE_STATE_VENDOR,
    PARSE_STATE_DESCRIPTION,
    PARSE_STATE_MAINTAINER,
    PARSE_STATE_LICENSE,
    PARSE_STATE_FILES,
    PARSE_STATE_FILES_NAME,
    PARSE_STATE_SIGNATURE
};

enum PARSE_EVENT {
    PARSE_EVENT_DEFAULT = 0,
    PARSE_EVENT_TOKEN,
    PASRE_EVENT_START_SEQUENCE,
    PASRE_EVENT_END_SEQUENCE,
    PASRE_EVENT_START_MAPPING,
    PASRE_EVENT_END_MAPPING
};

enum MANIFEST_TAG {
    MANIFEST_TAG_MANIFEST = 0,
    MANIFEST_TAG_NAME,
    MANIFEST_TAG_VERSION,
    MANIFEST_TAG_ARCH,
    MANIFEST_TAG_REGIONS,
    MANIFEST_TAG_DEPENDS,
    MANIFEST_TAG_CONFLICTS,
    MANIFEST_TAG_PRIORITY,
    MANIFEST_TAG_SOURCE,
    MANIFEST_TAG_VENDOR,
    MANIFEST_TAG_DESCRIPTION,
    MANIFEST_TAG_LICENSE,
    MANIFEST_TAG_FILES,
    MANIFEST_TAG_SIGNATURE,
    MANIFEST_TAG_COUNT,
};

static const char *tagname[] = {
    "manifest",
    "name",
    "version",
    "arch",
    "regions",
    "depends",
    "conflicts",
    "priority",
    "source",
    "vendor",
    "description",
    "license",
    "files",
    "signature"
};

enum MANIFEST_TAG get_tag_from_name(const char *tag)
{
    int i;

    for (i = 0; i < MANIFEST_TAG_COUNT; i++) {
        if (!strcmp(tag, tagname[i])) {
            return i;
        }
    }

    return -1;
}

enum PARSE_STATE eval_new_tag(char *token)
{
    enum MANIFEST_TAG tag = get_tag_from_name(token);

    switch (tag) {
    case MANIFEST_TAG_MANIFEST:
        return PARSE_STATE_MANIFEST;
    case MANIFEST_TAG_NAME:
        return PARSE_STATE_NAME;
    case MANIFEST_TAG_VERSION:
        return PARSE_STATE_VERSION;
    case MANIFEST_TAG_ARCH:
        return PARSE_STATE_ARCH;
    case MANIFEST_TAG_REGIONS:
        return PARSE_STATE_REGIONS;
    case MANIFEST_TAG_DEPENDS:
        return PARSE_STATE_DEPENDS;
    case MANIFEST_TAG_CONFLICTS:
        return PARSE_STATE_CONFLICTS;
    case MANIFEST_TAG_PRIORITY:
        return PARSE_STATE_PRIORITY;
    case MANIFEST_TAG_SOURCE:
        return PARSE_STATE_SOURCE;
    case MANIFEST_TAG_VENDOR:
        return PARSE_STATE_VENDOR;
    case MANIFEST_TAG_DESCRIPTION:
        return PARSE_STATE_DESCRIPTION;
    case MANIFEST_TAG_LICENSE:
        return PARSE_STATE_LICENSE;
    case MANIFEST_TAG_FILES:
        return PARSE_STATE_FILES;
    case MANIFEST_TAG_SIGNATURE:
        return PARSE_STATE_SIGNATURE;
    default:
        return PARSE_STATE_ERROR;
    }
}

char *allocate_and_copy_str(const char *str)
{
    char *dest;

    if (!(dest = malloc(sizeof(strlen(str) + 1))))
        return NULL;

    strcpy(dest, str);

    return dest;
}

enum PARSE_STATE eval_input(struct mpk_pkginfo *pkg, enum PARSE_STATE state,
    yaml_event_t event)
{
    switch (state) {
    case PARSE_STATE_START:
        switch (event.type) {
        case YAML_STREAM_START_EVENT:
        case YAML_DOCUMENT_START_EVENT:
        case YAML_MAPPING_START_EVENT:
            return PARSE_STATE_START;
        case YAML_SCALAR_EVENT:
            return eval_new_tag((char *)event.data.scalar.value);
        default:
            break;
        }
        break;
    case PARSE_STATE_MANIFEST:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            /* set pkg->manifest */
            if (mpk_version_deserialize(&pkg->manifest, NULL,
                    (char *)event.data.scalar.value,
                    strlen((char *)event.data.scalar.value))
                    != MPK_SUCCESS) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_START;
        default:
            return PARSE_STATE_ERROR;
        }
        break;
    case PARSE_STATE_NAME:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if(!(pkg->name = allocate_and_copy_str(
                     (char *)event.data.scalar.value))) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_START;
            break;
        default:
            return PARSE_STATE_ERROR;
            break;
        }
        break;
    case PARSE_STATE_VERSION:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (mpk_version_deserialize(&pkg->version, NULL,
                    (char *)event.data.scalar.value,
                    strlen((char *)event.data.scalar.value))
                    != MPK_SUCCESS) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_START;
        default:
            return PARSE_STATE_ERROR;
        }
        break;
    case PARSE_STATE_ARCH:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            /* set pkg->arch */
            if (mpk_pkginfo_arch_deserialize(&pkg->arch,
                    (char *)event.data.scalar.value) != MPK_SUCCESS) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_START;
        default:
            return PARSE_STATE_ERROR;
        }
        break;
    case PARSE_STATE_REGIONS:
        /* TODO: what about an empty region list? */
        switch (event.type) {
        case YAML_SEQUENCE_START_EVENT:
            return PARSE_STATE_REGIONS_LIST;
            break;
        default:
            return PARSE_STATE_ERROR;
            break;
        }
    case PARSE_STATE_REGIONS_LIST:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            /* add new region to list */
            if (mpk_stringlist_add(&pkg->regions,
                    (char *)event.data.scalar.value) != MPK_SUCCESS) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_REGIONS_LIST;
        case YAML_SEQUENCE_END_EVENT:
            return PARSE_STATE_START;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_CONFLICTS_NAME:
        break;
    case PARSE_STATE_CONFLICTS_VERSION:
        break;
    case PARSE_STATE_CONFLICTS_OPERATOR:
        break;
    default:
        syslog(LOG_ERR, "invalid parser state");
        return PARSE_STATE_ERROR;
    }

    /* if we get here something unexpected did happen */
    return PARSE_STATE_ERROR;
}

mpk_ret_t mpk_manifest_read(struct mpk_pkginfo *pkginfo, const char *filename)
{
    FILE *f;
    yaml_parser_t parser;
    yaml_event_t event;
    int done = 0;
    enum PARSE_STATE state = PARSE_STATE_START;

    if (!(f = fopen(filename, "r"))) {
        syslog(LOG_ERR, "could not open manifest file %s", filename);
        return MPK_FAILURE;
    }

    if (!yaml_parser_initialize(&parser)) {
        syslog(LOG_ERR, "could not initialize yaml parser");
        fclose(f);
        return MPK_FAILURE;
    }

    yaml_parser_set_input_file(&parser, f);

    /* parse file */

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            syslog(LOG_ERR, "error while parsing manifest file");
            yaml_parser_delete(&parser);
            fclose(f);
            return MPK_FAILURE;
        }

        state = eval_input(pkginfo, state, event);

        if (state == PARSE_STATE_ERROR) {
            syslog(LOG_ERR, "error while parsing manifest file");
            yaml_event_delete(&event);
            fclose(f);
            mpk_pkginfo_delete(pkginfo);
            return MPK_FAILURE;
        } else if (state == PARSE_STATE_STOP) {
            done = 1;
        }

/*
        switch (event.type) {
        case YAML_STREAM_START_EVENT:
            syslog(LOG_DEBUG, "YAML_STREAM_START_EVENT");
            break;
        case YAML_STREAM_END_EVENT:
            syslog(LOG_DEBUG, "YAML_STREAM_END_EVENT");
            done = 1;
            break;
        case YAML_DOCUMENT_START_EVENT:
            syslog(LOG_DEBUG, "YAML_DOCUMENT_START_EVENT");
            break;
        case YAML_DOCUMENT_END_EVENT:
            syslog(LOG_DEBUG, "YAML_DOCUMENT_END_EVENT");
            break;
        case YAML_SEQUENCE_START_EVENT:
            syslog(LOG_DEBUG, "YAML_SEQUENCE_START_EVENT");
            break;
        case YAML_SEQUENCE_END_EVENT:
            syslog(LOG_DEBUG, "YAML_SEQUENCE_END_EVENT");
            break;
        case YAML_MAPPING_START_EVENT:
            syslog(LOG_DEBUG, "YAML_MAPPING_START_EVENT");
            break;
        case YAML_MAPPING_END_EVENT:
            syslog(LOG_DEBUG, "YAML_MAPPING_END_EVENT");
            break;
        case YAML_ALIAS_EVENT:
            syslog(LOG_DEBUG, "YAML_ALIAS_EVENT: %s",
                event.data.alias.anchor);
            break;
        case YAML_SCALAR_EVENT:
            syslog(LOG_DEBUG, "YAML_SCALAR_EVENT: %s",
                event.data.scalar.value);
            break;
        default:
            syslog(LOG_ERR, "received unknown event from parser");
            yaml_event_delete(&event);
            fclose(f);
            return MPK_FAILURE;
        }

*/

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(f);

    return MPK_SUCCESS;
}


mpk_ret_t mpk_manifest_write(const char *filename, struct mpk_pkginfo *pkginfo)
{
    return MPK_SUCCESS;
}
