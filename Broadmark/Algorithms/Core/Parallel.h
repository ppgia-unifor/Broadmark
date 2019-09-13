

#pragma once

#include <cmath>

// Utility class to perform some parallel computing related operations
class Parallel {
public:
	// Gets the triangular number of 'n'
	// In other words, computes the running sum from 1 to 'n'
	static size_t triangularNumber(size_t n) {
		return n * (n - 1) / 2;
	}
	// Inversely, finds the value which the triangular number is 'n'
	static size_t solveTriangularNumber(size_t n) {
		double r = 0.0f;
		r = sqrt(1 + 8 * n);
		r = (r - 1) / 2;
		return (size_t)ceil(r) + 1;
	}

	// Finds the amount of the 'triangle' a thread has to compute
	static void balanceTriangularWorkload(size_t *ranges, size_t n, size_t t) {
		balanceTriangularWorkload(ranges, n, t, 1);
	}
	// Finds the amount of the 'triangle' a thread has to compute, considering SIMD alignment
	static void balanceTriangularWorkload(size_t *ranges, size_t n, size_t t, size_t alignment) {
		const size_t work = triangularNumber(n) / t;
		ranges[0] = 0;
		for (size_t i = 1; i < t; i++) {
			ranges[t - i] = n - solveTriangularNumber(i * work);
			if (ranges[t - i] % alignment != 0) {
				ranges[t - i] += alignment - ranges[t - i] % alignment;
			}
		}

		ranges[t] = n - 1;
	}
};