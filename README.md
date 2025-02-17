> **Disclaimer** This is a work-in-progress.
Any API, feature and/or functionality is volatile and open to change at any time.
The project lacks proper documentation, varying degrees of comprehensibility in error messages
and does not ensure stability of implemented features.
# Dluau
A minimal 'runtime' that extends the [Luau](https://github.com/luau-lang/luau) language with a more flexible feature-set for loading and running external Luau C API DLLs.
The goal of this project is to stay minimal, mostly serving as just an extended luau environment,
meaning that this project does not come with a standard library with general purpose tools.
However, i am working on a [General Purpose Library]() that serves as a substitute for the lack of standard library, focused on modularity, only loading the dynamic libraries that you are actually using.
## Features
Provides a shared runtime environment `dluaulib` for ensuring compatibility between external dynamic libraries that import/export dluau C API.

Extends the Luau C API with a minimal set of utilities to standardize/synchronize userdata type tag creation
and namecall stringatoms with the goal of making it easier to extend the environment with external Luau C API that can be dynamically loaded with the builtin `dlimport` library.

Also does it provide an abstraction layer for the windows executable host. As opposed to Linux, Windows differentiates between 'true' console applications and windows applications in the form of subsystems,
the latter allowing you to create GUI applications and the former behaving like your standard console app. Dluau always runs the environment as a Windows subsystem from the host
and emulates the console behavior to behave functionally almost the same as your standard console app.
What this means is that you can potentially run both gui apps and console apps from the same host application.

It also provides a couple of quality-of-life features like a `nameof<T>(variable: T)` precompiler function and a precompiled `script` 'library' which get resolved to string literals before compilation.

The baseline for a Luau runtime these days seems to be that it at least provides a `task` library that loosely ports the Roblox Studio version, so also this 'runtime' has one implemented.

The `os` library features have been reverted to the vanilla lua version.
`os.execute`, `os.getenv`, `os.remove` and `os.rename` have been reenabled.
### DLL loading
You can dynamically import the symbols from `dluaulib`, which is the most flexible approach, but also the least optimal one.
For statically linking with cmake for C/C++ projects i've made a basic cmake [stub library](https://github.com/jipgg/dluaulib-stub) of the dluaulib.dll and Luau + dluau API that you can add as a
subdirectory to your CMakeLists.txt for creating your own DLLs without having to build the whole source of this project.
Allows you to link with the dluaulib symbol entry points without the need for it to have the full implementation of the source code.
For an example/reference [cmake project using the stub library](https://github.com/jipgg/dluau.gpm)

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
I am not sure if i will keep this convention yet.
My main motivation for this is to make it less easy to accidentally import non-`lua_CFunction`s
which would result in undefined behavior. Dynamically importing function symbols is inherently type unsafe.

### searching DLLs
The searching algorithm for finding the DLL behaves the same as with the regular `require` function, meaning that it support aliases.
When loading a dll with dlimport it also adds the DLL's directory as a location for the system to find other DLL dependencies.
So when your DLL imports symbols from another DLL it will find that dependency even if it is not in the conventional places where the system
searches in like PATH as long as a copy of the dll exists inside one of the loaded `dlmodule` directories.

Use `dlimport.searchpath` in cases you want to search for an installed DLL in the system. Returns the absolute path on found.

## Autocompletion/LSP
The dluau api definition and documentation files for [luau-lsp](https://github.com/JohnnyMorganz/luau-lsp) can be found [here](lsp/).
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
At this point in time only **Windows** is being fully supported and mainly worked on out of personal development convenience,
but the plan is to fully support Linux in the future once i've gotten the project to a relatively stable state.
No plans for macOS support at the moment, however.
## Installation
You can download the latest binaries from [releases](https://github.com/jipgg/dluau/releases) or build from source.
### Building from source
#### Dependencies
**[nlohman::json] (https://github.com/nlohmann/json)** for json parsing

**[Boost.Container] (https://github.com/boostorg/container)** for a `flat_map` and `flat_set` implementation.
The project uses the C++23 Standard, but sadly `std::flat_map` and `std::flat_set` support/implementation in the big 3 compilers is still very limited.
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
# Contact
Best way of contacting me is leaving me a DM on Discord at `jipg`.
