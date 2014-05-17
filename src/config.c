/**
 * @file config.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <syslog.h>
#include "config.h"

mpk_ret_t mpk_config_init(const char *config_file)
{
    /* TODO: read config file */

    syslog(LOG_INFO, "read config");

    return MPK_SUCCESS;
}

void mpk_config_destroy()
{

}
