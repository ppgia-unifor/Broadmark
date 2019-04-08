

#pragma once


#include "Broadphase/Object.h"
#include "Broadphase/OverlapChaches/SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"
#include "Core/Math/Plane.h"
#include "Core/Structures/Pool.h"
#include "KD_Node.h"
#include "KD_Object.h"

#include <stack>




class KD : public BaseAlgorithm<KD_Object, SimpleCache> {
public:
	MemoryPool<KD_Node> m_nodePool;
	KD_Node *m_root;
	std::vector<KD_Node*> m_stack;
	KD_Object **m_objectsArray;
	union {
		KD_Object** m_buffer;
		KD_Object** m_collisionList;
	};
	size_t m_bufferCapacity;
	size_t m_staticObjects;
	bool m_incremental;


	virtual ~KD();

	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override;

	void UpdateObjects(const SceneFrame& frameData) override;
	void CleanCache() override;
	void UpdateStructures() override;
	void SearchOverlaps() override;

	void Validate() const override;
	

protected:
	// Evaluators
	bool Evaluate_Heuristic(KD_Node *node) const;
	bool Evaluate_Partitionate(KD_Node *node) const;
	bool Evaluate_Trim(KD_Node *node) const;

	// Operators
	void WaterfallNode(KD_Node *node);
	void FinishLeaf(KD_Node* leaf) const;
	void LiftSubtree(KD_Node *subtree);
	void Partitionate(KD_Node *node);
	void Trim(KD_Node* subtree);
	void Translate(KD_Node *node) const;
	KD_Node* Erase(KD_Node *node);
	
	// Objects array related
	void organizeSectionAroundPlane(KD_Node* n) const;
	void refitToEnd(KD_Node* n) const;
	void refitToStart(KD_Node* n) const;
	Plane FindBestPlane(KD_Node* n) const;
	void memoryTransfer_LeftToRight(size_t copyPoint, int transfered, size_t subtreeSize);
	void memoryTransfer_RightToLeft(size_t copyPoint, int transfered, size_t subtreeSize);

	// Broadphase related
	void sort_Node_Objects(KD_Node* n) const;
	void sort_Node_CollisionList(KD_Node* n) const;
	void sap_NodeNode(KD_Node* n, Axis axis);
	void sap_NodeParentCollision(KD_Node* n, Axis axis);
	void sap_objectArray_avx(KD_Object* p1, KD_Object** array2, int array2_count, Axis axis);

	// Validation
	void Validate_SortedObjects() const;
	void Validate_SortedCollisionListSection(KD_Node* n) const;

	#pragma region Helpers
	void Stack_Push(KD_Node *node) {
		m_stack.push_back(node);
	}
	KD_Node* Stack_Pop() {
		KD_Node* output = m_stack.back();
		m_stack.pop_back();
		return output;
	}
	void Stack_Clear() {
		m_stack.clear();
	}

	KD_Node* Stack_Get(size_t i) {
		return m_stack[i];
	}
	size_t Stack_Count() const {
		return m_stack.size();
	}

	void monotonicResize(size_t newCapacity) {
		if (newCapacity > m_bufferCapacity) {
			newCapacity = (size_t)(1.5f * newCapacity);

			KD_Object **newBuffer = (KD_Object**)_aligned_malloc(sizeof(KD_Object*) * newCapacity, 16);
			memcpy(newBuffer, m_buffer, m_bufferCapacity * sizeof(KD_Object*));
			_aligned_free(m_buffer);

			m_bufferCapacity = newCapacity;
			m_buffer = newBuffer;
		}
	}
	#pragma endregion
};