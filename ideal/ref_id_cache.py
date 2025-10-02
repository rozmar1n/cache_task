class IdealCache:
    def __init__(self, size, requests):
        self.size = size
        self.requests = requests
        self.cache = {}  
        self.nums_in_req = {}  
        self.fill_request_positions()

    def fill_request_positions(self):
        for i, req in enumerate(self.requests):
            if req not in self.nums_in_req:
                self.nums_in_req[req] = []
            self.nums_in_req[req].append(i)

    def number_in_future(self, start, elem):
        if elem not in self.nums_in_req:
            return -1
        positions = self.nums_in_req[elem]
        for pos in positions:
            if pos >= start:
                return pos
        return -1

    def is_in_future(self, start, elem):
        return self.number_in_future(start, elem) != -1

    def least_important_el(self, request_num):
        current_key = self.requests[request_num]
        ret = self.number_in_future(request_num + 1, current_key)
        if ret == -1:
            return current_key

        for key in self.cache:
            next_req = self.cache[key]['next_req']
            if next_req == -1:
                return key
            if ret < next_req:
                ret = next_req
        
        return self.requests[ret]

    def lookup_update(self, request_num, slow_get_page):
        key = self.requests[request_num]
        
        if key not in self.cache:
            new_page = slow_get_page(key)
            
            if not self.is_in_future(request_num + 1, key):
                return False
                
            if len(self.cache) >= self.size and self.size > 0:
                for_removal = self.least_important_el(request_num)
                if key == for_removal:
                    return False
                if for_removal in self.cache:
                    del self.cache[for_removal]
            
            self.cache[key] = {
                'page': new_page,
                'next_req': self.number_in_future(request_num + 1, key)
            }
            return False
        return True

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



