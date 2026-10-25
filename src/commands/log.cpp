#include "commands/log.h"
#include "commit.h"
#include "object.h"
#include <iostream>
#include <queue>
#include <unordered_set>
#include <utility>

#include "parsers/LogParser.h"
#include "repository.h"

namespace commands {
void log(std::vector<std::string> &args) {
  const auto options = LogParser::parse(args);
  GitRepository repo = GitRepository::find();
  std::string commit = GitObject::find(repo, options.commit);
  struct PendingCommit {
    GitCommit commit;
    std::int64_t timestamp;
    std::size_t order;
  };
  const auto older = [](const PendingCommit &a, const PendingCommit &b) {
    if (a.timestamp != b.timestamp) {
      return a.timestamp < b.timestamp;
    }
    return a.order > b.order;
  };
  std::priority_queue<PendingCommit, std::vector<PendingCommit>,
                      decltype(older)>
      pending(older);
  std::unordered_set<std::string> discovered;
  std::size_t order = 0;
  const auto enqueue = [&](const std::string &id) {
    if (discovered.insert(id).second) {
      auto next = GitCommit::read(repo, id);
      const auto timestamp = next.get_committer_timestamp();
      pending.push({std::move(next), timestamp, order++});
    }
  };

  enqueue(commit);
  while (!pending.empty()) {
    auto current = pending.top().commit;
    pending.pop();
    std::cout << current.print_commit() << "\n";
    for (const auto &parent : current.get_parents()) {
      enqueue(parent);
    }
  }
}
} // namespace commands
