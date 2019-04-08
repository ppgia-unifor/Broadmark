


#include "DBVT.h"
#include "Dependencies\Bullet3\Bullet3Collision\BroadPhaseCollision\b3DynamicBvhBroadphase.h"



#define Vec3Conv(v) {v[0], v[1], v[2]}

DBVT::DBVT(bool deferred) {
	m_deferred = deferred;
}

DBVT::~DBVT() {
	delete m_broadphase;
}

void DBVT::Initialize(InflatedSettings settings, const SceneFrame& frameData) {
	BaseAlgorithm<Object, NullCache>::Initialize(settings, frameData);

	m_broadphase = new b3DynamicBvhBroadphase(m_settings.m_numberOfObjects + 1);
	m_broadphase->m_deferedcollide = m_deferred;

	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		const Aabb& aabb = frameData.m_aabbs[i];

		m_broadphase->createProxy(Vec3Conv(aabb.m_min), Vec3Conv(aabb.m_max), i + 1, nullptr, 1, 1);
	}
}

void DBVT::UpdateObjects(const SceneFrame& frameData) {
	BaseAlgorithm<Object, NullCache>::UpdateObjects(frameData);

	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		const Aabb& aabb = frameData.m_aabbs[i];

		m_broadphase->setAabb(i + 1, Vec3Conv(aabb.m_min), Vec3Conv(aabb.m_max), nullptr);
	}
}

void DBVT::SearchOverlaps() {
	m_broadphase->calculateOverlappingPairs();

	m_cache.m_numOfOverlaps = m_broadphase->getOverlappingPairCache()->getNumOverlappingPairs();
}
