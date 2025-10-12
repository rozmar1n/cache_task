#!/usr/bin/env python3
"""Построение графиков для сравнения результатов LIRS и идеального кэша."""

import argparse
import csv
from pathlib import Path

import matplotlib.pyplot as plt


def load_results(directory: Path):
    data = {}
    if not directory.exists():
        raise FileNotFoundError(f"Директория {directory} не найдена.")

    for file_path in sorted(directory.glob("*.csv")):
        with file_path.open("r", encoding="utf-8") as file_obj:
            reader = csv.DictReader(file_obj)
            row = next(reader, None)
            if row is None:
                continue
            dataset = row["dataset"]
            total_requests = int(row["total_requests"])
            hits = int(row["hits"])
            hit_ratio = float(row["hit_ratio"])
            data[dataset] = {
                "total_requests": total_requests,
                "hits": hits,
                "hit_ratio": hit_ratio,
            }
    return data


def main():
    parser = argparse.ArgumentParser(
        description="Построить графики по результатам экспериментов LIRS vs Ideal."
    )
    parser.add_argument(
        "--lirs-dir",
        type=Path,
        default=Path("experiments") / "lirs_vs_ideal" / "results_lirs",
        help="Директория с результатами LIRS (по умолчанию experiments/lirs_vs_ideal/results_lirs).",
    )
    parser.add_argument(
        "--ideal-dir",
        type=Path,
        default=Path("experiments") / "lirs_vs_ideal" / "results_ideal",
        help="Директория с результатами Ideal (по умолчанию experiments/lirs_vs_ideal/results_ideal).",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("experiments") / "lirs_vs_ideal" / "lirs_vs_ideal_comparison.png",
        help="Файл для сохранения графика (по умолчанию experiments/lirs_vs_ideal/lirs_vs_ideal_comparison.png).",
    )
    parser.add_argument(
        "--show",
        action="store_true",
        help="Показать график на экране (требует доступного GUI).",
    )

    args = parser.parse_args()

    lirs_data = load_results(args.lirs_dir)
    ideal_data = load_results(args.ideal_dir)

    datasets = sorted(set(lirs_data.keys()) & set(ideal_data.keys()))
    if not datasets:
        raise RuntimeError("Нет совпадающих наборов данных между LIRS и Ideal.")

    hit_ratios_lirs = [lirs_data[name]["hit_ratio"] for name in datasets]
    hit_ratios_ideal = [ideal_data[name]["hit_ratio"] for name in datasets]
    diff = [ideal - lirs for ideal, lirs in zip(hit_ratios_ideal, hit_ratios_lirs)]

    fig, axes = plt.subplots(2, 1, figsize=(10, 10), sharex=True)

    ax_top = axes[0]
    indices = range(len(datasets))
    width = 0.35

    ax_top.bar([i - width / 2 for i in indices], hit_ratios_lirs, width=width, label="LIRS")
    ax_top.bar([i + width / 2 for i in indices], hit_ratios_ideal, width=width, label="Ideal")
    ax_top.set_ylabel("Доля попаданий (hit ratio)")
    ax_top.set_title("Сравнение hit ratio: LIRS против идеального кэша")
    ax_top.grid(axis="y", linestyle="--", alpha=0.4)
    ax_top.legend()

    ax_bottom = axes[1]
    ax_bottom.bar(indices, diff, color="tab:red")
    ax_bottom.axhline(0.0, color="black", linewidth=0.8)
    ax_bottom.set_ylabel("Разница hit ratio (Ideal - LIRS)")
    ax_bottom.set_xlabel("Наборы данных")
    ax_bottom.grid(axis="y", linestyle="--", alpha=0.4)
    ax_bottom.set_xticks(list(indices))
    ax_bottom.set_xticklabels(datasets, rotation=45, ha="right")

    fig.tight_layout()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(args.output, dpi=200)
    print(f"График сохранён в {args.output}")

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
