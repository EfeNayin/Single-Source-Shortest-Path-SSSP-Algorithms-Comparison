# SSSP Algorithm Comparison

Comparison of Dijkstra's and Bellman-Ford algorithms on real-world road network datasets from the 9th DIMACS Implementation Challenge.

**Course:** CENG383 - Algorithms  
**Student:** Efe Nayın (202211049)  
**Date:** April 2026

---

## Overview

This project benchmarks two classic single-source shortest path (SSSP) algorithms:
- **Dijkstra's Algorithm**: O((V+E) log V) using min-heap
- **Bellman-Ford Algorithm**: O(V*E) brute-force relaxation

**Key Finding:** Dijkstra is **24,321x faster** on NY dataset and **29,371x faster** on COL dataset.

---

## Datasets

From [9th DIMACS Challenge - Shortest Paths](http://www.disp.uniroma2.it/challenge9/download.shtml):

| Dataset | Nodes | Edges | Description |
|---------|-------|-------|-------------|
| NY | 264,346 | 733,846 | New York City road network |
| COL | 435,666 | 1,057,066 | Colorado road network |

Place `.gr` files in `Distance Graph/` folder.

---

## Requirements

**C++ Compilation:**
- g++ with C++17 support
- Windows: MSYS2/MinGW with psapi.h for memory tracking
- Compile flags: `-O3 -std=c++17 -lpsapi`

**Python Visualization:**
```bash
pip install pandas matplotlib numpy
```

---

## How to Run

### 1. Compile the benchmark

```bash
g++ -O3 -std=c++17 SSSP_benchmark.cpp -o SSSP_benchmark.exe -lpsapi
```

### 2. Run the benchmark

```bash
./SSSP_benchmark.exe
```

**Note:** Bellman-Ford is very slow! On these datasets:
- NY: ~641 seconds (~10.7 minutes) for 1 source
- COL: ~1,571 seconds (~26 minutes) for 1 source

Total runtime: ~1 hour

### 3. Generate graphs

```bash
python Create_Graphs.py
```

Creates 4 PNG files:
- `runtime_comparison.png` (log scale)
- `scalability_comparison.png` 
- `memory_comparison.png`
- `speedup_comparison.png`

---

## Results

### Runtime Performance

| Dataset | Dijkstra (avg, 10 sources) | Bellman-Ford (1 source) | Speedup |
|---------|---------------------------|------------------------|---------|
| NY | 26.376 ms | 641,493.941 ms | 24,321x |
| COL | 53.509 ms | 1,571,605.388 ms | 29,371x |

### Why is Bellman-Ford so slow?

For NY dataset with 264,346 nodes and 733,846 edges:
- Bellman-Ford performs: (V-1) × E = 264,345 × 733,846 ≈ **193 billion operations**
- At ~300M ops/sec → ~643 seconds = 10.7 minutes ✓

Dijkstra with priority queue: ~10 million operations → 26ms ✓

---

## Testing Methodology

**Source Nodes:**
- **Dijkstra:** Tested from 10 different source nodes, averaged
- **Bellman-Ford:** Tested from 1 source node only (due to extreme runtime)

This is still a fair comparison because both algorithms are deterministic - running Bellman-Ford 10 times would just give 10 similar numbers.

---

## Hardware

- **CPU:** 13th Gen Intel Core i7-13700H
- **RAM:** 16 GB
- **OS:** Windows 11
- **Compiler:** g++ 14.2.0 (MSYS2 MinGW)

---

## File Structure

```
SSSP_Project/
├── SSSP_benchmark.cpp           # Main benchmark code
├── Create_Graphs.py             # Visualization script
├── benchmark_results.csv        # Output results
├── runtime_comparison.png       # Graph 1
├── scalability_comparison.png   # Graph 2
├── memory_comparison.png        # Graph 3
├── speedup_comparison.png       # Graph 4
└── Distance Graph/              # Place datasets here
    ├── USA-road-d.NY.gr
    └── USA-road-d.COL.gr
```

---

## References

- [9th DIMACS Implementation Challenge](http://www.disp.uniroma2.it/challenge9/)
- Dijkstra, E. W. (1959). A note on two problems in connexion with graphs.
- Bellman, R. (1958). On a routing problem.
- Duan, R., et al. (2025). Breaking the Sorting Barrier for Directed Single-Source Shortest Paths. STOC 2025.

---

## License

This project is for educational purposes (CENG383 course assignment).