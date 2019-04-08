#pragma once


#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"
#include "Broadphase/Algorithms/BaseAlgorithm.h"
#include "Broadphase/Algorithms/SoAAlgorithm.h"

#include <unordered_set>
#include <omp.h>
#include "Core/Math/Plane.h"


class Grid_Object : public Object {
public:
	size_t m_minCell[3];
	size_t m_maxCell[3];
};

class Grid_Base : public BaseAlgorithm<Grid_Object, SimpleCache> {
protected:
	std::vector<Grid_Object*>* m_cells = nullptr;
	size_t m_cellsPerDimension = 0;
	size_t m_numberOfCells = 0;

public:
	virtual ~Grid_Base() {
		delete[] m_cells;
	}

	void SearchOverlaps() override {
		for (size_t c = 0; c < m_numberOfCells; c++) {
			std::vector<Grid_Object*>& objects = m_cells[c];
			if (objects.size() < 2) {
				continue;
			}

			for (size_t i = 0; i < objects.size() - 1; i++) {
				const Grid_Object* a = objects[i];
				for (size_t j = i + 1; j < objects.size(); j++) {
					const Grid_Object* b = objects[j];

					if (a->m_id == b->m_id) {
						continue;
					}

					if (Aabb::Test(a->m_aabb, b->m_aabb)) {
						this->m_cache.AddPair(a, b);
					}
				}
			}
		}
	}
};