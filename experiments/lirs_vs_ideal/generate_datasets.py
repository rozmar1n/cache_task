#!/usr/bin/env python3
"""Генерация случайных наборов данных для сравнения LIRS и идеального кэша."""

import argparse
import random
from pathlib import Path


def write_requests(file_obj, requests, max_per_line=32):
    for start in range(0, len(requests), max_per_line):
        chunk = requests[start : start + max_per_line]
        file_obj.write(" ".join(map(str, chunk)) + "\n")


def generate_dataset(cache_min, cache_max, requests_min, requests_max, key_min, key_max):
    cache_size = random.randint(cache_min, cache_max)
    total_requests = random.randint(requests_min, requests_max)
    requests = [random.randint(key_min, key_max) for _ in range(total_requests)]
    return cache_size, requests


def main():
    parser = argparse.ArgumentParser(
        description="Сгенерировать наборы данных для сравнения LIRS и идеального кэша."
    )
    parser.add_argument(
        "-n",
        "--num-datasets",
        type=int,
        default=5,
        help="Количество создаваемых наборов данных (по умолчанию 5).",
    )
    parser.add_argument(
        "--cache-min",
        type=int,
        default=16,
        help="Минимальный размер кэша (по умолчанию 16).",
    )
    parser.add_argument(
        "--cache-max",
        type=int,
        default=256,
        help="Максимальный размер кэша (по умолчанию 256).",
    )
    parser.add_argument(
        "--requests-min",
        type=int,
        default=200,
        help="Минимальное количество запросов (по умолчанию 200).",
    )
    parser.add_argument(
        "--requests-max",
        type=int,
        default=2000,
        help="Максимальное количество запросов (по умолчанию 2000).",
    )
    parser.add_argument(
        "--key-min",
        type=int,
        default=1,
        help="Минимальный идентификатор страницы (по умолчанию 1).",
    )
    parser.add_argument(
        "--key-max",
        type=int,
        default=500,
        help="Максимальный идентификатор страницы (по умолчанию 500).",
    )
    parser.add_argument(
        "-s",
        "--seed",
        type=int,
        default=None,
        help="Фиксированное значение seed для повторяемости результатов.",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("experiments") / "lirs_vs_ideal" / "datasets",
        help="Директория для сохранения файлов .dat (по умолчанию experiments/lirs_vs_ideal/datasets).",
    )

    args = parser.parse_args()

    if args.seed is not None:
        random.seed(args.seed)

    output_dir: Path = args.output
    output_dir.mkdir(parents=True, exist_ok=True)

    for idx in range(1, args.num_datasets + 1):
        cache_size, requests = generate_dataset(
            args.cache_min,
            args.cache_max,
            args.requests_min,
            args.requests_max,
            args.key_min,
            args.key_max,
        )

        file_path = output_dir / f"dataset_{idx:03d}.dat"
        with file_path.open("w", encoding="utf-8") as file_obj:
            file_obj.write(f"{cache_size}\n")
            file_obj.write(f"{len(requests)}\n")
            write_requests(file_obj, requests)

        print(
            f"Создан {file_path.name}: cache_size={cache_size}, requests={len(requests)}, key_range=[{args.key_min}, {args.key_max}]"
        )


if __name__ == "__main__":
    main()
