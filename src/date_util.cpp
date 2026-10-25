#import "date_util.h"

#include <iomanip>
#include <sstream>

std::string format_unix_time(int64_t unix_timestamp_str) {
  std::time_t rawtime = static_cast<std::time_t>(unix_timestamp_str);
  std::tm *timeinfo = std::localtime(&rawtime);

  std::ostringstream oss;
  oss << std::put_time(timeinfo, "%a %b %d %H:%M:%S %Y %z");

  return oss.str();
}
