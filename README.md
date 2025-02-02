> **Disclaimer** This is a work-in-progress.
Any API, feature and/or functionality is volatile and open to change any time.
The project lacks proper documentation, varying degrees of comprehensibility in error messages
and does not ensure stability of implemented features.
# Dluau
A minimal runtime that extends the [Luau](https://github.com/luau-lang/luau) language and turns it into a more flexible, general purpose and dynamically extensible scripting environment.
## Features
It extends the Luau C API with a minimal set of utilities to standardize/synchronize userdata type tags
and namecall stringatoms for more easily extending the environment with external C API that can be dynamically loaded with the builtin `dlimport` library.
It also provides a couple of quality-of-life features like a `nameof` pseudo function and a `script` 'library' which get resolved to string literals before compilation.
Also does it implement a `task` library that is essentially a port of Roblox's task library with some minor differences and features.
### Creating a `dlmodule`
## Platform support
At this point in time only Windows is being fully supported and mainly worked on out of personal development convenience,
but the plan is to fully support Linux in the future once i've gotten the project to a relatively stable state.
No plans for macOS support at the moment, however.
## To build
### Dependencies
The external project dependencies are mostly self-contained in the project, but it currently does
require you to resolve the CMake Boost.Container package on your own. I personally use vcpkg for this.
Eventually i will probably just add the dependency as a subdirectory to remove the hassle of needing to use
external package managers/installations for building from source.
### Build command example:
```sh
cmake --preset default
cmake --build --preset cli --config Release
cmake --build --preset lib --config Release
```
