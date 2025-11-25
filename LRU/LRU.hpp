#pragma once

#include <list>
#include <unordered_map>
#include <cstddef>
#include <iostream>


namespace LRU {
template <typename T, typename KeyT = int>
class cache_t {
    size_t sz_;
    std::list<T> cache_;
    
    using ListIt = typename std::list<T>::iterator;
    std::unordered_map<KeyT, ListIt> hash_;
    
    bool full() const {
        return cache_.size() == sz_;
    }
    
public:
    cache_t(size_t sz) : sz_(sz) {}
    
    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page)
    {    
        auto hit = hash_.find(key);
        if (hit == hash_.end()) { // not found
            T new_page = slow_get_page(key); // может бросить, состояние не меняем

            bool pushed = false;
            try {
                cache_.push_front(new_page); // может бросить
                pushed = true;
                hash_[key] = cache_.begin(); // может бросить

                if (cache_.size() > sz_ && cache_.size() != 0) {
                    KeyT rem_key = cache_.back().id;
                    cache_.pop_back();
                    hash_.erase(rem_key);
                }
                return false;
            } catch (...) {
                if (pushed) {
                    cache_.pop_front();
                }
                hash_.erase(key);
                throw;
            }
        }
        auto eltit = hit->second;
        if (eltit != cache_.begin())
            cache_.splice(cache_.begin(), cache_,
                          eltit, std::next(eltit));
        return true;
    }  

    class Dumper {
        const cache_t &cache_;

        explicit Dumper(const cache_t &cache) : cache_(cache) {}
        friend class cache_t;

        void dump_header(std::ostream &os) const {
            os << "CACHE: sz=" << cache_.sz_
               << " size=" << cache_.cache_.size()
               << "\n";
        }

        void dump_list(std::ostream &os) const {
            os << "List (front -> back):\n";
            size_t idx = 0;
            for (const auto &page : cache_.cache_) {
                os << "  [" << idx << "] id=" << page.id << "\n";
                ++idx;
            }
        }

        void dump_hash(std::ostream &os) const {
            os << "Hash entries:\n";
            for (const auto &kv : cache_.hash_) {
                os << "  key=" << kv.first
                   << " -> id=" << kv.second->id
                   << "\n";
            }
        }
    public:
        void dump(std::ostream &os = std::cout) const {
            dump_header(os);
            dump_list(os);
            dump_hash(os);
            os << std::flush;
        }
    };

    Dumper dumper() const { return Dumper(*this); }
    void print() const { dumper().dump(); }
};
} /*namespace LRU*/
