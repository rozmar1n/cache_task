from collections import deque


class IdealCache:
    def __init__(self, size, requests):
        self.size = size
        self.requests = requests
        self.cache = {}
        self.positions = {}
        for idx, key in enumerate(self.requests):
            self.positions.setdefault(key, deque()).append(idx)

    def next_use(self, key):
        positions = self.positions.get(key)
        if not positions:
            return -1
        return positions[0]

    def least_important_el(self, request_num):
        candidate = None
        farthest_use = -1

        for key in self.cache:
            next_idx = self.next_use(key)
            if next_idx == -1:
                return key
            if next_idx > farthest_use:
                farthest_use = next_idx
                candidate = key

        return candidate

    def lookup_update(self, request_num, slow_get_page):
        key = self.requests[request_num]
        positions = self.positions.get(key)
        if positions and positions[0] == request_num:
            positions.popleft()

        if key in self.cache:
            return True

        new_page = slow_get_page(key)

        if self.next_use(key) == -1:
            return False

        if len(self.cache) >= self.size and self.size > 0:
            for_removal = self.least_important_el(request_num)
            if key == for_removal:
                return False
            if for_removal in self.cache:
                del self.cache[for_removal]

        self.cache[key] = new_page
        return False

    def run_cache(self, slow_get_page):
        hits = 0
        for i in range(len(self.requests)):
            if self.lookup_update(i, slow_get_page):
                hits += 1
        return hits

def test_cache():
    size = int(input().strip())
    
    n_hits = int(input().strip())
    
    requests = list(map(int, input().strip().split()))
    
    if len(requests) != n_hits:
        raise ValueError(f"Expected {n_hits} requests, but got {len(requests)}")
    
    cache = IdealCache(size, requests)
    
    def slow_get_page(key):
        return key  
    
    hits = cache.run_cache(slow_get_page)
    print(hits)

if __name__ == "__main__":
    try:
        test_cache()
    except ValueError as e:
        print(f"Error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")
