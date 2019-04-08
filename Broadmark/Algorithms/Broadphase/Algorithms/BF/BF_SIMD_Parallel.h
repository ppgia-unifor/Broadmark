#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"
#include "Core/Parallel.h"

#include <omp.h>


#define FloatToSIMD(x) *(_simd*)&(x)
template<typename _simd, size_t w>
class BF_SIMD_Parallel : public SoAAlgorithm<Object, SimpleCache, _simd, w> {
public:
	typedef StructAlignment(32) std::array<_simd, 6> SimdObject;

	void SearchOverlaps() override {
		omp_lock_t writelock;
		omp_init_lock(&writelock);

		size_t ranges[64]; // big enought for modern processors
		Parallel::balanceTriangularWorkload(ranges, this->m_settings.m_numberOfObjects, this->m_settings.m_numThreads, w);

		#pragma omp parallel num_threads(this->m_settings.m_numThreads)
		{
			std::vector<ObjectPair> partialResults;
			const int tid = omp_get_thread_num();
			int results[w];

			for (size_t i = ranges[tid]; i < ranges[tid + 1]; i += w) {
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
						//const size_t mask = min(w, this->m_settings.m_numberOfObjects - j);
						for (size_t k = 0; k < w; k++) {
							if (results[k] != 0) {
								const Object *a_obj = &this->m_objects[i + k];
								const Object *b_obj = &this->m_objects[j + k];
								ASSERT(a_obj->m_id < this->m_settings.m_numberOfObjects);
								ASSERT(b_obj->m_id < this->m_settings.m_numberOfObjects);
 
								partialResults.emplace_back(a_obj, b_obj);
							}
						}
					}
				}
			}

			omp_set_lock(&writelock);
			for (const ObjectPair pair : partialResults) {
				this->m_cache.AddPair(pair.m_a, pair.m_b);
			}
			omp_unset_lock(&writelock);
		}
	}
};

#undef FloatToSIMD
class BF_SSE_Parallel : public BF_SIMD_Parallel<__m128, 4> {};
class BF_AVX_Parallel : public BF_SIMD_Parallel<__m256, 8> {};

