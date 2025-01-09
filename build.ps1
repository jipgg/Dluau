cmake --preset compilation-database .
cmake --preset default .
cmake --build cmake-out --target minlu
cmake --build cmake-out --target minlu.cli
cmake --build cmake-out --target minlu.standard
