

#pragma once

#include "Aabb.h"
#include "Axis.h"
#include "Macros.h"
#include <future>
#include "Broadphase/Object.h"


// Implementation of a 'Plane' object. Used extensively for the KD-Tree implementation
struct Plane {
	Axis m_axis;
	float m_position;

	Plane(): m_axis(), m_position(0) {}
	Plane(Axis axis, float position) :
		m_axis(axis), m_position(position) {}

	inline void split(const Aabb& aabb, Aabb& left, Aabb& right) {
		ASSERT(aabb.m_min[m_axis] < m_position);
		ASSERT(aabb.m_max[m_axis] > m_position);

		left = aabb;
		right = aabb;

		left.m_max[m_axis] = m_position;
		right.m_min[m_axis] = m_position;
	}

	inline bool isLeft(const Vec3& min) const {
		return (min[m_axis] < m_position);
	}
	inline bool isRight(const Vec3& max) const {
		return (max[m_axis] > m_position);
	}
	inline bool isStrictlyLeft(const Vec3& max) const {
		return (max[m_axis] < m_position);
	}
	inline bool isStrictlyRight(const Vec3& min) const {
		return (min[m_axis] > m_position);
	}

	template<typename Object_Type>
	static Plane FindBestPlane(Object_Type* objects, int n) {
		__m256d mean = _mm256_setzero_pd();
		__m256d variance = _mm256_setzero_pd();
		const __m256d half = _mm256_set1_pd(0.5);
		const __m256d inverseN = _mm256_set1_pd(1.0 / (double)n);

		for (size_t i = 0; i < n; i++) {
			Object_Type& proxy = objects[i];

			__m256d center = _mm256_add_pd(
				_mm256_cvtps_pd(_mm_load_ps(proxy.m_aabb.m_min.m_data)),
				_mm256_cvtps_pd(_mm_load_ps(proxy.m_aabb.m_max.m_data)));
			center = _mm256_mul_pd(center, half);
			mean = _mm256_add_pd(mean, center);
			variance = _mm256_fmadd_pd(center, center, variance);
		}

		variance = _mm256_sub_pd(variance, _mm256_mul_pd(_mm256_mul_pd(mean, mean), inverseN));
		variance = _mm256_mul_pd(variance, inverseN);
		mean = _mm256_mul_pd(mean, inverseN);

		Vec3 m_;
		Vec3 v_;
		_mm_store_ps(m_.m_data, _mm256_cvtpd_ps(mean));
		_mm_store_ps(v_.m_data, _mm256_cvtpd_ps(variance));

		return findPlaneUsingMeanAndVariance(m_, v_);
	}
	template<typename Object_Type>
	static Plane FindBestPlane(Object_Type** objects, int n) {
		__m256d mean = _mm256_setzero_pd();
		__m256d variance = _mm256_setzero_pd();
		const __m256d half = _mm256_set1_pd(0.5);
		const __m256d inverseN = _mm256_set1_pd(1.0 / (double)n);

		for (size_t i = 0; i < n; i++) {
			Object_Type& proxy = *objects[i];

			__m256d center = _mm256_add_pd(
				_mm256_cvtps_pd(_mm_load_ps(proxy.m_aabb.m_min.m_data)),
				_mm256_cvtps_pd(_mm_load_ps(proxy.m_aabb.m_max.m_data)));
			center = _mm256_mul_pd(center, half);
			mean = _mm256_add_pd(mean, center);
			variance = _mm256_fmadd_pd(center, center, variance);
		}

		variance = _mm256_sub_pd(variance, _mm256_mul_pd(_mm256_mul_pd(mean, mean), inverseN));
		variance = _mm256_mul_pd(variance, inverseN);
		mean = _mm256_mul_pd(mean, inverseN);

		Vec3 m_;
		Vec3 v_;
		_mm_store_ps(m_.m_data, _mm256_cvtpd_ps(mean));
		_mm_store_ps(v_.m_data, _mm256_cvtpd_ps(variance));

		return findPlaneUsingMeanAndVariance(m_, v_);
	}
	static Plane findPlaneUsingVolume(const Vec3& min, const Vec3& max) {
		Vec3 extents = max - min;
		Vec3 center = (extents * 0.5f) + min;

		return findPlaneUsingMeanAndVariance(center, extents);
	}
	static Plane findPlaneUsingMeanAndVariance(Vec3& mean, Vec3& variance) {
		Axis choosenAxis;
		if (variance[0] > variance[1]) {
			if (variance[0] > variance[2]) {
				choosenAxis = Axis::X;
			} else {
				choosenAxis = Axis::Z;
			}
		} else {
			if (variance[1] > variance[2]) {
				choosenAxis = Axis::Y;
			} else {
				choosenAxis = Axis::Z;
			}
		}

		return Plane(choosenAxis, mean[choosenAxis]);
	}
};
