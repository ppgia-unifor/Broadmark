#pragma once


#include "Core/Math/Aabb.h"
#include <fstream>
#include "Macros.h"


// Scene header data, optimized to be easy to read from C++
struct SceneHeader {
	char m_sceneName[128];
	char m_objectsType[128];
	int m_numberOfObjects;
	int m_numberOfFrames;
	int m_padding1;
	int m_padding2;
	Aabb m_worldAabb;
};
struct SceneFrame {
	Aabb *m_aabbs;
};

// Scene file handler, performs scene reading and memory management.
class Scene {
private:
	std::ifstream m_file;
	SceneHeader m_header;
	SceneFrame m_frame;

public:
	Scene(const char* filePath) {
		m_file = std::ifstream(filePath, std::ifstream::binary);
		m_file.read((char*)&m_header, sizeof(SceneHeader));
		ValidateHeader();

		// Safety margin for physics erros
		m_header.m_worldAabb.m_min = m_header.m_worldAabb.m_min * 1.1f;
		m_header.m_worldAabb.m_max = m_header.m_worldAabb.m_max * 1.1f;

		m_frame.m_aabbs = new Aabb[m_header.m_numberOfObjects];
		memset(m_frame.m_aabbs, 0, m_header.m_numberOfObjects * sizeof(Aabb));
	}
	~Scene() = default;

	const SceneHeader& GetHeader() const {
		return m_header;
	}
	const SceneFrame& GetNextFrame() {
		m_file.read((char*)m_frame.m_aabbs, m_header.m_numberOfObjects * sizeof(Aabb));
		ValidateFrame();

		return m_frame;
	}
	bool HasNextFrame() const {
		return !m_file.eof();
	}

	void ValidateHeader() const {
		ASSERT(strlen(m_header.m_sceneName) <= 127);
		ASSERT(strlen(m_header.m_objectsType) <= 127);
		ASSERT(m_header.m_numberOfObjects > 0);
		ASSERT(m_header.m_numberOfFrames > 0);
		ASSERT(m_header.m_padding1 == 0);
		ASSERT(m_header.m_padding2 == 0);
		ASSERT(m_header.m_worldAabb.Validate());
	}
	void ValidateFrame() const {
		for (size_t i = 0; i < m_header.m_numberOfObjects; i++) {
			ASSERT(m_frame.m_aabbs[i].Validate());
			//ASSERT(m_header.m_worldAabb.m_min <= m_frame.m_aabbs[i].m_min);
			//ASSERT(m_frame.m_aabbs[i].m_max <= m_header.m_worldAabb.m_max);
		}
	}
};