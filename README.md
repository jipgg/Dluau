# Dluau
An informal, work-in-progress [Luau](https://luau.org/) runtime with a minimal core, focused on ease of extensibility and minimalism in the form
of providing a synchronized environment for loading external luau C API dynamic libraries and statically resolving dependencies.

In its current stage, the project does lack proper debugging information and error messages in certain aspects as well as
a very volatile standard library API. Suggestions for the standard library features and design decisions would be HIGHLY appreciated.
## Resolves Dependencies Statically
The runtime does some mild regex-based preprocessing at startup for `dlload`, `dlrequire`, `require` and `nameof`, which allows it
to resolve dependencies (scripts, DLLs and Standard library dependencies) statically,
minimizing the runtime overhead and keeping performance more consistent by doing a little more heavy-lifting up front.

As a fun side effect this essentially 'sandboxes' your environment by
exclusively being able to use the DLLs and scripts you've specified yourself explicitly in the
original source.

## Minimal Core
The core is meant to only minimally extend the luau language in terms of
added libraries, only adding features that are inherently essential for the
runtime to work in a synchronized manner between loaded external DLLs.

`dlload`, `dlrequire` and `require` calls are resolved at startup, significantly
removing the overhead at runtime by turning them into a simple hash table lookup.
All of these functions use the same logic for resolving paths. `"@aliases"` are supported.

### C API
The core exports all the symbols of the Luau API as well as extra utility API for:
- registering type names (which get added to the internal lua_CompileOptions)
- creating unique (light)userdata tags
* registering stringatoms for the `__namecall` userdata metamethod dynamically (the runtime registers a namecallatom in lua_callbacks for returning consistent atoms between dlls)

I will probably set up a basic example/template for creating dlmodules soon.
Main things to know is that at startup, when a `dlinit` symbol is exported by your DLL is invokes that function as a `void(*)(lua_State*)`. 
If you specify a `dlrequire` function in your DLL this one gets automatically invoked and its data returned when you call `dlrequire` in luau.
The type of this dlrequire in C is the same as `lua_CFunction`.

## Modular Standard Library
The standard library is subdivided in a multitude of DLLs.

It only loads the DLLs that you are actually using in your scripts.
Main reasoning for this approach is to keep the runtime as lightweight as possible regardless of how big the standard library eventually becomes.

The standard library is accessed through the global `std` instead of using `require`. This seemed like the most ergonomic option.

At this current stage the library is a bit all over the place, mostly just reusing some of my previously made modules for an older project, but it does
already provide some very useful features like some FFI support for C bindings as well as some basic tools to make POD C struct bindings.

Lacks proper documentation at the moment, but this is planned.

## Platform support
Currently Windows is exclusively supported. I will try to eventually port it to Linux once the project has reached a somewhat mature stage.

## Autocompletion/LSP
The definition files for [luau-lsp](https://github.com/JohnnyMorganz/luau-lsp) can be found [here](lsp).
It provides 2 definition files, one for the core and onde for the standard library.
Documentation files will be coming eventually.

## CLI Commands
Example:
```ps1
dluau some_source_file.luau some_source_dir/ -O1 -D2
```
`-O1` `-O2` `-O0` optimization level
`-D0` `-D1` `-D2` debug level
`source/` `source.luau` source specification -> can be done multiple times.

## Installation
Download the latest [binaries](https://github.com/jipgg/Dluau/releases) or build from source.
While not necessary, it is recommended to the the binary directory to you PATH.
### Build from source.
It does have a multitude of cmake package dependencies,
which need to be resolved on your machine.
I personally use [vcpkg](https://vcpkg.io/en/) to resolve this on Windows.
**To build:**
```ps1
cmake --preset default
cmake --build --preset core
cmake --build --preset cli
cmake --build --preset std
```
## Contact
For any questions or suggestions you can contact me on Discord at `jipg`.
