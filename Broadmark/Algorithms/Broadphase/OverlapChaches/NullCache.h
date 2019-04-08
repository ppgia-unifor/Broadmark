#pragma once



#include "Broadphase\OverlapCache.h"
#include "Broadphase\ObjectPair.h"
#include <vector>
#include <algorithm>
#include <unordered_set>


class NullCache : public OverlapCache {
public:
	int m_numOfOverlaps;

public:
	virtual ~NullCache() = default;

	bool HasPair(const Object* a, const Object* b) override { return false; }

	void AddPair(const ObjectPair& p) override { }

	void AddPair(const Object* a, const Object* b) override { }

	void RemovePair(const Object* a, const Object* b) override { }

	size_t Size() const override {
		return m_numOfOverlaps;
	}
	void Clean() override {
		m_numOfOverlaps = 0;
	}

	void Validate() const override {
		ASSERT(m_numOfOverlaps >= 0);
	}
};
