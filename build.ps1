cmake --preset compilation-database .
cmake --preset default .
cmake --build cmake-out --target minluau
cmake --build cmake-out --target minluau.cli
cmake --build cmake-out --target minluau.modules
