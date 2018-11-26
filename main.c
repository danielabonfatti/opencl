#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>

#define TAMANHO_VETOR 6

char* AbrirKernel(void);

int main(int argvc, char** argv)
{
	//Definicao das variaveis para armazenar os objetos OpenCL
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

	//Funcao apenas para abrir o arquivo que contem o kernel
	char* source = AbrirKernel();

	//Obtem identificadores de plataforma e dispositivo. É solicitada uma GPU
	clGetPlatformIDs(1, &platformId, NULL);
	clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceId, NULL);

	//Cria um contexto com o dispositivo retornado anteriormente
	context = clCreateContext(0, 1, &deviceId, NULL, NULL, NULL);

	//Cria uma fila de comandos para o dispositivo
	queue = clCreateCommandQueue(context, deviceId, 0, NULL);

	//Cria o objeto do programa a partir do kernel (kernel esta armazenado no vetor 'source')
	program = clCreateProgramWithSource(context, 1, &source, NULL, NULL);

	//Compila o programa para todos os dispositivos do contexto
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	//Retorna o kernel a partir do programa compilado
	kernel = clCreateKernel(program, "Kernel", NULL);

	//Cria os objetos de memória para comunicação com a memoria global do dispotitivo encontrado
	bufInputA = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, TAMANHO_VETOR * sizeof(int), (void*)inputA, NULL);
	bufInputB = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, TAMANHO_VETOR * sizeof(int), (void*)inputB, NULL);
	bufOutput = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, TAMANHO_VETOR * sizeof(int), (void*)output, NULL);

	//Argumentos que serão passados para o kernel
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufInputA);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufInputB);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufOutput);
	clSetKernelArg(kernel, 3, sizeof(cl_int), &tamanho);

	//Envia o kernel para ser executado no dispositivo
	clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalSize, NULL, 0, NULL, NULL);

	//Nesta parte o kernel e' executado, bloqueando o host ate o termino da execucao
	clFinish(queue);

	//Transfere os resultados da execução para a memoria do host
	clEnqueueReadBuffer(queue, bufOutput, CL_TRUE, 0, TAMANHO_VETOR * sizeof(int), output, 0, NULL, NULL);
	
	int i;
	for (i = 0; i < TAMANHO_VETOR; i++)
	{
		printf("%d + %d = %d\n", inputA[i], inputB[i], output[i]);
	}

	//Libera os recursos e encerra o programa
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
