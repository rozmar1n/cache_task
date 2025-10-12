# Cache Task

Реализация трёх алгоритмов управления кэшем (LRU, LIRS и «идеальный») в рамках первого задания курса MIPT ILab по C++. Каждый алгоритм снабжён модульными тестами и простой CLI-утилитой для ручных экспериментов и прогонки наборов запросов.

## Реализованные алгоритмы

- **LRU (Least Recently Used)** — классический стековый алгоритм, хранящий в кэше последние использованные страницы.
- **LIRS (Low Inter-reference Recency Set)** — более продвинутая стратегия, разделяющая страницы на LIR/HIR и обеспечивающая лучшую адаптацию к паттернам доступа.
- **Ideal cache** — алгоритм для эталонных значений и генерации ответов на больших тестах.

## Требования

- Компилятор с поддержкой C++17 (GCC, Clang или MSVC).
- CMake 3.15+.
- Доступ в интернет при первой сборке (скачивается GoogleTest).
- Python 3.8+ (только если нужно регенерировать тесты для идеального кэша).

## Сборка

```bash
cmake -S . -B build
cmake --build build
```

По умолчанию собираются бинарники тестов и CLI для каждого алгоритма.

## Тестирование

Запустить все тесты целиком:

```bash
cmake --build build --target test
```

или напрямую:

- `build/LRU/LRU_tests`
- `build/LIRS/LIRS_tests`
- `build/ideal/ideal_tests`
- `build/ideal/ideal_big_tests` — прогон уже сгенерированных big-тестов для идеального кэша.

## CLI и формат ввода

Каждая утилита читает со стандартного ввода размер кэша, количество запросов и последовательность ключей, после чего выводит число cache-hit'ов.

```text
<cache_size>
<requests_count>
<key_1> <key_2> ... <key_n>
```

Команды запуска:

- `build/LRU/prog`
- `build/LIRS/LIRS.out`
- `build/ideal/ideal.out`

Пример:

```bash
printf "2\n5\n1 2 1 3 1\n" | build/LRU/prog
```

## Структура проекта

```
.
├── CMakeLists.txt
├── LIRS/         # LIRS cache + тесты и CLI
├── LRU/          # LRU cache, общая обвязка и CLI
├── ideal/        # Алгоритм идеального кэша, big-tests и генераторы
├── experiments/  # Сравнение алгоритмов LIRS vs ideal
└── README.md
```

## Генерация тестов

В каталоге `ideal/` лежат скрипты для построения наборов тестов:

- `python ideal/gen_tests.py` — пересоздаёт unit-тесты для GoogleTest.
- `python ideal/gen_big_tests.py` — генерирует файлы `.dat` и `.sol` в `ideal/big_tests/` для нагрузочного тестирования.

Перед запуском убедитесь, что у вас установлен Python 3 и зависимости из стандартной библиотеки достаточно (дополнительные пакеты не требуются).

## Эксперимент LIRS vs Ideal

В каталоге `experiments/lirs_vs_ideal/` находятся все вспомогательные инструменты:

- `datasets/` — входные `.dat` с тестовыми паттернами.
- `results_lirs/`, `results_ideal/` — CSV-выгрузки с количеством попаданий и hit ratio для каждого алгоритма.
- `run_experiment.cc` → бинарник `lirs_vs_ideal_benchmark`, запускаемый через CMake.
- `generate_datasets.py` — генератор случайных сценариев.
- `plot_results.py` — построение графиков (по умолчанию `lirs_vs_ideal_comparison.png`).

Последовательность работы:

1. Сгенерировать данные (опционально):  
   `python experiments/lirs_vs_ideal/generate_datasets.py -n 10 --seed 42`
2. Собрать утилиту:  
   `cmake --build build --target lirs_vs_ideal_benchmark`
3. Прогнать сравнение:  
   `build/experiments/lirs_vs_ideal/lirs_vs_ideal_benchmark`
4. Построить график:  
   `python experiments/lirs_vs_ideal/plot_results.py`

Готовый график показывает два бар-чарта (hit ratio LIRS и ideal) и их разницу для каждого набора данных.
