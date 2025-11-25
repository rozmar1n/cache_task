#pragma once

#include <unordered_map>
#include <cstddef>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <optional>

namespace ideal {
template <typename T, typename KeyT = int>
class cache_t {
    size_t sz_;
    std::vector<KeyT> requests_;
    
    struct hash_entry {
        T page;
        int next_req = -1;
    };

    std::unordered_map<KeyT, hash_entry> hash_;
    std::unordered_map<KeyT, std::vector<int>> nums_in_req_;

    bool full() const {
        return hash_.size() == sz_;
    }
private:
    void fill_in_the_table() {
        for (int i = 0; i < requests_.size(); i++) {
            nums_in_req_[requests_[i]].push_back(i);
        }
    }

    int number_in_future(int start, KeyT elem) {
        std::vector<int>& nums = nums_in_req_[elem];

        auto it = std::lower_bound(nums.begin(), nums.end(), start);
        if (it != nums.end()) {
            return *it;
        }

        return -1;
    }
    bool is_in_future(int start, KeyT elem) {
        return number_in_future(start, elem) != -1;
    }
    KeyT least_important_el(int request_num) {
        int ret = request_num; 
        for (auto it = hash_.begin(); it != hash_.end(); it++) {
            T el = it->second.page;
            int fn = it->second.next_req;
            if (fn == -1) {
                return it->first;
            }
             
            if (ret < fn) {
                ret = fn;
            }
        }
        return requests_[ret];
    }

public:
    cache_t(size_t sz, std::vector<KeyT> rq) : 
        sz_(sz),
        requests_(rq) {
        fill_in_the_table();
    };
    
    template <typename F>
    bool lookup_update(int request_num, F slow_get_page)
    {    
        KeyT key = requests_[request_num];
        auto hit = hash_.find(key);
        if (hit == hash_.end()) { 
            T new_page = slow_get_page(key);
            if (!is_in_future(request_num + 1, key)) {
                return false;
            }

            std::optional<std::pair<KeyT, hash_entry>> evicted;
            try {
                if (full() && hash_.size() != 0) {
                    KeyT for_removal = least_important_el(request_num);
                    if(key == for_removal) return false;

                    auto fr_it = hash_.find(for_removal);
                    assert(fr_it != hash_.end());
                    evicted = std::make_pair(fr_it->first, fr_it->second);
                    hash_.erase(fr_it);
                }
                
                hash_entry entry;
                entry.page = new_page;
                entry.next_req = number_in_future(request_num + 1, key);
                hash_[key] = std::move(entry);
                return false;
            } catch (...) {
                if (evicted) {
                    hash_.emplace(evicted->first, std::move(evicted->second));
                }
                throw;
            }
        }
        hit->second.next_req = number_in_future(request_num + 1, key);
        return true;
    }
    
    template <typename F>
    int run_cache(F slow_get_page) {
        int counter = 0;
        for(int i = 0; i < requests_.size(); i++) {
            if(lookup_update(i, slow_get_page)) {
                counter++;
            }
        }
        return counter;
    }

    class Dumper {
        const cache_t &cache_;

        explicit Dumper(const cache_t &cache) : cache_(cache) {}
        friend class cache_t;

        void dump_requests(std::ostream &os, std::optional<size_t> current) const {
            os << "Requests: [ ";
            for (size_t i = 0; i < cache_.requests_.size(); i++) {
                if (current && i == *current) {
                    os << ">>" << cache_.requests_[i] << "<<";
                } else {
                    os << cache_.requests_[i];
                }
                if (i + 1 < cache_.requests_.size()) {
                    os << " ";
                }
            }
            os << " ]" << std::endl;
        }

        void dump_hash(std::ostream &os) const {
            os << "Hash table:" << std::endl;
            for (const auto &p : cache_.hash_) {
                os << "  key=" << p.first 
                   << " -> id=" << p.second.page.id 
                   << std::endl;
            }
        }

        void dump_impl(std::optional<size_t> request_num, std::ostream &os) const {
            os << "=== CACHE DEBUG PRINT ===" << std::endl;
            os << "Cache size: " << cache_.sz_ << std::endl;
            dump_requests(os, request_num);
            dump_hash(os);
            os << "=========================" << std::endl;
        }
    public:
        void dump(std::ostream &os = std::cout) const {
            dump_impl(std::nullopt, os);
        }

        void dump(size_t request_num, std::ostream &os = std::cout) const {
            dump_impl(request_num, os);
        }
    };

    Dumper dumper() const { return Dumper(*this); }
    void print() const { dumper().dump(); }
    void print(size_t request_num) const { dumper().dump(request_num); }


};
} /*namespace ideal*/
