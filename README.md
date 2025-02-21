# Dluau

Luau runtime with a minimal core, focused on ease of extensibility in the form
of providing a synchronized environment for loading external luau C API dynamic libraries.
## Basic preprocessing
The runtime does some regex-based preprocessing at startup for `dlload`, `dlrequire` and `require`, which allows it
to resolve dependencies (scripts, DLLs and Standard library dependencies) statically,
minimizing the runtime overhead and keeping performance more consistent by doing a little more heavylifting up front.

As a fun side effect this essentially 'sandboxes' your environment by
exclusively being able to use the DLLs and scripts you've specified yourself explicitly in the
original source.

Adds a `nameof` preprocessed function that essentially resolves the specified variable name to a `("string literal")`.

## The Core
This part is meant to only minimally extend the luau language in terms of
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

