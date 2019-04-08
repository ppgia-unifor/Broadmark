/*

Sweep and Prune implementations Copyright (C) 2008 Daniel Joseph Tracy.
All rights reserved.  Email: daniel.joseph.tracy@gmail.com Web: www.sweepandprune.com

This library is free software; you can redistribute it and/or modify it under the terms of EITHER:

	(1) The GNU Lesser General Public License as published by the Free
	Software Foundation; either version 2.1 of the License, or (at your
	option) any later version. The text of the GNU Lesser General Public
	License is included with this library in the file LICENSE.TXT.

	(2) The BSD-style license that is included with this library in the file
	LICENSE-BSD.TXT.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the files LICENSE.TXT and LICENSE-BSD.TXT for more details.

*/

/*
 *  Utility.hpp
 *  CollisionDetectionTestBed
 */

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <vector>
#include <algorithm>
#include <assert.h>
#include <limits>
#include <iostream>

namespace SweepAndPrune {

	// ::max() and ::min() won't work here for sentinels
	#define PLUS_INFINITY (std::numeric_limits<float>::infinity())
	#define MINUS_INFINITY (-std::numeric_limits<float>::infinity())

	template<typename Coordinate>
	struct extrema {
		Coordinate location;
		unsigned int kind : 2;
		signed int aabbIndex : 30;

		// Minima < Maxima means that AABBs that "just touch" swap and overlap
		enum Which {
			Uninitialized = 0, Minima = 1, Maxima = 2, Sentinel = 3
		};

		inline bool operator< (const extrema& rhs) const {
			if (location != rhs.location)
				return(location < rhs.location);
			else if (kind != rhs.kind)
				return(kind < rhs.kind);
			else
				return(aabbIndex < rhs.aabbIndex);
		}

		extrema(Coordinate _location, Which _kind, int _aabbIndex) : location(_location), kind(_kind), aabbIndex(_aabbIndex) {
		}
		extrema() : location(0), kind(Uninitialized), aabbIndex(-1) {
		}

		static extrema& sentinel(Which kind) {
			static extrema left(MINUS_INFINITY, Sentinel, -1);
			static extrema right(PLUS_INFINITY, Sentinel, -1);

			if (kind == Minima)
				return left;
			else
				return right;
		}
	};

	// An array-based set with no references, but sorted batch processing
	template<typename Type>
	struct BatchedSet {
		std::vector<Type> set;
		std::vector<Type> adds;
		std::vector<Type> deletes;

	public:
		void clear() {
			set.clear(); adds.clear(); deletes.clear();
		}

		void insert(const Type& x) {
			adds.push_back(x);
		}

		void erase(const Type& x) {
			deletes.push_back(x);
		}

		void sort() {
			std::sort(adds.begin(), adds.end());
			int from = set.size() - 1;
			set.resize(set.size() + adds.size());
			int to = set.size() - 1;
			while (from >= 0) {
				if (adds.size() > 0 && adds.back() > set[from]) {
					set[to--] = adds.back();
					adds.pop_back();
				} else {
					set[to--] = set[from--];
				}
			}
			while (!adds.empty()) {
				set[to--] = adds.back();
				adds.pop_back();
			}
			if (to != -1)
				std::cout << "Mismatch in merging operation" << std::endl;
			adds.clear();

			std::sort(deletes.begin(), deletes.end());
			from = 0, to = 0;
			int u = 0, size = deletes.size(), size2 = set.size();
			while (u < size) {
				if (deletes[u] == set[from]) {
					++from;
					++u;
				} else {
					set[to++] = set[from++];
				}
			}
			size = set.size();
			while (from < size) {
				set[to++] = set[from++];
			}
			set.resize(to);
			deletes.clear();
		}
	};

	template<class EventBasedAlgorithm>
	class FullOutputAdapter {
		typedef typename EventBasedAlgorithm::Coord Coordinate;
		typedef typename EventBasedAlgorithm::CKey ClientKey;

		EventBasedAlgorithm itree;
		BatchedSet<std::pair<ClientKey, ClientKey> > candidates;

	public:
		static const bool event_based_output = false;

