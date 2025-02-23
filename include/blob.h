#ifndef BLOB_H
#define BLOB_H

#include <boost/filesystem.hpp>
#include <string>

#include "object.h"

class GitBlob : public GitObject {
public:
  GitBlob(const std::string &data = std::string(""));

  void deserialise(const std::string &data) override;
  std::string serialise(GitRepository &repo) override;

  void init();

protected:
  std::string blobData;
};

#endif // BLOB_H
