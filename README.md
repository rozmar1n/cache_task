# Cache Task

This repository contains my solution for the first task in the MIPT ILab C++ course by Konstantin Vladimirov.
## Technical Stack

- C++ (94.8%)
- CMake (5.2%)

## Prerequisites

To build and run this project, you need:
- C++ compiler with C++11 support or higher
- CMake (3.x or higher)

## Building the Project

To build the project:

```bash
cmake -S . -B build
cmake --build buld
```

## Running Tests

After building the project, you can run the tests using:

LIRS cache:

```bash
build/LIRS/LIRS_tests
```

LRU cache:

```bash
build/LRU/LRU_tests
```

Ideal cache:

```bash
build/ideal/ideal_tests

#for big tests
build/ideal/ideal_big_tests
```

## Project Structure

```
.
├── CMakeLists.txt
├── README.md
├── /LIRS
│   └── [source and tests]
├── /LRU
│   └── [source and tests]
├── /ideal
    └── [source and tests]
```


## To run CLI versions of program

LIRS cache:

```bash
build/LIRS/LIRS.out
```

LRU cache:

```bash
build/LRU/LRU.out
```

ideal cache:

```bash
build/ideal/ideal.out
```

