#include <iostream>
#include "LRU_context.hpp"

int main(int argc, char **argv) {

    cache_test_context ctx;
    ctx.take_data();
    std::cout << ctx.run() << std::endl;

    return 0;
}