		static float optimalEntitiesPerDomain() {
			return EventBasedAlgorithm::optimalEntitiesPerDomain();
		}
		int numBoxes() {
			return itree.numBoxes();
		}
		void SetSpatialBounds(Coordinate boundsMin[], Coordinate boundsMax[]) {
			itree.SetSpatialBounds(boundsMin, boundsMax);
		}
		bool SpaceFullyContainsAABB(Coordinate min[], Coordinate max[]) {
			return itree.SpaceFullyContainsAABB(min, max);
		}
		void clear() {
			itree.clear();
		}
		int insert(ClientKey objectKey, Coordinate min[], Coordinate max[]) {
			return itree.insert(objectKey, min, max);
		}
		void erase(int key) {
			itree.erase(key);
		}
		void UpdateAABB(int key, Coordinate min[], Coordinate max[]) {
			itree.UpdateAABB(key, min, max);
		}
		void AcquireAABB(int key, Coordinate min[], Coordinate max[]) {
			itree.AcquireAABB(key, min, max);
		}
		void SetGridParameters(Coordinate boundsMin[], Coordinate boundsMax[], int _numCells[], int _numThreads = 1) {
			itree.SetGridParameters(boundsMin, boundsMax, _numCells, _numThreads);
		}

		void GatherCollisions(std::vector<std::pair<ClientKey, ClientKey> >& pairs) {
			itree.GatherEvents(candidates.adds, candidates.deletes);
			candidates.sort();
			pairs.insert(pairs.end(), candidates.set.begin(), candidates.set.end());
		}
	};

	// Tests one-dimensional extrema for overlap: is one max less than the other min?
	template<typename Type>
	inline static bool IntervalWithin(const Type& minOne, const Type& maxOne, const Type& minTwo, const Type& maxTwo) {
		if (maxOne < minTwo || maxTwo < minOne)
			return false;
		else
			return true;
	}

	template<typename Data>
	class array_map {
		std::vector<Data> data;
		std::vector<bool> valid;
		std::vector<int> holes;

		bool valid_location(int key) {
			if (key < 0) {
				std::cout << "Key is negative" << std::endl;
				return false;
			}
			if ((int)data.size() <= key) {
				std::cout << "Key is larger than size" << std::endl;
				return false;
			}
			for (int i = 0; i < (int)holes.size(); ++i)
				if (holes[i] == key) {
					std::cout << "Key is in hole list" << std::endl;
					return false;
				}
			if (!valid[key])
				std::cout << "Entry marked is invalid" << std::endl;

			return true;
		}

	public:
		void clear() {
			data.clear();
			holes.clear();
			valid.clear();
		}
		bool present(int key) {
			return valid[key];
		}
		int insert(const Data& _data) {
			int loc;
			if (holes.size() == 0) {
				loc = (int)data.size();
				data.push_back(_data);
				valid.push_back(true);
			} else {
				loc = holes.back();
				holes.pop_back();
				data[loc] = _data;
				valid[loc] = true;
			}
			return loc;
		}

		void erase(int key) {
			assert(valid_location(key));
			holes.push_back(key);
			valid[key] = false;
		}

		Data& operator[] (int key) {
			return data[key];
		}

		int population() {
			return data.size() - holes.size();
		}
		int end() {
			return data.size();
		}

		template<class Function>
		void apply(Function function) {
			for (int i = 0; i < (int)data.size(); ++i) {
				if (valid[i])
					function(data[i]);
			}
		}

		template<class Function>
		void applyToKey(Function function) {
			for (int i = 0; i < (int)data.size(); ++i) {
				if (valid[i])
					function(i);
			}
		}

		void gatherNKeys(int N, std::vector<int>& keys) {
			int dSize = (int)data.size();
			for (int i = 0, n = 0; i < dSize && n < N; ++i /*= (i+1) % dSize*/) {
				if (valid[i]) {
					++n;
					keys.push_back(i);
				}
			}
		}
	};

	template<typename KeyType>
	struct FilterNothing {
		bool pass(const KeyType& one, const KeyType& two) {
			return true;
		}
	};

}; // namespace SweepAndPrune

#endif
