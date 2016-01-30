/**
 * @file file.c
 */
#ifndef _FILE_H
#define _FILE_H

#include <string>
#include <openssl/sha.h>


#define MPK_FILEHASH_SIZE SHA256_DIGEST_LENGTH

namespace RPS {

class File {
public:
    enum class Type {
        Undefined = -1, /**< undefined type */
        ReadOnly,
        Executable,
        Writeable,
        Symlink,
        Directory
    };

public:
    File();
    ~File();

};

}


#endif /* _FILE_H */
