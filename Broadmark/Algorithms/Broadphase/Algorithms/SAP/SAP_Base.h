#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"
#include "Core/Math/Plane.h"

#include <execution>


class SAP : public BaseAlgorithm<Object, SimpleCache> {
public:
	void SearchOverlaps() override {
		const Plane best = Plane::FindBestPlane(this->m_objects, this->m_settings.m_numberOfObjects);
		const Axis axis = best.m_axis;
		std::sort(m_objects, m_objects + m_settings.m_numberOfObjects, ObjectComparer<Object>(axis));
		
		for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
			const Object& a = m_objects[i];
			for (size_t j = i + 1; j < m_settings.m_numberOfObjects; j++) {
				const Object& b = m_objects[j];

				if (b.m_aabb.m_min[axis] > a.m_aabb.m_max[axis]) {
					break;
				}

				if (Aabb::Test(a.m_aabb, b.m_aabb)) {
					m_cache.AddPair(&a, &b);
				}
			}
		}
	}
};