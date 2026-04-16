# vim: filetype=markdown

This file contains information useful to a developer of Dextool.

# Setup

Compared to a normal installation of Dextool a developer have additional needs
such as compiling a full debug build (contracts activated) and compiling the
tests.

A quick and easy way to setup a development build is to run the script from
`tools`.

Example:
```sh
./tools/dev_setup.d
```

This gives access to the make target _test_.

To run the tests:
```sh
# build and run the unit tests
make check

# build and run the integration tests
make check_integration
```

# Optional Linker Configuration

The build can use the `mold` linker, which may reduce link time for larger
builds.

`DEXTOOL_USE_MOLD` controls this behavior:
 * `AUTO` (default): use `mold` if found in `$PATH`
 * `ON`: require `mold`; CMake fails if it is missing
 * `OFF`: never use `mold`

Example:
```sh
cd build
cmake -DDEXTOOL_USE_MOLD=ON ..
```

# Coverage Builds

To measure code coverage for the D implementation itself, configure a debug/test
build with `TEST_WITH_COV=ON` and use a DMD-compatible compiler frontend. Both
`dmd` and `ldmd2` are supported by the CMake coverage flow, but `ldmd2` is
recommended because current integration tests are known to fail with `dmd`.

Example:
```sh
cmake -S . -B build-cov \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TEST=ON \
  -DTEST_WITH_COV=ON \
  -DD_COMPILER="$(command -v ldmd2)"

cmake --build build-cov --target dextool_debug
cmake --build build-cov --target check --parallel
cmake --build build-cov --target check_integration --parallel
```

Coverage output is written to `build-cov/coverage`.

# API Documentation

This describes how to build the API documentation for Dextool (all plugins and the support libraries).

Re-configure cmake with the documentation directive on:
```sh
cd build
cmake -DBUILD_DOC=ON ..
```

For the documentation tool to run it requires that dmd has created the `.json` files with type information. This is done by rebuilding all modules:
```sh
make clean
make all
```

Now lets generate the documentation with the tool.
```sh
./tools/build_doc.d --ddox
```

If you do not have access to internet, remove the `--ddox` parameter.
