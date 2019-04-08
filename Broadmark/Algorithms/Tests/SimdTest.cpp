

#undef min
#undef max
#undef fabs
#include "Dependencies/doctest.h"

#include "Core/Math/SIMD.h"
#include <iostream>
#include <string>


using namespace std;


TEST_CASE("SIMD_GreaterThan_SSE") {
	const __m128 a = _mm_set_ps(1, 2, 3, 4);
	const __m128 t1 = _mm_set_ps(5, 5, 5, 5);
	const __m128 t2 = _mm_set_ps(2, 2, 2, 2);

	CHECK(SIMD::IsGreater(a, t1) == false);
	CHECK(SIMD::IsGreater(t1, a) == true);
	CHECK(SIMD::IsGreater(a, t2) == false);
	CHECK(SIMD::IsGreater(t2, a) == false);
}


TEST_CASE("SIMD_GreaterThan_AVX") {
	const __m256 a = _mm256_set_ps(1, 2, 3, 4, 1, 2, 3, 4);
	const __m256 t1 = _mm256_set_ps(5, 5, 5, 5, 5, 5, 5, 5);
	const __m256 t2 = _mm256_set_ps(2, 2, 2, 2, 2, 2, 2, 2);

	CHECK(SIMD::IsGreater(a, t1) == false);
	CHECK(SIMD::IsGreater(t1, a) == true);
	CHECK(SIMD::IsGreater(a, t2) == false);
	CHECK(SIMD::IsGreater(t2, a) == false);
}