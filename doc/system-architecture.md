# System Architecture

fail-save, transaction based


## File System Layout

The files of each pacakge are installed in separate directory located in '/usr/apps'. This directory is only changed by the package service. The apps store any data they need to '/var/appdata'. 


## Parition Layout

* every updatable partition necessary to enter recovery is duplicated
* a recovery system can alway repair the system in case flashing fails

Partition layout example

| Partition   | Description                                               | Mounted         |
|-------------|-----------------------------------------------------------|-----------------|
| bl1         | Bootloader stage 1; will never be updated                 |                 |
| pt          | partition table, never updated, changed atomically        |                 |
| bl2         | Bootloader stage 2                                        |                 |
| bl2-b       | backup of bootloader stage 2                              |                 |
| rec-kernel  | recovery image kernel                                     |                 |
| rec-system  | recovery image                                            |                 |
| kernel      | contains the linux kernel                                 |                 |
| system      | system image                                              | / ro            |
| apps        | contains apps, only changed by RPS                        | /usr/apps rw    |
| appdata     | application data storage                                  | /var/appdata rw |


## System Update

System is updated by writing new images of the partitions bl2, bl2-b, rec-kernel, rec-system, kernel, system.

TODO: Fails safe update strategy


## Factory Reset

For a factory reset simply the partitions 'apps' and 'appdata' have to be emptied. 


