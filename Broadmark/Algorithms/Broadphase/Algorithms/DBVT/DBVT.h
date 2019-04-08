#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Settings.h"
#include "Core/Scene.h"
#include "Broadphase/OverlapChaches/NullCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"


class b3DynamicBvhBroadphase;

#define Vec3Conv(v) {v[0], v[1], v[2]}
class DBVT : public BaseAlgorithm<Object, NullCache> {
protected:
	b3DynamicBvhBroadphase *m_broadphase = nullptr;
	bool m_deferred;

public:
	DBVT(bool deferred);
	virtual ~DBVT();

	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override;
	void UpdateObjects(const SceneFrame& frameData) override;
	void SearchOverlaps() override;
};


class DBVT_D : public DBVT {
public:
	DBVT_D() : DBVT(true) {}
};

class DBVT_F : public DBVT {
public:
	DBVT_F() : DBVT(false) {}
};