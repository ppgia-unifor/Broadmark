


#pragma once


#include "Vec3.h"
#include <xmmintrin.h>
#include <immintrin.h>


// SIMD SoA AABB type
typedef std::array<__m128, 6> SSEObject;
typedef std::array<__m256, 6> AVXObject;


// Some useful SIMD methods
class SIMD {
public:
	static bool IsGreater(const __m128& a, const __m128& b) {
		const __m128 componentWiseGreaterThan = _mm_cmple_ps(a, b); // !(a > b)
		return _mm_testz_ps(componentWiseGreaterThan, componentWiseGreaterThan);
	}
	static bool IsGreater(const __m256& a, const __m256& b) {
		const __m256 componentWiseGreaterThan = _mm256_cmp_ps(a, b, _CMP_LE_OS);
		return _mm256_testz_ps(componentWiseGreaterThan, componentWiseGreaterThan);
	}
};
