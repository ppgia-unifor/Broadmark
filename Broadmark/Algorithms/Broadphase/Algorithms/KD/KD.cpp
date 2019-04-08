

#include "KD.h"


KD::~KD() {
	_aligned_free(m_objectsArray);
	_aligned_free(m_buffer);
	m_nodePool.dispose();
}

void KD::Initialize(InflatedSettings settings, const SceneFrame& frameData) {
	BaseAlgorithm<KD_Object, SimpleCache>::Initialize(settings, frameData);

	m_objectsArray = (KD_Object**)_aligned_malloc(sizeof(KD_Object*) * m_settings.m_numberOfObjects, 16);

	m_bufferCapacity = (size_t)(m_settings.m_numberOfObjects * 2);
	m_buffer = (KD_Object**)_aligned_malloc(sizeof(KD_Object*) * m_bufferCapacity, 16);
	m_incremental = false;

	m_nodePool.initialize((int)pow(2, 20)); // se passar disso viu...
	
	const PoolHandle handle = m_nodePool.alloc();
	m_root = new(m_nodePool.get(handle)) KD_Node();

	m_root->m_objectsStart = 0;
	m_root->m_objectsEnd = m_settings.m_numberOfObjects;
	m_root->m_left = nullptr;
	m_root->m_right = nullptr;
	m_root->m_aabb.m_min = { -1e30f, -1e30f, -1e30f };
	m_root->m_aabb.m_max = { +1e30f, +1e30f, +1e30f };

	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		m_objectsArray[i] = &m_objects[i];
		m_objectsArray[i]->m_node = m_root;
		m_objectsArray[i]->m_aabb.Grow(m_settings.m_margin);
	}
}

void KD::UpdateObjects(const SceneFrame& frameData) {
	m_staticObjects = 0;
	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		if (Aabb::Inside(m_objects[i].m_aabb, frameData.m_aabbs[i])) {
			m_objects[i].m_static = true;
			m_staticObjects++;
		} else {
			m_objects[i].m_aabb = frameData.m_aabbs[i];
			m_objects[i].m_aabb.Grow(m_settings.m_margin);
			m_objects[i].m_static = false;
		}
	}

	m_incremental = m_staticObjects > m_settings.m_numberOfObjects * 0.5f;
	std::cout << m_staticObjects;
}

void KD::CleanCache() {
	if (!m_incremental) {
		m_cache.Clean();
	} else if (m_staticObjects < m_settings.m_numberOfObjects) {
		auto& pairs = m_cache.m_overlaps;
		for (ObjectPair& p : pairs) {
			const KD_Object* a = (const KD_Object*)p.m_a;
			const KD_Object* b = (const KD_Object*)p.m_b;

			// removes pairs that do not collide or that have dynamic objects
			if (!p.Validate() || !a->m_static || !b->m_static) {
				p.m_a = nullptr;
			}
		}

		auto p = std::partition(pairs.begin(), pairs.end(), [](const ObjectPair& p) { return p.m_a != nullptr; });
		pairs.resize(pairs.size() - std::distance(p, pairs.end()));
	}
}

