import random
from typing import List, Tuple
import os
from ref_id_cache import IdealCache

def generate_test_case(min_size: int = 1, 
                      max_size: int = 10, 
                      min_requests: int = 5, 
                      max_requests: int = 20, 
                      min_page: int = 1, 
                      max_page: int = 100) -> Tuple[int, List[int]]:
    """
    Генерирует случайный тестовый случай для кэша
    
    Returns:
        Tuple[int, List[int]]: (размер_кэша, список_запросов)
    """
    cache_size = random.randint(min_size, max_size)
    n_requests = random.randint(min_requests, max_requests)
    requests = [random.randint(min_page, max_page) for _ in range(n_requests)]
    
    return cache_size, requests

def generate_cpp_test(test_number: int, cache_size: int, requests: List[int], expected_hits: int) -> str:
    """Генерирует C++ тест в формате Google Test"""
    test_name = f"test_{test_number}"
    
    test_code = f"""
TEST(idealTests, {test_name}) {{
    size_t ch_size = {cache_size};
    std::vector<int> pages = {{{', '.join(map(str, requests))}}};
    int expected = {expected_hits};

    ideal::cache_t<page_t> ch{{ch_size, pages}};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}}
"""
    return test_code

def run_python_cache(cache_size: int, requests: List[int]) -> int:
    """Запускает Python-версию кэша и возвращает количество попаданий"""
    cache = IdealCache(cache_size, requests)
    def slow_get_page(key):
        return key
    return cache.run_cache(slow_get_page)

def generate_test_suite(n_tests: int = 10) -> None:
    """Генерирует набор тестов и сохраняет их в файл"""
    
    cpp_header = """
#include <gtest/gtest.h>
#include "ideal.hpp"
#include <vector>

struct page_t {
    int id;
};

page_t slow_get_page(int id) {
    page_t retpg;
    retpg.id = id;
    return retpg;
}

"""
    
    tests = []
    print(f"Generating {n_tests} test cases...")
    
    for i in range(n_tests):
        cache_size, requests = generate_test_case()
        
        expected_hits = run_python_cache(cache_size, requests)
        
        cpp_test = generate_cpp_test(i + 1, cache_size, requests, expected_hits)
        tests.append(cpp_test)
        
        print(f"Test {i + 1}:")
        print(f"Cache size: {cache_size}")
        print(f"Requests: {requests}")
        print(f"Expected hits: {expected_hits}")
        print("-" * 50)

    with open("ideal_tests.cc", "w") as f:
        f.write(cpp_header)
        for test in tests:
            f.write(test)

if __name__ == "__main__":
    random.seed(42)
    
    generate_test_suite(10)  # Генерируем 10 тестов
    print("Tests have been generated and saved to 'cache_tests.cpp'")
