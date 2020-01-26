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
		std::streampos fsize = m_file.tellg();
		m_file.seekg(0, std::ios::end);
		fsize = m_file.tellg() - fsize;
		STRONG_ASSERT(fsize > sizeof(SceneHeader), " -The scene file is too small to be a valid scene file! Aborting...");
		m_file.seekg(0, std::ios::beg);

		m_file.read((char*)&m_header, sizeof(SceneHeader));
		ValidateHeader();

		size_t expectedSize;
		expectedSize = sizeof(Aabb) * m_header.m_numberOfObjects;
		expectedSize *= 1 + m_header.m_numberOfFrames;
		expectedSize += sizeof(SceneHeader);
		STRONG_ASSERT(fsize == expectedSize, " -The specified scene file is too small considering its header contents. Aborting...");

		// Safety margin for physics errors
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

	static void ValidateString(const char* str, const int maxlen) {
		int i = 0;
		for (; i < maxlen; ++i) {
			if (str[i] == 0) {
				STRONG_ASSERT(i > 0, " -The scene file has an unreadable scene name or object type. Aborting..."); // has to have at least one valid caracter
				break;
			}

			// All characters should be printable, else they are garbage
			STRONG_ASSERT(isprint((unsigned char)str[i]), " -The scene file has an unreadable scene name or object type. Aborting...");
		}

		for (; i < maxlen; ++i) {
			STRONG_ASSERT(str[i] == 0, " -The scene file has an unreadable scene name or object type. Aborting..."); // all other characters should be zero
		}
	}
	void ValidateHeader() const {
		STRONG_ASSERT(strlen(m_header.m_sceneName) <= 127, " -The scene file has an unreadable scene name. Aborting...");
		STRONG_ASSERT(strlen(m_header.m_objectsType) <= 127, " -The scene file has an unreadable object type. Aborting...");
		Scene::ValidateString(m_header.m_sceneName, 127);
		Scene::ValidateString(m_header.m_objectsType, 127);

		STRONG_ASSERT(m_header.m_numberOfObjects > 0, " -The scene file has a negative number of objects. Aborting...");
		STRONG_ASSERT(m_header.m_numberOfFrames > 0, " -The scene file has a negative number of frames. Aborting...");
		STRONG_ASSERT(m_header.m_padding1 == 0, " -The scene file has invalid data. Aborting...");
		STRONG_ASSERT(m_header.m_padding2 == 0, " -The scene file has invalid data. Aborting...");
		STRONG_ASSERT(m_header.m_worldAabb.Validate(), " -The scene file has an invalid world aabb. Aborting...");
	}
	void ValidateFrame() const {
		for (size_t i = 0; i < m_header.m_numberOfObjects; i++) {
			ASSERT(m_frame.m_aabbs[i].Validate());
			//ASSERT(m_header.m_worldAabb.m_min <= m_frame.m_aabbs[i].m_min);
			//ASSERT(m_frame.m_aabbs[i].m_max <= m_header.m_worldAabb.m_max);
		}
	}
};