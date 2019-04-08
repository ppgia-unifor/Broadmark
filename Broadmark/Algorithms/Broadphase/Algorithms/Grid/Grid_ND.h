#pragma once


#include "Grid_Base.h"


class Grid_2D : public Grid_Base {
public:
	void UpdateStructures() override {
		double n = m_settings.m_numberOfObjects / (double)m_settings.m_Grid_ObjectsPerCell;
		n = ceil(sqrt(n));

		m_cellsPerDimension = n;
		m_numberOfCells = n * n + 1;
		m_cells = new std::vector<Grid_Object*>[m_numberOfCells];

		const Aabb& world = m_settings.m_worldAabb;

		for (size_t i = 0; i < m_numberOfCells; i++) {
			m_cells[i].clear();
		}

		for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
			Grid_Object& object = m_objects[i];
			Vec3 pos2cells_min = (object.m_aabb.m_min - world.m_min) / (world.m_max - world.m_min);
			Vec3 pos2cells_max = (object.m_aabb.m_max - world.m_min) / (world.m_max - world.m_min);

			pos2cells_min = pos2cells_min.Clamp01() * (m_cellsPerDimension - 1);
			pos2cells_max = pos2cells_max.Clamp01() * (m_cellsPerDimension - 1);

			for (size_t j = 0; j < 3; j++) {
				object.m_minCell[j] = floor(pos2cells_min[j]);
				object.m_maxCell[j] = floor(pos2cells_max[j]);
			}

			for (size_t x = object.m_minCell[0]; x <= object.m_maxCell[0]; x++) {
				for (size_t z = object.m_minCell[2]; z <= object.m_maxCell[2]; z++) {
					const size_t id = x * m_cellsPerDimension + z;
					ASSERT(id < m_numberOfCells);
					m_cells[id].push_back(&object);
				}
			}
		}
	}
};


class Grid_3D : public Grid_Base {
public:
	void UpdateStructures() override {
		double n = m_settings.m_numberOfObjects / (double)m_settings.m_Grid_ObjectsPerCell;
		n = ceil(pow(n, 1.0f / 3.0f));

		m_cellsPerDimension = n;
		m_numberOfCells = n * n * n + 1;
		m_cells = new std::vector<Grid_Object*>[m_numberOfCells];

		const Aabb& world = m_settings.m_worldAabb;

		for (size_t i = 0; i < m_numberOfCells; i++) {
			m_cells[i].clear();
		}

		for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
			Grid_Object& object = m_objects[i];
			Vec3 pos2cells_min = (object.m_aabb.m_min - world.m_min) / (world.m_max - world.m_min);
			Vec3 pos2cells_max = (object.m_aabb.m_max - world.m_min) / (world.m_max - world.m_min);

			pos2cells_min = pos2cells_min.Clamp01() * (m_cellsPerDimension - 1);
			pos2cells_max = pos2cells_max.Clamp01() * (m_cellsPerDimension - 1);

			for (size_t j = 0; j < 3; j++) {
				object.m_minCell[j] = floor(pos2cells_min[j]);
				object.m_maxCell[j] = floor(pos2cells_max[j]);
			}

			for (size_t x = object.m_minCell[0]; x <= object.m_maxCell[0]; x++) {
				for (size_t y = object.m_minCell[1]; y <= object.m_maxCell[1]; y++) {
					for (size_t z = object.m_minCell[2]; z <= object.m_maxCell[2]; z++) {
						const size_t id = x * m_cellsPerDimension * m_cellsPerDimension + y * m_cellsPerDimension + z;
						ASSERT(id < m_numberOfCells);
						m_cells[id].push_back(&object);
					}
				}
			}
		}
	}
};