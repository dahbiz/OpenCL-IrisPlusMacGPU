#include <CL/opencl.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

// OpenCL kernel for matrix multiplication
const char* kernelSource = R"(
__kernel void matmul(const int N, __global const float* A, __global const float* B, __global float* C) {
    int row = get_global_id(0);
    int col = get_global_id(1);

    float sum = 0.0f;
    for (int k = 0; k < N; ++k) {
        sum += A[row * N + k] * B[k * N + col];
    }
    C[row * N + col] = sum;
}
)";

// Error checking utility
void checkError(cl_int err, const char* name) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error: " << name << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
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

int main(int argc, char* argv[]) 
{
    const int N = atoi(argv[1]);
    std::vector<float> A(N * N); // Initialize matrix A
    std::vector<float> B(N * N); // Initialize matrix B
    std::vector<float> C(N * N, 0.0f); // Matrix to store the result

    // Init 
    for (int i = 0; i < N * N; ++i) 
    {
        A[i] = rand() % 100;
        B[i] = rand() % 200;
    }

    // // CPU Timing
    // auto start_cpu = std::chrono::high_resolution_clock::now();
    // matmul_cpu(A, B, C, N);
    // auto end_cpu = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> cpu_duration = end_cpu - start_cpu;

    // std::cout << "CPU matrix multiplication took: " << cpu_duration.count() << " seconds" << std::endl;
    // std::cout << "FIRST ELEMENT: " << C[0] << std::endl;
    // std::cout << "MIDDLE ELEMENT: " << C[N*N/2] << std::endl;
    // std::cout << "LAST ELEMENT: " << C[N*N-1] << std::endl;

    // OpenCL setup
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    cl_int err;

    // Get platform and device
    err = clGetPlatformIDs(1, &platform, nullptr);
    checkError(err, "clGetPlatformIDs");

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr); // Specifically request GPU
    checkError(err, "clGetDeviceIDs");

    // Create OpenCL context
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    checkError(err, "clCreateContext");

    // Create command queue
    queue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err, "clCreateCommandQueue");

    // Create buffers
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N * N, A.data(), &err);
    checkError(err, "clCreateBuffer A");

    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N * N, B.data(), &err);
    checkError(err, "clCreateBuffer B");

    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N * N, nullptr, &err);
    checkError(err, "clCreateBuffer C");

    // Create program
    program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    checkError(err, "clCreateProgramWithSource");

    // Build program
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, nullptr);
        std::cerr << "Build error: " << buildLog << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create kernel
    kernel = clCreateKernel(program, "matmul", &err);
    checkError(err, "clCreateKernel");

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(int), &N);
    checkError(err, "clSetKernelArg 0");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferA);
    checkError(err, "clSetKernelArg 1");
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferB);
    checkError(err, "clSetKernelArg 2");
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufferC);
    checkError(err, "clSetKernelArg 3");

    // Enqueue kernel execution
    size_t lX = 16;
    size_t lY = 16;
    size_t gX = ((N + lX - 1) / lX) * lX;
    size_t gY = ((N + lY - 1) / lY) * lY;

    size_t globalSize[2] = { gX, gY }; // Global size for 2D execution
    size_t localSize[2] = { lX, lY }; // Local size for 2D execution

    // Check for valid sizes
    if (gX % lX != 0 || gY % lY != 0) 
    {
        std::cerr << "Error: Global size must be divisible by local size." << std::endl;
        exit(EXIT_FAILURE);
    }

    auto start_gpu = std::chrono::high_resolution_clock::now();
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalSize, localSize, 0, nullptr, nullptr);
    checkError(err, "clEnqueueNDRangeKernel");

    // Wait for completion
    clFinish(queue);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> gpu_duration = end_gpu - start_gpu;

    // Read back results
    err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * N * N, C.data(), 0, nullptr, nullptr);
    checkError(err, "clEnqueueReadBuffer");

    std::cout << "GPU matrix multiplication took: " << gpu_duration.count() << " seconds" << std::endl;
    std::cout << "FIRST ELEMENT: " << C[0] << std::endl;
    std::cout << "MIDDLE ELEMENT: " << C[N*N/2] << std::endl;
    std::cout << "LAST ELEMENT: " << C[N*N-1] << std::endl;

    // Cleanup
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
