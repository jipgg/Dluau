> **Disclaimer** This is a work-in-progress.
Any API, feature and/or functionality is volatile and open to change at any time.
The project lacks proper documentation, varying degrees of comprehensibility in error messages
and does not ensure stability of implemented features.
# Dluau
A minimal runtime that extends the [Luau](https://github.com/luau-lang/luau) language and turns it into a more flexible, general purpose and dynamically extensible scripting environment.
## Features
It extends the Luau C API with a minimal set of utilities to standardize/synchronize userdata type tags
and namecall stringatoms for more easily extending the environment with external Luau C API that can be dynamically loaded with the builtin `dlimport` library.

For this to work reliably the runtime consists of a shared dll named `dluaulib.dll` which other dlls can link to or import symbols from.

Also does it provide an abstraction layer for the windows executable host. As opposed to Linux, Windows differentiates between true console applications and windows applications,
the latter allowing you to create GUI applications and the former behaving like your standard console app. Dluau always runs the environment as a Windows subsystem from the host
and emulates the console behavior to behave functionally the same as it would work on Linux.
What this means is that you can potentially run both gui apps and console apps from the same host application.

It also provides a couple of quality-of-life features like a `nameof` pseudo function and a `script` 'library' which get resolved to string literals before compilation.
Also does it implement a `task` library that is essentially a port of Roblox's task library with some minor differences and features.

The `os` library features have been reverted to the vanilla lua version.
`os.execute`, `os.getenv`, `os.remove` and `os.rename` have been re-enabled.
### DLL loading
I've made a basic cmake [stub library (outdated)](https://github.com/jipgg/dluaulib-stub) of the dluaulib.dll and Luau + dluau API that you can add as a
subdirectory to your CMakeLists.txt for creating your own DLLs without having to build the whole source.
Or a [cmake project example (outdated)](https://github.com/jipgg/dluau-dlmodule-example) to clone/use as reference that
already links the stub library and gives you a base project to start with.
**To link the stub library with cmake:**
```cmake
add_subdirectory(path/to/dluaulib-stub)
target_link_libraries(yourtarget PRIVATE dluaulib-stub)
```
After building your own DLL you can then include it with `dlimport.require('mymodule')`.
For this to work you need to export a `dlrequire` lua_CFunction from your dlmodule, this function will get invoked when called from luau.
```luau
type example = {
    -- some api
}
local example = dlimport.require('@somewhere/somedll') :: example
```
You can also import specific lua_CFunctions by first loading the dlmodule and then calling the `importfunction` method.
This again requires you to prefix your exported C function symbol with `dlexport_`*.
```luau
local dlexample: dlmodule = dlimport.load('@somewhere/somedll')
local my_imported_function: (some_arg: string)->() = dlexample:importfunction('some_imported_function_symbol')
-- note: actual exported dll symbol would be `dlexport_some_imported_function_symbol`
my_imported_function("hello world!")
```
This convention serves as a safeguard because i didn't find a practical way to ensure an exported symbol is indeed of type lua_CFunction,
since dlimport also supports creating bindings of 'true' C functions. This seemed like the best compromise to create a barrier between
safe luau c functions and UB territory that can happen when loading functions with different parameters and return types,
since lua_CFunctions are always the same type (`int(*)(lua_State*)`).  

### searching DLLs
The searching algorithm for finding the DLL behaves the same as with regular the `require('someluaumodule')`
meaning that you can also use '@aliases' in this function.
When loading a dll with dlimport it also adds the dll's directory as a location for the system to find other DLL dependencies.
So when your dll imports symbols from another DLL it will find that dependency even if it is not in the conventional places where the system
searches in like PATH as long as a copy of the dll exists inside the dlmodule directory.

Use `dlimport.searchpath` when you want to search for the absolute path of a DLL in the system.
## Autocompletion/LSP
the dluau api definition and documentation files for [luau-lsp](https://github.com/JohnnyMorganz/luau-lsp) can be found [here](lsp/).
### vscode
In VSCode you can then add these 2 files to your definition- and documentation files in the luau-lsp settings to get the autocompletion working.
### Neovim
For Neovim users there is a basic LSP configuration script called `nvimlspsetup.lua` in [lsp/](lsp/) for luau-lsp to resolve the aliases etc.
You can either create an `.nvim.lua` rc file, if you have this option enabled, to set up the lsp (like this project does it) or put the script source inside your neovim config somewhere.
Afterwards call
```lua
require('path.to.nvimlspsetup').setup({"path/to/definition/file.d.luau"}, {"path/to/docs/file.json"})
```
to set up the LSP.
## Platform support
At this point in time only Windows is being fully supported and mainly worked on out of personal development convenience,
but the plan is to fully support Linux in the future once i've gotten the project to a relatively stable state.
No plans for macOS support at the moment, however.
## Installation
You can download the latest binaries from [releases](https://github.com/jipgg/dluau/releases) or build from source.
### Building from source
#### Dependencies
- [dyncall] (https://dyncall.org/) for dynamically calling C functions and creating struct mappings.
- [nlohman::json] (https://github.com/nlohmann/json) for json parsing
- [Boost.Container] (https://github.com/boostorg/container) for a C++20 `flat_map` and `flat_set` implementation.
#### Resolving dependencies
The external project dependencies are mostly self-contained in the project, but it currently does
require you to resolve the CMake Boost.Container package on your own. I personally use vcpkg for this.
Eventually i will probably just add the dependency as a subdirectory to remove the hassle of needing to use
external package managers/installations for building from source.
#### General build example:
```sh
cmake --preset default
cmake --build --preset cli --config Release
cmake --build --preset lib --config Release
```
## Using the CLI
To run a luau file all you have to do is specify the file as an argument when running the CLI.
This can be done multiple times.
```sh
dluau path/to/script.luau path/to/other_script.luau ...
```
Also does it support to run all the luau scripts inside a a specific folder by specifying the source as a folder with appending the argument with '/'.
```sh
dluau scripts/
```
To specify the optimization level when compiling the luau source you can specify `-O<number here>`.
To specify the debug level the same applies but use `-D`.
The order of where you specify these flags in the command does not matter.
Default debug level is 1 and default optimization level is 0 when no flags are specified.
**Example:**
```
dluau -O2 example.luau -D0
```
## Ignore the tests folder
The tests are mostly just for me to quickly mash up some scripts to test the environment and check if functionality stayed relatively the same.
They are by no means formal tests that can ensure the stability of the build.
Also are the not all up-to-date with the latest build.
