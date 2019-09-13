#pragma once



#include "Broadphase\OverlapCache.h"
#include "Broadphase\ObjectPair.h"
#include <vector>
#include <algorithm>
#include <unordered_set>


// Pair cache implementation based on hashing.
// Pairs are stored in random order, as defined by the std::unordered_set
// This implementation is useful for algorithms that require checking if a
// pair is already present on the cache
class HashedCache : public OverlapCache {
private:
	std::unordered_set<ObjectPair> m_overlaps;

public:
	virtual ~HashedCache() = default;

	bool HasPair(const Object* a, const Object* b) override {
		return std::find(m_overlaps.begin(), m_overlaps.end(), ObjectPair(a, b)) != m_overlaps.end();
	}

	void AddPair(const ObjectPair& p) override {
		ASSERT(p.Validate());
		m_overlaps.insert(p);
	}
	void AddPair(const Object* a, const Object* b) override {
		ASSERT(a->m_id != b->m_id);
		ASSERT(Aabb::Test(a->m_aabb, b->m_aabb));

		if (a->m_id < b->m_id) {
			m_overlaps.insert(ObjectPair(a, b));
		} else {
			m_overlaps.insert(ObjectPair(b, a));
		}
	}

	void RemovePair(const Object* a, const Object* b) override {
		ASSERT(a->m_id != b->m_id);
		ASSERT(!Aabb::Test(a->m_aabb, b->m_aabb));
		if (a->m_id < b->m_id) {
			m_overlaps.erase(ObjectPair(a, b));
		} else {
			m_overlaps.erase(ObjectPair(b, a));
		}
	}

	size_t Size() const override {
		return m_overlaps.size();
	}
	void Clean() override {
		m_overlaps.clear();
	}

	void Validate() const override {
		#ifdef DEBUG
		// Looking for invalid entries
		for (const ObjectPair& pair : m_overlaps) {
			ASSERT(pair.Validate());
		}
		#endif
	}
};
