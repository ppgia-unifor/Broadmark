#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Settings.h"
#include "Core/Scene.h"
#include "Broadphase\BroadphaseInterface.h"
#include "Broadphase/OverlapChaches/NullCache.h"

#include <type_traits>


class None : public BroadphaseInterface {
public:
	NullCache m_cache;

public:
	virtual ~None() = default;


	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override {}
	void UpdateObjects(const SceneFrame& frameData) override {}
	void SearchOverlaps() override {}

	void CleanCache() override {
		m_cache.Clean();
	}

	OverlapCache* GetOverlaps() override {
		return &m_cache;
	}
};
