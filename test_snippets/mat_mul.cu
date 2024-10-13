#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

// CUDA kernel for matrix multiplication
__global__ void matmul(const int N, const float* A, const float* B, float* C) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;  // Calculate row index
    int col = blockIdx.x * blockDim.x + threadIdx.x;  // Calculate column index

    if (row < N && col < N) {
        float sum = 0.0f;
        for (int k = 0; k < N; ++k) {
            sum += A[row * N + k] * B[k * N + col];
        }
        C[row * N + col] = sum;
    }
}

// CPU implementation for matrix multiplication
void matmul_cpu(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C, int N) {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                sum += A[row * N + k] * B[k * N + col];
            }
            C[row * N + col] = sum;
        }
    }
}

int main(int argc, char* argv[]) {
    const int N = (argc > 1) ? atoi(argv[1]) : 1024;  // Set default size if not provided
    std::vector<float> A(N * N); // Initialize matrix A
    std::vector<float> B(N * N); // Initialize matrix B
    std::vector<float> C(N * N, 0.0f); // Matrix to store the result

    // Initialize matrices
    for (int i = 0; i < N * N; ++i) {
        A[i] = rand() % 100;
        B[i] = rand() % 200;
    }

    // CPU Timing
//    auto start_cpu = std::chrono::high_resolution_clock::now();
//    matmul_cpu(A, B, C, N);
//    auto end_cpu = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double> cpu_duration = end_cpu - start_cpu;

//    std::cout << "CPU matrix multiplication took: " << cpu_duration.count() << " seconds" << std::endl;
//    std::cout << "FIRST ELEMENT: " << C[0] << std::endl;
//    std::cout << "MIDDLE ELEMENT: " << C[N*N/2] << std::endl;
//    std::cout << "LAST ELEMENT: " << C[N*N-1] << std::endl;

    // Device pointers
    float *d_A, *d_B, *d_C;

    // Allocate device memory
    cudaMalloc((void**)&d_A, sizeof(float) * N * N);
    cudaMalloc((void**)&d_B, sizeof(float) * N * N);
    cudaMalloc((void**)&d_C, sizeof(float) * N * N);

    // Copy data to device
    cudaMemcpy(d_A, A.data(), sizeof(float) * N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B.data(), sizeof(float) * N * N, cudaMemcpyHostToDevice);

    // Define block and grid sizes
    dim3 blockSize(16, 16); // 16x16 threads per block
    dim3 gridSize((N + blockSize.x - 1) / blockSize.x, (N + blockSize.y - 1) / blockSize.y); // Grid size

    // GPU Timing
    auto start_gpu = std::chrono::high_resolution_clock::now();
    // Launch the kernel
    matmul<<<gridSize, blockSize>>>(N, d_A, d_B, d_C);
    // Wait for the GPU to finish
    cudaDeviceSynchronize();
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> gpu_duration = end_gpu - start_gpu;

    // Copy the result back to host
    cudaMemcpy(C.data(), d_C, sizeof(float) * N * N, cudaMemcpyDeviceToHost);

    std::cout << "GPU matrix multiplication took: " << gpu_duration.count() << " seconds" << std::endl;
    std::cout << "FIRST ELEMENT: " << C[0] << std::endl;
    std::cout << "MIDDLE ELEMENT: " << C[N*N/2] << std::endl;
    std::cout << "LAST ELEMENT: " << C[N*N-1] << std::endl;

    // Free device memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}

