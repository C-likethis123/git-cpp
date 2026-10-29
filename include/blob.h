#ifndef BLOB_H
#define BLOB_H

#include <string>

#include "object.h"

class GitBlob : public GitObject {
public:
  GitBlob(const std::string &data = std::string(""));

  static GitBlob read(const std::string &hash);
  void deserialise(const std::string &data) override;
  std::string serialise() override;

  void init();

protected:
  std::string blobData;
};

#endif // BLOB_H
