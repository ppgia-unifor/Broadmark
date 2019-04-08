

#pragma once

#include "Macros.h"
#include <memory>


template<typename T>
class Stack {
public:
	T *m_data;
	int m_count;
	int m_capacity;

public:
	Stack() {
		m_count = 0;
		m_capacity = 0;
	}
	Stack(int capacity) {
		m_data = (T*)_aligned_malloc(sizeof(T) * capacity, 16);
		m_count = 0;
		m_capacity = capacity;
	}
	~Stack() {
		if (m_data) {
			_aligned_free(m_data);
		}
	}

	void push(T item) {
		ASSERT(m_count < m_capacity);
		m_data[m_count] = item;
		m_count++;
	}
	T pop() {
		ASSERT(m_count > 0);
		m_count--;
		return m_data[m_count];
	}
	T peek() {
		ASSERT(m_count > 0);
		return m_data[m_count - 1];
	}

	void clear() {
		m_count = 0;
	}
	int size() const {
		return m_count;
	}
};