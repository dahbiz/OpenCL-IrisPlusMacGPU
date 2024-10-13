#include <CL/cl.h>
#include <iostream>

void checkError(cl_int err, const char* name) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error: " << name << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    cl_platform_id platform;
    cl_device_id device;
    cl_uint numPlatforms;
    cl_int err;

    // Get platform
    err = clGetPlatformIDs(1, &platform, &numPlatforms);
    checkError(err, "clGetPlatformIDs");

    // Get device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
    checkError(err, "clGetDeviceIDs");

    // Query maximum work group size
    size_t maxWorkGroupSize;
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, nullptr);
    checkError(err, "clGetDeviceInfo max work group size");

    // Query maximum work item sizes
    size_t maxWorkItemSizes[3]; // 3D size array for max work item sizes
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItemSizes), maxWorkItemSizes, nullptr);
    checkError(err, "clGetDeviceInfo max work item sizes");

    // Print results
    std::cout << "Maximum Work Group Size: " << maxWorkGroupSize << std::endl;
    std::cout << "Maximum Work Item Sizes: ";
    for (int i = 0; i < 3; ++i) {
        std::cout << maxWorkItemSizes[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}