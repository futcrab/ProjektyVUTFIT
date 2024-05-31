/**
 * @file LineMandelCalculator.cc
 * @author Peter Ďurica <xduric05@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over lines
 * @date 31.10.2023
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <immintrin.h>

#include <stdlib.h>


#include "LineMandelCalculator.h"


LineMandelCalculator::LineMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
	BaseMandelCalculator(matrixBaseSize, limit, "LineMandelCalculator")
{
	// @TODO allocate & prefill memory
	data = (int *)(_mm_malloc(height * width * sizeof(int), 64));
	Zreal = (float *)(_mm_malloc(width * sizeof(float), 64));
	Zimag = (float *)(_mm_malloc(width * sizeof(float), 64));
	RealX = (float *)(_mm_malloc(width * sizeof(float), 64));

	counter = (int *)(_mm_malloc(width * sizeof(int), 64));

	std::fill(data, data + height * width, limit);
}

LineMandelCalculator::~LineMandelCalculator() {
	// @TODO cleanup the memory
	_mm_free(data);
	_mm_free(Zreal);
	_mm_free(Zimag);
	_mm_free(RealX);
	_mm_free(counter);
	data = NULL;
	Zreal = NULL;
	Zimag = NULL;
	RealX = NULL;
	counter = NULL;
}


int * LineMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers
	auto Zreal_loc = Zreal;
	auto Zimag_loc = Zimag;
	auto data_loc = data;
	auto RealX_loc = RealX;
	auto counter_loc = counter;

	for (int i = 0; i < height/2; i++){
		float y = y_start + i * dy; // current imaginary value
		
		#pragma omp for simd simdlen(64) aligned(RealX_loc: 64)
		for (int w = 0; w < width; w++){
			RealX_loc[w] = x_start + w * dx; // current real value
		}

		#pragma omp for simd simdlen(64) aligned(counter_loc: 64)
		for (int w = 0; w < width; w++){
			counter_loc[w] = 0;
		}
		
		#pragma omp for simd simdlen(64) aligned(RealX_loc: 64, Zreal_loc: 64, Zimag_loc: 64)
		for (int w = 0; w < width; w++){
			Zreal_loc[w] = RealX_loc[w];
			Zimag_loc[w] = y;
		}
		
		for (int l = 0; l < limit; l++){
			
			#pragma omp for simd simdlen(64) aligned(Zreal_loc: 64, Zimag_loc: 64, data_loc: 64, counter_loc: 64, RealX_loc: 64)
			for (int j = 0; j < width; j++){

				float r2 = Zreal_loc[j] * Zreal_loc[j];
				float i2 = Zimag_loc[j] * Zimag_loc[j];

				Zimag_loc[j] = 2.0f * Zreal_loc[j] * Zimag_loc[j] + y;
				Zreal_loc[j] = r2 - i2 + RealX_loc[j];
				
				if (r2 + i2 <= 4.0f){
					counter_loc[j]++;
				}
			}
		}
		#pragma omp for simd simdlen(64) aligned(data_loc: 64, counter_loc: 64)
		for (int j = 0; j < width; j++){
			data[i * width + j] = counter_loc[j];
			data[(height - (1+i)) * width + j] = counter_loc[j];
		}
	}
	return data;
}
