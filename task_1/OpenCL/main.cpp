#include <iostream>
#include <vector>
#include <CL/cl.h>

// Код, который будет выполнятся на GPU/CPU
const char* Code = R"(
__kernel void hello() {
  int thread_id = get_global_id(0);
  int total_threads = get_global_size(0);

  // не std::cout потому что для OpenCL потокобезопасен printf
  printf("Hello from thread %d out of %d\n", thread_id, total_threads);
}
)";

int main() {
  cl_int err;

  // Ищем хост
  cl_platform_id platform;
  err = clGetPlatformIDs(1, &platform, NULL);
  if (err != CL_SUCCESS) {
    std::cerr << "Failed to find OpenCL platform." << std::endl;
    return 1;
  }

  // Ищем вычислительное устройство
  cl_device_id device;
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
  if (err != CL_SUCCESS) {
    std::cerr << "Failed to find OpenCL device." << std::endl;
    return 1;
  }

  cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
  cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
  cl_program program = clCreateProgramWithSource(context, 1, &Code, NULL, &err);
  err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);  
  if (err != CL_SUCCESS) {
    std::cerr << "Failes to build programm for GPU/CPU device." << std::endl;
    return 1;
  }
  cl_kernel kernel = clCreateKernel(program, "hello", &err);
  size_t global_work_size = 4; 
  clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
  clFinish(queue);

  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
  return 0;
}
