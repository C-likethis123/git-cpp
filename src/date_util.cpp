#import "date_util.h"

#include <iomanip>
#include <sstream>

std::string format_unix_time(std::string &unix_timestamp_str) {
  long long raw_time_val = std::stoll(unix_timestamp_str);
  std::time_t rawtime = static_cast<std::time_t>(raw_time_val);
  std::tm *timeinfo = std::localtime(&rawtime);

  std::ostringstream oss;
  oss << std::put_time(timeinfo, "%a %b %d %H:%M:%S %Y %z");

  return oss.str();
}
