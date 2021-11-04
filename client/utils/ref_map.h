#pragma once

#include <unordered_map>
#include <string>
#include "types.h"

template<class Loader>
class RefMap {
    using T = typename Loader::ValueType;
    struct Value {
        T value;
        u32 refcount = 0;
    };
public:
    ~RefMap() {
        for(auto &val: map) {
            Loader::errorForgot(val.second.value);
            Loader::unload(val.second.value);
        }
    }

    template<typename ...Targs>
    T load(const std::string &key, Targs &&...args) {
        auto iter = map.find(key);
        if(iter != map.end()) {
            iter->second.refcount++;
            return iter->second.value;
        }
        else {
            T value = Loader::load(key, args...);
            map[key] = Value{ value, 1 };
            return value;
        }
    }

    bool tryUnload(T  &to_unload) {
        if(!Loader::isValid(to_unload)) {
            return false;
        }
        for(auto iter = map.begin(); iter != map.end(); ++iter) {
            if(Loader::areSame(iter->second.value, to_unload)) {
                iter->second.refcount--;
                if(iter->second.refcount == 0) {
                    Loader::unload(to_unload);
                    map.erase(iter);
                    return true;
                }
                return false;
            } 
        }
        Loader::errorNotFound(to_unload);
        return false;
    }

    u32 getRefCount(const T &to_find) {
        for(auto &iter: map) {
            if(Loader::areSame(iter->second.value, to_find)) {
                return iter.second.refcount;
            }
        }
        Loader::errorNotFound(to_find);
        return 0;
    }

    u32 getRefCount(const std::string &key) {
        return map[key].refcount;
    }

private:
    std::unordered_map<std::string, Value> map;
};