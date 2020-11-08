
#define PROGRAM_FILE "math.cl"

#include "../util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <vector>

#include <CL/cl.h>

using namespace std;

int main() 
{
    /* Host/device data structures */
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_int i, err;

    /* Program/kernel data structures */
    cl_program program;
    FILE* program_handle;
    size_t program_size, log_size;
    cl_kernel kernel, kernel2, kernel3;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0) {
        perror("Couldn't find any platforms");
        exit(1);
    }

    /* Access a device */
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err < 0) {
        perror("Couldn't find any devices");
        exit(1);
    }

    /* Create the context */
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }

    /* Read program file and place content into buffer */
    program_handle = fopen(PROGRAM_FILE, "r");
    if (program_handle == NULL) {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    vector<char> program_buffer(program_size + 1, 0);
    fread(program_buffer.data(), sizeof(char), program_size, program_handle);
    fclose(program_handle);

    /* Create program from file */
    char* prog_buf = program_buffer.data();
    program = clCreateProgramWithSource(context, 1, (const char**)&prog_buf, &program_size, &err);
    if (err < 0) {
        perror("Couldn't create the program");
        exit(1);
    }

    /* Build program */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0) {
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        vector<char> program_log(log_size + 1, 0);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, &program_log[0], NULL);
        printf("%s\n", &program_log[0]);
        exit(1);
    }

    /* Create a CL command queue for the device*/
    queue = clCreateCommandQueue(context, device, 0, &err);
    if (err < 0) {
        perror("Couldn't create the command queue");
        exit(1);
    }

    size_t width = 300, height = 200;

    kernel = clCreateKernel(program, "hanning", &err);
    if (err < 0) {
        perror("Couldn't create the kernel");
        exit(1);
    }
    kernel2 = clCreateKernel(program, "cosine2d", &err);
    if (err < 0) {
        perror("Couldn't create the kernel");
        exit(1);
    }
    kernel3 = clCreateKernel(program, "gauss2d", &err);
    if (err < 0) {
        perror("Couldn't create the kernel");
        exit(1);
    }

    // cos_w
    cl_mem cosw = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * width, nullptr, &err);
    if (err < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cosw);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel, 1, sizeof(int), (int*)&width);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &width, NULL, 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the kernel execution command");
        exit(1);
    }
    vector<double> host_cosw(width);
    err = clEnqueueReadBuffer(queue, cosw, CL_TRUE, 0, sizeof(double) * width, host_cosw.data(), 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the read buffer command");
        exit(1);
    }

    // cos_h
    cl_mem cosh = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * height, nullptr, &err);
    if (err < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cosh);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel, 1, sizeof(int), (int*)&height);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &height, NULL, 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the kernel execution command");
        exit(1);
    }
    vector<double> host_cosh(height);
    err = clEnqueueReadBuffer(queue, cosw, CL_TRUE, 0, sizeof(double) * height, host_cosh.data(), 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the read buffer command");
        exit(1);
    }

    // cos_2d
    cl_mem cos2d = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * width * height, nullptr, &err);
    if (err < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }
    err = clSetKernelArg(kernel2, 0, sizeof(cl_mem), &cos2d);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel2, 1, sizeof(cl_mem), &cosw);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel2, 2, sizeof(cl_mem), &cosh);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel2, 3, sizeof(int), (int*)&width);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel2, 4, sizeof(int), (int*)&height);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    size_t cos2d_work_size[2] = { width, height };
    err = clEnqueueNDRangeKernel(queue, kernel2, 2, NULL, cos2d_work_size, NULL, 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the kernel execution command");
        exit(1);
    }
    vector<double> host_cos2d(width* height);
    err = clEnqueueReadBuffer(queue, cos2d, CL_TRUE, 0, sizeof(double) * width * height, host_cos2d.data(), 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the read buffer command");
        exit(1);
    }
    dump2text("cos2d-gpu", host_cos2d.data(), width, height);

    // guass2d
    cl_mem guass2d = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * width * height, nullptr, &err);
    if (err < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }
    err = clSetKernelArg(kernel3, 0, sizeof(cl_mem), &guass2d);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel3, 1, sizeof(int), (int*)&width);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    err = clSetKernelArg(kernel3, 2, sizeof(int), (int*)&height);
    if (err < 0) {
        perror("Couldn't set the kernel argument");
        exit(1);
    }
    size_t work_size[2] = { width, height };
    err = clEnqueueNDRangeKernel(queue, kernel3, 2, NULL, work_size, NULL, 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the kernel execution command");
        exit(1);
    }
    vector<double> host_guass2d(width * height);
    err = clEnqueueReadBuffer(queue, guass2d, CL_TRUE, 0, sizeof(double) * width * height, host_guass2d.data(), 0, NULL, NULL);
    if (err < 0) {
        perror("Couldn't enqueue the read buffer command");
        exit(1);
    }
    dump2text("guass2d-gpu", host_guass2d.data(), width, height);

    /* Deallocate resources */
    clReleaseMemObject(cosw);
    clReleaseMemObject(cosh);
    clReleaseMemObject(guass2d);

    clReleaseKernel(kernel);
    clReleaseKernel(kernel3);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    return 0;
}