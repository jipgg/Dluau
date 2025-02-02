> **Disclaimer** This is a work-in-progress.
Any API, feature and/or functionality is volatile and open to change at any time.
The project lacks proper documentation, varying degrees of comprehensibility in error messages
and does not ensure stability of implemented features.
# Dluau
A minimal runtime that extends the [Luau](https://github.com/luau-lang/luau) language and turns it into a more flexible, general purpose and dynamically extensible scripting environment.
## Features
It extends the Luau C API with a minimal set of utilities to standardize/synchronize userdata type tags
and namecall stringatoms for more easily extending the environment with external C API that can be dynamically loaded with the builtin `dlimport` library.
It also provides a couple of quality-of-life features like a `nameof` pseudo function and a `script` 'library' which get resolved to string literals before compilation.
Also does it implement a `task` library that is essentially a port of Roblox's task library with some minor differences and features.
### DLL loading
I've made a basic cmake [stub library](https://github.com/jipgg/dluaulib-stub) of the dluaulib.dll and Luau + dluau API that you can add as a
subdirectory to your CMakeLists.txt for creating your own DLLs without having to build the whole source.
**To link the stub library with cmake:**
```cmake
add_subdirectory(path/to/dluaulib-stub)
target_link_libraries(yourtarget PRIVATE dluaulib-stub)
```
After building your own DLL you can then include it with `dlimport.require('mymodule')`.
For this to work you need to export a `dlrequire` lua_CFunction from your dlmodule, this function will get invoked when called from luau.
You can also import specific lua_CFunctions by first loading the dlmodule and then calling the `importfunction` method.
This again requires you to prefix your exported C function symbol with `dlfunction_*`.
This convention serves as a safeguard because i didn't find a practical way to ensure an exported symbol is indeed of type lua_CFunction,
since dlimport also supports creating bindings of 'true' c functions (currently only supports primitive c types as return type and parameters,
but i plan on eventually support aggregate types (c structs) once i out a good way to register these dynamically in luau) this seemed like the best compromise to create a barrier between
safe luau c functions and UB territory that can happen when loading functions with different parameters and return types,
since lua_CFunctions are always the same type (`int(*)(lua_State*)`).  

The searching algorithm for finding the DLL behaves the same as with regular the `require('someluaumodule')`
meaning that you can also use '@aliases' in this function.
When loading a dll with dlimport it also adds the dll's directory as a location for the system to find other DLL dependencies.
So when your dll imports symbols from another DLL it will find that dependency even if it is not in the conventional places where the system
searches in like PATH as long as a copy of the dll exists inside the dlmodule directory.
> still need to make an example project to put here as reference
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
### General build example:
```sh
cmake --preset default
cmake --build --preset cli --config Release
cmake --build --preset lib --config Release
```
## Working with the environment
> not written yet
## Ignore the tests folder
The tests are mostly just for me to quickly mash up some scripts to test the environment and check if functionality stayed relatively the same.
They are by no means formal tests that can ensure the stability of the build.
