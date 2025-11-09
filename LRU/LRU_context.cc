#include "LRU_context.hpp"
#include "LRU.hpp"
#include <iostream>

LRU::page_t LRU::slow_get_page(int key) {
    return page_t{key};
}

LRU::cache_test_context::cache_test_context()
    : cz(0), data() {}

bool LRU::cache_test_context::take_data() {
    if(!(std::cin >> cz)) {
        std::cout << "invalid input" << std::endl;
        return 0;
    }

    int n;
    std::cin >> n;
    data.reserve(n);
    for (int i = 0; i < n; ++i) {
        int key;
        std::cin >> key;
        data.push_back(key);
    }
    return 1;
}

int LRU::cache_test_context::run() {
    cache_t<page_t> c{cz};
    int counter = 0;

    for (int key : data)
        counter += c.lookup_update(key, slow_get_page);

    return counter;
}
