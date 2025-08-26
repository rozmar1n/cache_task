#include <vector>

namespace LRU {
struct page_t {
    int id;
};
page_t slow_get_page(int key);

struct cache_test_context {
    size_t cz;    
    std::vector<int> data;

public:
    cache_test_context();
    int run(); 
    void take_data();
};
} /*namespace LRU*/
