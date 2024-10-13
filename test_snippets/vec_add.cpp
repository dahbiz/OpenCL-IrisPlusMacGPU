#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <array>

// Error checking utility
void checkError(cl_int err, const char* name) 
{
    if (err != CL_SUCCESS) {
        std::cerr << "Error: " << name << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// OpenCL kernel for vector addition
const char* kernelSource = R"(
__kernel void vector_add(__global const float* A, __global const float* B, __global float* C, const int N) 
{
    int id = get_global_id(0);
    if (id < N) {
        C[id] = A[id] + B[id];
    }
}
)";

void vector_add_cpu(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C) {
    const size_t N = A.size();
    for (size_t i = 0; i < N; ++i) {
        C[i] = A[i] + B[i];
    }
}

int main() {
    constexpr size_t N = 1 << 27; // Size of the vectors
    std::vector<float> A(N, 1.0f); // Initialize vector A with 1.0f
    std::vector<float> B(N, 2.0f); // Initialize vector B with 2.0f
    std::vector<float> C(N); // Vector for results

    // CPU Timing
    auto start_cpu = std::chrono::high_resolution_clock::now();
    vector_add_cpu(A, B, C);
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> cpu_duration = end_cpu - start_cpu;

    std::cout << "CPU vector addition took: " << cpu_duration.count() << " seconds" << std::endl;
    std::cout << "CPU Result: " << C[0] << std::endl;

    // OpenCL setup
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    cl_int err;

    // Get platform and device
    checkError(clGetPlatformIDs(1, &platform, nullptr), "clGetPlatformIDs");
    checkError(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, nullptr), "clGetDeviceIDs");

    // Create OpenCL context
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    checkError(err, "clCreateContext");

    // Create command queue
    queue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err, "clCreateCommandQueue");

    // Create buffers
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, A.data(), &err);
    checkError(err, "clCreateBuffer A");

    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, B.data(), &err);
    checkError(err, "clCreateBuffer B");

    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N, nullptr, &err);
    checkError(err, "clCreateBuffer C");

    // Create program
    program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    checkError(err, "clCreateProgramWithSource");

    // Build program
    checkError(clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr), "clBuildProgram");

    // Create kernel
    kernel = clCreateKernel(program, "vector_add", &err);
    checkError(err, "clCreateKernel");

    // Set kernel arguments
    checkError(clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA), "clSetKernelArg 0");
    checkError(clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB), "clSetKernelArg 1");
    checkError(clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC), "clSetKernelArg 2");
    int n = static_cast<int>(N);
    checkError(clSetKernelArg(kernel, 3, sizeof(int), &n), "clSetKernelArg 3");

    // Enqueue kernel execution
    size_t globalSize = 256;  // Number of elements
    size_t localSize = 128; // Adjust as needed; experiment for optimization

    auto start_gpu = std::chrono::high_resolution_clock::now();
    checkError(clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, &localSize, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // Wait for completion
    checkError(clFinish(queue), "clFinish");
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> gpu_duration = end_gpu - start_gpu;

    std::cout << "GPU vector addition took: " << gpu_duration.count() << " seconds" << std::endl;
    std::cout << "GPU Result: " << C[N/2 + 100] << std::endl;

    // Read back results
    checkError(clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * N, C.data(), 0, nullptr, nullptr), "clEnqueueReadBuffer");

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
