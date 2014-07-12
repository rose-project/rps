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
#include <syslog.h>
#include "stringhelper.h"
#include "mpk/defines.h"
#include "mpk/manifest.h"

#define TMP_STR_BUFFER_SIZE 1024

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
    MANIFEST_TAG_TOOL,
    MANIFEST_TAG_FILES,
    MANIFEST_TAG_SIGNATURE,
    MANIFEST_TAG_COUNT,
};

/**
 * @brief A manifest file tag.
 */
static struct manifest_tag {
    const char *name; /**< The string defining the tag. */
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
    { "tool" },
    { "data" },
    { "signature" }
};

/**
 * @brief manifest_tag_id determines the MANIFEST_TAG enum value from string
 * @param tag the manifest tag a a null terminated string
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

int parse_string(char **str, json_t *in)
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
        if (mpk_stringlist_addend(strl, str) != MPK_SUCCESS) {
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
        if (!val1 || json_typeof(val1) != JSON_OBJECT) {
            mpk_pkgreflist_delete(pkgs);
            return MPK_FAILURE;
        }
        struct mpk_pkgref *pkg = NULL;
        if (!(pkg = malloc(sizeof(struct mpk_pkgref)))) {
            mpk_pkgreflist_delete(pkgs);
            return MPK_FAILURE;
        }
        pkg->name = NULL;

        char *key;
        json_t *val2;
        json_object_foreach(val1, key, val2) {
            if (strcmp(key, "name") == 0) {
                pkg->name = allocate_and_copy_str(json_string_value(val2));
            } else if (strcmp(key, "version") == 0) {
                const char *str = json_string_value(val2);
                if (strlen(str) == 0) {
                    pkg->ver = MPK_VERSION_DEFAULT;
                    continue;
                }
                if (mpk_version_deserialize(&pkg->ver, NULL, str, strlen(str))
                        != MPK_SUCCESS) {
                    if (pkg->name)
                        free(pkg->name);
                    free(pkg);
                    mpk_pkgreflist_delete(pkgs);
                    return MPK_FAILURE;
                }
            } else {
                syslog(LOG_ERR, "Undefined tag in pkgref list");
                if (pkg->name)
                    free(pkg->name);
                free(pkg);
                mpk_pkgreflist_delete(pkgs);
                return MPK_FAILURE;
            }
        }
        if (mpk_pkgreflist_addend(pkgs, pkg) != MPK_SUCCESS) {
            if (pkg->name)
                free(pkg->name);
            free(pkg);
            mpk_pkgreflist_delete(pkgs);
            return MPK_FAILURE;
        }
        pkg = NULL;
    }

    return MPK_SUCCESS;
}

int parse_int(int *i, json_t *in)
{
    *i = json_integer_value(in);
    return MPK_SUCCESS;
}

int parse_hexstring(unsigned char hex[], int blen, json_t *in)
{
    const char *hexstr = json_string_value(in);
    if (!hexstr)
        return MPK_FAILURE;

    if (strlen(hexstr) == 0) {
        memset(hex, 0, blen);
        return MPK_EMPTY;
    }

    return read_hexstr(hex, blen, hexstr);
}

int parse_filelist(struct mpk_filelist *files, json_t *in)
{
    if (!in || json_typeof(in) != JSON_ARRAY)
        return MPK_FAILURE;

    int i;
    json_t *val1;
    json_array_foreach(in, i, val1) {
        if (!val1 || json_typeof(val1) != JSON_OBJECT) {
            mpk_filelist_delete(files);
            return MPK_FAILURE;
        }
        struct mpk_file *file = NULL;
        if (!(file = malloc(sizeof(struct mpk_file)))) {
            mpk_filelist_delete(files);
            return MPK_FAILURE;
        }
        file->name = NULL;

        char *key;
        json_t *val2;
        json_object_foreach(val1, key, val2) {
            if (strcmp(key, "name") == 0) {
                file->name = allocate_and_copy_str(json_string_value(val2));
            } else if (strcmp(key, "hash") == 0) {

                switch (parse_hexstring(file->hash, MPK_FILEHASH_SIZE, val2)) {
                case MPK_EMPTY:
                    file->hash_is_set = false;
                    break;
                case MPK_SUCCESS:
                    file->hash_is_set = true;
                    break;
                case MPK_FAILURE:
                default:
                    if (file->name)
                        free(file->name);
                    free(file);
                    mpk_filelist_delete(files);
                    return MPK_FAILURE;
                }
            } else {
                syslog(LOG_ERR, "Undefined tag in pkgref list");
                if (file->name)
                    free(file->name);
                free(file);
                mpk_filelist_delete(files);
                return MPK_FAILURE;
            }
        }

        if (mpk_filelist_addend(files, file) != MPK_SUCCESS) {
            if (file->name)
                free(file->name);
            free(file);
            mpk_filelist_delete(files);
            return MPK_FAILURE;
        }
        file = NULL;
    }

    return MPK_SUCCESS;
}

int parse_signature(unsigned char bytearray[], bool *is_signed, json_t *in)
{
    switch (parse_hexstring(bytearray, MPK_PKGINFO_SIGNATURE_LEN, in)) {
    case MPK_EMPTY:
        *is_signed = false;
        return MPK_SUCCESS;
    case MPK_SUCCESS:
        *is_signed = true;
        return MPK_SUCCESS;
    case MPK_FAILURE:
    default:
        return MPK_FAILURE;
    }
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
    case MANIFEST_TAG_TOOL:
        return parse_filelist(&pkg->tool, value);
    case MANIFEST_TAG_FILES:
        return parse_filelist(&pkg->data, value);
    case MANIFEST_TAG_SIGNATURE:
        return parse_signature(pkg->signature, &pkg->is_signed, value);
    default:
        return MPK_FAILURE;
    }
}

int mpk_manifest_read(struct mpk_pkginfo *pkginfo, const char *filename)
{
    json_t *root = json_load_file(filename, 0, NULL);
    if (!root)
        return MPK_FAILURE;

    char *key;
    json_t *value;
    json_object_foreach(root, key, value) {
        handle_manifest_tag(pkginfo, key, value);
    }

    json_decref(root);

    return MPK_SUCCESS;
}

int mpk_manifest_write(const char *filename, struct mpk_pkginfo *pkg)
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
    if (json_object_set_new(root, "arch", json_string(pkg->arch)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    /* regions */
    struct mpk_stringlist_item *strl_it;
    json_t *regions_array = json_array();
    if (!regions_array) {
        json_decref(root);
        return MPK_FAILURE;
    }
    for (strl_it = pkg->regions.lh_first; strl_it;
            strl_it = strl_it->items.le_next) {
        if (json_array_append_new(regions_array, json_string(strl_it->str))
                != 0) {
            json_decref(regions_array);
            json_decref(root);
            return MPK_FAILURE;
        }
    }
    if (json_object_set(root, "regions", regions_array) != 0) {
        json_decref(regions_array);
        json_decref(root);
        return MPK_FAILURE;
    }
    json_decref(regions_array);

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

    /* tools */
    struct mpk_file *f;
    json_t *tools_array = json_array();
    if (!tools_array) {
        json_decref(root);
        return MPK_FAILURE;
    }
    for (f = pkg->tool.lh_first; f; f = f->items.le_next) {
        json_t *tools_item = json_object();
        if (!tools_item) {
            json_decref(tools_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (json_object_set_new(tools_item, "name", json_string(f->name))
                != 0) {
            json_decref(tools_item);
            json_decref(tools_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (f->hash_is_set) {
            write_hexstr(tmp_str, f->hash, MPK_FILEHASH_SIZE);
            tmp_str[MPK_FILEHASH_SIZE * 2] = 0;
        } else {
            tmp_str[0] = 0;
        }
        if (json_object_set_new(tools_item, "hash", json_string(tmp_str))
                != 0) {
            json_decref(tools_item);
            json_decref(tools_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        if (json_array_append(tools_array, tools_item) != 0) {
            json_decref(tools_item);
            json_decref(tools_array);
            json_decref(root);
            return MPK_FAILURE;
        }
        json_decref(tools_item);
    }
    if (json_object_set(root, "tools", tools_array) != 0) {
        json_decref(tools_array);
        json_decref(root);
        return MPK_FAILURE;
    }
    json_decref(tools_array);

    /* files */
    json_t *files_array = json_array();
    if (!files_array) {
        json_decref(root);
        return MPK_FAILURE;
    }
    for (f = pkg->data.lh_first; f; f = f->items.le_next) {
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
        if (f->hash_is_set) {
            write_hexstr(tmp_str, f->hash, MPK_FILEHASH_SIZE);
            tmp_str[MPK_FILEHASH_SIZE * 2] = 0;
        } else {
            tmp_str[0] = 0;
        }
        if (json_object_set_new(files_item, "hash", json_string(tmp_str)) != 0) {
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
    if (json_object_set(root, "data", files_array) != 0) {
        json_decref(files_array);
        json_decref(root);
        return MPK_FAILURE;
    }
    json_decref(files_array);

    /* signature */
    if (pkg->is_signed) {
        write_hexstr(tmp_str, pkg->signature, MPK_PKGINFO_SIGNATURE_LEN);
        tmp_str[MPK_PKGINFO_SIGNATURE_LEN * 2] = 0;
    } else {
        tmp_str[0] = 0;
    }
    if (json_object_set_new(root, "signature", json_string(tmp_str)) != 0) {
        json_decref(root);
        return MPK_FAILURE;
    }

    if (json_dump_file(root, filename, JSON_INDENT(4)|JSON_PRESERVE_ORDER) != 0)
        return MPK_FAILURE;

    json_decref(root);

    return MPK_SUCCESS;
}
