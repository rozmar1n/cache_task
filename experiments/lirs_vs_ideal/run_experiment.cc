#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "LIRS.hpp"
#include "ideal.hpp"

namespace fs = std::filesystem;

struct page_t {
    int id;
};

page_t slow_get_page(int id) {
    return page_t{id};
}

struct dataset_t {
    std::string name;
    size_t cache_size{};
    std::vector<int> requests;
};

dataset_t load_dataset(const fs::path &path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Не удалось открыть файл: " + path.string());
    }

    dataset_t data;
    data.name = path.stem().string();

    size_t requests_count = 0;
    input >> data.cache_size >> requests_count;

    if (!input) {
        throw std::runtime_error("Некорректный заголовок в файле: " + path.string());
    }

    data.requests.resize(requests_count);
    for (size_t i = 0; i < requests_count; ++i) {
        if (!(input >> data.requests[i])) {
            throw std::runtime_error("Недостаточно данных в файле: " + path.string());
        }
    }

    return data;
}

int run_lirs(size_t cache_size, const std::vector<int> &requests) {
    LIRS::cache_t<page_t> cache(cache_size);
    int hits = 0;

    for (int key : requests) {
        if (cache.lookup_update(key, slow_get_page)) {
            ++hits;
        }
    }

    return hits;
}

int run_ideal(size_t cache_size, const std::vector<int> &requests) {
    ideal::cache_t<page_t> cache(cache_size, requests);
    return cache.run_cache(slow_get_page);
}

void ensure_directory(const fs::path &dir) {
    std::error_code ec;
    if (!fs::exists(dir) && !fs::create_directories(dir, ec) && ec) {
        throw std::runtime_error("Не удалось создать директорию: " + dir.string() + " (" + ec.message() + ")");
    }
}

void write_result(const fs::path &out_dir,
                  const std::string &dataset_name,
                  size_t cache_size,
                  size_t total_requests,
                  int hits,
                  double hit_ratio) {
    ensure_directory(out_dir);

    fs::path out_file = out_dir / (dataset_name + ".csv");
    std::ofstream output(out_file);
    if (!output) {
        throw std::runtime_error("Не удалось открыть файл для записи: " + out_file.string());
    }

    output << "dataset,cache_size,total_requests,hits,hit_ratio\n";
    output << dataset_name << ','
           << cache_size << ','
           << total_requests << ','
           << hits << ','
           << std::fixed << std::setprecision(6) << hit_ratio << '\n';
}

int main(int argc, char **argv) {
    fs::path base_dir = fs::path("experiments") / "lirs_vs_ideal";
    fs::path datasets_dir = base_dir / "datasets";
    fs::path lirs_results_dir = base_dir / "results_lirs";
    fs::path ideal_results_dir = base_dir / "results_ideal";

    if (argc >= 2) {
        datasets_dir = fs::path(argv[1]);
    }
    if (argc >= 3) {
        lirs_results_dir = fs::path(argv[2]);
    }
    if (argc >= 4) {
        ideal_results_dir = fs::path(argv[3]);
    }

    ensure_directory(datasets_dir);
    ensure_directory(lirs_results_dir);
    ensure_directory(ideal_results_dir);

    std::vector<fs::path> dataset_files;
    for (const auto &entry : fs::directory_iterator(datasets_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dat") {
            dataset_files.push_back(entry.path());
        }
    }

    std::sort(dataset_files.begin(), dataset_files.end());

    if (dataset_files.empty()) {
        std::cerr << "В директории " << datasets_dir
                  << " не найдено файлов с расширением .dat\n";
        return 1;
    }

    std::cout << "Найдено " << dataset_files.size()
              << " наборов данных. Выполняю сравнение...\n";

    for (const auto &path : dataset_files) {
        dataset_t dataset = load_dataset(path);
        const size_t total_requests = dataset.requests.size();

        int lirs_hits = run_lirs(dataset.cache_size, dataset.requests);
        int ideal_hits = run_ideal(dataset.cache_size, dataset.requests);

        double lirs_ratio = total_requests == 0
                                ? 0.0
                                : static_cast<double>(lirs_hits) / total_requests;
        double ideal_ratio = total_requests == 0
                                 ? 0.0
                                 : static_cast<double>(ideal_hits) / total_requests;

        write_result(lirs_results_dir,
                     dataset.name,
                     dataset.cache_size,
                     total_requests,
                     lirs_hits,
                     lirs_ratio);

        write_result(ideal_results_dir,
                     dataset.name,
                     dataset.cache_size,
                     total_requests,
                     ideal_hits,
                     ideal_ratio);

        std::cout << "Набор " << dataset.name
                  << ": LIRS hits=" << lirs_hits << " ("
                  << std::fixed << std::setprecision(3) << lirs_ratio * 100.0
                  << "%), Ideal hits=" << ideal_hits << " ("
                  << std::fixed << std::setprecision(3) << ideal_ratio * 100.0
                  << "%)\n";
    }

    std::cout << "Результаты сохранены в:\n  LIRS -> " << lirs_results_dir
              << "\n  Ideal -> " << ideal_results_dir << '\n';

    return 0;
}
