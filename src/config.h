/**
 * @file config.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief simple api to access libmpk configuration
 */
#ifndef _CONFIG_H
#define _CONFIG_H

/**
 * @brief mpk_config_init reads the libmpk config file
 * @param config_file the config file to read
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_config_init(const char *config_file);

void mpk_config_destroy();

#endif /* _CONFIG_H */
