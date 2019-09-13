#pragma once



#include "Macros.h"
#include "Core/Math/Axis.h"
#include "Dependencies/json.hpp"
#include <xmmintrin.h>
#include <immintrin.h>
#undef min
#undef max

// Minimalistic Vector 3 implementation
StructAlignment(16)
struct Vec3 {
public:
	float m_data[4]{};

	Vec3() : m_data{ 0, 0, 0, 0 } { }
	template<typename Number>
	Vec3(const Number x, const Number y, const Number z) {
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
		m_data[3] = 0;
	}

	float& operator[](const int i) {
		return m_data[i];
	}
	const float& operator[](const int i) const {
		return m_data[i];
	}
	float& operator[](const Axis a) {
		return m_data[(const int)a];
	}
	const float& operator[](const Axis a) const {
		return m_data[(const int)a];
	}

	friend bool operator==(const Vec3& lhs, const Vec3& rhs) { return lhs.m_data[0] == rhs.m_data[0] && lhs.m_data[1] == rhs.m_data[1] && lhs.m_data[2] == rhs.m_data[2]; }
	friend bool operator!=(const Vec3& lhs, const Vec3& rhs) { return !(lhs == rhs); }

	friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs) { return {lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]}; }
	friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs) { return {lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]}; }
	friend Vec3 operator*(const Vec3& lhs, const Vec3& rhs) { return {lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]}; }
	friend Vec3 operator/(const Vec3& lhs, const Vec3& rhs) { return {lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2]}; }

	template<typename Number>
	friend Vec3 operator*(const Vec3& lhs, const Number rhs) { return {lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs}; }
	template<typename Number>
	friend Vec3 operator/(const Vec3& lhs, const Number rhs) { return {lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs}; }

	friend bool operator<(const Vec3& lhs, const Vec3& rhs) { return lhs[0] < rhs[0] && lhs[1] < rhs[1] && lhs[2] < rhs[2]; }
	friend bool operator<=(const Vec3& lhs, const Vec3& rhs) { return lhs[0] <= rhs[0] && lhs[1] <= rhs[1] && lhs[2] <= rhs[2]; }
	friend bool operator>(const Vec3& lhs, const Vec3& rhs) { return lhs[0] > rhs[0] && lhs[1] > rhs[1] && lhs[2] > rhs[2]; }
	friend bool operator>=(const Vec3& lhs, const Vec3& rhs) { return lhs[0] >= rhs[0] && lhs[1] >= rhs[1] && lhs[2] >= rhs[2]; }

	Vec3 Clamp01() const {
		return {
			std::clamp(m_data[0], 0.0f, 1.0f),
			std::clamp(m_data[1], 0.0f, 1.0f),
			std::clamp(m_data[2], 0.0f, 1.0f)
		};
	}

	static Vec3 Min(const Vec3& lhs, const Vec3& rhs) {
		return {
			std::min(lhs[0], rhs[0]),
			std::min(lhs[1], rhs[1]),
			std::min(lhs[2], rhs[2])
		};
	}
	static Vec3 Max(const Vec3& lhs, const Vec3& rhs) {
		return {
			std::max(lhs[0], rhs[0]),
			std::max(lhs[1], rhs[1]),
			std::max(lhs[2], rhs[2])
		};
	}
};

using nlohmann::json;
void to_json(json& j, const Vec3& v);
void from_json(const json& j, Vec3& v);