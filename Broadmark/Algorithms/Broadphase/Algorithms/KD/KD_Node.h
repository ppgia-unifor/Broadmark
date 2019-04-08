

#pragma once


#include "Broadphase/Object.h"
#include "Core/Math/Plane.h"


class KD_Node {
public:
	KD_Node *m_parent = nullptr;
	KD_Node *m_left = nullptr;
	KD_Node *m_right = nullptr;

	Aabb m_aabb;
	Plane m_plane;
	size_t m_objectsStart;
	size_t m_objectsEnd;
	size_t m_collisionStart;
	size_t m_collisionEnd;

	size_t m_statics;
	size_t m_staticPopulation;
	size_t m_population;

	Axis m_sapAxis;

	int m_delayedOffset;
	bool m_translated;
	bool m_appliedDelayedOffset;

	size_t count() const {
		return m_objectsEnd - m_objectsStart;
	}

	size_t colCount() const {
		return m_collisionEnd - m_collisionStart;
	}

	bool isRoot() const { return m_parent == nullptr; }
	bool isLeaf() const { return m_left == nullptr; }
	bool isLeft() const { return m_parent->m_left == this; }
	bool isRight() const { return m_parent->m_right == this; }

	void setChildreenAABBs() const {
		m_left->m_aabb.m_min = m_aabb.m_min;
		m_left->m_aabb.m_max = m_aabb.m_max;
		m_right->m_aabb.m_min = m_aabb.m_min;
		m_right->m_aabb.m_max = m_aabb.m_max;

		m_left->m_aabb.m_max[m_plane.m_axis] = m_plane.m_position;
		m_right->m_aabb.m_min[m_plane.m_axis] = m_plane.m_position;
	}

	void ValidateTree() {
		// Searching corruption
		if (m_parent != nullptr) {
			ASSERT(m_parent != m_left);
			ASSERT(m_parent != m_right);
		}

		if (!isLeaf()) {
			ASSERT(m_left != m_right);
		}

		ASSERT(m_objectsStart <= m_objectsEnd);
		ASSERT(m_collisionStart <= m_collisionEnd);
		ASSERT(m_aabb.Validate());

		// Is plane within AABB?
		if (!isLeaf()) {
			ASSERT(m_aabb.m_min[m_plane.m_axis] < m_plane.m_position);
			ASSERT(m_aabb.m_max[m_plane.m_axis] > m_plane.m_position);
		}

		// The node's AABB must be a part of its parent's AABB
		if (m_parent != nullptr) {
			Axis axis = m_parent->m_plane.m_axis;
			for (int i = 0; i < 3; i++) {
				if (i == (int)axis) {
					if (isLeft()) {
						ASSERT(m_aabb.m_min[i] == m_parent->m_aabb.m_min[i]);
						ASSERT(m_aabb.m_max[i] == m_parent->m_plane.m_position);
					} else {
						ASSERT(m_aabb.m_min[i] == m_parent->m_plane.m_position);
						ASSERT(m_aabb.m_max[i] == m_parent->m_aabb.m_max[i]);
					}
				} else {
					ASSERT(m_aabb.m_min[i] == m_parent->m_aabb.m_min[i]);
					ASSERT(m_aabb.m_max[i] == m_parent->m_aabb.m_max[i]);
				}
			}
		}

		// Validate children
		if (m_left != nullptr) { m_left->ValidateTree(); }
		if (m_right != nullptr) { m_right->ValidateTree(); }
	}
};