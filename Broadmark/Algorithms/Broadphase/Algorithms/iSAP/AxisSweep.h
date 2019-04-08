#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Settings.h"
#include "Core/Scene.h"
#include "Broadphase/OverlapChaches/NullCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"


class bt32BitAxisSweep3;
class btBroadphaseProxy;

class AxisSweep : public BaseAlgorithm<Object, NullCache> {
protected:
	bt32BitAxisSweep3 *m_broadphase = nullptr;
	btBroadphaseProxy **m_proxies = nullptr;

public:
	virtual ~AxisSweep();
	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override;
	void UpdateObjects(const SceneFrame& frameData) override;
	void SearchOverlaps() override;
};
