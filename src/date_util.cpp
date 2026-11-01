#include "date_util.h"

#include <format>
#include <stdexcept>

std::string format_commit_time(std::chrono::sys_seconds timestamp,
                               const std::string &timezone) {
  if (timezone.size() != 5 || (timezone[0] != '+' && timezone[0] != '-') ||
      timezone.find_first_not_of("0123456789", 1) != std::string::npos) {
    throw std::runtime_error("Invalid commit timezone: " + timezone);
  }
  const auto hours = std::stoi(timezone.substr(1, 2));
  const auto minutes = std::stoi(timezone.substr(3, 2));
  if (hours > 23 || minutes > 59) {
    throw std::runtime_error("Invalid commit timezone: " + timezone);
  }
  const auto offset = std::chrono::minutes{(hours * 60 + minutes) *
                                           (timezone[0] == '-' ? -1 : 1)};
  const auto local =
      std::chrono::local_seconds{timestamp.time_since_epoch() + offset};
  return std::format("{:%a %b %d %H:%M:%S %Y} {}", local, timezone);
}
