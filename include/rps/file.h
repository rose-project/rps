/**
 * @file file.c
 */
#ifndef _FILE_H
#define _FILE_H

#include <string>
#include <vector>
#include <openssl/sha.h>


#define MPK_FILEHASH_SIZE SHA256_DIGEST_LENGTH

namespace RPS {

class File {
public:
    File();
    ~File();

    std::string name() const;
    void setName(const std::string &name);
    const std::vector<uint8_t> &hash() const;
    void setHash(const std::vector<uint8_t> &hash);

private:
    std::string mName;
    std::vector<uint8_t> mHash;
};

}


#endif /* _FILE_H */
