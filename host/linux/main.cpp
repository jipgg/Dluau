#include "../host_main.hpp"

int main(int argc, char** argv) {
    std::vector<std::string_view> args;
    for (int i{}; i < argc; ++i) args.emplace_back(argv[i]);
    return host_main(args);
}
