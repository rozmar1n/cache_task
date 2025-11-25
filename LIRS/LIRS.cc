#include "LIRS.hpp"
struct page_t {
    int id;
};

page_t slow_get_page(int id) {
    page_t retpg;
    retpg.id = id;
    return retpg;
}

int main() {
    size_t ch_size = 0;

    if(!(std::cin >> ch_size)) {
        std::cout << "invalid input" << std::endl;
        return 1;
    }
    if (ch_size <= 1) {
        std::cout << "cache size must be at least 2" << std::endl;
        return 1;
    }

    LIRS::cache_t<page_t> ch{ch_size};
    
    int N_pushes = 0;
    
    if(!(std::cin >> N_pushes)) {
        std::cout << "invalid input" << std::endl;
        return 1;
    }
    if (N_pushes <= 0) {
        std::cout << "number of pushes must be at least 1" << std::endl;
        return 1;
    }

    int counter = 0;
    int hit = 0;
    for(int i = 0; i < N_pushes; i++) {
        std::cin >> hit;
        if(ch.lookup_update(hit, slow_get_page)) {
            counter++;
        }
    }
    std::cout <<counter << std::endl;

    return 0;
}
