__kernel void Kernel(__global int* inputA, __global int* inputB, __global int* output, int tamanho)
{
	int i = get_global_id(0);

	output[i] = inputA[i] + inputB[i];
}