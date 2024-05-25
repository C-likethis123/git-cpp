# Case Study

# Error 
/workspace/git-cpp/src/object.cpp: In constructor ‘GitObject::GitObject(const string&)’:
/workspace/git-cpp/src/object.cpp:14:57: error: binding reference of type ‘std::string&’ {aka ‘std::__cxx11::basic_string<char>&’} to ‘const string’ {aka ‘const std::__cxx11::basic_string<char>’} discards qualifiers
   14 | GitObject::GitObject(const std::string& format): format(format) {
      |                                                         ^~~~~~


# TODO: Reasons