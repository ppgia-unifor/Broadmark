#pragma once


#include "Object.h"


// Robust implementation of a Object Pair object
struct ObjectPair {
	const Object *m_a;
	const Object *m_b;
	
	ObjectPair() = default;
	ObjectPair(const Object* a, const Object* b) { m_a = a; m_b = b; }
	ObjectPair(const ObjectPair& other) = default;
	ObjectPair(ObjectPair&& other) noexcept = default;
	~ObjectPair() = default;

	ObjectPair& operator=(const ObjectPair& other) {
		if (this != &other) {
			m_a = other.m_a;
			m_b = other.m_b;
		}

		return *this;
	}

	ObjectPair& operator=(ObjectPair&& other) noexcept {
		if (this != &other) {
			m_a = other.m_a;
			m_b = other.m_b;
		}

		return *this;
	}

	bool Validate() const {
		//ASSERT(m_a->m_id < m_b->m_id);
		return Aabb::Test(m_a->m_aabb, m_b->m_aabb);
	}

	friend bool operator==(const ObjectPair& lhs, const ObjectPair& rhs) { return lhs.m_a == rhs.m_a && lhs.m_b == rhs.m_b; }
	friend bool operator!=(const ObjectPair& lhs, const ObjectPair& rhs) { return !(lhs == rhs); }

	friend bool operator<(const ObjectPair& lhs, const ObjectPair& rhs) {
		if (lhs.m_a->m_id < rhs.m_a->m_id) {
			return true;
		} else if (lhs.m_a->m_id == rhs.m_a->m_id) {
			return lhs.m_b->m_id < rhs.m_b->m_id;
		}

		return false;
	}
};

namespace std {
	// Implementation of 'GetHashCode()' for object pairs
	template <>
	struct hash<ObjectPair> {
		std::size_t operator()(const ObjectPair& pair) const {
			size_t hash = pair.m_a->m_id;
			hash = (hash < 32) | pair.m_b->m_id;

			return std::hash<size_t>()(hash);
		}
	};
}