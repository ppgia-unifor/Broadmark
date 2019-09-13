#pragma once



#include "Dependencies\doctest.h"
#include <type_traits> // for static_assert

// This file provides a ASSERT and COMMENT macros for debuging and logging
// These are wired to 'doctest' for more useful info on assertions

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

