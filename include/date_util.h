#ifndef DATEUTIL_H
#define DATEUTIL_H

#include <chrono>
#include <string>

std::string format_commit_time(std::chrono::sys_seconds timestamp,
                               const std::string &timezone);
#endif // DATEUTIL_H
