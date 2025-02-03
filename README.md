# Git C++lient

## Contents

A Git client written in C++, so that I can:
- learn C++
- learn Git

Project structure:

- [src](src)
  - this is where the meat of the project is: the implementation
- [include](include)
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

## Dev setup
1. Clone repository
2. Install the VSCode [clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) extension

## Compiling

To compile the project and run the tests:

```
mkdir build (if not created yet)
cd build
../run.sh [-p] (specify to run in release mode)
```

## Documentation

If you have Doxygen installed you can also build the documentation by enabling the `BUILD_DOCS` CMake option, and then running `make doxygen`:

```
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON
make doxygen
```

This will generate the documentation in the `html` folder.