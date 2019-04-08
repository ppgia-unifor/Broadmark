#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"


class BF : public BaseAlgorithm<Object, SimpleCache> {
public:
	void SearchOverlaps() override {
		for (size_t i = 0; i < m_settings.m_numberOfObjects - 1; i++) {
			const Object& a = m_objects[i];
			for (size_t j = i + 1; j < m_settings.m_numberOfObjects; j++) {
				const Object& b = m_objects[j];

				if (Aabb::Test(a.m_aabb, b.m_aabb)) {
					m_cache.AddPair(&a, &b);
				}
			}
		}
	}
};