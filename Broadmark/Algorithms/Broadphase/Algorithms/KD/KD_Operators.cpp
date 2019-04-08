

#include "KD.h"


bool KD::Evaluate_Heuristic(KD_Node *node) const {
	const float pop = (float)node->m_population;
	const float pe = (float)node->m_left->m_population;
	const float pd = (float)node->m_right->m_population;
	const float pc = pop - pe - pd;

	if (pd == 0.0f && pe == 0.0f) {
		return false;
	}

	const float max = (pop * pop - pop) / 2.0f;
	const float min = ((pop / 2.0f) * (pop / 2) - (pop / 2)); // * 2 / 2

	const float eval = (pc * pc + pe * pe + pd * pd - pop) * 0.5f + pc * (pe + pd);
	const float normalized_eval = (eval - min) / (max - min);

	const float balance = pe < pd ? pe / (pd + pc) : pd / (pe + pc);

	return normalized_eval > balance;
}
bool KD::Evaluate_Partitionate(KD_Node *node) const {
	return node->count() > m_settings.m_KD_ObjectsPerLeaf;
}
bool KD::Evaluate_Trim(KD_Node *node) const {
	return
		node->m_population < m_settings.m_KD_ObjectsPerLeaf ||
		node->m_left->m_population == 0 ||
		node->m_right->m_population == 0;
}

void KD::WaterfallNode(KD_Node *node) {
	if (node->count() == 0) {
		return;
	}

	const size_t oldStart = node->m_objectsStart;
	const size_t oldEnd = node->m_objectsEnd;

	organizeSectionAroundPlane(node);

	int transfered = node->m_objectsStart - oldStart;
	if (transfered > 0) {
		const size_t copyPoint = node->m_left->m_objectsEnd;
		const size_t subtreeSize = oldStart - copyPoint;

		memoryTransfer_RightToLeft(copyPoint, transfered, subtreeSize);

		node->m_left->m_population += transfered;
		node->m_left->m_objectsEnd += transfered;
		if (!node->m_left->isLeaf()) {
			node->m_left->m_right->m_delayedOffset += transfered;
		}
	} else {
		ASSERT(transfered == 0);
	}

	transfered = oldEnd - node->m_objectsEnd;
	if (transfered > 0) {
		const size_t copyPoint = node->m_objectsEnd;
		const size_t subtreeSize = node->m_right->m_objectsStart - oldEnd;

		memoryTransfer_LeftToRight(copyPoint, transfered, subtreeSize);

		node->m_right->m_population += transfered;
		node->m_right->m_objectsStart -= transfered;
		if (!node->m_right->isLeaf()) {
			node->m_right->m_left->m_delayedOffset -= transfered;
		}
	} else {
		ASSERT(transfered == 0);
	}

	ASSERT(node->m_population == node->count() + node->m_left->m_population + node->m_right->m_population);
}
void KD::FinishLeaf(KD_Node* leaf) const {
	for (size_t i = leaf->m_objectsStart; i < leaf->m_objectsEnd; i++) {
		KD_Object *p = m_objectsArray[i];

		p->m_node = leaf;
	}
}
void KD::LiftSubtree(KD_Node *subtree) {
	KD_Node *node;

	const size_t start = subtree->m_objectsStart - subtree->m_left->m_population;
	const size_t end = subtree->m_objectsEnd + subtree->m_right->m_population;

	ASSERT(start >= 0);
	ASSERT(end <= m_settings.m_numberOfObjects);

	subtree->m_objectsStart = start;
	subtree->m_objectsEnd = end;
	for (size_t i = start; i < end; i++) {
		KD_Object *p = m_objectsArray[i];

		p->m_node = subtree;
	}


	const size_t breakPoint = Stack_Count();
	Stack_Push(subtree->m_left);
	while (Stack_Count() > breakPoint) {
		node = Stack_Pop();
		if (!node->isLeaf()) {
			Stack_Push(node->m_right);
			Stack_Push(node->m_left);
		}

		node->m_objectsStart = start;
		node->m_objectsEnd = start;
		node->m_delayedOffset = 0;
		node->m_population = 0;
	}
	Stack_Push(subtree->m_right);
	while (Stack_Count() > breakPoint) {
		node = Stack_Pop();
		if (!node->isLeaf()) {
			Stack_Push(node->m_right);
			Stack_Push(node->m_left);
		}

		node->m_objectsStart = end;
		node->m_objectsEnd = end;
		node->m_delayedOffset = 0;
		node->m_population = 0;
	}

	ASSERT(subtree->m_left->m_population == 0);
	ASSERT(subtree->m_right->m_population == 0);
}

