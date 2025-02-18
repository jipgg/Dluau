#include <dluau.h>
#include <lualib.h>
#include <string>
#include <dluau.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
using std::vector, boost::container::flat_set;
using boost::container::flat_map, std::string;

class Userdata_type_registry{
    flat_set<string> registry_;
    flat_map<string, int> tags_;
    vector<const char*> zarray_;
public:
    auto zarray() -> const char* const* const {
        if (zarray_.size() > 0) return zarray_.data();
        return nullptr;
    } 
    auto exists(const string& v) -> bool {
        return registry_.contains(v);
    }
    auto add(string v) -> bool {
        if (registry_.contains(v)) return false;
        registry_.emplace(std::move(v));
        sync_containers();
        return true;
    }
    auto add_tagged(string v) -> bool {
        if (registry_.contains(v)) return false;
        tags_.emplace(v, dluau_newuserdatatag());
        registry_.emplace(std::move(v));
        sync_containers();
        return true;
    }
    auto get_tag(const string& v) -> int {
        if (not tags_.contains(v)) return 0;
        return tags_[v];
    }
    auto remove(string v) -> bool {
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

static Userdata_type_registry userdata_types;

auto dluau_gettagfromtype(const char *tname) -> int {
    return userdata_types.get_tag(tname);
}
auto dluau_istyperegistered(const char *tname) -> bool {
    return userdata_types.exists(tname);
}

auto dluau_registertypetagged(const char *tname) -> int {
    userdata_types.add_tagged(tname);
    dluau::compile_options->userdataTypes = userdata_types.zarray();
    return userdata_types.get_tag(tname);
}
void dluau_registertype(const char* tname) {
    userdata_types.add(tname);
    dluau::compile_options->userdataTypes = userdata_types.zarray();
}
