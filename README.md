# Git C++lient

## Contents

A Git client written in C++, so that I can:
- learn C++
- learn Git

Project structure:

- [src](src)
  - the `adder` library source code
  - this is where the meat of the project is: the implementation
- [include](include/adder)
  - adder (to be removed)
  - tclap (tclap)
  - the public interface of the library
- [app](app)
  - the application which uses the libraries in `include`
- [tests](tests)
  - the test code
  - each `x.cpp` file has a corresponding `x_t.cpp` file here with tests
- [ext](ext)
  - external libraries, e.g. Catch2 testing framework
- [.github/workflows/ci.yml](.github/workflows/ci.yml)
  - the GitHub Actions configuration

## Compiling

To compile the project and run the tests:

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make test
```

## Documentation

If you have Doxygen installed you can also build the documentation by enabling the `BUILD_DOCS` CMake option, and then running `make doxygen`:

```
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON
make doxygen
```

This will generate the documentation in the `html` folder.