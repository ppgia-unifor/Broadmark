#pragma once



#include "Broadphase\OverlapCache.h"
#include "Broadphase\ObjectPair.h"
#include <vector>
#include <algorithm>
#include <unordered_set>


class SimpleCache : public OverlapCache {
public:
	std::vector<ObjectPair> m_overlaps;
	bool m_allowsDuplicates = false;

public:
	virtual ~SimpleCache() = default;

	bool HasPair(const Object* a, const Object* b) override {
		return std::find(m_overlaps.begin(), m_overlaps.end(), ObjectPair(a, b)) != m_overlaps.end();
	}

	void AddPair(const ObjectPair& p) override {
		ASSERT(p.Validate());
		m_overlaps.push_back(p);
	}
	void AddPair(const Object* a, const Object* b) override {
		ASSERT(a->m_id != b->m_id);
		ASSERT(Aabb::Test(a->m_aabb, b->m_aabb));
		m_overlaps.emplace_back(a, b);
	}

	void RemovePair(const Object* a, const Object* b) override {
		ASSERT(a->m_id != b->m_id);
		ASSERT(!Aabb::Test(a->m_aabb, b->m_aabb));
		const auto it = std::remove(m_overlaps.begin(), m_overlaps.end(), ObjectPair(a, b));
		ASSERT(it != m_overlaps.end());
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

		// Looking for duplicates
		if (!m_allowsDuplicates) {
			std::unordered_set<ObjectPair> uniquePairs;
			for (const ObjectPair& pair : m_overlaps) {
				if (uniquePairs.find(pair) == uniquePairs.end()) {
					uniquePairs.insert(pair);
				}
				else {
					ASSERT(0);
				}
			}
		}
		#endif
	}
};
