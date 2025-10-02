#pragma once

#include <unordered_map>
#include <cstddef>
#include <vector>
#include <cassert>
#include <iostream>


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
        KeyT req_id = requests_[request_num];
        
        int ret = number_in_future(request_num + 1, req_id);
        if (ret == -1) return req_id;
        
        for (auto it = hash_.begin(); it != hash_.end(); it++) {
            T el = it->second.page;
            int fn = it->second.next_req;
            if (fn == -1) {
                std::cout << "Something strange" <<std::endl;
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
            if (full() && hash_.size() != 0) {
                KeyT for_removal = least_important_el(request_num);
                if(key == for_removal) return false;

                auto fr_it = hash_.find(for_removal);
                assert(fr_it != hash_.end());
                hash_.erase(for_removal);
            }
            
            hash_[key].page = new_page;
            hash_[key].next_req = number_in_future(request_num + 1, key);
            return false;
        }
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

    void print() const {
        using std::cout;
        using std::endl;
    
        cout << "=== CACHE DEBUG PRINT ===" << endl;
    
        cout << "Cache size: " << sz_ << endl;
        cout << "Requests: [ ";
        for (size_t i = 0; i < requests_.size(); i++) {
            cout << requests_[i] << (i + 1 < requests_.size() ? " " : "");
        }
        cout << " ]" << endl;
    
        cout << "Hash table:" << endl;
        for (const auto &p : hash_) {
            cout << "  key=" << p.first 
                 << " -> id=" << p.second->id 
                 << endl;
        }
    
        cout << "=========================" << endl;
    }

    void print(size_t request_num) const {
        using std::cout;
        using std::endl;
    
        cout << "=== CACHE DEBUG PRINT ===" << endl;
        
        cout << "Cache size: " << sz_ << endl;
        cout << "Requests: [ ";
        for (size_t i = 0; i < requests_.size(); i++) {
            if (i == request_num) {
                cout << ">>" << requests_[i] << "<< ";
            } else {
                cout << requests_[i] << " ";
            }
        }
        cout << "]" << endl;
    
        cout << "Hash table:" << endl;
        for (const auto &p : hash_) {
            cout << "  key=" << p.first 
                 << " -> id=" << p.second->id 
                 << endl;
        }
    
        cout << "=========================" << endl;
    }


};
} /*namespace ideal*/


