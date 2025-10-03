#pragma once

#include <list>
#include <unordered_map>
#include <cstddef>
#include <optional>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <vector>

namespace LIRS {


template <typename T, typename KeyT = int>
class cache_t {

    size_t sz_;
    size_t LIR_cap;
    size_t HIR_cap;
    size_t LIR_count = 0;
    
    enum status {
        INVALID = -1,
        LIR, 
        HIR_RES, 
        HIR_NRES
    };


    std::list<KeyT> StackS_;
    std::list<KeyT> StackQ_;
    
    std::vector<T> cache_;
    std::vector<size_t> free_list_;

    using ListIt = typename std::list<KeyT>::iterator;
    using VecIt = typename std::vector<T>::iterator;

    struct HashEntry_t {
        std::optional<ListIt> itS;
        std::optional<ListIt> itQ;
        size_t pg_idx; 
       
        status st = INVALID;

        HashEntry_t() = default;

        HashEntry_t(std::optional<ListIt> s, 
                    std::optional<ListIt> q,
                    status st = INVALID,
                    size_t p = SIZE_MAX) 
        :   itS(s), 
            itQ(q),
            st(st),
            pg_idx(p) {}
    };

    void free_place() {
        KeyT victim_id = StackQ_.back();
        auto vit = hash_.find(victim_id);
        assert(vit != hash_.end());
        auto &victim = vit->second;
        
        bool in_StackS = victim.itS != std::nullopt;
        
        if (in_StackS) {
            victim.st = HIR_NRES;
            assert(victim.pg_idx != SIZE_MAX); 
            
            delete_from_cache(victim.pg_idx);
            victim.pg_idx = SIZE_MAX;
            
            victim.itQ = std::nullopt;
            StackQ_.pop_back();
            prune_StackS();
        } else {
            assert(victim.pg_idx != SIZE_MAX); 
                
            delete_from_cache(victim.pg_idx);
            
            StackQ_.pop_back();
            hash_.erase(victim_id);
        }
    }

    std::unordered_map<KeyT, HashEntry_t> hash_;
private:
    size_t add_to_cache(T page) {
        if (free_list_.empty()) {
            //TODO: обработать случай, когда нет места
            assert(!StackQ_.empty());
            free_place();
        } 
        
        size_t idx = free_list_.back();
        free_list_.pop_back();
        cache_[idx] = page;
        return idx;
    }

    size_t delete_from_cache(size_t idx) {
        //TODO: подумать, стоит ли как-то занулить страницу в кэше
        free_list_.push_back(idx);
        return idx;
    }

    void prune_StackS() {
        while (!StackS_.empty()) {
            KeyT bottom_id = StackS_.back();
            auto it = hash_.find(bottom_id);
            assert(it != hash_.end());
            auto &entry = it->second;
            
            if (entry.st == LIR) {
                if (LIR_count > LIR_cap) {
                    entry.st = HIR_RES;
                    LIR_count--;
                    
                    add_to_StackQ(bottom_id);
                    
                    StackS_.pop_back();
                    entry.itS = std::nullopt;
                } else {
                    break;
                }
            } 
            else {
                StackS_.pop_back();
                
                if (entry.st == HIR_RES) {
                    entry.itS = std::nullopt;
                } else {
                    assert(entry.pg_idx == SIZE_MAX);
                    hash_.erase(bottom_id);
                }
            }
        }
    }

    void add_to_StackQ(KeyT p_id) {
        if (StackQ_.size() < HIR_cap) {
            StackQ_.push_front(p_id);
            hash_[p_id].itQ = StackQ_.begin();
            return;
        }
        
        assert(!StackQ_.empty());
        free_place();

        StackQ_.push_front(p_id);
        hash_[p_id].itQ = StackQ_.begin();
    }

    void add_new(T page) {
        StackS_.push_front(page.id);
    
        size_t idx = add_to_cache(page);
        if (LIR_count < LIR_cap) {
            hash_[page.id] = HashEntry_t(StackS_.begin(), 
                                         std::nullopt, LIR, idx);
            LIR_count++;
        } else {
            hash_[page.id] = HashEntry_t(StackS_.begin(), 
                                         std::nullopt, HIR_RES, idx);
            add_to_StackQ(page.id);  
        }
        prune_StackS();
    }
 

    void proc_LIR(ListIt elem) {
        StackS_.splice(StackS_.begin(), StackS_, elem);
        prune_StackS();
        return;
    }

    void proc_HIR_RES(HashEntry_t& elem) {
        bool is_in_StackS = elem.itS.has_value();
 
        if (is_in_StackS) {
            StackS_.splice(StackS_.begin(), StackS_, elem.itS.value());
     
            elem.st = LIR;
            LIR_count++;
            if (elem.itQ) {
                StackQ_.erase(elem.itQ.value());
                elem.itQ = std::nullopt;
            }
     
             prune_StackS();
        } else {
            KeyT key = *elem.itQ.value();
            
            StackQ_.splice(StackQ_.begin(), StackQ_, elem.itQ.value());
            
            StackS_.push_front(key);
            elem.itS = StackS_.begin();
     
            prune_StackS();
        }
    }

