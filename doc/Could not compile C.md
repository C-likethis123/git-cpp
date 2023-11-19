# Case Study

Tried to link the INIH library to this project. 
Compilation was successful but linking causes the following error

```
[90%] Linking CXX executable adder_app
/usr/bin/ld: ../src/libinih.a(inih.cpp.o): in function `INIReader::INIReader(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)':
/workspace/git-cpp/src/inih.cpp:22: undefined reference to `ini_parse'
/usr/bin/ld: ../src/libinih.a(inih.cpp.o): in function `INIReader::INIReader(char const*, unsigned long)':
/workspace/git-cpp/src/inih.cpp:28: undefined reference to `ini_parse_string'
collect2: error: ld returned 1 exit status
make[2]: *** [app/CMakeFiles/adder_app.dir/build.make:104: app/adder_app] Error 1
make[1]: *** [CMakeFiles/Makefile2:988: app/CMakeFiles/adder_app.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
```

# Issue

This was attributed to two causes:
1. From the error messages, ini_parse and ini_parse_string was not defined. I did not link inih.c in file.
2. The project could not detect the C file. This was discovered by running `ar -t libinih.a`
  - Enabled C in this project: `enable_language(C)`
  - If this does not work, we can explicitly set language settings for the file