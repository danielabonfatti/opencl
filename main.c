#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>

#define TAMANHO_VETOR 6

char* AbrirKernel(void);

int main(int argvc, char** argv)
{
	cl_platform_id platformId;
	cl_device_id deviceId;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;
	cl_mem bufInputA;
	cl_mem bufInputB;
	cl_mem bufOutput;

	size_t globalSize[1] = { TAMANHO_VETOR };

	int inputA[TAMANHO_VETOR] = { 1, 3, 5, 7, 9, 11 };
	int inputB[TAMANHO_VETOR] = { 0, 2, 4, 6, 8, 10 };
	int output[TAMANHO_VETOR];

	int tamanho = TAMANHO_VETOR;

	char* source = AbrirKernel();

	clGetPlatformIDs(1, &platformId, NULL);
	clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceId, NULL);
	context = clCreateContext(0, 1, &deviceId, NULL, NULL, NULL);
	queue = clCreateCommandQueue(context, deviceId, 0, NULL);
	program = clCreateProgramWithSource(context, 1, &source, NULL, NULL);
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	kernel = clCreateKernel(program, "Kernel", NULL);

	bufInputA = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, TAMANHO_VETOR * sizeof(int), (void*)inputA, NULL);
	bufInputB = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, TAMANHO_VETOR * sizeof(int), (void*)inputB, NULL);
	bufOutput = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, TAMANHO_VETOR * sizeof(int), (void*)output, NULL);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufInputA);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufInputB);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufOutput);
	clSetKernelArg(kernel, 3, sizeof(cl_int), &tamanho);

	clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalSize, NULL, 0, NULL, NULL);

	clFinish(queue);

	clEnqueueReadBuffer(queue, bufOutput, CL_TRUE, 0, TAMANHO_VETOR * sizeof(int), output, 0, NULL, NULL);
	
	int i;
	for (i = 0; i < TAMANHO_VETOR; i++)
	{
		printf("%d + %d = %d\n", inputA[i], inputB[i], output[i]);
	}

	clReleaseMemObject(bufInputA);
	clReleaseMemObject(bufInputB);
	clReleaseMemObject(bufOutput);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	system("pause");
	return 0;
}

char* AbrirKernel(void)
{
	FILE *f;
	char *source;
	size_t source_size, program_size;

	f = fopen("kernel.cl", "rb");
	if (!f)
	{
		printf("Falha ao abrir kernel\n");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	program_size = ftell(f);
	rewind(f);
	source = (char*)malloc(program_size + 1);
	source[program_size] = '\0';
	fread(source, sizeof(char), program_size, f);

	fclose(f);

	return source;
}