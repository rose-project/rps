/**
 * @file manifest.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
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
    MANIFEST_TAG_UNDEFINED = -1,
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

/**
 * @brief manifest_tag_id determines the MANIFEST_TAG enum value from string
 * @param str the manifest tag a a null terminated string
 * @return tag id fromMANIFEST_TA  or MANIFEST_TAG_UNDEFINED in case of an error
 */
enum MANIFEST_TAG manifest_tag_id(const char *tag)
{
    for (int i = 0; i < MANIFEST_TAG_COUNT; i++) {
        if (strcmp(manifest_tag[i].name, tag) == 0)
            return i;
    }
    return MANIFEST_TAG_UNDEFINED;
}

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

char *allocate_and_copy_str(const char *str)
{
    char *dest;

    if (!(dest = malloc(strlen(str) + 1)))
        return NULL;

    strcpy(dest, str);

    return dest;
}

int parse_version(struct mpk_version *version, json_t *in)
{
    const char *str = json_string_value(in);
    return mpk_version_deserialize(version, NULL, str, strlen(str));
}

int parse_string(const char **str, json_t *in)
{
    const char *in_str = json_string_value(in);
    if (!in_str) {
        return MPK_FAILURE;
    }
    *str = allocate_and_copy_str(in_str);
    return MPK_SUCCESS;
}

int parse_stringlist(struct mpk_stringlist *strl, json_t *in)
{
    if (!in || json_typeof(in) != JSON_ARRAY)
        return MPK_FAILURE;

    int i;
    json_t *value;
    json_array_foreach(in, i, value) {
        const char *str = json_string_value(value);
        if (mpk_stringlist_add(strl, str) != MPK_SUCCESS) {
            mpk_stringlist_delete(strl);
        }
    }

    return MPK_SUCCESS;
}

int parse_pkgreflist(struct mpk_pkgreflist *pkgs, json_t *in)
{
    if (!in || json_typeof(in) != JSON_ARRAY)
        return MPK_FAILURE;

    int i;
    json_t *val1;
    json_array_foreach(in, i, val1) {
        char *key;
        json_t *val2;
        if (!val1 || json_typeof(in) != JSON_OBJECT) {
            mpk_pkgreflist_delete(pkgs);
            return MPK_FAILURE;
        }
        json_object_foreach(val1, key, val2) {
            if (strcmp(key, "name") == 0) {
                /* TODO */
            } else if (strcmp(key, "name") == 0) {
                /* TODO */
            } else {
                /* TODO */
            }
        }
        /* TODO */
    }

    return MPK_SUCCESS;
}

int parse_int(int *i, json_t *in)
{
    *i = json_integer_value(in);
    return MPK_SUCCESS;
}

int parse_filelist(struct mpk_filelist *files, json_t *in)
{
    /* TODO */
}

int parse_hexstring(unsigned char hex[], int blen, json_t *in)
{
    const char *hexstr = json_string_value(in);
    if (!hexstr)
        return MPK_FAILURE;

    return read_hexstr(hex, blen, hexstr);
}

int handle_manifest_tag(struct mpk_pkginfo *pkg, char *tag, json_t *value)
{
    enum MANIFEST_TAG tag_id = manifest_tag_id(tag);
    if (tag_id == MANIFEST_TAG_UNDEFINED)
        return MPK_FAILURE;

    switch (tag_id) {
    case MANIFEST_TAG_MANIFEST:
        return parse_version(&pkg->manifest, value);
    case MANIFEST_TAG_NAME:
        return parse_string(&pkg->name, value);
    case MANIFEST_TAG_VERSION:
        return parse_version(&pkg->version, value);
    case MANIFEST_TAG_ARCH:
        return parse_string(&pkg->arch, value);
    case MANIFEST_TAG_REGIONS:
        return parse_stringlist(&pkg->regions, value);
    case MANIFEST_TAG_DEPENDS:
        return parse_pkgreflist(&pkg->depends, value);
    case MANIFEST_TAG_CONFLICTS:
        return parse_pkgreflist(&pkg->conflicts, value);
    case MANIFEST_TAG_PRIORITY:
        return parse_int(&pkg->priority, value);
    case MANIFEST_TAG_SOURCE:
        return parse_string(&pkg->source, value);
    case MANIFEST_TAG_VENDOR:
        return parse_string(&pkg->vendor, value);
    case MANIFEST_TAG_DESCRIPTION:
        return parse_string(&pkg->description, value);
    case MANIFEST_TAG_MAINTAINER:
        return parse_string(&pkg->maintainer, value);
    case MANIFEST_TAG_LICENSE:
        return parse_string(&pkg->license, value);
    case MANIFEST_TAG_FILES:
        return parse_string(&pkg->files, value);
    case MANIFEST_TAG_SIGNATURE:
        return parse_string(&pkg->signature, value);
    default:
        return MPK_FAILURE;
    }
}

mpk_ret_t mpk_manifest_read(struct mpk_pkginfo *pkginfo, const char *filename)
{
    /* TODO: JSON instead of yaml */

    json_t *root = json_load_file(filename, 0, NULL);
    if (!root)
        return MPK_FAILURE;

    void *iter = json_object_iter(root);
    char *key;
    json_t *value;
    json_object_foreach(root, key, value) {
        handle_manifest_tag(pkginfo, key, value);
    }

    json_decref(root);

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

    /* architecture */
    /* TODO */

    /* regions */
    /* TODO */

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