void KD::Partitionate(KD_Node *node) {
	ASSERT(node->m_left == nullptr);
	ASSERT(node->m_right == nullptr);

	node->m_plane = FindBestPlane(node);

	node->m_left = new(m_nodePool.get(m_nodePool.alloc())) KD_Node();
	node->m_left->m_parent = node;
	node->m_left->m_objectsStart = node->m_objectsStart;
	node->m_left->m_objectsEnd = node->m_objectsStart;

	node->m_right = new(m_nodePool.get(m_nodePool.alloc())) KD_Node();
	node->m_right->m_parent = node;
	node->m_right->m_objectsStart = node->m_objectsEnd;
	node->m_right->m_objectsEnd = node->m_objectsEnd;

	node->setChildreenAABBs();
}
void KD::Trim(KD_Node* subtree) {

	const size_t start = subtree->m_objectsStart - subtree->m_left->m_population;
	const size_t end = subtree->m_objectsEnd + subtree->m_right->m_population;
	ASSERT(start >= 0);
	ASSERT(end <= m_settings.m_numberOfObjects);

	subtree->m_objectsStart = start;
	subtree->m_objectsEnd = end;

	for (size_t i = start; i < end; i++) {
		KD_Object *p = m_objectsArray[i];

		p->m_node = subtree;
	}


	const size_t breakPoint = Stack_Count();
	Stack_Push(subtree->m_right);
	Stack_Push(subtree->m_left);
	while (Stack_Count() > breakPoint) {
		KD_Node *node = Stack_Pop();
		if (!node->isLeaf()) {
			Stack_Push(node->m_right);
			Stack_Push(node->m_left);
		}
		m_nodePool.free(node);
	}

	subtree->m_left = nullptr;
	subtree->m_right = nullptr;
	subtree->m_plane = Plane();
}

void KD::Translate(KD_Node *node) const {
	const Axis axis = node->m_plane.m_axis;
	const float factor = 1.0f / (node->m_population * 2);
	float mean = 0.0f;

	for (size_t i = node->m_objectsStart; i < node->m_objectsEnd; i++) {
		KD_Object *p = m_objectsArray[i];

		mean += (p->m_aabb.m_max[axis] + p->m_aabb.m_min[axis]) * factor;
	}
	node->m_plane.m_position = mean;
}
KD_Node* KD::Erase(KD_Node *node) {
	KD_Node *bigBranch, *smallBranch;
	KD_Node *aux;

	if (node->m_left->m_population > node->m_right->m_population) {
		bigBranch = node->m_left;
		smallBranch = node->m_right;
	} else {
		bigBranch = node->m_right;
		smallBranch = node->m_left;
	}

	const size_t start = node->m_objectsStart - node->m_left->m_population;
	const size_t end = node->m_objectsEnd + node->m_right->m_population;
	ASSERT(start >= 0);
	ASSERT(end <= m_settings.m_numberOfObjects);

	bigBranch->m_objectsStart = start;
	bigBranch->m_objectsEnd = end;

	for (size_t i = start; i < end; i++) {
		KD_Object *p = m_objectsArray[i];

		p->m_node = bigBranch;
	}

	bigBranch->m_parent = node->m_parent;
	bigBranch->m_population = node->m_population;
	bigBranch->m_delayedOffset = 0;
	bigBranch->m_aabb.m_min = node->m_aabb.m_min;
	bigBranch->m_aabb.m_max = node->m_aabb.m_max;

	const size_t breakPoint = Stack_Count();
	if (!bigBranch->isLeaf()) {
		bigBranch->setChildreenAABBs();
		Stack_Push(bigBranch->m_left);
		while (Stack_Count() > breakPoint) {
			aux = Stack_Pop();
			if (!aux->isLeaf()) {
				Stack_Push(aux->m_right);
				Stack_Push(aux->m_left);
				aux->setChildreenAABBs();
			}

			aux->m_objectsStart = start;
			aux->m_objectsEnd = start;
			aux->m_delayedOffset = 0;
			aux->m_population = 0;
		}
		Stack_Push(bigBranch->m_right);
		while (Stack_Count() > breakPoint) {
			aux = Stack_Pop();
			if (!aux->isLeaf()) {
				Stack_Push(aux->m_right);
				Stack_Push(aux->m_left);
				aux->setChildreenAABBs();
			}

			aux->m_objectsStart = end;
			aux->m_objectsEnd = end;
			aux->m_delayedOffset = 0;
			aux->m_population = 0;
		}

		ASSERT(bigBranch->m_left->m_population == 0);
		ASSERT(bigBranch->m_right->m_population == 0);
	}


	// free smallbranch nodes
	Stack_Push(smallBranch);
	while (Stack_Count() > breakPoint) {
		aux = Stack_Pop();
		if (!aux->isLeaf()) {
			Stack_Push(aux->m_right);
			Stack_Push(aux->m_left);
		}

		m_nodePool.free(aux);
	}

	// linking grandparent to the bigbranch
	if (node->m_parent) {
		if (node->isLeft()) {
			node->m_parent->m_left = bigBranch;
		} else {
			node->m_parent->m_right = bigBranch;
		}
	} else {
		m_root = bigBranch;
	}

	m_nodePool.free(node);

	return bigBranch;
}