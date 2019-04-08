


#include "KD.h"

void KD::Validate() const {
	#ifdef DEBUG_MODE
	m_root->ValidateTree();
	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		const KD_Object* p = m_objectsArray[i];
		const KD_Node* n = p->m_node;

		ASSERT(n->m_objectsStart <= i);
		ASSERT(i <= n->m_objectsEnd);
		ASSERT(Aabb::Inside(n, p) == true);

		// The object must overlap the node's plane if it is not on a leaf node
		if (!n->isLeaf()) {
			const Plane& plane = n->m_plane;

			ASSERT(p->m_aabb.m_min[plane.m_axis] <= plane.m_position);
			ASSERT(p->m_aabb.m_max[plane.m_axis] >= plane.m_position);
		}
	}

	std::stack<const KD_Node*> stack;
	stack.push(m_root);
	while (!stack.empty()) {
		const KD_Node* n = stack.top();
		stack.pop();
		if (!n->isLeaf()) {
			stack.push(n->m_right);
			stack.push(n->m_left);


			ASSERT(n->m_left->m_objectsEnd <= n->m_objectsStart);
			ASSERT(n->m_objectsEnd <= n->m_right->m_objectsStart);

			if (n->m_left->isLeaf()) {
				ASSERT(n->m_left->m_objectsEnd == n->m_objectsStart);
			}
			if (n->m_right->isLeaf()) {
				ASSERT(n->m_objectsEnd == n->m_right->m_objectsStart);
			}
		}

		ASSERT(n->m_population >= n->count());
		if (n->isLeaf()) {
			ASSERT(n->m_population == n->count());
		} else {
			ASSERT(n->m_population ==
				n->count() +
				n->m_left->m_population +
				n->m_right->m_population);
		}

		if (m_incremental) {
			ASSERT(n->count() >= n->m_statics);
			if (n->isLeaf()) { ASSERT(n->m_staticPopulation == n->m_statics); } else {
				ASSERT(n->m_staticPopulation >= n->m_statics);
				ASSERT(n->m_population >= n->m_staticPopulation);

				ASSERT(n->m_staticPopulation ==
					n->m_statics +
					n->m_left->m_staticPopulation +
					n->m_right->m_staticPopulation);
			}
		}
	}
	#endif
}

void KD::Validate_SortedObjects() const {
	std::stack<const KD_Node*> stack;

	stack.push(m_root);
	while (!stack.empty()) {
		const KD_Node* node = stack.top();
		stack.pop();
		if (!node->isLeaf()) {
			stack.push(node->m_right);
			stack.push(node->m_left);
		}

		if (node->m_objectsEnd == 0) {
			continue; // avoids size_t overflows
		}
		if (node->m_staticPopulation < node->m_population) {
			const Axis axis = node->m_sapAxis;
			for (size_t i = node->m_objectsStart; i < node->m_objectsEnd - 1; i++) {
				ASSERT(m_objectsArray[i]->m_aabb.m_min[axis] <= m_objectsArray[i + 1]->m_aabb.m_min[axis]);
			}
		}
	}
}

void KD::Validate_SortedCollisionListSection(KD_Node* n) const {
	if (n->m_collisionStart == n->m_collisionEnd) { return; }

	for (size_t i = n->m_collisionStart; i < n->m_collisionEnd - 1; i++) {
		const Axis axis = n->m_sapAxis;
		ASSERT(m_collisionList[i]->m_aabb.m_min[axis] <= m_collisionList[i + 1]->m_aabb.m_min[axis]);
	}
}