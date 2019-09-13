#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Settings.h"
#include "Core/Scene.h"
#include "BaseAlgorithm.h"

#include <type_traits>
#include <array>



// Extension of the 'BaseAlgorithm' class that maintains a Structure-of-Arrays (SoA)
// storage of objects AABBs. This class is useful for implementing SIMD based algorithms
// As for 'BaseAlgorithm', the 'Object' and 'PairCache' classes are template arguments
// Additionally, the SIMD type and width are also template arguments
// Do not forget that new algorithms should be registered at 'Algorithms.cpp'
#define FloatToSIMD(x) *(_simd*)&(x)
template<typename ObjectType, typename CacheType, typename _simd, size_t w>
class SoAAlgorithm : public BaseAlgorithm<ObjectType, CacheType> {
protected:
	StructAlignment(32) float *m_min[3];
	StructAlignment(32) float *m_max[3];

public:
	virtual ~SoAAlgorithm() {
		for (size_t i = 0; i < 3; i++) {
			_aligned_free(m_min[i]);
			_aligned_free(m_max[i]);
		}
	}

	typedef StructAlignment(32) std::array<_simd, 6> SimdObject;


	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override {
		BaseAlgorithm<ObjectType, CacheType>::Initialize(settings, frameData);

		// Alocs a float for each object + 8 extra floats for padding
		m_min[0] = (float*)_aligned_malloc((this->m_settings.m_numberOfObjects + 8) * sizeof(float), 32);
		m_min[1] = (float*)_aligned_malloc((this->m_settings.m_numberOfObjects + 8) * sizeof(float), 32);
		m_min[2] = (float*)_aligned_malloc((this->m_settings.m_numberOfObjects + 8) * sizeof(float), 32);
		m_max[0] = (float*)_aligned_malloc((this->m_settings.m_numberOfObjects + 8) * sizeof(float), 32);
		m_max[1] = (float*)_aligned_malloc((this->m_settings.m_numberOfObjects + 8) * sizeof(float), 32);
		m_max[2] = (float*)_aligned_malloc((this->m_settings.m_numberOfObjects + 8) * sizeof(float), 32);

		this->UpdateSOA();
		for (size_t i = this->m_settings.m_numberOfObjects; i < this->m_settings.m_numberOfObjects + 8; i++) {
			m_min[0][i] = 0x999999 + 1;
			m_min[1][i] = 0x999999 + 1;
			m_min[2][i] = 0x999999 + 1;
			m_max[0][i] = 0x999999;
			m_max[1][i] = 0x999999;
			m_max[2][i] = 0x999999;
		}
	}

	void UpdateObjects(const SceneFrame& frameData) override {
		BaseAlgorithm<ObjectType, CacheType>::UpdateObjects(frameData);
		this->UpdateSOA();
	}

	void SearchOverlaps() override = 0;


	// Updates the entire SoA storage
	void UpdateSOA() {
		this->UpdateSOA(0, this->m_settings.m_numberOfObjects);
	}
	// Updates a specific portion of the SoA storage
	void UpdateSOA(int start, int end) {
		for (size_t i = start; i < end; i++) {
			m_min[0][i] = this->m_objects[i].m_aabb.m_min[0];
			m_min[1][i] = this->m_objects[i].m_aabb.m_min[1];
			m_min[2][i] = this->m_objects[i].m_aabb.m_min[2];
			m_max[0][i] = this->m_objects[i].m_aabb.m_max[0];
			m_max[1][i] = this->m_objects[i].m_aabb.m_max[1];
			m_max[2][i] = this->m_objects[i].m_aabb.m_max[2];
		}
	}

	// Fetches one object from the SoA array
	__forceinline SimdObject LoadObject(const size_t i) {
		return {
			FloatToSIMD(m_min[0][i]),
			FloatToSIMD(m_min[1][i]),
			FloatToSIMD(m_min[2][i]),
			FloatToSIMD(m_max[0][i]),
			FloatToSIMD(m_max[1][i]),
			FloatToSIMD(m_max[2][i])
		};
	}
};

