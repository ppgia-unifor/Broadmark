


#include "KD.h"


void KD::SearchOverlaps() {
	if (m_incremental) {
		if (m_staticObjects == m_settings.m_numberOfObjects) {
			return;
		}
	}


	#pragma region Root
	KD_Node* node = m_root;
	node->m_collisionStart = node->m_collisionEnd = 0;
	if (node->count() > 0) {
		sort_Node_Objects(node);
		sap_NodeNode(node, node->m_sapAxis);
	}

	if (!node->isLeaf()) {
		Stack_Push(node->m_right);
		Stack_Push(node->m_left);
	}
	#pragma endregion

	while (Stack_Count() != 0) {
		node = Stack_Pop();

		size_t collisionHead = node->m_parent->m_collisionEnd;
		node->m_collisionStart = collisionHead;
		Plane plane = node->m_parent->m_plane;

		size_t capacityNeeded = collisionHead;
		capacityNeeded += node->m_parent->colCount();
		monotonicResize(capacityNeeded);

		#pragma region Father Collision List
		size_t start = node->m_parent->m_collisionStart;
		size_t end = node->m_parent->m_collisionEnd;

		if (node->isLeft()) { // Se for um n� Left
			for (size_t j = start; j < end; j++) {
				KD_Object* proxy = m_collisionList[j];

				if (plane.isLeft(proxy->m_aabb.m_min)) {
					m_collisionList[collisionHead] = proxy;
					collisionHead++;
				}
			}
		} else { // Se for um n� Right
			for (size_t j = start; j < end; j++) {
				KD_Object* proxy = m_collisionList[j];

				if (plane.isRight(proxy->m_aabb.m_max)) {
					m_collisionList[collisionHead] = proxy;
					collisionHead++;
				}
			}
		}
		#pragma endregion

		capacityNeeded = collisionHead;
		capacityNeeded += node->m_parent->count();
		monotonicResize(capacityNeeded);

		#pragma region Father Objects

		start = node->m_parent->m_objectsStart;
		end = node->m_parent->m_objectsEnd;

		memcpy(
			&m_collisionList[collisionHead],
			m_objectsArray + start,
			(end - start) * sizeof(KD_Object*));

		collisionHead += end - start;

		#pragma endregion

		node->m_collisionEnd = collisionHead;
		#pragma region Avoid traversing over static subtrees
		if (!node->isLeaf()) {
			if (m_incremental) {
				if (node->m_staticPopulation == node->m_population) {
					size_t collist_statics = 0;
					for (size_t i = node->m_collisionStart; i < node->m_collisionEnd; i++) {
						if (m_collisionList[i]->m_static) {
							collist_statics++;
						}
					}

					if (collist_statics != node->colCount()) {
						Stack_Push(node->m_right);
						Stack_Push(node->m_left);
					} else {
						continue;
					}
				} else {
					Stack_Push(node->m_right);
					Stack_Push(node->m_left);
				}
			} else {
				Stack_Push(node->m_right);
				Stack_Push(node->m_left);
			}
		}
		#pragma endregion


		if (node->count() == 0) {
			continue;
		}

		sort_Node_Objects(node);
		sort_Node_CollisionList(node);


		#ifdef DEBUG_MODE
		Validate_SortedCollisionListSection(node);
		#endif

		sap_NodeNode(node, node->m_sapAxis);
		sap_NodeParentCollision(node, node->m_sapAxis);
	}
}

void KD::sort_Node_Objects(KD_Node* n) const {
	const Axis axis = FindBestPlane(n).m_axis;
	std::sort(
		m_objectsArray + n->m_objectsStart,
		m_objectsArray + n->m_objectsEnd,
		ObjectPointerComparer<KD_Object*>(axis));
	n->m_sapAxis = axis;
}

void KD::sort_Node_CollisionList(KD_Node* n) const {
	if (n->m_collisionStart == n->m_collisionEnd) {
		return;
	}
	ASSERT(n->m_collisionEnd <= m_bufferCapacity);

	std::sort(m_collisionList + n->m_collisionStart,
		m_collisionList + n->m_collisionEnd,
		ObjectPointerComparer<KD_Object*>(n->m_sapAxis));
}

void KD::sap_NodeNode(KD_Node* n, Axis axis) {
	if (m_incremental && n->m_statics == n->count()) { return; }

	for (size_t i = n->m_objectsStart; i < n->m_objectsEnd - 1; i++) {
		KD_Object* p1 = m_objectsArray[i];

		sap_objectArray_avx(p1, &m_objectsArray[i + 1], n->m_objectsEnd - (i + 1), axis);
	}
}

void KD::sap_NodeParentCollision(KD_Node* n, Axis axis) {
	if (n->colCount() == 0) {
		return;
	}
	ASSERT(n->m_collisionEnd <= m_bufferCapacity);


	size_t c1 = n->m_objectsStart;
	size_t c2 = n->m_collisionStart;
	while (c1 < n->m_objectsEnd && c2 < n->m_collisionEnd) {
		KD_Object* p1 = m_objectsArray[c1];
		KD_Object* p2 = m_collisionList[c2];

		if (p1->m_aabb.m_min[axis] < p2->m_aabb.m_min[axis]) {
			// p1 contra a collision list
			sap_objectArray_avx(p1, &m_collisionList[c2], n->m_collisionEnd - c2, axis);
			c1++;
		} else {
			// p2 contra os objetos
			sap_objectArray_avx(p2, &m_objectsArray[c1], n->m_objectsEnd - c1, axis);
			c2++;
		}
	}
}

