#include "catch2/catch.hpp"
#include "date_util.h"

TEST_CASE("Commit dates use the recorded offset", "[date]") {
  const std::chrono::sys_seconds epoch{};
  CHECK(format_commit_time(epoch, "+0800") == "Thu Jan 01 08:00:00 1970 +0800");
  CHECK(format_commit_time(epoch, "-0530") == "Wed Dec 31 18:30:00 1969 -0530");
  CHECK(format_commit_time(epoch, "-0000") == "Thu Jan 01 00:00:00 1970 -0000");
  CHECK(format_commit_time(epoch - std::chrono::seconds{1}, "+0000") ==
        "Wed Dec 31 23:59:59 1969 +0000");
}

TEST_CASE("Commit date formatting rejects malformed offsets", "[date]") {
  const auto offset = GENERATE("", "0800", "+08x0", "+0860", "+2400");
  CHECK_THROWS_AS(format_commit_time(std::chrono::sys_seconds{}, offset),
                  std::runtime_error);
}
