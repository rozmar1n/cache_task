#include "ideal.hpp"
#include <iostream>

struct page_t {
    int id;
};

page_t sgp(int id) {
    return page_t{id};
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
    ideal::cache_t<page_t> ch(sz, req);
    std::cout << ch.run_cache(sgp) << std::endl;
    return 0;
}
