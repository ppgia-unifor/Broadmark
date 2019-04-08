#pragma once



#include "Dependencies\doctest.h"
#include <type_traits> // for static_assert


void initializeDocTestForAssertionsOutOfTestCases();
#ifdef DEBUG_MODE
#define ASSERT(condition) CHECK(condition)
#else 
#define ASSERT(condition)
#endif



#include <sstream>
#define COMMENT(a) {             \
	std::stringstream ss;         \
	ss << a;                       \
	std::cout << ss;                \
}

