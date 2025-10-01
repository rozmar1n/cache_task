#include "ideal.hpp"
#include <iostream>

struct page_t {
    int id;
};

page_t sgp(int id) {
    return page_t{id};
}

int run_cache(size_t sz, std::vector<int> pages) {
    ideal::cache_t<page_t> ch{sz, pages};
    int counter = 0;
    for(int i = 0; i < pages.size(); i++) {
        if(ch.lookup_update(i, sgp)) {
            counter++;
        }
    }
    return counter;
} 

int main() {
    size_t sz = 0;
    int n_hits = 0;
    std::cin >> sz >> n_hits;
    std::vector<int> req;
    for(int i = 0; i < n_hits; i++) {
        int hit;
        std::cin >> hit;
        req.push_back(hit);
    }
    std::cout << run_cache(sz, req) << std::endl;
    return 0;
}
