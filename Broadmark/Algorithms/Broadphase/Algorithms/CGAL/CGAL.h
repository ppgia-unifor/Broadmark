#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Settings.h"
#include "Core/Scene.h"
#include "Broadphase/OverlapChaches/SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"

#if USE_CGAL
#undef min
#undef max
#include "CGAL\Box_intersection_d\Box_with_handle_d.h"
#include "CGAL\box_intersection_d.h"
#include "CGAL\Bbox_3.h"




class CGAL_Internal : public BaseAlgorithm<Object, SimpleCache> {
private:
	typedef CGAL::Box_intersection_d::Box_with_handle_d<float, 3, Object*> CGAL_Box_Type;
	class CGAL_Box : public CGAL_Box_Type {
	public:
		CGAL_Box() = default;
		CGAL_Box(float *lo, float *hi, Object* handle) : CGAL_Box_Type(lo, hi, handle) {}
	};

	static CGAL_Internal *m_instance;
	CGAL_Box *m_boxes = nullptr;

public:
	virtual ~CGAL_Internal() {
		delete m_boxes;
	}

	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override {
		BaseAlgorithm<Object, SimpleCache>::Initialize(settings, frameData);

		m_boxes = new CGAL_Box[settings.m_numberOfObjects];
		for (size_t i = 0; i < settings.m_numberOfObjects; i++) {
			m_boxes[i] = CGAL_Box(frameData.m_aabbs[i].m_min.m_data, frameData.m_aabbs[i].m_max.m_data, &m_objects[i]);
		}
	}

	void UpdateObjects(const SceneFrame& frameData) override {
		BaseAlgorithm<Object, SimpleCache>::UpdateObjects(frameData);

		for (int i = 0; i < m_settings.m_numberOfObjects; i++) {
			m_boxes[i] = CGAL_Box(frameData.m_aabbs[i].m_min.m_data, frameData.m_aabbs[i].m_max.m_data, &m_objects[i]);
		}
	}

	void SearchOverlaps() override {
		CGAL::box_self_intersection_d(
			m_boxes,
			m_boxes + m_settings.m_numberOfObjects,
			[&](const CGAL_Box& b1, const CGAL_Box& b2) { this->m_cache.AddPair(b1.handle(), b2.handle()); });
	}
};


#else

#include "Broadphase/Algorithms/Misc/None.h"

class CGAL_Internal : public None {};
#endif