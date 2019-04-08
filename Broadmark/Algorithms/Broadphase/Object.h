

#pragma once


#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Math/Aabb.h"
#include "Core/Math/Axis.h"


StructAlignment(16)
class Object {
public:
	Aabb m_aabb;
	int m_id;
	
	Object() {
		m_id = -1;
	}
	Object(const Aabb& aabb, size_t uid) {
		m_aabb = aabb;
		m_id = uid;
	}

	friend bool operator==(const Object& lhs, const Object& rhs) { return lhs.m_id == rhs.m_id; }
	friend bool operator!=(const Object& lhs, const Object& rhs) { return !(lhs == rhs); }
};

//template<typename ObjectType, Axis axis>
//bool ObjectComparer(const ObjectType& p1, const ObjectType& p2) {
//	return p1.m_aabb.m_min[axis] < p2.m_aabb.m_min[axis];
//}

template<typename ObjectType>
struct ObjectComparer {
private:
	Axis m_axis;

public:
	ObjectComparer(Axis axis) : m_axis(axis) {}

	bool operator()(const ObjectType& p1, const ObjectType& p2) {
		return p1.m_aabb.m_min[m_axis] < p2.m_aabb.m_min[m_axis];
	}
};


template<typename ObjectType>
struct ObjectPointerComparer {
private:
	Axis m_axis;

public:
	ObjectPointerComparer(Axis axis) : m_axis(axis) {}

	bool operator()(const ObjectType& p1, const ObjectType& p2) {
		return p1->m_aabb.m_min[m_axis] < p2->m_aabb.m_min[m_axis];
	}
};
