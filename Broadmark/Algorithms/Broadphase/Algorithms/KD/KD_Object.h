


#pragma once


#include "Broadphase/Algorithms/KD/KD_Node.h"


class KD_Object : public Object {
public:
	KD_Node * m_node = nullptr;
	bool m_static = false;
};
