#include <list>
#include <unordered_map>
#include <cstddef>
#include <optional>
#include <cstdlib>
#include <iostream>
#include <cassert>

namespace LIRS {


template <typename T, typename KeyT = int>
class cache_t {

    size_t sz_;
    size_t LIR_cap;
    size_t HIR_cap;
    size_t LIR_count = 0;
    size_t res_count = 0;
    size_t HIR_res_count = 0;
    
    enum status {
        INVALID = -1,
        LIR, 
        HIR_RES, 
        HIR_NRES
    };


    std::list<KeyT> StackS;
    std::list<KeyT> StackQ;

    using ListIt = typename std::list<KeyT>::iterator;

    struct HashEntry_t {
        std::optional<ListIt> itS;
        std::optional<ListIt> itQ;
        
        std::optional<T> page;
        status st = INVALID;

        HashEntry_t() = default;

        HashEntry_t(std::optional<ListIt> s, 
                    std::optional<ListIt> q) 
        :   itS(s), 
            itQ(q),
            st(HIR_NRES) {}
        HashEntry_t(std::optional<ListIt> s, 
                    std::optional<ListIt> q,
                    status st,
                    std::optional<T> v) 
        :   itS(s), 
            itQ(q),
            st(st),
            page(v) {}
    };

private:
    std::unordered_map<KeyT, HashEntry_t> hash_;

    void prune_StackS() {
        while (!StackS.empty()) {
            KeyT bottom_id = StackS.back();
            auto it = hash_.find(bottom_id);
            assert(it != hash_.end());
            auto &entry = it->second;
            
            if (entry.st == LIR) {
                if (LIR_count > LIR_cap) {
                    entry.st = HIR_RES;
                    LIR_count--;
                    
                    add_to_StackQ(bottom_id, false);
                    
                    StackS.pop_back();
                    entry.itS = std::nullopt;
                } else {
                    break;
                }
            } 
            else {
                StackS.pop_back();
                
                if (entry.st == HIR_RES) {
                    entry.itS = std::nullopt;
                } else {
                    hash_.erase(bottom_id);
                }
            }
        }
    }

    void add_to_StackQ(KeyT p_id, bool is_new_resident = true) {
        if (StackQ.size() < HIR_cap) {
            StackQ.push_front(p_id);
            hash_[p_id].itQ = StackQ.begin();
            if (is_new_resident) {
                HIR_res_count++;
                res_count++;
            } else {
                HIR_res_count++;  
            }
            return;
        }
    
        KeyT fst_hir_id = StackQ.back();
        auto fst_hit_it = hash_.find(fst_hir_id);
        assert(fst_hit_it != hash_.end());
        auto &fst_hit_ref = fst_hit_it->second;
    
        bool in_StackS = fst_hit_ref.itS != std::nullopt;
    
        if (in_StackS) {
            fst_hit_ref.st    = HIR_NRES;
            fst_hit_ref.page  = std::nullopt;
            fst_hit_ref.itQ   = std::nullopt;
            HIR_res_count--;
            res_count--;
            StackQ.pop_back();
            prune_StackS();
        } else {
            hash_.erase(fst_hir_id);
            HIR_res_count--;
            res_count--;
            StackQ.pop_back();
        }

        StackQ.push_front(p_id);
        hash_[p_id].itQ = StackQ.begin();
        if (is_new_resident) {
            HIR_res_count++;
            res_count++;
        } else {
            HIR_res_count++;
        }
    }

    void add_new(T page) {
        StackS.push_front(page.id);
    
        if (LIR_count < LIR_cap) {
            hash_[page.id] = HashEntry_t(StackS.begin(), 
                                         std::nullopt, LIR, page);
            LIR_count++;
            res_count++;
        } else {
            hash_[page.id] = HashEntry_t(StackS.begin(), 
                                         std::nullopt, HIR_RES, page);
            add_to_StackQ(page.id);  
        }
    
        prune_StackS();
    }
 

    void proc_LIR(ListIt elem) {
        StackS.splice(StackS.begin(), StackS, elem);
        prune_StackS();
        return;
    }

    void proc_HIR_RES(HashEntry_t& elem) {
        bool is_in_StackS = elem.itS.has_value();
 
        if (is_in_StackS) {
            StackS.splice(StackS.begin(), StackS, elem.itS.value());
     
            elem.st = LIR;
            LIR_count++;
            HIR_res_count--;
            if (elem.itQ) {
                StackQ.erase(elem.itQ.value());
                elem.itQ = std::nullopt;
            }
     
             prune_StackS();
        } else {
            KeyT key = *elem.itQ.value();
            
            StackQ.splice(StackQ.begin(), StackQ, elem.itQ.value());
            
            StackS.push_front(key);
            elem.itS = StackS.begin();
     
            prune_StackS();
        }
    }

    void proc_HIR_NRES(HashEntry_t& elem, T page) {
        assert(elem.page == std::nullopt);
        
        elem.page = page;
        
        if (elem.itS.has_value()) {
            StackS.splice(StackS.begin(), StackS, elem.itS.value());
        } else {
            StackS.push_front(page.id);
            elem.itS = StackS.begin();
        }
    
        if (LIR_count < LIR_cap) {
            elem.st = LIR;
            LIR_count++;
            res_count++;
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
                  << " StackS.size=" << StackS.size()
                  << " StackQ.size=" << StackQ.size()
                  << "\n";
    
        std::cout << "StackS (front -> back):\n";
        size_t idx = 0;
        for (auto key = StackS.begin(); key != StackS.end(); ++key, ++idx) {
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
        for (auto it = StackQ.begin(); it != StackQ.end(); ++it, ++idx) {
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
            std::cout << " page=" << (kv.second.page ? "yes" : "no");
            std::cout << "\n";
        }

        std::cout << std::flush;
    }
};
} //LIRS
