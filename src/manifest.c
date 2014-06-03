/**
 * @file manifest.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <yaml.h>
#include <jansson.h>
#include <memory.h>
#include <string.h>
#include "stringhelper.h"
#include "manifest.h"

#define TMP_STR_BUFFER_SIZE 1024

enum PARSE_STATE {
    PARSE_STATE_ERROR = -1,
    PARSE_STATE_START,
    PARSE_STATE_START_DOC,
    PARSE_STATE_START_STREAM,
    PARSE_STATE_START_MFST,
    PARSE_STATE_END_MFST,
    PARSE_STATE_END_DOC,
    PARSE_STATE_END,
    PARSE_STATE_PARSING,
    PARSE_STATE_MANIFEST,
    PARSE_STATE_NAME,
    PARSE_STATE_VERSION,
    PARSE_STATE_ARCH,
    PARSE_STATE_REGIONS,
    PARSE_STATE_REGIONS_LIST,
    PARSE_STATE_DEPENDS,
    PARSE_STATE_DEPENDS_LIST,
    PARSE_STATE_DEPENDS_LISTITEM,
    PARSE_STATE_DEPENDS_NAME,
    PARSE_STATE_DEPENDS_VERSION,
    PARSE_STATE_DEPENDS_OPERATOR,
    PARSE_STATE_CONFLICTS,
    PARSE_STATE_CONFLICTS_LIST,
    PARSE_STATE_CONFLICTS_LISTITEM,
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
    PARSE_STATE_FILE_LIST,
    PARSE_STATE_FILE_LISTITEM_NAME,
    PARSE_STATE_FILE_LISTITEM_HASH,
    PARSE_STATE_FILE_LISTITEM_END,
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
    MANIFEST_TAG_MAINTAINER,
    MANIFEST_TAG_LICENSE,
    MANIFEST_TAG_FILES,
    MANIFEST_TAG_SIGNATURE,
    MANIFEST_TAG_COUNT,
};

static struct manifest_tag {
    const char *name;
    /* TODO */
} manifest_tag[] = {
    { "manifest" },
    { "name" },
    { "version" },
    { "arch" },
    { "regions" },
    { "depends" },
    { "conflicts" },
    { "priority" },
    { "source" },
    { "vendor" },
    { "description" },
    { "maintainer" },
    { "license" },
    { "files" },
    { "signature" }
};

mpk_ret_t mpk_manifest_test()
{
    return MPK_SUCCESS;
}
 
enum MANIFEST_TAG get_tag_from_name(const char *tag)
{
    int i;

