

#pragma once


#include <memory>
#include "Macros.h"


// Basic memory pool implementation

#define TEMPLATE_DEFINITION template <typename T>
typedef size_t PoolHandle;

TEMPLATE_DEFINITION
struct PoolIsland {
	T m_data;
	PoolHandle m_next;
};

TEMPLATE_DEFINITION
class MemoryPool {
protected:
	PoolIsland<T> *m_pool;
	
public:
	size_t m_capacity;
	size_t m_count;
	PoolHandle m_head;
	PoolHandle m_tail;

	void initialize(size_t capacity);
	void dispose();

	PoolHandle alloc();
	void free(void* ptr);
	void free(PoolHandle handle);

	T* get(PoolHandle i) { return &m_pool[i].m_data; }
};

TEMPLATE_DEFINITION
void MemoryPool<T>::initialize(size_t capacity) {
	m_capacity = capacity;
	m_count = 0;
	m_head = 0;
	m_tail = 0;

	
	m_pool = (PoolIsland<T>*)_aligned_malloc(sizeof(PoolIsland<T>) * m_capacity, 16);
	ASSERT(m_pool != nullptr);

	for (size_t i = 0; i < m_capacity; i++) {
		m_pool[i].m_next = i + 1;
	}

	m_pool[m_capacity - 1].m_next = -1;
}
TEMPLATE_DEFINITION
void MemoryPool<T>::dispose() {
	if (m_pool != nullptr) {
		_aligned_free(m_pool);
	}
	m_capacity = 0;
	m_count = 0;
	m_head = 0;
	m_tail = 0;
}

TEMPLATE_DEFINITION
PoolHandle MemoryPool<T>::alloc() {
	ASSERT(m_count < m_capacity); // array bound check

	const size_t handle = m_head;

	m_head = m_pool[handle].m_next;

	if (m_tail < handle) {
		m_tail = handle;
	}
	m_count++;
	
	return handle;
}
TEMPLATE_DEFINITION
void MemoryPool<T>::free(void* ptr) {
	PoolIsland<T> *island = static_cast<PoolIsland<T>*>(ptr);

	const PoolHandle handle = (size_t)(island - m_pool);

	ASSERT(handle >= 0); // checks if it is a valid offset
	ASSERT(handle < m_capacity);
	free(handle);
}
TEMPLATE_DEFINITION
void MemoryPool<T>::free(PoolHandle handle) {
	// checks if there is this element and if it is really in use
	ASSERT(handle >= 0);
	
	m_pool[handle].m_next = m_head;
	m_head = handle;

	if (m_tail == handle) {
		m_tail--;
	}	

	m_count--;
}

#undef TEMPLATE_DEFINITION