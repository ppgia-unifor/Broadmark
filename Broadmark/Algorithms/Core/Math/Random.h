

#pragma once


#include "Configs.h"
#include <random>
#include <functional>
#include "Core/Math/Vec3.h"


class Random {
private:
	static std::uniform_real_distribution<float> real_distribution;
	static std::uniform_int_distribution<int> int_distribution;
	static std::mt19937 engine;

public:
	inline static void Init() {
		srand(RANDOM_SEED);
		real_distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
		int_distribution = std::uniform_int_distribution<int>(INT_MIN, INT_MAX);
		engine = std::mt19937(RANDOM_SEED);
	}


	inline static int Natural(int min, int max) {
		ASSERT(min < max);
		int val = int_distribution(engine);
		val %= (max - min);
		val += min;

		return val;
	}
	inline static float Value(float min, float max) {
		ASSERT(min < max);
		float val = real_distribution(engine);
		val *= (max - min);
		val += min;

		return val;
	}
	inline static Vec3 PositionInsideAABB(const Vec3& aabbMin, const Vec3& aabbMax) {
		ASSERT(aabbMin < aabbMax);
		return {
			Random::Value(aabbMin[0], aabbMax[0]),
			Random::Value(aabbMin[1], aabbMax[1]),
			Random::Value(aabbMin[2], aabbMax[2])
		};
	}
	inline static Vec3 PositionInsideCube(float min, float max) {
		ASSERT(min < max);
		return {
			Random::Value(min, max),
			Random::Value(min, max),
			Random::Value(min, max)
		};
	}

	inline static void FillArray(float* array, size_t count, float min, float max) {
		ASSERT(min < max);
		for (size_t i = 0; i < count; i++) {
			array[i] = Random::Value(min, max);
		}
	}
};
