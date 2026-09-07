# CPU vs GPU String Comparator

This repository compares multiple CPU and GPU implementations of a string search algorithm designed to count the occurrences of a target word within large text files (up to 8 GB). The project was developed to evaluate parallelization strategies and their impact on throughput and performance.

## Implementations

### CPU Versions
1.  **Naive Search:** A baseline sequential approach.
2.  **KMP (Knuth-Morris-Pratt):** An optimized pattern matching algorithm.
3.  **KMP with Load Balancing:** Divides the file into fixed-size chunks (e.g., 5 MB) dynamically assigned to threads, preventing load imbalance.
4.  **KMP with Chunks and `-O3`:** The fully optimized CPU version reaching peak performance.

*Hotspot Analysis:* Initial profiling revealed that `strlen()` was consuming nearly 40% of the execution time. Pre-computing the length drastically improved the performance. The CPU version achieved a throughput of up to ~7 GB/s.

### GPU Versions (CUDA)
The GPU implementation went through several iterations to maximize memory bandwidth utilization and minimize stalls on an NVIDIA GeForce RTX 3060 Mobile:
1.  **Stride Code (Baseline GPU):** Uses shared memory to fetch 96-byte blocks with an 11-byte overlap. However, hardware granularity (32-byte memory transactions) caused severe fetch overhead (~1.33 GB of redundant reads for a 4 GB file) and numerous barrier stalls.
2.  **Stride Tail:** Addresses memory bus under-utilization by utilizing 16-byte aligned vectorial transfers and processing much larger chunks (5-10 KB) per block. This reduced the fetch overhead to ~20 MB and crushed the stall barrier.
3.  **Stride Fast (Final GPU):** By maximizing block occupancy and minimizing tail effects (partial wave execution), the final GPU version achieved a massive throughput of over **170 GB/s**.

## Project Goals
*   Ensure correctness (counting the exact number of occurrences).
*   Achieve a minimum throughput of 5 GB/s for files larger than 4 GB.
*   Demonstrate that the GPU implementation significantly outperforms the CPU version.

## Detailed Analysis
For a deep dive into the algorithm mechanics, performance bottlenecks (e.g., *Stall Long Scoreboard*, *Tail Effect*), hardware constraints, and speedup graphs, please refer to the following documents:
*   `StringComparator_CPU.pdf`
*   `StringComparator_GPU.pdf`

## Authors

This project was made for the Computer Architecture exam, with Megan Maremmani ( [@ItsMegan605](https://github.com/ItsMegan605) ) and Eleonora Sgorbini ( [@ele239](https://github.com/ele239) ). 
Grade: 30
