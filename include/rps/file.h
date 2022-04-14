/**
 * @file file.c
 */
#ifndef _FILE_H
#define _FILE_H

#include <openssl/sha.h>
#include <string>
#include <vector>

#define MPK_FILEHASH_SIZE SHA256_DIGEST_LENGTH

namespace rose
{

class File
{
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

} // namespace rose

#endif /* _FILE_H */
