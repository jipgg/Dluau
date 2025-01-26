#include <dluau.h>
#include <lualib.h>
#include <vector>
#include <string>
#include <shared.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
namespace bc = boost::container;
using std::string;

class userdata_type_registry{
    bc::flat_set<string> registry_;
    bc::flat_map<string, int> tags_;
    std::vector<const char*> zarray_;
public:
    const char* const* zarray() const {
        if (zarray_.size() > 0) return zarray_.data();
        return nullptr;
    } 
    bool exists(const string& v) {
        return registry_.contains(v);
    }
    bool add(string v) {
        if (registry_.contains(v)) return false;
        registry_.emplace(std::move(v));
        sync_containers();
        return true;
    }
    bool add_tagged(string v) {
        if (registry_.contains(v)) return false;
        tags_.emplace(v, dluau_newuserdatatag());
        registry_.emplace(std::move(v));
        sync_containers();
        return true;
    }
    int get_tag(const string& v) {
        if (not tags_.contains(v)) return 0;
        return tags_[v];
    }
    bool remove(string v) {
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

static userdata_type_registry userdata_types;

int dluau_gettagfromtname(const char *tname) {
    return userdata_types.get_tag(tname);
}
bool dluau_istyperegistered(const char *tname) {
    return userdata_types.exists(tname);
}

int dluau_registertypetagged(const char *tname) {
    userdata_types.add_tagged(tname);
    shared::compile_options->userdataTypes = userdata_types.zarray();
    return userdata_types.get_tag(tname);
}
void dluau_registertype(const char* tname) {
    userdata_types.add(tname);
    shared::compile_options->userdataTypes = userdata_types.zarray();
}
