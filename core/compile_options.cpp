#include <lumin.h>
#include <lualib.h>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <boost/container/flat_set.hpp>
namespace bc = boost::container;

class Cztring_registry{
    bc::flat_set<std::string> registry_;
    std::vector<const char*> zarray_;
public:
    const char* const* zarray() const {
        if (zarray_.size() > 0) return zarray_.data();
        return nullptr;
    } 
    bool exists(const std::string& v) {
        return registry_.contains(v);
    }
    bool add(std::string v) {
        if (registry_.contains(v)) return false;
        registry_.emplace(std::move(v));
        sync_containers();
        return true;
    }
    bool remove(std::string v) {
        if (not registry_.contains(v)) return false;
        registry_.erase(v);
        sync_containers();
        return true;
    }
private:
    void sync_containers() {
        const size_t size = registry_.size();
        zarray_.resize(size + 1);
        for (int i{}; i < size; ++i) {
            zarray_[i] = registry_.nth(i)->c_str();
        }
        zarray_.back() = nullptr;
    }
};

static Cztring_registry userdata_types;

void lumin_adduserdatatype(const char *tname) {
    userdata_types.add(tname);
    lumin_globalcompileoptions->userdataTypes = userdata_types.zarray();
}