    for (i = 0; i < MANIFEST_TAG_COUNT; i++) {
        if (!strcmp(tag, manifest_tag[i].name)) {
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
    case MANIFEST_TAG_MAINTAINER:
        return PARSE_STATE_MAINTAINER;
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

    if (!(dest = malloc(strlen(str) + 1)))
        return NULL;

    strcpy(dest, str);

    return dest;
}


enum PARSE_STATE eval_input(struct mpk_pkginfo *pkg, enum PARSE_STATE state,
    yaml_event_t event)
{
    static struct mpk_pkgref *tmp_pkgref = NULL;
    static char *tmp_str = NULL;
    static struct mpk_file *tmp_file = NULL;

    /* TODO: move parsing of different types of lists (pkgref list, file lists)
     * to separate funktions and reuuse them if possible.
     */

    switch (state) {
    case PARSE_STATE_START:
        switch (event.type) {
        case YAML_STREAM_START_EVENT:
            return PARSE_STATE_START_STREAM;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_START_STREAM:
        switch (event.type) {
        case YAML_DOCUMENT_START_EVENT:
            return PARSE_STATE_START_DOC;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_START_DOC:
        switch (event.type) {
        case YAML_MAPPING_START_EVENT:
            return PARSE_STATE_START_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_START_MFST:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            return eval_new_tag((char *)event.data.scalar.value);
        case YAML_MAPPING_END_EVENT:
            return PARSE_STATE_END_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_END_MFST:
        switch (event.type) {
        case YAML_DOCUMENT_END_EVENT:
            return PARSE_STATE_END_DOC;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_END_DOC:
        switch (event.type) {
        case YAML_STREAM_END_EVENT:
            return PARSE_STATE_END;
        default:
            return PARSE_STATE_ERROR;
        }
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
            return PARSE_STATE_START_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_NAME:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if(!(pkg->name = allocate_and_copy_str(
                     (char *)event.data.scalar.value))) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_START_MFST;
            break;
        default:
            return PARSE_STATE_ERROR;
            break;
        }
    case PARSE_STATE_VERSION:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (mpk_version_deserialize(&pkg->version, NULL,
                    (char *)event.data.scalar.value,
                    strlen((char *)event.data.scalar.value))
                    != MPK_SUCCESS) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_START_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_ARCH:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            /* set pkg->arch */
            if (mpk_pkginfo_arch_deserialize(&pkg->arch,
                    (char *)event.data.scalar.value) != MPK_SUCCESS) {
                return PARSE_STATE_ERROR;
            }
            return PARSE_STATE_START_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_REGIONS:
        /* FIXME: what about an empty region list? */
        switch (event.type) {
        case YAML_SEQUENCE_START_EVENT:
            return PARSE_STATE_REGIONS_LIST;
        case YAML_SEQUENCE_END_EVENT:
            return PARSE_STATE_START_MFST;
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
            return PARSE_STATE_START_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_DEPENDS:
        /* FIXME: Empty list? */
        switch (event.type) {
        case YAML_SEQUENCE_START_EVENT:
            return PARSE_STATE_DEPENDS_LIST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_DEPENDS_LIST:
        switch (event.type) {
        case YAML_MAPPING_START_EVENT:
            if (!(tmp_pkgref = malloc(sizeof(struct mpk_pkgref)))) {
                return PARSE_STATE_ERROR;
            }
            mpk_pkgref_initempty(tmp_pkgref);
            return PARSE_STATE_DEPENDS_LISTITEM;
        case YAML_SEQUENCE_END_EVENT:
            return PARSE_STATE_START_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_DEPENDS_LISTITEM:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!strcmp((char *)event.data.scalar.value, "name")) {
                return PARSE_STATE_DEPENDS_NAME;
            } else if (!strcmp((char *)event.data.scalar.value, "version")) {
                return PARSE_STATE_DEPENDS_VERSION;
            } else if (!strcmp((char *)event.data.scalar.value, "op")) {
                return PARSE_STATE_DEPENDS_OPERATOR;
            } else {
                goto return_error;
            }
        case YAML_MAPPING_END_EVENT:
            if (!tmp_pkgref || !tmp_str) {
                goto return_error;
            }
            tmp_pkgref->name = tmp_str;
            if (mpk_pkgreflist_add(&pkg->depends, tmp_pkgref) != MPK_SUCCESS) {
                goto return_error;
            }
            tmp_pkgref = NULL;
            tmp_str = NULL;
            return PARSE_STATE_DEPENDS_LIST;
        default:
            goto return_error;
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_DEPENDS_NAME:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (tmp_str) { /* check if name has been set before -> error */
                goto return_error;
                return PARSE_STATE_ERROR;
            }
            if(!(tmp_str = allocate_and_copy_str(
                     (char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_DEPENDS_LISTITEM;
        default:
            goto return_error;
        }
    case PARSE_STATE_DEPENDS_VERSION:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (mpk_version_deserialize(&tmp_pkgref->ver, NULL,
                    (char *)event.data.scalar.value,
                    strlen((char *)event.data.scalar.value))
                    != MPK_SUCCESS) {
                goto return_error;
            }
            return PARSE_STATE_DEPENDS_LISTITEM;
        default:
            goto return_error;
        }
    case PARSE_STATE_DEPENDS_OPERATOR:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (mpk_version_operator_deserialize(&tmp_pkgref->op, 0,
                    (char *)event.data.scalar.value,
                    strlen((char *)event.data.scalar.value))
                    != MPK_SUCCESS) {
                goto return_error;
            }
            return PARSE_STATE_DEPENDS_LISTITEM;
        default:
            goto return_error;
        }
    case PARSE_STATE_CONFLICTS:
        switch (event.type) {
        case YAML_SEQUENCE_START_EVENT:
            return PARSE_STATE_CONFLICTS_LIST;
        default:
            return PARSE_STATE_ERROR;
        }
        break;
    case PARSE_STATE_CONFLICTS_LIST:
        switch (event.type) {
        case YAML_MAPPING_START_EVENT:
            if (!(tmp_pkgref = malloc(sizeof(struct mpk_pkgref)))) {
                return PARSE_STATE_ERROR;
            }
            mpk_pkgref_initempty(tmp_pkgref);
            return PARSE_STATE_CONFLICTS_LISTITEM;
        case YAML_SEQUENCE_END_EVENT:
            return PARSE_STATE_START_MFST;
        default:
            return PARSE_STATE_ERROR;
        }
        break;
    case PARSE_STATE_CONFLICTS_LISTITEM:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!strcmp((char *)event.data.scalar.value, "name")) {
                return PARSE_STATE_CONFLICTS_NAME;
            } else if (!strcmp((char *)event.data.scalar.value, "version")) {
                return PARSE_STATE_CONFLICTS_VERSION;
            } else if (!strcmp((char *)event.data.scalar.value, "op")) {
                return PARSE_STATE_CONFLICTS_OPERATOR;
            } else {
                goto return_error;
            }
        case YAML_MAPPING_END_EVENT:
            if (!tmp_pkgref || !tmp_str) {
                goto return_error;
            }
            tmp_pkgref->name = tmp_str;
            if (mpk_pkgreflist_add(&pkg->conflicts, tmp_pkgref)
                    != MPK_SUCCESS) {
                goto return_error;
            }
            tmp_pkgref = NULL;
            tmp_str = NULL;
            return PARSE_STATE_CONFLICTS_LIST;
        default:
            goto return_error;
            return PARSE_STATE_ERROR;
        }
    case PARSE_STATE_CONFLICTS_NAME:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (tmp_str) {
                goto return_error;
                return PARSE_STATE_ERROR;
            }
            if(!(tmp_str = allocate_and_copy_str(
                     (char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_CONFLICTS_LISTITEM;
        default:
            goto return_error;
        }
        break;
    case PARSE_STATE_CONFLICTS_VERSION:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (mpk_version_deserialize(&tmp_pkgref->ver, NULL,
                    (char *)event.data.scalar.value,
                    strlen((char *)event.data.scalar.value))
                    != MPK_SUCCESS) {
                goto return_error;
            }
            return PARSE_STATE_CONFLICTS_LISTITEM;
        default:
            goto return_error;
        }
        break;
    case PARSE_STATE_CONFLICTS_OPERATOR:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (mpk_version_operator_deserialize(&tmp_pkgref->op, 0,
                    (char *)event.data.scalar.value,
                    strlen((char *)event.data.scalar.value))
                    != MPK_SUCCESS) {
                goto return_error;
            }
            return PARSE_STATE_CONFLICTS_LISTITEM;
        default:
            goto return_error;
        }
    case PARSE_STATE_PRIORITY:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            pkg->priority = atoi((char *)event.data.scalar.value);
            return PARSE_STATE_START_MFST;
        default:
            goto return_error;
        }
    case PARSE_STATE_SOURCE:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!(pkg->source
                    = allocate_and_copy_str((char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_START_MFST;
        default:
            goto return_error;
        }
    case PARSE_STATE_VENDOR:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!(pkg->vendor
                    = allocate_and_copy_str((char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_START_MFST;
        default:
            goto return_error;
        }
    case PARSE_STATE_DESCRIPTION:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!(pkg->description
                    = allocate_and_copy_str((char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_START_MFST;
        default:
            goto return_error;
        }
    case PARSE_STATE_MAINTAINER:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!(pkg->maintainer
                    = allocate_and_copy_str((char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_START_MFST;
        default:
            goto return_error;
        }
    case PARSE_STATE_LICENSE:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!(pkg->license
                    = allocate_and_copy_str((char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_START_MFST;
        default:
            goto return_error;
        }
    case PARSE_STATE_FILES:
        switch (event.type) {
        case YAML_SEQUENCE_START_EVENT:
            return PARSE_STATE_FILE_LIST;
        default:
            goto return_error;
        }
    case PARSE_STATE_FILE_LIST:
        switch (event.type) {
        case YAML_SEQUENCE_END_EVENT:
            return PARSE_STATE_START_MFST;
        case YAML_MAPPING_START_EVENT:
            if (tmp_file || !(tmp_file = mpk_file_create()))
                goto return_error;
            return PARSE_STATE_FILE_LISTITEM_NAME;
        default:
            goto return_error;
        }
    case PARSE_STATE_FILE_LISTITEM_NAME:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!tmp_file || !(tmp_file->name
                    = allocate_and_copy_str((char *)event.data.scalar.value))) {
                goto return_error;
            }
            return PARSE_STATE_FILE_LISTITEM_HASH;
        default:
            goto return_error;
        }
    case PARSE_STATE_FILE_LISTITEM_HASH:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (!tmp_file) {
                goto return_error;
            }
            if (*(char *)event.data.scalar.value == '-') {
                memset(tmp_file->hash, 0, sizeof(tmp_file->hash));
            } else {
                if (mpk_pkginfo_signature_deserialize(tmp_file->hash,
                        (char *)event.data.scalar.value)
                        != MPK_SUCCESS) {
                    goto return_error;
                }
            }
            return PARSE_STATE_FILE_LISTITEM_END;
        default:
            goto return_error;
        }
    case PARSE_STATE_FILE_LISTITEM_END:
        switch (event.type) {
        case YAML_MAPPING_END_EVENT:
            if (!tmp_file) {
                goto return_error;
            }
            if (mpk_filelist_add(&pkg->files, tmp_file) != MPK_SUCCESS) {
                goto return_error;
            }
            tmp_file = NULL;
            return PARSE_STATE_FILE_LIST;
        default:
            goto return_error;
        }
    case PARSE_STATE_SIGNATURE:
        switch (event.type) {
        case YAML_SCALAR_EVENT:
            if (*((char *)event.data.scalar.value) == '-') {
                memset(pkg->signature, 0, sizeof(pkg->signature));
            } else {
                if (read_hexstr(pkg->signature,  sizeof(pkg->signature),
                        (char *)event.data.scalar.value) != MPK_SUCCESS) {
                    goto return_error;
                }
            }
            return PARSE_STATE_START_MFST;
        default:
            goto return_error;
        }
    default:
        syslog(LOG_ERR, "invalid parser state");
        return PARSE_STATE_ERROR;
    }

return_error:
    if (tmp_file)
        mpk_file_delete(&tmp_file);
    if (tmp_str)
        free(tmp_str);
    if (tmp_pkgref)
        free(tmp_pkgref);
    return PARSE_STATE_ERROR;
}


mpk_ret_t mpk_manifest_read(struct mpk_pkginfo *pkginfo, const char *filename)
{
    /* TODO: JSON instead of yaml */


    FILE *f;
    yaml_parser_t parser;
    yaml_event_t event;
    int done = 0;
    enum PARSE_STATE state = PARSE_STATE_START;

    if (!(f = fopen(filename, "r"))) {
        syslog(LOG_ERR, "could not open manifest file (%s)", filename);
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
        } else if (state == PARSE_STATE_END) {
            done = 1;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(f);

    return MPK_SUCCESS;
}

mpk_ret_t write_yaml_pkgreflist(yaml_document_t *doc, int map, const char *tag,
    struct mpk_pkgreflist *pkgs)
{
    int key, sequence, item, ret;
    struct mpk_pkgreflist_item *pkg =  NULL;

    if (!pkgs)
        return MPK_FAILURE;

    key = yaml_document_add_scalar(doc, NULL, (unsigned char *)tag,
        strlen(tag), YAML_PLAIN_SCALAR_STYLE);
    if (!key)
        return MPK_FAILURE;

    sequence = yaml_document_add_sequence(doc, NULL, YAML_BLOCK_SEQUENCE_STYLE);
    if (!sequence)
        return MPK_FAILURE;

    ret = yaml_document_append_mapping_pair(doc, map, key, sequence);
    if (!ret)
        return MPK_FAILURE;

    for (pkg = pkgs->lh_first; pkg; pkg = pkg->items.le_next) {
        int name_tag, name_value;
        int version_tag, version_value;
        int op_tag, op_value;
        int tmp_str_len;
        char *tmp_str;
        int len;
        int pkg_map;

        pkg_map = yaml_document_add_mapping(doc, NULL, YAML_FLOW_MAPPING_STYLE);
        if (!pkg_map)
            return MPK_FAILURE;

        /* name */

        name_tag = yaml_document_add_scalar(doc, NULL,
            (unsigned char *)"name", strlen("name"),
                YAML_PLAIN_SCALAR_STYLE);
        if (!name_tag)
            return MPK_FAILURE;

        name_value = yaml_document_add_scalar(doc, NULL,
            (unsigned char *)pkg->pkgref->name, strlen(pkg->pkgref->name),
                YAML_PLAIN_SCALAR_STYLE);
        if (!name_value)
            return MPK_FAILURE;

        if (!yaml_document_append_mapping_pair(doc, pkg_map, name_tag,
                name_value))
            return MPK_FAILURE;

        /* version */

        if (!mpk_version_isempty(&pkg->pkgref->ver)) {
            version_tag = yaml_document_add_scalar(doc, NULL,
                (unsigned char *)"version", strlen("version"),
                    YAML_PLAIN_SCALAR_STYLE);
            if (!version_tag)
                return MPK_FAILURE;

            tmp_str_len = mpk_version_serializedsize(&pkg->pkgref->ver);
            tmp_str = malloc(tmp_str_len + 1);
            if (!tmp_str)
                return MPK_FAILURE;
            if (mpk_version_serialize(tmp_str, &len, tmp_str_len,
                    &pkg->pkgref->ver) != MPK_SUCCESS) {
                free(tmp_str);
                return MPK_FAILURE;
            }
            version_value = yaml_document_add_scalar(doc, NULL,
                (unsigned char *)tmp_str, strlen(tmp_str),
                YAML_PLAIN_SCALAR_STYLE);
            if (!version_value) {
                free(tmp_str);
                return MPK_FAILURE;
            }
            free(tmp_str);

            if (!yaml_document_append_mapping_pair(doc, pkg_map, version_tag,
                    version_value))
                return MPK_FAILURE;
        }

        /* operator */

        if (pkg->pkgref->op != MPK_VERSION_OPERATOR_UNKNOWN) {
            op_tag = yaml_document_add_scalar(doc, NULL,
                (unsigned char *)"op", strlen("op"), YAML_PLAIN_SCALAR_STYLE);
            if (!op_tag)
                return MPK_FAILURE;

            op_value = yaml_document_add_scalar(doc, NULL,
                (unsigned char *)mpk_version_operator_strings[pkg->pkgref->op],
                strlen(mpk_version_operator_strings[pkg->pkgref->op]),
                YAML_PLAIN_SCALAR_STYLE);
            if (!op_value)
                return MPK_FAILURE;
        }

        if (!yaml_document_append_sequence_item(doc, sequence, pkg_map))
            return MPK_FAILURE;
    }

    return MPK_SUCCESS;
}

mpk_ret_t mpk_manifest_write(const char *filename, struct mpk_pkginfo *pkg)
{
    json_t *root;
    struct mpk_pkgreflist_item *p;
    char tmp_str[TMP_STR_BUFFER_SIZE];
    int len;

    root = json_object();

     /* manifest */
    if (mpk_version_serialize(tmp_str, &len, TMP_STR_BUFFER_SIZE - 1,
            &pkg->manifest) != MPK_SUCCESS) {
        json_decref(root);
        return MPK_FAILURE;
    }
    if (len >= TMP_STR_BUFFER_SIZE) {
        json_decref(root);
        return MPK_FAILURE;
    }
    tmp_str[len] = 0;
    if (json_object_set_new(root, "manifest", json_string(tmp_str)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* name */
    if (json_object_set_new(root, "name", json_string(pkg->name)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* version */
    if (mpk_version_serialize(tmp_str, &len, TMP_STR_BUFFER_SIZE - 1,
            &pkg->version) != MPK_SUCCESS) {
        json_decref(root);
        return MPK_FAILURE;
    }
    if (len >= TMP_STR_BUFFER_SIZE) {
        json_decref(root);
        return MPK_FAILURE;
    }
    tmp_str[len] = 0;
    if (json_object_set_new(root, "version", json_string(tmp_str)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* depends */
    json_t *depends_array = json_array();
    if (!depends_array) {
        json_decref(root);
        return MPK_FAILURE;
    }
    for (p = pkg->depends.lh_first; p; p = p->items.le_next) {
        json_t *depends_item = json_object();
        if (!depends_item) {
            json_decref(depends_array);
            return MPK_FAILURE;
        }
        if (json_object_set_new(depends_item, "name",
                json_string(p->pkgref->name)) != 0) {
            json_decref(depends_item);
            json_decref(depends_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (mpk_version_serialize(tmp_str, &len, TMP_STR_BUFFER_SIZE - 1,
                &p->pkgref->ver) != MPK_SUCCESS) {
            json_decref(depends_item);
            json_decref(depends_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (len >= TMP_STR_BUFFER_SIZE) {
            json_decref(depends_item);
            json_decref(depends_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        tmp_str[len] = 0;
        if (json_object_set_new(depends_item, "version",
                json_string(tmp_str)) != 0) {
            json_decref(depends_item);
            json_decref(depends_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (json_array_append(depends_array, depends_item) != 0) {
            json_decref(depends_item);
            json_decref(depends_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        json_decref(depends_item);
    }
    if (json_object_set(root, "depends", depends_array) != 0) {
        json_decref(depends_array);
        json_decref(root);
        return MPK_FAILURE;
    }
    json_decref(depends_array);

    /* conflicts */
    json_t *conflicts_array = json_array();
    if (!conflicts_array) {
        json_decref(root);
        return MPK_FAILURE;
    }
    for (p = pkg->conflicts.lh_first; p; p = p->items.le_next) {
        json_t *conflicts_item = json_object();
        if (!conflicts_item) {
            json_decref(conflicts_array);
            return MPK_FAILURE;
        }
        if (json_object_set_new(conflicts_item, "name",
                json_string(p->pkgref->name)) != 0) {
            json_decref(conflicts_item);
            json_decref(conflicts_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (mpk_version_serialize(tmp_str, &len, TMP_STR_BUFFER_SIZE - 1,
                &p->pkgref->ver) != MPK_SUCCESS) {
            json_decref(conflicts_item);
            json_decref(conflicts_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (len >= TMP_STR_BUFFER_SIZE) {
            json_decref(conflicts_item);
            json_decref(conflicts_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        tmp_str[len] = 0;
        if (json_object_set_new(conflicts_item, "version",
                json_string(tmp_str)) != 0) {
            json_decref(conflicts_item);
            json_decref(conflicts_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (json_array_append(conflicts_array, conflicts_item) != 0) {
            json_decref(conflicts_item);
            json_decref(conflicts_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        json_decref(conflicts_item);
    }
    if (json_object_set(root, "conflicts", conflicts_array) != 0) {
        json_decref(conflicts_array);
        json_decref(root);
        return MPK_FAILURE;
    }
    json_decref(conflicts_array);

    /* priority */
    if (json_object_set_new(root, "priority", json_integer(100)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* source */
    if (json_object_set_new(root, "source", json_string(pkg->source)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* vendor */
    if (json_object_set_new(root, "vendor", json_string(pkg->vendor)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* desciption */
    if (json_object_set_new(root, "description",
            json_string(pkg->description)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* maintainer */
    if (json_object_set_new(root, "maintainer",
            json_string(pkg->maintainer)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* license */
    if (json_object_set_new(root, "license", json_string(pkg->license)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* files */
    struct mpk_file *f;
    json_t *files_array = json_array();
    if (!files_array) {
        json_decref(root);
        return MPK_FAILURE;
    }
    for (f = pkg->files.lh_first; f; f = f->items.le_next) {
        json_t *files_item = json_object();
        if (!files_item) {
            json_decref(files_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (json_object_set_new(files_item, "name", json_string(f->name))
                != 0) {
            json_decref(files_item);
            json_decref(files_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (json_object_set_new(files_item, "hash", json_string("-")) != 0) {
            json_decref(files_item);
            json_decref(files_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (json_array_append(files_array, files_item) != 0) {
            json_decref(files_item);
            json_decref(files_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        json_decref(files_item);
    }
    if (json_object_set(root, "files", files_array) != 0) {
        json_decref(files_array);
        json_decref(root);
        return MPK_FAILURE;
    }
    json_decref(files_array);

    /* signature */
    if (json_object_set_new(root, "signature", json_string("-")) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    if (json_dump_file(root, filename, JSON_INDENT(4)|JSON_PRESERVE_ORDER) != 0)
        return MPK_FAILURE;

    json_decref(root);

    return MPK_SUCCESS;
}
