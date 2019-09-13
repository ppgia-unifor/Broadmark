#pragma once


#include "ObjectPair.h"

// Base class for Collision Pairs containers
class OverlapCache {
public:
	virtual ~OverlapCache() = default;

	virtual bool HasPair(const Object* a, const Object* b) = 0;
	virtual void AddPair(const ObjectPair& p) = 0;
	virtual void AddPair(const Object* a, const Object* b) = 0;
	virtual void RemovePair(const Object* a, const Object* b) = 0;

	virtual size_t Size() const = 0;
	virtual void Clean() = 0;

	virtual void Validate() const = 0;
};