# Benchmarking OpenCL and CUDA on Different Devices

This repository contains benchmarking results comparing the performance of OpenCL on CPU vs GPU and OpenCL GPU vs CUDA GPU. The devices used for the benchmarks are:
- **CPU:** Intel(R) Core(TM) i5-7360U CPU @ 2.30GHz
- **GPU 1:** Intel(R) Iris(TM) Plus Graphics 640 (OpenCL)
- **GPU 2:** NVIDIA A100-SXM-64GB (CUDA)

> **Note**: This benchmarking was done purely for fun and out of curiosity about how different devices handle computation tasks. It is not meant to be an exhaustive or highly scientific comparison.

## Installation Instructions

### Installing `clinfo` on macOS

`clinfo` is a tool that provides detailed information about all available OpenCL platforms and devices. First install `clinfo` on macOS using **Homebrew**.

1. **Install clinfo**:
    ```bash
    brew install clinfo
    ```

2. **Verify installation**:
    ```bash
    clinfo
    ```
    You should see detailed information about your OpenCL platforms and devices.

### Installing `clinfo` on Linux

On most Linux distributions, `clinfo` is available through the default package manager.

1. **Ubuntu/Debian**:
    ```bash
    sudo apt update
    sudo apt install clinfo
    ```

2. **Verify installation**:
    ```bash
    clinfo
    ```
    This should display information about your OpenCL platforms and devices.

## Benchmark Plots

Below are the performance comparisons in matrix multiplication for different devices:

1. **OpenCL: CPU vs GPU**
   - CPU: Intel(R) Core(TM) i5-7360U @ 2.30GHz
   - GPU: Intel(R) Iris(TM) Plus Graphics 640

   ![Benchmark Plot: OpenCL CPU vs GPU](./plots/benchmark_plot_CPU_GPU.pdf)

2. **OpenCL GPU vs CUDA GPU**
   - GPU 1: Intel(R) Iris(TM) Plus Graphics 640 (OpenCL)
   - GPU 2: NVIDIA A100-SXM-64GB (CUDA)

   ![Benchmark Plot: OpenCL GPU vs CUDA GPU](./plots/benchmark_plot_GPU_GPU.pdf)


## The Credits 🎬
Big shoutout to **Z. Dahbi** for running these benchmarks and not getting distracted by instagram videos (well, maybe a little). All benchmarks were performed with a cup of tea (Moroccan Atay) in hand ☕ and curiosity ❤️.

**Disclaimer:** No GPUs were harmed in the making of this benchmark. However, some neurons might have been fried due to excessive debugging!

Done purely for the fun of it. If these results make you giggle or question reality, mission accomplished! 😄