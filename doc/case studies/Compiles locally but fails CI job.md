# motivation

It was really annoying.

# error 1: return ntohl(value); // if data is big-endian (common in file formats)

Issue: In mac, some libraries transitively include it. However my CI was running on ubuntu.
To fix it, I had to specify different library versions based on different types of builds:
1. add a config.h.in file
2. add the support for it in CMakeLists.txt
3. Update the run scripts and specify the version to build

# error 2: missing vectors

Issue: even though I included <vector> in tree.h transitively, there are still compiler errors in the CI job because that library wasn't included.

Adding <vector> is also in line with the "include what you use" principle.

Also had to add <algorithm> for `std::sort`