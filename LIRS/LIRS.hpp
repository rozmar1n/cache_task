#pragma once

#include <list>
#include <unordered_map>
#include <cstddef>
#include <optional>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <vector>

namespace LIRS {


template <typename T, typename KeyT = int>
class cache_t {

    size_t sz_;
    size_t lir_cap;
    size_t hir_cap;
    size_t lir_counter = 0;
    
    enum status {
        INVALID = -1,
        LIR, 
        HIR_RES, 
        HIR_NRES
    };


    std::list<KeyT> stack_s_;
    std::list<KeyT> stack_q_;
    
    std::vector<T> cache_;
    std::vector<size_t> free_list_;

    using ListIt = typename std::list<KeyT>::iterator;
    using VecIt = typename std::vector<T>::iterator;

    struct HashEntry_t {
        std::optional<ListIt> itS;
        std::optional<ListIt> itQ;
        std::optional<size_t> pg_idx; 
       
        status st = INVALID;

        HashEntry_t() = default;

        HashEntry_t(std::optional<ListIt> s, 
                    std::optional<ListIt> q,
                    status st = INVALID,
                    std::optional<size_t> p = std::nullopt) 
        :   itS(s), 
            itQ(q),
            st(st),
            pg_idx(p) {}
    };


    std::unordered_map<KeyT, HashEntry_t> hash_;

public:
    cache_t(size_t sz, double hir_ratio = 0.1) : sz_(sz) {
        if (sz_ >= 2) {
            hir_cap = std::max(1UL, static_cast<size_t>(sz_ * hir_ratio));
            lir_cap = sz_ - hir_cap;
            
            cache_.resize(sz_);
            free_list_.reserve(sz_);
            for (size_t i = 0; i < sz_; i++) {
                free_list_.push_back(sz_ - 1 - i);
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
                process_hir_res_hit(eltit);
                return true;
            case HIR_NRES:
                process_hir_nres_hit(eltit, slow_get_page(key));
                return false;
            default:
                assert(false && "Unknown state");
        }
        return false;
    }


    void print() const {
        std::cout << "CACHE: sz=" << sz_
                  << " LIR_cap=" << lir_cap
                  << " HIR_cap=" << hir_cap
                  << " LIR_count=" << lir_counter
                  << " StackS.size=" << stack_s_.size()
                  << " StackQ.size=" << stack_q_.size()
                  << "\n";
    
        std::cout << "StackS (front -> back):\n";
        size_t idx = 0;
        for (auto key = stack_s_.begin(); key != stack_s_.end(); ++key, ++idx) {
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
        for (auto it = stack_q_.begin(); it != stack_q_.end(); ++it, ++idx) {
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
            std::cout << " page=" << (kv.second.pg_idx.has_value() ? "yes" : "no");
            std::cout << "\n";
        }

        std::cout << std::flush;
    }
private:
    void free_place() {
        KeyT victim_id = stack_q_.back();
        auto vit = hash_.find(victim_id);
        assert(vit != hash_.end());
        auto &victim = vit->second;
        
        bool in_StackS = victim.itS != std::nullopt;
        
        if (in_StackS) {
            victim.st = HIR_NRES;
            assert(victim.pg_idx.has_value()); 
            
            delete_from_cache(victim.pg_idx.value());
            victim.pg_idx = std::nullopt;
            
            victim.itQ = std::nullopt;
            stack_q_.pop_back();
            prune_StackS();
        } else {
            assert(victim.pg_idx.has_value()); 
                
            delete_from_cache(victim.pg_idx.value());
            
            stack_q_.pop_back();
            hash_.erase(victim_id);
        }
    }

    size_t add_to_cache(T page) {
        if (free_list_.empty()) {
            assert(!stack_q_.empty());
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
        while (!stack_s_.empty()) {
            KeyT bottom_id = stack_s_.back();
            auto it = hash_.find(bottom_id);
            assert(it != hash_.end());
            auto &entry = it->second;
            
            if (entry.st == LIR) {
                if (lir_counter > lir_cap) {
                    entry.st = HIR_RES;
                    lir_counter--;
                    
                    add_to_StackQ(bottom_id, it->second);
                    
                    stack_s_.pop_back();
                    entry.itS = std::nullopt;
                } else {
                    break;
                }
            } 
            else {
                stack_s_.pop_back();
                
                if (entry.st == HIR_RES) {
                    entry.itS = std::nullopt;
                } else {
                    assert(entry.pg_idx == std::nullopt);
                    hash_.erase(bottom_id);
                }
            }
        }
    }

    void add_to_StackQ(KeyT p_id, HashEntry_t& entry) {
        if (stack_q_.size() < hir_cap) {
            stack_q_.push_front(p_id);
            entry.itQ = stack_q_.begin();
            return;
        }
        
        assert(!stack_q_.empty());
        free_place();

        stack_q_.push_front(p_id);
        entry.itQ = stack_q_.begin();
    }

    void add_new(T page) {
        stack_s_.push_front(page.id);
    
        size_t idx = add_to_cache(page);;
        auto &entry_it = hash_[page.id];
        if (lir_counter < lir_cap) {
            entry_it = HashEntry_t(stack_s_.begin(), 
                                         std::nullopt, LIR, idx);
            lir_counter++;
        } else {
            entry_it = HashEntry_t(stack_s_.begin(), 
                                         std::nullopt, HIR_RES, idx);
            add_to_StackQ(page.id, entry_it);  
        }
        prune_StackS();
    }
 

    void proc_LIR(ListIt elem) {
        stack_s_.splice(stack_s_.begin(), stack_s_, elem);
        prune_StackS();
        return;
    }

    void process_hir_res_hit(HashEntry_t& elem) {
        bool is_in_StackS = elem.itS.has_value();
 
        if (is_in_StackS) {
            stack_s_.splice(stack_s_.begin(), stack_s_, elem.itS.value());
     
            elem.st = LIR;
            lir_counter++;
            if (elem.itQ) {
                stack_q_.erase(elem.itQ.value());
                elem.itQ = std::nullopt;
            }
     
             prune_StackS();
        } else {
            KeyT key = *elem.itQ.value();
            
            stack_q_.splice(stack_q_.begin(), stack_q_, elem.itQ.value());
            
            stack_s_.push_front(key);
            elem.itS = stack_s_.begin();
     
            prune_StackS();
        }
    }

    void process_hir_nres_hit(HashEntry_t& elem, T page) {
        assert(elem.pg_idx == std::nullopt);
        
        elem.pg_idx = add_to_cache(page);        
        if (elem.itS.has_value()) {
            stack_s_.splice(stack_s_.begin(), stack_s_, elem.itS.value());
        } else {
            stack_s_.push_front(page.id);
            elem.itS = stack_s_.begin();
        }
    
        if (lir_counter < lir_cap) {
            elem.st = LIR;
            lir_counter++;
        } else {
            elem.st = HIR_RES;
            add_to_StackQ(page.id, elem);
        }
        prune_StackS();
    }


};
} //LIRS
