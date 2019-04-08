


#pragma once

#include "Broadphase/BroadphaseInterface.h"
#include <memory>
#include <vector>


class Algorithms {
public:
	static std::unique_ptr<BroadphaseInterface> Create(const char* algorithmName);
	static std::vector<const char*> EnumerateAlgorithms();
};

