


#include "KD.h"

void KD::organizeSectionAroundPlane(KD_Node* n) const {
	Plane plane = n->m_plane;

	for (size_t j = n->m_objectsStart; j < n->m_objectsEnd;) {
		KD_Object* proxy = m_objectsArray[j];

		if (plane.isStrictlyRight(proxy->m_aabb.m_min)) {
			n->m_objectsEnd--;
			m_objectsArray[j] = m_objectsArray[n->m_objectsEnd];
			m_objectsArray[n->m_objectsEnd] = proxy;
		} else if (plane.isStrictlyLeft(proxy->m_aabb.m_max)) {
			m_objectsArray[j] = m_objectsArray[n->m_objectsStart];
			m_objectsArray[n->m_objectsStart] = proxy;
			n->m_objectsStart++;
			j++;
		} else {
			proxy->m_node = n;
			j++; // the object lies in the splitting plane
		}
	}
}

void KD::refitToEnd(KD_Node* n) const {
	for (size_t i = n->m_objectsStart; i < n->m_objectsEnd;) {
		KD_Object* p = m_objectsArray[i];

		if (!Aabb::Inside(n, p)) {
			n->m_objectsEnd--;
			m_objectsArray[i] = m_objectsArray[n->m_objectsEnd];
			m_objectsArray[n->m_objectsEnd] = p;
		} else {
			i++; // i is only incremented if we didn't perform a swap
		}
	}
}

void KD::refitToStart(KD_Node* n) const {
	for (size_t i = n->m_objectsStart; i < n->m_objectsEnd;) {
		KD_Object* p = m_objectsArray[i];

		if (!Aabb::Inside(n, p)) {
			m_objectsArray[i] = m_objectsArray[n->m_objectsStart];
			m_objectsArray[n->m_objectsStart] = p;
			n->m_objectsStart++;
		}
		i++; // i is always incremented when refiting to the start
	}
}

Plane KD::FindBestPlane(KD_Node* n) const {
	KD_Object** start = &m_objectsArray[n->m_objectsStart];
	return Plane::FindBestPlane(start, n->count());
}

void KD::memoryTransfer_LeftToRight(size_t copyPoint, int transfered, size_t subtreeSize) {
	ASSERT(copyPoint >= 0);
	ASSERT(copyPoint <= m_settings.m_numberOfObjects);
	ASSERT(transfered >= 0);
	ASSERT(subtreeSize >= 0);
	if (subtreeSize == 0) { return; }

	monotonicResize(transfered);

	memcpy(
		&m_buffer[0],
		&m_objectsArray[copyPoint],
		transfered * sizeof(KD_Object*));
	// moving the subtree to engulf the buffer and make room to it at the end
	memmove(
		&m_objectsArray[copyPoint],
		&m_objectsArray[copyPoint + transfered],
		subtreeSize * sizeof(KD_Object*));
	// copying the buffer to the new position
	memcpy(
		&m_objectsArray[copyPoint + subtreeSize],
		&m_buffer[0],
		transfered * sizeof(KD_Object*));
}

void KD::memoryTransfer_RightToLeft(size_t copyPoint, int transfered, size_t subtreeSize) {
	ASSERT(copyPoint >= 0);
	ASSERT(copyPoint <= m_settings.m_numberOfObjects);
	ASSERT(transfered >= 0);
	ASSERT(subtreeSize >= 0);
	if (subtreeSize == 0) { return; }

	monotonicResize(transfered);

	memcpy(
		&m_buffer[0],
		&m_objectsArray[copyPoint + subtreeSize],
		transfered * sizeof(KD_Object*));
	// moving the subtree to engulf the buffer and make room to it at the end
	memmove(
		&m_objectsArray[copyPoint + transfered],
		&m_objectsArray[copyPoint],
		subtreeSize * sizeof(KD_Object*));
	// copying the buffer to the new position
	memcpy(
		&m_objectsArray[copyPoint],
		&m_buffer[0],
		transfered * sizeof(KD_Object*));
}