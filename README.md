# Lumin
> **Disclaimer:** This is a Work-In-Progress. It is nowhere near a stable nor complete state. 

Lumin is a lightweight and highly extensible Luau runtime environment, designed with flexibility, modularity, and ease of integration in mind. It offers a minimal core and powerful DLL loading features, allowing seamless interoperability between Luau scripts and native C++ modules. Lumin is perfect for projects that require dynamic extension and modular code execution.
## Key Features
- **Extensible**: Easily extend the runtime with custom modules and bindings.
- **Minimal Core**: Focuses on providing only the essential functionality for a lean environment.
- **Modular Architecture**: Breaks down functionality into modular components, so you can add or remove features as needed.
- **Dynamic DLL Loading**: Load and manage external DLL modules with ease, allowing for runtime flexibility.
- **Simple C++ Interfacing**: Exports a simple convention of `lua_CFunctions` that can be dynamically loaded and used in Luau scripts.
- **Caching for Performance**: Caches loaded DLL modules to ensure that each library is only loaded once during the runtime session, optimizing performance.
## Interfacing with C++ and DLLs
Lumin makes it simple to interface with external C++ code and DLLs. The core convention is that **each DLL** you want to integrate with the Luau runtime must export a function with the signature of `lua_CFunction`. The expected function name in each DLL is **`loadmodule`**.
You can easily load modules from these DLLs using the `dll.loadmodule` feature in Luau. Additionally, you can dynamically load other functions with the `dll.loadfunction(dllname, function_symbol)` method. This gives you full flexibility to interact with external C++ code, making it a breeze to extend the runtime.
### Caching DLLs
Lumin handles DLL caching automatically, ensuring that once a DLL is loaded, it will not be loaded again during the same runtime session, optimizing performance and preventing unnecessary redundancy.
### Future Plans
- **Relative Path Searching:** Lumin will automatically search for DLLs in directories relative to the current script's execution environment, making module loading more flexible.
- **Packaging and Installing External Modules:** Plans to support external module packaging and installation to simplify the deployment and usage of additional libraries.uture Plans
- **(Hopefully) Support C API bindings that are not limited to `lua_CFunctions`:** This might not be feasible, but would be a great feat.
## Building
```bash
    cmake --preset default
    cmake --build --preset sdk
    cmake --build --preset extras
    cmake --build --preset core
```
