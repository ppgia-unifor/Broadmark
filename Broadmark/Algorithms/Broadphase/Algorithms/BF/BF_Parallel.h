#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"

#include "Core/Parallel.h"
#include <omp.h>


class BF_Parallel : public BaseAlgorithm<Object, SimpleCache> {
public:


	void SearchOverlaps() override {
		omp_lock_t writelock;
		omp_init_lock(&writelock);

		size_t ranges[64]; // big enought for modern processors
		Parallel::balanceTriangularWorkload(ranges, m_settings.m_numberOfObjects, m_settings.m_numThreads);

		#pragma omp parallel num_threads(m_settings.m_numThreads)
		{
			std::vector<ObjectPair> partialResults;
			const int tid = omp_get_thread_num();

			for (size_t i = ranges[tid]; i < ranges[tid + 1]; i++) {
				const Object& a = m_objects[i];
				for (size_t j = i + 1; j < m_settings.m_numberOfObjects; j++) {
					const Object& b = m_objects[j];

					if (Aabb::Test(a.m_aabb, b.m_aabb)) {
						partialResults.emplace_back(&a, &b);
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