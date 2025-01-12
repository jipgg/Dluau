cmake --preset compilation-database .
cmake --preset default .
cmake --build cmake-out --target lumin-sdk
cmake --build cmake-out --target lumin-std
