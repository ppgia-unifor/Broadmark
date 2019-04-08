#pragma once


#include "Grid_ND.h"


template<typename GridStructure>
class Grid_ND_Parallel : public GridStructure {
public:
	void SearchOverlaps() override {
		omp_lock_t writelock;
		omp_init_lock(&writelock);

#pragma omp parallel num_threads(this->m_settings.m_numThreads) 
		{
			// find the cells range that this thread has to work on
			// we try to give a equal number of objects per thread
			const int tid = omp_get_thread_num();
			const int objectsPerThread = this->m_settings.m_numberOfObjects / omp_get_num_threads();
			const int start = tid * objectsPerThread;
			const int end = start + objectsPerThread;
			int numberOfObjects = 0;

			int firstCell = 0;
			if (start > 0) {
				for (int c = 0; c < this->m_numberOfCells; c++) {
					numberOfObjects += this->m_cells[c].size();
					if (numberOfObjects > start) {
						firstCell = c + 1;
						break;
					}
				}
			}

			int lastCell = this->m_numberOfCells;
			if (end < this->m_settings.m_numberOfObjects) {
				for (int c = firstCell; c < this->m_numberOfCells; c++) {
					numberOfObjects += this->m_cells[c].size();
					if (numberOfObjects > end) {
						lastCell = c + 1;
						break;
					}
				}
			}

			std::vector<ObjectPair> pairs;
			for (int c = firstCell; c < lastCell; c++) {
				std::vector<Grid_Object*>& objects = this->m_cells[c];
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
							pairs.emplace_back(a, b);
						}
					}
				}
			}

			omp_set_lock(&writelock);
			for (const ObjectPair pair : pairs) {
				this->m_cache.AddPair(pair.m_a, pair.m_b);
			}
			omp_unset_lock(&writelock);
		}
	}
};

typedef Grid_ND_Parallel<Grid_2D> Grid_2D_Parallel;
typedef Grid_ND_Parallel<Grid_3D> Grid_3D_Parallel;