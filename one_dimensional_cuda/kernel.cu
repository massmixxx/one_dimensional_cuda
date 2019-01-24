#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <bitset>
#include "kernel.h"
#include "dev_array.h"
#include <stdio.h>
#include <vector>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <chrono>
#include "BitMapSaver.h"


using namespace std;
using namespace thrust;



__global__ void countAutomatKernel(unsigned char *result, unsigned int size, unsigned char RULE, int CYCLE)
{
	size_t t = threadIdx.x + (blockDim.x * blockIdx.x);
	if (t > size) return;

	unsigned int location = size * CYCLE + t;
	unsigned int previousRow = location-size;
	
	if (t == 0) {
		result[location] = (RULE & 1 << (7 & (result[previousRow + 1] << 2 | result[previousRow] << 1 | result[location - 1]))) ? 1 : 0;
		return;
	}
	if (t == size - 1) {
		result[location] = (RULE & 1 << (7 & (result[previousRow - size +1] << 2 | result[previousRow] << 1 | result[previousRow- 1]))) ? 1 : 0;
		return;
	}
	result[location] = (RULE & 1 << (7 & (result[previousRow - 1] << 2 | result[previousRow] << 1 | result[previousRow + 1]))) ? 1 : 0;
}

kernel::kernel() { }
kernel::~kernel() {}

int kernel::printCUDA(bool isImage, unsigned char RULE,unsigned int CYCLES,unsigned int DIMENSION)
{
	cout << "IN printCUDA" << endl;

	int threadsPerBlock = DIMENSION;
	unsigned long long blocksPerGrid = 1;
	if (DIMENSION > 1024) {
		threadsPerBlock = 1024;
		blocksPerGrid = (DIMENSION / threadsPerBlock) + 1;
	}

	unsigned char *result = new unsigned char[DIMENSION*CYCLES];
	
	for (unsigned int i = 0; i < DIMENSION; i++)
		result[i] = 0;
		
	result[(DIMENSION / 2)] = 1;
	dev_array<unsigned char> device_A(DIMENSION*CYCLES);
	device_A.set(result, DIMENSION*CYCLES);

	auto chrono_time_start = chrono::system_clock::now();

	for (unsigned int i = 1; i < CYCLES; i++) {
		countAutomatKernel << <blocksPerGrid, threadsPerBlock >> > (device_A.getData(), DIMENSION, RULE, i);		
	}

	auto chrono_time_end = chrono::system_clock::now();

	cudaDeviceSynchronize();
	device_A.get(result, DIMENSION*CYCLES); 

	if (isImage)
		BitMapSaver::SaveBitmap(result, DIMENSION, CYCLES, "GPUImageResult");

	device_A.~dev_array();
	delete result;

	auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono_time_end - chrono_time_start);
	auto elapsedMicro = chrono::duration_cast<chrono::microseconds>(chrono_time_end - chrono_time_start);
	cout << ">GPU< Czas chrono: " << elapsed.count() << "milisekund. |  "<< elapsedMicro.count() << " microsekund." << endl << endl;

	return 0;
}



