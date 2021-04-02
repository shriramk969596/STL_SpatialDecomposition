# ESS cmake build tools.

This repository contains CMake modules to facilitate the following tasks:

- Dependencies management
- Product packagement
- Windows development and debug

the utilities can be used by including the following cmake modules:
- ess_debug
- ess_platform
- ess_external
- ess_deploy
- ess_test

*Note that this tools require CMake 3.10*

## Use Notes
To help your fellow developers, please add the following text in your project readme:
```
This Project uses ESS build tools CMake modules, to update the modules use:
**> git submodule update --init --recursive**

This step is required the first time you checkout the code.
For more information please visit the [ESS Build Tools repo](https://gitlab.essteyr.com/ESS/BuildTools)
```

## Example

```
cmake_minimum_required(VERSION 3.10)
project(test_project)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(ess_debug)
include(ess_platform)
include(ess_external)
include(ess_deploy)
include(ess_test)
```
The presented snippet expects this repository to be available under the folder name cmake.
It is not needed to include all modules, but recommended.

## ess_debug:

Provides some asserts that allow us to secure and identify issues in our cmake scripts:
```
ess_assert(IS_TARGET <target name>)      # is the symbol a defined target?
ess_assert(FILE_EXISTS <file full path>) @ does the file exist?
ess_assert(VAR_DEFINED <variable name>)  # is the cmake variable defined?
```
We can print relevant information about a target with:
```
ess_debug_target(<target name>)
```
There is the option of print debug messages
```
ess_debug_message("text")
```
This messages are only visible when passing the flag ```-DESS_CMAKE_DEBUG=on```

## ess_platform

This module defines the following cached variables:

- ESS_PLATFORM_ARCH: current architecture
- ESS_PLATFORM: current platform


## ess_external

This module defines the function ```ess_fetch_external``` to download and use pre-compiled packages maintained by ESS.
Using this module keeps development machines clean and enforces builds to use specific versions of software across platforms.

This module stores pre-compiled binaries in a separate location in your system. This is designed this way to share dependencies across different builds and/or projects.
- In Linux systems, it defaults to ```$HOME/.ess_third_party```
- In windows, it defaults to ```X:\.essthirdparty``` (where X is the same drive as your build folder)

```
ess_fetch_external(NAME <package name> VERSION <three number version> 
     [FIND_PACKAGE] 
     [UNIX_ONLY]
     [COMPONENTS <list of components>]
     [EXTRA_PACKAGES <list of packages>])
```
- FIND_PACKAGE: use CMakes find_package functionality after download, if not present a file named <package name>.cmake must be present in the package and will be used.
- UNIX_ONLY: package is not present in windows builds.
- COMPONENTS: a list of components to import (like boost: thread system regex...)
- EXTRA_PACKAGES: some external dependencies include a extra packages that may or need to be configured. (like lapack, including lapacke and cblas)

SYSTEM install, for some exceptions it is permitted to use the system installation of some library: using SYSTEM instead of version tuple.
This is not widely supported and not recommended, but in some cases we are forced to use it because of third party implementations. 

### fetch external configuration Variables

- Change the default location for packages download: write an environment variable called ```ESS_THIRDPARTY_DIR``` with the desired path.

## ess_internal

This module defines the function ```ess_fetch_internal``` to download and use pre-compiled packages developed by ESS.
Using this module keeps development machines clean and enforces builds to use specific versions of software across platforms.

The binaries are stored in the same location as the [External Dependencies](#ess_external)

```
ess_fetch_external(NAME <package name> VERSION <three number version| or LATEST> 
     [UNIX_ONLY])
```
- UNIX_ONLY: package is not present in windows builds.

### fetch internal configuration Variables

- Change the default location for packages download: write an environment variable called ```ESS_THIRDPARTY_DIR``` with the desired path.
- Prevent ess build tools from downloading the binaries during every compilation: call cmake with ```-DESS_FORCE_INTERNAL_DOWNLOAD=off```

## ess_deploy:

This module facilitates the creation of a package of the compiled artifacts. It recursively fetches dependencies to make them available in the target machine. Using this module reduces the amount of dependencies required in the target machine.

When used to generate a **Visual Studio Build**, it copies the necessary dlls to the runtime folder, so the binary can be executed from the IDE.

```
ess_deploy(TARGETS <list of targets> 
           GROUP  <name of the package>
           [HEADERS <list of header files to deploy>])
```

After using ess_deploy, it is required to add ```install(EXPORT <name of the package> DESTINATION cmake)``` to generate the transitive cmake module. This procedure minimizes the amount of configuration files required to be maintained as other project can just include the file <name of the package>.cmake file generated. 

### deploy configuration Variables

-  To copy and overwrite the dlls every time cmake runs (Visual Studio), use ```-DESS_FORCE_LIBRARIES_COPY=on``` with the cmake invocation.

## ess_test

Support for unit tests:

```
ess_add_test(TARGET <asociated target> NAME <test executable name>
    FILES <sources>
    [LIBS <sources>]
    [INCLUDE <sources>]
    [OPTIONS <sources>]
    [DEFINITIONS <sources>]
    [CATCH]
)
```

All the sources provided will be compiled and linked into a single executable per invocation. 
The executable will be made available to CTest.
- INCLUDE: Extra includes
- LIBS: Extra libraries
- OPTIONS: compile flags 
- DEFINITIONS: preprocessor definitions 
- CATCH: flag makes available to the programmer the catch.hpp unit test header included in this submodule.
- GTEST: flag makes available to the programmer the google-test  infrastructure.

### Testing libraires

Currently different testing libraries can be used:

| Library       | C++ standard  | requires packages?  |   documentation |
| ------------- |:-------------:|:----:|:--------------:|
| catch         | >= c++11      |  no, header only    |   https://github.com/google/googletest |
| gtest         | < c++11       | yes, requires precompiled libraries | https://github.com/catchorg/Catch2 |


### Coverage
The test module includes as well the capability to instrument tests and executables for a coverage analysis.
use ```-DESS_COVERAGE=1``` to the cmake invocation to add coverage information in unit-tests. Use ```-DESS_COVERAGE_ALL=1``` to instrument every executable produced.

Currently coverage works with GCC and gcov.


## Compiler Options
The ESS Build tools provide a easy way to pass some compilation flags to the targets. Such tools are defined in the ess_compile_options module.

### Debug symbols even in Release mode

To make symbol names available (for debug and profiling) in the release build, link against the target ```ESS::debug_symbols```
This tool will automatically detect the language to pass the correct symbols (i.e. cuda code)

