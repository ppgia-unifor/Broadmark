

#pragma once

// usefull macros for application development

// find out if we are in debug mode
#if defined(DEBUG) | defined (_DEBUG) | defined(MIXED_MODE)
#define DEBUG_MODE
#endif
//#define ALLOW_DRAWS
//#define ALLOW_PRINTS


#define __STRINGIZING_OP(x) #x // stringize 'lil trick
// macro to create strings from compile time literals
#define STRINGIZE(x) __STRINGIZING_OP(x)

// macro to get the file and line
#define DEBUG_INFO "File: " __FILE__ ", Line: " STRINGIZE(__LINE__)

// Make something 16 bytes aligned
#define StructAlignment(a) __declspec(align(a))


#include "Core/Assertions.h"