void KD::sap_objectArray_avx(KD_Object* p1, KD_Object** array2, int array2_count, Axis axis) {
	ASSERT(p1 != nullptr);
	ASSERT(array2 != nullptr);
	ASSERT(array2[0] != p1);
	ASSERT(array2_count > 0);


	// Evita carregar p1 nos registradores se a condi��o do
	// SAP j� for falsa.
	if (array2[0]->m_aabb.m_min[axis] > p1->m_aabb.m_max[axis]) { return; }
	KD_Object* p2;
	StructAlignment(32) float p2_data_avx[48];
	StructAlignment(32) int p2_indexes_avx[8];
	StructAlignment(32) int overlap_avx[8];

	const __m256 p1_minx = _mm256_set1_ps(p1->m_aabb.m_min[0]);
	const __m256 p1_miny = _mm256_set1_ps(p1->m_aabb.m_min[1]);
	const __m256 p1_minz = _mm256_set1_ps(p1->m_aabb.m_min[2]);
	const __m256 p1_maxx = _mm256_set1_ps(p1->m_aabb.m_max[0]);
	const __m256 p1_maxy = _mm256_set1_ps(p1->m_aabb.m_max[1]);
	const __m256 p1_maxz = _mm256_set1_ps(p1->m_aabb.m_max[2]);


	for (size_t j = 0; j < array2_count;) {
		size_t k = 0; // # de p2s v�lidos encontrados

					  // Preenche os 8 slots com p2s v�lidos
		if (m_incremental && p1->m_static) {
			while (j < array2_count && k < 8) {
				p2 = array2[j];
				if (p1->m_aabb.m_max[axis] >= p2->m_aabb.m_min[axis]) {
					if (!p2->m_static) {
						p2_data_avx[0 + k] = p2->m_aabb.m_min[0]; // min x
						p2_data_avx[8 + k] = p2->m_aabb.m_min[1]; // min y
						p2_data_avx[16 + k] = p2->m_aabb.m_min[2];// min z
						p2_data_avx[24 + k] = p2->m_aabb.m_max[0];// max x
						p2_data_avx[32 + k] = p2->m_aabb.m_max[1];// max y
						p2_data_avx[40 + k] = p2->m_aabb.m_max[2];// max z
						p2_indexes_avx[k] = j;
						k++;
					}
					j++;
				} else { break; }
			}
		} else {
			while (j < array2_count && k < 8) {
				p2 = array2[j];
				if (p1->m_aabb.m_max[axis] >= p2->m_aabb.m_min[axis]) {
					p2_data_avx[0 + k] = p2->m_aabb.m_min[0]; // min x
					p2_data_avx[8 + k] = p2->m_aabb.m_min[1]; // min y
					p2_data_avx[16 + k] = p2->m_aabb.m_min[2];// min z
					p2_data_avx[24 + k] = p2->m_aabb.m_max[0];// max x
					p2_data_avx[32 + k] = p2->m_aabb.m_max[1];// max y
					p2_data_avx[40 + k] = p2->m_aabb.m_max[2];// max z
					p2_indexes_avx[k] = j;
					k++;
					j++;
				} else { break; }
			}
		}

		if (k == 0) { return; }

		for (; k < 8; k++) {
			p2_data_avx[0 + k] = 0.0f;
			p2_data_avx[8 + k] = 0.0f;
			p2_data_avx[16 + k] = 0.0f;
			p2_data_avx[24 + k] = 0.0f;
			p2_data_avx[32 + k] = 0.0f;
			p2_data_avx[40 + k] = 0.0f;
			p2_indexes_avx[k] = -1;
		}

		// P�gina 547 do Realtime Collision Detection
		// Four AABBs against Four AABBs (adaptado pra 8x8)

		// preenche os registradores do AVX
		const __m256 p2_minx = _mm256_load_ps(&p2_data_avx[0]);
		const __m256 p2_miny = _mm256_load_ps(&p2_data_avx[8]);
		const __m256 p2_minz = _mm256_load_ps(&p2_data_avx[16]);
		const __m256 p2_maxx = _mm256_load_ps(&p2_data_avx[24]);
		const __m256 p2_maxy = _mm256_load_ps(&p2_data_avx[32]);
		const __m256 p2_maxz = _mm256_load_ps(&p2_data_avx[40]);

		// calcula a AABB da interse��o
		const __m256 ax = _mm256_max_ps(p1_minx, p2_minx);
		const __m256 bx = _mm256_min_ps(p1_maxx, p2_maxx);
		const __m256 ay = _mm256_max_ps(p1_miny, p2_miny);
		const __m256 by = _mm256_min_ps(p1_maxy, p2_maxy);
		const __m256 az = _mm256_max_ps(p1_minz, p2_minz);
		const __m256 bz = _mm256_min_ps(p1_maxz, p2_maxz);
		// olha se a interse��o � uma AABB v�lida
		// i.e., procura por algum eixo "ao avesso"
		const __m256 t1 = _mm256_cmp_ps(ax, bx, _CMP_LE_OS);
		const __m256 t2 = _mm256_cmp_ps(ay, by, _CMP_LE_OS);
		const __m256 t3 = _mm256_cmp_ps(az, bz, _CMP_LE_OS);
		// se todos t�o OK, t� colidindo.
		__m256 result = _mm256_and_ps(t1, t2);
		result = _mm256_and_ps(result, t3);

		if (_mm256_testz_ps(result, result)) { continue; }

		// Tira o resultado do AVX e p�e em mem�ria normal
		_mm256_store_ps((float*)overlap_avx, result);

		for (k = 0; k < 8; k++) {
			int idx = p2_indexes_avx[k];
			if (idx == -1) { return; }

			if (overlap_avx[k] != 0) {
				p2 = array2[idx];
				m_cache.AddPair(p1, p2);
			}
		}
	}
}