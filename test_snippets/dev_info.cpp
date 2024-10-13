#include <CL/cl.h>
#include <iostream>
#include <vector>

void checkError(cl_int err, const char* name) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error: " << name << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Query platforms
    cl_uint numPlatforms;
    cl_int err = clGetPlatformIDs(0, nullptr, &numPlatforms);
    checkError(err, "clGetPlatformIDs");

    std::vector<cl_platform_id> platforms(numPlatforms);
    err = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    checkError(err, "clGetPlatformIDs");

    for (cl_uint i = 0; i < numPlatforms; ++i) 
    {
        char platformName[128];
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, nullptr);
        checkError(err, "clGetPlatformInfo");
        std::cout << "Platform Name: " << platformName << std::endl;
        
        // Query devices
        cl_uint numDevices;
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
        checkError(err, "clGetDeviceIDs");

        std::vector<cl_device_id> devices(numDevices);
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices.data(), nullptr);
        checkError(err, "clGetDeviceIDs");

        for (cl_uint j = 0; j < numDevices; ++j) {
            char deviceName[128];
            err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
            checkError(err, "clGetDeviceInfo");

            std::cout << "  Device Name: " << deviceName << std::endl;
        }
    }
    return 0;
}
