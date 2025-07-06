#ifndef BLOB_H
#define BLOB_H

#include <string>

#include "object.h"

class GitBlob : public GitObject {
public:
  GitBlob(const std::string &data = std::string(""));

  static GitBlob read(GitRepository &repo, const std::string &hash);
  void deserialise(const std::string &data) override;
  std::string serialise(GitRepository &repo) override;

  void init();

protected:
  std::string blobData;
};

#endif // BLOB_H
