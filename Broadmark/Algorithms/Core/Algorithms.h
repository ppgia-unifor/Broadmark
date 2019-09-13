


#pragma once

#include "Broadphase/BroadphaseInterface.h"
#include <memory>
#include <vector>


// Helper class to instantiate algorithms by name, as C++ lacks reflection
// All algorithms must be registered within this class, at the 'Algorithms.cpp' file
class Algorithms {
public:
	static std::unique_ptr<BroadphaseInterface> Create(const char* algorithmName);
	static std::vector<const char*> EnumerateAlgorithms();
};

