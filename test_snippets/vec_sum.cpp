#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <chrono>

// Error checking utility
void checkError(cl_int err, const char* name) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error: " << name << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

const char* kernelSource = R"(
__kernel void vector_sum(__global const float* A, __global float* B, const int N) {
    int id = get_global_id(0);
    float sum = 0.0f;

    // Each work item computes its own partial sum
    for (int i = id; i < N; i += get_global_size(0)) {
        sum += A[i];
    }

    // Write the partial sum to global memory
    B[id] = sum; // Each work item writes its result to its corresponding location in B
}
)";

// CPU implementation for vector summation
float vector_sum_cpu(const std::vector<float>& A, int N) {
    float sum = 0.0f;
    for (int i = 0; i < N; ++i) {
        sum += A[i];
    }
    return sum;
}

int main() {
    const int N = 1 << 27; // Size of the vectors
    std::vector<float> A(N, 1.0f); // Initialize vector A with 1.0f
    std::vector<float> B(N); // Buffer for partial sums
    float gpu_sum = 0.0f; // To store the final GPU result

    // CPU Timing
    auto start_cpu = std::chrono::high_resolution_clock::now();
    float cpu_sum = vector_sum_cpu(A, N);
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> cpu_duration = end_cpu - start_cpu;

    std::cout << "CPU vector summation result: " << cpu_sum << std::endl;
    std::cout << "CPU vector summation took: " << cpu_duration.count() << " seconds" << std::endl;

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

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, nullptr);
    checkError(err, "clGetDeviceIDs");

    // Create OpenCL context
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    checkError(err, "clCreateContext");

    // Create command queue
    queue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err, "clCreateCommandQueue");

    // Create buffers
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, A.data(), &err);
    checkError(err, "clCreateBuffer A");

    cl_mem bufferB = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N, nullptr, &err);
    checkError(err, "clCreateBuffer B");

    // Create program
    program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    checkError(err, "clCreateProgramWithSource");

    // Build program
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error: clBuildProgram (" << err << ")" << std::endl;

        // Get the size of the build log
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);

        // Allocate memory for the build log
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
        std::cerr << "Build log:\n" << log.data() << std::endl;

        exit(EXIT_FAILURE);
    }

    // Create kernel
    kernel = clCreateKernel(program, "vector_sum", &err);
    checkError(err, "clCreateKernel");

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    checkError(err, "clSetKernelArg 0");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    checkError(err, "clSetKernelArg 1");
    err = clSetKernelArg(kernel, 2, sizeof(int), &N);
    checkError(err, "clSetKernelArg 2");

    // Enqueue kernel execution
    size_t localSize = 128;  // Number of work items per work group
    size_t globalSize = ((N + localSize - 1) / localSize) * localSize; // Round up to nearest multiple of localSize
    auto start_gpu = std::chrono::high_resolution_clock::now();
    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, &localSize, 0, nullptr, nullptr);
    checkError(err, "clEnqueueNDRangeKernel");

    // Wait for completion
    clFinish(queue);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> gpu_duration = end_gpu - start_gpu;

    // Read back results
    std::vector<float> partial_sums(globalSize); // Buffer to read partial sums
    err = clEnqueueReadBuffer(queue, bufferB, CL_TRUE, 0, sizeof(float) * globalSize, partial_sums.data(), 0, nullptr, nullptr);
    checkError(err, "clEnqueueReadBuffer");

    // Sum the partial sums on the host
    for (int i = 0; i < globalSize; ++i) 
    {
        gpu_sum += partial_sums[i];
    }

    std::cout << "GPU vector summation result: " << gpu_sum << std::endl;
    std::cout << "GPU vector summation took: " << gpu_duration.count() << " seconds" << std::endl;

    // Cleanup
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