    void proc_HIR_NRES(HashEntry_t& elem, T page) {
        assert(elem.pg_idx == SIZE_MAX);
        
        elem.pg_idx = add_to_cache(page);        
        if (elem.itS.has_value()) {
            StackS_.splice(StackS_.begin(), StackS_, elem.itS.value());
        } else {
            StackS_.push_front(page.id);
            elem.itS = StackS_.begin();
        }
    
        if (LIR_count < LIR_cap) {
            elem.st = LIR;
            LIR_count++;
            prune_StackS();
        } else {
            elem.st = HIR_RES;
            add_to_StackQ(page.id);
            return;
        }
    }


public:
    cache_t(size_t sz, double hir_ratio = 0.1) : sz_(sz) {
        if (sz_ >= 2) {
            HIR_cap = std::max(1UL, static_cast<size_t>(sz_ * hir_ratio));
            LIR_cap = sz_ - HIR_cap;
            
            cache_.resize(sz_);
            free_list_.reserve(sz_);
            for (size_t i = 0; i < sz_; i++) {
                free_list_.push_back(sz - 1 - i);
            }

        } else if (sz_ <= 1) {
            //WARN "cache size should be at least 2"
            throw std::invalid_argument("Cache size must be at least 2");
        }
    }

    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page) {
        auto hit = hash_.find(key);
        if (hit == hash_.end()) {
            add_new(slow_get_page(key));    
            return false;
        }
        auto &eltit = hit->second;

        switch (eltit.st) {
            case LIR:
                proc_LIR(eltit.itS.value());
                return true;
            case HIR_RES:
                proc_HIR_RES(eltit);
                return true;
            case HIR_NRES:
                proc_HIR_NRES(eltit, slow_get_page(key));
                return false;
            default:
                assert(false && "Unknown state");
        }
        return false;
    }


    void print() const {
        std::cout << "CACHE: sz=" << sz_
                  << " LIR_cap=" << LIR_cap
                  << " HIR_cap=" << HIR_cap
                  << " LIR_count=" << LIR_count
                  << " StackS.size=" << StackS_.size()
                  << " StackQ.size=" << StackQ_.size()
                  << "\n";
    
        std::cout << "StackS (front -> back):\n";
        size_t idx = 0;
        for (auto key = StackS_.begin(); key != StackS_.end(); ++key, ++idx) {
            std::cout << "  [" << idx << "] id=" << *key;
            auto h = hash_.find(*key);
            if (h != hash_.end()) {
                std::cout << " st=";
                switch (h->second.st) {
                    case LIR:     std::cout << "LIR";       break;
                    case HIR_RES: std::cout << "HIR_RES";   break;
                    case HIR_NRES:std::cout << "HIR_NRES";  break;
                }
                std::cout << " itS=" << (h->second.itS ? "yes" : "no");
                std::cout << " itQ=" << (h->second.itQ ? "yes" : "no");
            } else {
                std::cout << " (no hash entry)";
            }
            std::cout << "\n";
        }

        std::cout << "StackQ (front -> back):\n";
        idx = 0;
        for (auto it = StackQ_.begin(); it != StackQ_.end(); ++it, ++idx) {
            auto key = *it;
            std::cout << "  [" << idx << "] id=" << key;
            auto h = hash_.find(key);
            if (h != hash_.end()) {
                std::cout << " st=";
                switch (h->second.st) {
                    case LIR:     std::cout << "LIR";       break;
                    case HIR_RES: std::cout << "HIR_RES";   break;
                    case HIR_NRES:std::cout << "HIR_NRES";  break;
                }
                std::cout << " itS=" << (h->second.itS ? "yes" : "no");
                std::cout << " itQ=" << (h->second.itQ ? "yes" : "no");
            }
            std::cout << "\n";
        }

        std::cout << "Hash table entries:\n";
        for (const auto &kv : hash_) {
            std::cout << "  key=" << kv.first << " st=";
            switch (kv.second.st) {
                case LIR:     std::cout << "LIR";       break;
                case HIR_RES: std::cout << "HIR_RES";   break;
                case HIR_NRES:std::cout << "HIR_NRES";  break;
            }
            std::cout << " itS="  << (kv.second.itS  ? "yes" : "no");
            std::cout << " itQ="  << (kv.second.itQ  ? "yes" : "no");
            std::cout << " page=" << (cache_[kv.second.pg_idx] ? "yes" : "no");
            std::cout << "\n";
        }

        std::cout << std::flush;
    }
};
} //LIRS
