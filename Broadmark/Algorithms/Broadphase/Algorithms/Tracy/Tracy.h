#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Settings.h"
#include "Core/Scene.h"
#include "Broadphase/OverlapChaches/SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"


#undef min
#undef max
#include "Dependencies/Tracy/SweepAndPrune.hpp"


class Tracy : public BaseAlgorithm<Object, SimpleCache> {
protected:
	SweepAndPrune::Tracy_SweepAndPrune<3, int> *m_tracy = nullptr;
	std::vector<std::pair<int, int>> m_pairs;

public:
	virtual ~Tracy() {
		delete m_tracy;
	}

	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override {
		BaseAlgorithm<Object, SimpleCache>::Initialize(settings, frameData);

		double n = m_settings.m_numberOfObjects / (double)m_settings.m_Grid_ObjectsPerCell;
		n = ceil(pow(n, 1.0f / 3.0f));

		int m_gridSize[3];
		m_gridSize[0] = (int)n;
		m_gridSize[1] = (int)n;
		m_gridSize[2] = (int)n;

		m_tracy = new SweepAndPrune::Tracy_SweepAndPrune<3, int>();
		m_tracy->SetGridParameters(
			m_settings.m_worldAabb.m_min.m_data,
			m_settings.m_worldAabb.m_max.m_data,
			m_gridSize);

		for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
			m_tracy->insert(i, frameData.m_aabbs[i].m_min.m_data, frameData.m_aabbs[i].m_max.m_data);
		}
	}

	void UpdateObjects(const SceneFrame& frameData) override {
		BaseAlgorithm<Object, SimpleCache>::UpdateObjects(frameData);

		for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
			m_tracy->UpdateAABB(i, frameData.m_aabbs[i].m_min.m_data, frameData.m_aabbs[i].m_max.m_data);
		}
	}

	void SearchOverlaps() override {
		m_tracy->GatherCollisions(m_pairs);

		for (const std::pair<int, int> pair : m_pairs) {
			Object *p1 = &m_objects[pair.first];
			Object *p2 = &m_objects[pair.second];

			m_cache.AddPair(p1, p2);
		}

		m_pairs.clear();
	}
};
