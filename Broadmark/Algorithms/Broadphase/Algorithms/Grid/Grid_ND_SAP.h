#pragma once


#include "Grid_ND.h"


template<typename GridStructure>
class Grid_ND_SAP : public GridStructure {
public:
	void SearchOverlaps() override {
		for (size_t c = 0; c < this->m_numberOfCells; c++) {
			std::vector<Grid_Object*>& objects = this->m_cells[c];
			if (objects.size() < 2) {
				continue;
			}

			const Plane best = Plane::FindBestPlane(&objects[0], objects.size());
			const Axis axis = best.m_axis;
			std::sort(objects.begin(), objects.end(), ObjectPointerComparer<Grid_Object*>(axis));

			for (size_t i = 0; i < objects.size() - 1; i++) {
				const Grid_Object* a = objects[i];
				for (size_t j = i + 1; j < objects.size(); j++) {
					const Grid_Object* b = objects[j];

					if (a->m_id == b->m_id) {
						continue;
					}

					if (b->m_aabb.m_min[axis] > a->m_aabb.m_max[axis]) {
						break;
					}

					if (Aabb::Test(a->m_aabb, b->m_aabb)) {
						this->m_cache.AddPair(a, b);
					}
				}
			}
		}
	}
};

typedef Grid_ND_SAP<Grid_2D> Grid_2D_SAP;
typedef Grid_ND_SAP<Grid_3D> Grid_3D_SAP;