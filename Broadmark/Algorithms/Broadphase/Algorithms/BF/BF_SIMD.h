#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"


#define FloatToSIMD(x) *(_simd*)&(x)
template<typename _simd, size_t w>
class BF_SIMD : public SoAAlgorithm<Object, SimpleCache, _simd, w> {
public:
	typedef StructAlignment(32) std::array<_simd, 6> SimdObject;

	void SearchOverlaps() override {
		int results[w];
		for (size_t i = 0; i < this->m_settings.m_numberOfObjects - 1; i += w) {
			const SimdObject& a = {
				FloatToSIMD(this->m_min[0][i]),
				FloatToSIMD(this->m_min[1][i]),
				FloatToSIMD(this->m_min[2][i]),
				FloatToSIMD(this->m_max[0][i]),
				FloatToSIMD(this->m_max[1][i]),
				FloatToSIMD(this->m_max[2][i])
			};

			for (size_t j = i + 1; j < this->m_settings.m_numberOfObjects; j++) {
				const SimdObject& b = {
					FloatToSIMD(this->m_min[0][j]),
					FloatToSIMD(this->m_min[1][j]),
					FloatToSIMD(this->m_min[2][j]),
					FloatToSIMD(this->m_max[0][j]),
					FloatToSIMD(this->m_max[1][j]),
					FloatToSIMD(this->m_max[2][j])
				};

				if (Aabb::Test(a, b, results)) {
					for (size_t k = 0; k < w; k++) {
						if (results[k] != 0) {
							Object *a_obj = &this->m_objects[i + k];
							Object *b_obj = &this->m_objects[j + k];
							this->m_cache.AddPair(a_obj, b_obj);
						}
					}
				}
			}
		}
	}
};

#undef FloatToSIMD
class BF_SSE : public BF_SIMD<__m128, 4> {};
class BF_AVX : public BF_SIMD<__m256, 8> {};
