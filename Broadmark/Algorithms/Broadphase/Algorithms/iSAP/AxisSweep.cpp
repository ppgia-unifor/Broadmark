


#include "AxisSweep.h"
#include "Dependencies\Bullet2\btBroadphaseProxy.h"
#include "Dependencies\Bullet2\btAxisSweep3.h"


#define Vec3Conv(v) {v[0], v[1], v[2]}

AxisSweep::~AxisSweep() {
	delete m_broadphase;
	delete m_proxies;
}

void AxisSweep::Initialize(InflatedSettings settings, const SceneFrame& frameData) {
	BaseAlgorithm<Object, NullCache>::Initialize(settings, frameData);

	m_broadphase = new bt32BitAxisSweep3(Vec3Conv(m_settings.m_worldAabb.m_min), Vec3Conv(m_settings.m_worldAabb.m_max), m_settings.m_numberOfObjects + 1, nullptr, true);
	m_proxies = new btBroadphaseProxy*[m_settings.m_numberOfObjects];
	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		const Aabb& aabb = frameData.m_aabbs[i];

		m_proxies[i] = m_broadphase->createProxy(Vec3Conv(aabb.m_min), Vec3Conv(aabb.m_max), i + 1, nullptr, 1, 1, nullptr);
	}
}

void AxisSweep::UpdateObjects(const SceneFrame& frameData) {
	BaseAlgorithm<Object, NullCache>::UpdateObjects(frameData);

	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		const Aabb& aabb = frameData.m_aabbs[i];

		m_broadphase->setAabb(m_proxies[i], Vec3Conv(aabb.m_min), Vec3Conv(aabb.m_max), nullptr);
	}
}

void AxisSweep::SearchOverlaps() {
	m_broadphase->calculateOverlappingPairs(nullptr);

	m_cache.m_numOfOverlaps = m_broadphase->getOverlappingPairCache()->getNumOverlappingPairs();
}
