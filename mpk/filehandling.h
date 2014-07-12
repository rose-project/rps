/**
 * @file filehandling.h
 * @author Josef Raschen <josef@raschen.org>
 * @copyright 2014 Josef Raschen <josef@raschen.org>
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

#endif /* _FILEHANDLING_H */
