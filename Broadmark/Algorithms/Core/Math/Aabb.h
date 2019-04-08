

#pragma once

#include "Vec3.h"
#include "SIMD.h"


struct Aabb {
	Vec3 m_min;
	Vec3 m_max;

	Aabb() = default;
	Aabb(const Vec3& min, const Vec3& max) :
		m_min(min), m_max(max) {}

	Vec3 Center() const {
		return (m_min + m_max) / 2.0f;
	}
	Vec3 Size() const {
		return m_max - m_min;
	}
	void Grow(const Vec3& amount) {
		m_min = m_min - amount;
		m_max = m_max + amount;
	}
	bool Validate() const {
		return m_min <= m_max && m_min[3] == 0.0f && m_max[3] == 0.0f;
	}

	template<typename tA, typename tB>
	static bool Test(tA a, tB b) {
		return Test(a->m_aabb, b->m_aabb);
	}
	static bool Test(const Aabb& a, const Aabb& b) {
		return
			a.m_max[0] >= b.m_min[0] && a.m_min[0] <= b.m_max[0] &&
			a.m_max[1] >= b.m_min[1] && a.m_min[1] <= b.m_max[1] &&
			a.m_max[2] >= b.m_min[2] && a.m_min[2] <= b.m_max[2];
	}
	static bool Test(const SSEObject& a, const SSEObject& b, int results[4]) {
		__m128 p[3], q[3];
		for (size_t i = 0; i < 3; i++) {
			p[i] = _mm_max_ps(a[i], b[i]);
			q[i] = _mm_min_ps(a[i + 3], b[i + 3]);
		}

		__m128 t[3];
		for (size_t i = 0; i < 3; i++) {
			t[i] = _mm_cmple_ps(p[i], q[i]);
		}

		__m128 overlaps = _mm_and_ps(_mm_and_ps(t[0], t[1]), t[2]);
		if (_mm_testz_ps(overlaps, overlaps)) {
			return false;
		} else {
			// store results in main memory
			_mm_store_ps((float*)results, overlaps);
			return true;
		}
	}
	static bool Test(const AVXObject& a, const AVXObject& b, int results[8]) {
		__m256 p[3], q[3];
		for (size_t i = 0; i < 3; i++) {
			p[i] = _mm256_max_ps(a[i], b[i]);
			q[i] = _mm256_min_ps(a[i + 3], b[i + 3]);
		}

		__m256 t[3];
		for (size_t i = 0; i < 3; i++) {
			t[i] = _mm256_cmp_ps(p[i], q[i], _CMP_LE_OS);
		}

		__m256 overlaps = _mm256_and_ps(_mm256_and_ps(t[0], t[1]), t[2]);
		if (_mm256_testz_ps(overlaps, overlaps)) {
			return false;
		} else {
			// store results in main memory
			_mm256_store_ps((float*)results, overlaps);
			return true;
		}
	}

	static bool Inside(const Aabb& a, const Aabb& b) {
		return
			a.m_min[0] < b.m_min[0] &&
			a.m_min[1] < b.m_min[1] &&
			a.m_min[2] < b.m_min[2] &&
			b.m_max[0] < a.m_max[0] &&
			b.m_max[1] < a.m_max[1] &&
			b.m_max[2] < a.m_max[2];
	}
	template<typename tA, typename tB>
	static bool Inside(tA a, tB b) {
		return Aabb::Inside(a->m_aabb, b->m_aabb);
	}
};