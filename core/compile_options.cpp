#include <lumin.h>
#include <lualib.h>
#include <vector>
#include <string>


std::vector<std::string> types = {
    "module",
};
std::vector<const char*> arr = {
    "module",
    nullptr
};

void lumin_adduserdatatype(const char *tname) {
    std::string& t = types.emplace_back(tname);
    if (arr.size() > 1) arr.pop_back();
    arr.emplace_back(t.c_str());
    arr.emplace_back(nullptr);
    lumin_globalcompileoptions->userdataTypes = arr.data();
}
