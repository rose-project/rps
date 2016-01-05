/**
 * @file filehandling.h
 * 
 * Some methods for handling files and directories.
 */
#ifndef _FILEHANDLING_H
#define _FILEHANDLING_H

/**
 * @brief Remove a directory and all of its contents.
 * @param name The directory to delete.
 */
int mpk_filehandling_deletedir(const char *name);

/**
 * @brief Copy a directory.
 * @param dst Directory to copy to.
 * @param src Directory to copy from.
 */
int mpk_filehandling_copydir(const char *dst, const char *src);

/**
 * Copy a file.
 */
int mpk_filehandling_copyfile(const char *dst, const char *src);

/**
 * Creates a directory including parents if necessary.
 *
 * @param path The path to create
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_filehandling_createdir(const char *path, mode_t mode);

/**
 * @brief Returns a string allocated with malloc that contains the filename
 * without a path without a trailing suffix.
 * @param fpath The path of the file including its filename.
 * @return Pointer to the basename if successful or NULL otherwise.
 */
char *mpk_filehandling_basename(const char *fpath);

#endif /* _FILEHANDLING_H */