void KD::UpdateStructures() {
	if (m_incremental && m_staticObjects == m_settings.m_numberOfObjects) {
		return;
	}

	#pragma region Refit
	// fill the stack with the entire tree
	Stack_Push(m_root);
	for (size_t i = 0; i < Stack_Count(); i++) {
		KD_Node *node = Stack_Get(i);

		node->m_appliedDelayedOffset = false;
		if (!node->isLeaf()) {
			node->setChildreenAABBs();
			Stack_Push(node->m_right);
			Stack_Push(node->m_left);
		}
	}

	for (int i = Stack_Count() - 1; i > 0; i--) {
		KD_Node *node = Stack_Get(i);

		node->m_delayedOffset = 0;
		if (node->count() == 0) {
			continue;
		}

		const size_t oldStart = node->m_objectsStart;
		const size_t oldEnd = node->m_objectsEnd;
		if (node->isLeaf()) {
			if (node->isLeft()) {
				refitToEnd(node);

				int transfered = oldEnd - node->m_objectsEnd;
				if (transfered > 0) {
					node->m_parent->m_objectsStart -= transfered;
				} else {
					ASSERT(transfered == 0);
				}
			} else { // se for right
				refitToStart(node);

				int transfered = node->m_objectsStart - oldStart;
				if (transfered > 0) {
					node->m_parent->m_objectsEnd += transfered;
				} else {
					ASSERT(transfered == 0);
				}
			}

			node->m_population = node->count();
		} else { // se n�o for leaf
			if (node->isLeft()) {
				refitToEnd(node);

				int transfered = oldEnd - node->m_objectsEnd;
				if (transfered > 0) {
					const size_t copyPoint = node->m_objectsEnd;
					const size_t subtreeSize = node->m_parent->m_objectsStart - oldEnd;

					memoryTransfer_LeftToRight(copyPoint, transfered, subtreeSize);

					node->m_parent->m_objectsStart -= transfered;
					node->m_right->m_delayedOffset = -transfered;
				} else {
					ASSERT(transfered == 0);
				}
			} else { // if right
				refitToStart(node);

				int transfered = node->m_objectsStart - oldStart;
				if (transfered > 0) {
					const size_t copyPoint = node->m_parent->m_objectsEnd;
					const size_t subtreeSize = oldStart - copyPoint;

					memoryTransfer_RightToLeft(copyPoint, transfered, subtreeSize);

					node->m_parent->m_objectsEnd += transfered;
					node->m_left->m_delayedOffset = transfered;
				} else {
					ASSERT(transfered == 0);
				}
			}

			node->m_population = node->count();
			node->m_population += node->m_left->m_population;
			node->m_population += node->m_right->m_population;
		}
	}

	m_root->m_population = m_root->count();
	if (!m_root->isLeaf()) {
		m_root->m_population += m_root->m_left->m_population;
		m_root->m_population += m_root->m_right->m_population;
	}

	Stack_Clear();
	#pragma endregion

	#pragma region Update the tree and objects
	ASSERT(Stack_Count() == 0);

	Stack_Push(m_root);
	while (Stack_Count() != 0) {
		KD_Node *node = Stack_Pop();

		if (!node->isLeaf()) {
			if (!node->m_appliedDelayedOffset) {
				node->m_left->m_delayedOffset += node->m_delayedOffset;
				node->m_right->m_delayedOffset += node->m_delayedOffset;

				node->m_left->m_objectsStart += node->m_left->m_delayedOffset;
				node->m_left->m_objectsEnd += node->m_left->m_delayedOffset;
				node->m_right->m_objectsStart += node->m_right->m_delayedOffset;
				node->m_right->m_objectsEnd += node->m_right->m_delayedOffset;

				node->m_appliedDelayedOffset = true;
			}

			WaterfallNode(node);

			if (Evaluate_Trim(node)) {
				Trim(node);

				Stack_Push(node);
			} else if (Evaluate_Heuristic(node)) {
				if (!node->m_translated) {
					LiftSubtree(node);
					Translate(node);

					ASSERT(node->m_aabb.m_min[node->m_plane.m_axis] < node->m_plane.m_position);
					ASSERT(node->m_aabb.m_max[node->m_plane.m_axis] > node->m_plane.m_position);

					node->m_translated = true;
					Stack_Push(node);
				} else {
					node = Erase(node);
					node->m_translated = false;
					Stack_Push(node);
				}
			} else {
				node->setChildreenAABBs();
				node->m_translated = false;

				Stack_Push(node->m_right);
				Stack_Push(node->m_left);
			}
		} else {
			if (Evaluate_Partitionate(node)) {
				Partitionate(node);
				WaterfallNode(node);

				Stack_Push(node->m_right);
				Stack_Push(node->m_left);
			} else {
				FinishLeaf(node);
			}
		}
	}
	#pragma endregion

	#pragma region Compute Statics Population for each node
	if (m_staticObjects != m_settings.m_numberOfObjects) {
		// fill the stack with the entire tree
		Stack_Push(m_root);
		for (size_t i = 0; i < Stack_Count(); i++) {
			const KD_Node* node = Stack_Get(i);

			if (!node->isLeaf()) {
				Stack_Push(node->m_right);
				Stack_Push(node->m_left);
			}
		}

		for (int i = Stack_Count() - 1; i >= 0; i--) {
			KD_Node* node = Stack_Get(i);

			size_t statics = 0;
			for (size_t i = node->m_objectsStart; i < node->m_objectsEnd; i++) { if (m_objectsArray[i]->m_static) { statics++; } }
			node->m_statics = statics;
			node->m_staticPopulation = node->m_statics;

			if (!node->isLeaf()) {
				node->m_staticPopulation += node->m_left->m_staticPopulation;
				node->m_staticPopulation += node->m_right->m_staticPopulation;
			}
		}
		Stack_Clear();
	}
	#pragma endregion
}
