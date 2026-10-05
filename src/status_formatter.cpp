#include "status_formatter.h"
#include <cstdlib>
#include <ostream>
#include <unistd.h>

namespace status_formatter {

namespace {
constexpr const char *green = "\033[32m";
constexpr const char *red = "\033[31m";
constexpr const char *reset = "\033[0m";
} // namespace

bool use_status_colour() {
  const char *term = std::getenv("TERM");
  return isatty(STDOUT_FILENO) && std::getenv("NO_COLOR") == nullptr &&
         (term == nullptr || std::string(term) != "dumb");
}

void print_status(const StatusResult &status, std::ostream &out,
                  bool colour_enabled,
                  const std::filesystem::path &current_directory) {
  const bool has_no_unstaged_changes = status.unstaged.modified.empty() &&
                                       status.unstaged.deleted.empty();
  const bool has_no_staged_changes = status.staged.modified.empty() &&
                                     status.staged.added.empty() &&
                                     status.staged.deleted.empty();
  const bool has_no_untracked_files = status.untracked.empty();
  if (has_no_unstaged_changes && has_no_staged_changes && has_no_untracked_files) {
    out << "nothing to commit, working tree clean\n";
    return;
  }

  const auto print_files = [&](const std::vector<std::string> &files,
                               const char *label, const char *colour) {
    for (const auto &file : files) {
      // Lexical conversion also works for deleted files and preserves symlinks.
      const auto display_path =
          std::filesystem::path(file).lexically_relative(current_directory);
      out << (colour_enabled ? colour : "") << '\t' << label
          << display_path.generic_string()
          << (colour_enabled ? reset : "") << '\n';
    }
  };

  bool printed_section = false;
  const auto print_heading = [&](const char *heading) {
    if (printed_section) {
      out << '\n';
    }
    out << heading << '\n';
    printed_section = true;
  };

  if (!has_no_staged_changes) {
    print_heading("Changes to be committed:");
    print_files(status.staged.modified, "modified: ", green);
    print_files(status.staged.added, "new file: ", green);
    print_files(status.staged.deleted, "deleted: ", green);
  }
  if (!has_no_unstaged_changes) {
    print_heading("Changes not staged for commit:");
    print_files(status.unstaged.modified, "modified: ", red);
    print_files(status.unstaged.deleted, "deleted: ", red);
  }
  if (!has_no_untracked_files) {
    print_heading("Untracked files:");
    print_files(status.untracked, "", red);
    out << '\n';
  }
}

} // namespace status_formatter
