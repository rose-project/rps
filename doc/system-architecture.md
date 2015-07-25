
# System Architecture

fail-save, transaction based


## Parition Layout

* every updatable partition necessary to enter recovery is duplicated
* a recovery system can alway repair the system in case flashing fails

Partition layout example

``
bl1         | Bootloader Stage1                 | will never be updated
pt          | partition table                   | never updatet, changed atomically
bl2         | Bootloader stage 2                | 
bl2-b       | backup of bootloader stage 2      | 
rec-kernel  | recovery image kernel             | 
rec-system  | recovery image                    | 
kernel      | contains kernel image             | 
system      | recovery image                    | read only rootfs
app         | applications                      | applications, their configuration and resouces
user        | user data storage                 |
``


