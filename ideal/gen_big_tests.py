import random
import os
from ref_id_cache import IdealCache

def generate_test(test_id, out_dir="big_tests"):
    os.makedirs(out_dir, exist_ok=True)

    size = random.randint(100000, 150000)
    n_requests = random.randint(400000, 600000)
    requests = [random.randint(1, 100000000) for _ in range(n_requests)]

    cache = IdealCache(size, requests)

    def slow_get_page(key):
        return key

    hits = cache.run_cache(slow_get_page)

    dat_path = os.path.join(out_dir, f"{test_id:03d}.dat")
    with open(dat_path, "w") as f:
        f.write(f"{size}\n")
        f.write(f"{n_requests}\n")
        f.write(" ".join(map(str, requests)) + "\n")

    sol_path = os.path.join(out_dir, f"{test_id:03d}.sol")
    with open(sol_path, "w") as f:
        f.write(str(hits) + "\n")

    print(f"Test {test_id} saved: {dat_path}, {sol_path} "
          f"(size={size}, n_requests={n_requests}, hits={hits})")


if __name__ == "__main__":
    random.seed(42)

    generate_test(1)
    generate_test(2)
    generate_test(3)
    generate_test(4)
    generate_test(5)
    generate_test(6)
    generate_test(7)
    generate_test(8)
    generate_test(9)
    generate_test(10)
