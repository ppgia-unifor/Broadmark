#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"
#include "Core/Math/Plane.h"
#include "Core/Parallel.h"

#include <execution>
#include <omp.h>


#define FloatToSIMD(x) *(_simd*)&(x)
template<typename _simd, size_t w>
class SAP_SIMD_Parallel : public SoAAlgorithm<Object, SimpleCache, _simd, w> {
public:
	typedef StructAlignment(32) std::array<_simd, 6> SimdObject;

	void SearchOverlaps() override {
		const Plane best = Plane::FindBestPlane(this->m_objects, this->m_settings.m_numberOfObjects);
		const Axis axis = best.m_axis;
		std::sort(std::execution::par_unseq, this->m_objects, this->m_objects + this->m_settings.m_numberOfObjects, ObjectComparer<Object>(axis));
		this->UpdateSOA();

		omp_lock_t writelock;
		omp_init_lock(&writelock);

		#pragma omp parallel num_threads(this->m_settings.m_numThreads)
		{
			std::vector<ObjectPair> partialResults;
			int results[w];

			#pragma omp for
			for (int i = 0; i < this->m_settings.m_numberOfObjects - 1; i += w) {
				const SimdObject& a = {
					FloatToSIMD(this->m_min[0][i]),
					FloatToSIMD(this->m_min[1][i]),
					FloatToSIMD(this->m_min[2][i]),
					FloatToSIMD(this->m_max[0][i]),
					FloatToSIMD(this->m_max[1][i]),
					FloatToSIMD(this->m_max[2][i])
				};

				for (size_t j = i + 1; j < this->m_settings.m_numberOfObjects; j++) {
					if (SIMD::IsGreater(FloatToSIMD(this->m_min[(int)axis][j]), a[(int)axis + 3])) {
						break; // b.m_aabb.m_min[axis] > a.m_aabb.m_max[axis]
					}

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
								const Object *a_obj = &this->m_objects[i + k];
								const Object *b_obj = &this->m_objects[j + k];

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
class SAP_SSE_Parallel : public SAP_SIMD_Parallel<__m128, 4> {};
class SAP_AVX_Parallel : public SAP_SIMD_Parallel<__m256, 8> {};
