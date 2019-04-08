#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"
#include "Core/Math/Plane.h"
#include "Core/Parallel.h"

#include <execution>
#include <omp.h>


class SAP_Parallel : public BaseAlgorithm<Object, SimpleCache> {
public:
	void SearchOverlaps() override {
		const Plane best = Plane::FindBestPlane(this->m_objects, this->m_settings.m_numberOfObjects);
		const Axis axis = best.m_axis;
		std::sort(std::execution::par_unseq, m_objects, m_objects + m_settings.m_numberOfObjects, ObjectComparer<Object>(axis));

		omp_lock_t writelock;
		omp_init_lock(&writelock);

		#pragma omp parallel num_threads(m_settings.m_numThreads)
		{
			std::vector<ObjectPair> partialResults;

			#pragma omp for
			for (int i = 0; i < this->m_settings.m_numberOfObjects - 1; i++) {
				const Object& a = m_objects[i];
				for (size_t j = i + 1; j < m_settings.m_numberOfObjects; j++) {
					const Object& b = m_objects[j];

					if (b.m_aabb.m_min[axis] > a.m_aabb.m_max[axis]) {
						break;
					}

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