#include <iostream>
#include "LRU_context.hpp"

int main(int argc, char **argv) {

    LRU::cache_test_context ctx;
    if(!(ctx.take_data())) return 1; 
    std::cout << ctx.run() << std::endl;

    return 0;
}

