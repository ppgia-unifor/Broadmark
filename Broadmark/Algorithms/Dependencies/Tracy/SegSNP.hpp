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
 *  SegSNP.hpp
 *  CollisionDetectionTestBed
 */

#ifndef SEGMENTED_HPP
#define SEGMENTED_HPP

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <deque>
#include <iostream>

#include "Utility.hpp"
#include <cmath>
#include <assert.h>

#define LOCALSORTONLY 1

const int CHUNKSIZE = 6;

namespace SweepAndPrune {

	template<typename Coordinate>
	struct extremaLocator {
		void * chunk;
		extrema<Coordinate> * p;
	};

	template<int D, typename Key, typename Coordinate>
	struct AABB {
		extremaLocator<Coordinate> mins[D];
		extremaLocator<Coordinate> maxs[D];
		Key objectKey;
		bool inUse;
		AABB() : inUse(false) {
			for (int d = 0; d < D; ++d) {
				mins[d].chunk = 0;
				maxs[d].chunk = 0;
			}
		}
		bool PairOverlaps(AABB<D, Key, Coordinate>& other, int skipD) {
			for (int d = D - 1; d >= 0; --d)
				if ((d != skipD) && (!IntervalWithin(*(mins[d].p), *(maxs[d].p), *(other.mins[d].p), *(other.maxs[d].p))))
					return false;
			return true;
		}
	};

	template<int D, typename Coordinate, typename Key, class Filter, class SAPType>
	struct OverlapManager {
		typedef std::pair<Key, Key> triangularIndex; // Smaller key is first of pair

		Filter filter;
		std::vector<triangularIndex> sortedOverlaps;
		std::vector<triangularIndex> overlapEvents;
		std::vector<triangularIndex> unoverlapEvents;
		SAPType * parent;

		OverlapManager(Filter _filter, SAPType * _parent) : filter(_filter), parent(_parent) {
		}
		OverlapManager(SAPType * _parent) : parent(_parent) {
		}

		static inline triangularIndex compute_triangular_index(const Key a, const Key b) {
			if (a < b)
				return std::make_pair(a, b);
			else
				return std::make_pair(b, a);
		}

		// Assumes both that the client wants these events cleared as well, and that he will pass in empty vectors
		void GatherEvents(std::vector<triangularIndex>& overs, std::vector<triangularIndex>& unovers) {
			overs.swap(overlapEvents);
			unovers.swap(unoverlapEvents);
		}

		#if LOCALSORTONLY
		void RegisterExpandingSwap(const int a, const int b, const int d) {
			// If other dimensions are overlapping
			if (parent->boxes[a].PairOverlaps(parent->boxes[b], d) && filter.pass(parent->boxes[a].objectKey, parent->boxes[b].objectKey)) {
				triangularIndex val = compute_triangular_index(a, b);
				overlapEvents.push_back(val);
			}
		}

		void RegisterContractingSwap(int a, int b, int d) {
			// If was overlapping before this change
			if (parent->boxes[a].PairOverlaps(parent->boxes[b], d) && filter.pass(parent->boxes[a].objectKey, parent->boxes[b].objectKey)) {
				triangularIndex val = compute_triangular_index(a, b);
				unoverlapEvents.push_back(val);
			}
		}
		#else
		void RegisterExpandingSwap(const int a, const int b, const int d) {
			// If other dimensions are overlapping
			if (parent->boxes[a].PairOverlaps(parent->boxes[b], d) && filter.pass(parent->boxes[a].objectKey, parent->boxes[b].objectKey)) {
				triangularIndex val = compute_triangular_index(a, b);
				typename std::vector<triangularIndex>::iterator ptr = std::lower_bound(sortedOverlaps.begin(), sortedOverlaps.end(), val);

				if (ptr == sortedOverlaps.end() || *ptr != val) {
					sortedOverlaps.insert(ptr, val);
					overlapEvents.push_back(val);
				}
			}
		}

		void RegisterContractingSwap(int a, int b, int d) {
			// If was overlapping before this change
			triangularIndex val = compute_triangular_index(a, b);
			typename std::vector<triangularIndex>::iterator ptr = std::lower_bound(sortedOverlaps.begin(), sortedOverlaps.end(), val);
			if (ptr != sortedOverlaps.end() && *ptr == val) {
				sortedOverlaps.erase(ptr);
				unoverlapEvents.push_back(val);
			}
		}
		#endif
	};

	template<class SAPType, typename Coordinate, int chunkSizeP2>
	class ul_chunk {
		typedef ul_chunk<SAPType, Coordinate, chunkSizeP2> ThisType;
	public:
		// Primary data (used for data storage and during local sort
		const static int numElementsPerChunk = 1 << chunkSizeP2;
		extrema<Coordinate> elements[numElementsPerChunk];
		int lastSentinelIndex;
		ThisType * previousChunk, *nextChunk;

		// From base
		int d;
		SAPType * parent;

		// set of overlapping entities at end sentinel, maintained when d == 0
		std::vector<int> checkPoints;

		inline void check_overlap_expand(int one, int two) {
			if (one != two)
				parent->overlapManager.RegisterExpandingSwap(one, two, d);
		}

		inline void check_overlap_contract(int one, int two) {
			if (one != two)
				parent->overlapManager.RegisterContractingSwap(one, two, d);
		}

		inline void check_overlap_fully_expand(int one, int two) {
			if (one != two)
				parent->overlapManager.RegisterExpandingSwap(one, two, 200); // give bogus dimension to skip: check all dimensions!
		}

		inline void check_overlap_fully_contract(int one, int two) {
			if (one != two)
				parent->overlapManager.RegisterContractingSwap(one, two, 200);
		}

		inline void place_extrema(extrema<Coordinate> * const dst, const extrema<Coordinate> * const src) {
			*dst = *src; // write the entire extrema struct to this location
			if (src->kind == extrema<Coordinate>::Minima)
				parent->boxes[src->aabbIndex].mins[d].p = dst; // update index
			else
				parent->boxes[src->aabbIndex].maxs[d].p = dst; // update index
		}

		inline void place_extrema_remote(extrema<Coordinate> * const loc, const extrema<Coordinate> & value, ThisType * newChunk) {
			*loc = value; // write the entire extrema struct to this location
			if (value.kind == extrema<Coordinate>::Minima) {
				parent->boxes[value.aabbIndex].mins[d].p = loc; // update index
				parent->boxes[value.aabbIndex].mins[d].chunk = newChunk;
			} else {
				parent->boxes[value.aabbIndex].maxs[d].p = loc; // update index
				parent->boxes[value.aabbIndex].maxs[d].chunk = newChunk;
			}
		}

		bool insertion_sort_forward_minima_continued(const extrema<Coordinate> subject, extrema<Coordinate> * emptySpot) {
			do {
				extrema<Coordinate> * val = (emptySpot + 1);
				if (val->kind == extrema<Coordinate>::Maxima)
					check_overlap_contract(val->aabbIndex, subject.aabbIndex);
				place_extrema(emptySpot, val);
				++emptySpot;
			} while (*(emptySpot + 1) < subject);
			place_extrema(emptySpot, &subject);
			return((emptySpot + 1)->kind == extrema<Coordinate>::Sentinel);
		}
		bool insertion_sort_forward_maxima_continued(const extrema<Coordinate> subject, extrema<Coordinate> * emptySpot) {
			do {
				extrema<Coordinate> * val = (emptySpot + 1);
				place_extrema(emptySpot, val);
				if (val->kind == extrema<Coordinate>::Minima)
					check_overlap_expand(val->aabbIndex, subject.aabbIndex);
				++emptySpot;
			} while (*(emptySpot + 1) < subject);
			place_extrema(emptySpot, &subject);
			return((emptySpot + 1)->kind == extrema<Coordinate>::Sentinel);
		}
		bool insertion_sort_backward_minima_continued(const extrema<Coordinate> subject, extrema<Coordinate> * emptySpot) {
			do {
				extrema<Coordinate> * val = (emptySpot - 1);
				place_extrema(emptySpot, val);
				if (val->kind == extrema<Coordinate>::Maxima)
					check_overlap_expand(val->aabbIndex, subject.aabbIndex);
				--emptySpot;
			} while (subject < *(emptySpot - 1));
			place_extrema(emptySpot, &subject);
			return((emptySpot - 1)->kind == extrema<Coordinate>::Sentinel);
		}
		bool insertion_sort_backward_maxima_continued(const extrema<Coordinate> subject, extrema<Coordinate> * emptySpot) {
			do {
				extrema<Coordinate> * val = (emptySpot - 1);
				if (val->kind == extrema<Coordinate>::Minima)
					check_overlap_contract(val->aabbIndex, subject.aabbIndex);
				place_extrema(emptySpot, val);
				--emptySpot;
			} while (subject < *(emptySpot - 1));
			place_extrema(emptySpot, &subject);
			return((emptySpot - 1)->kind == extrema<Coordinate>::Sentinel);
		}

		inline bool insertion_sort_backward_maxima(extrema<Coordinate> * here) {
			if (*here < *(here - 1))
				return insertion_sort_backward_maxima_continued(*here, here);
			else
				return((here - 1)->kind == extrema<Coordinate>::Sentinel);
		}
		inline bool insertion_sort_backward_minima(extrema<Coordinate> * here) {
			if (*here < *(here - 1))
				return insertion_sort_backward_minima_continued(*here, here);
			else
				return((here - 1)->kind == extrema<Coordinate>::Sentinel);
		}
		inline bool insertion_sort_forward_minima(extrema<Coordinate> * here) {
			if (*(here + 1) < *here)
				return insertion_sort_forward_minima_continued(*here, here);
			else
				return((here + 1)->kind == extrema<Coordinate>::Sentinel);
		}
		inline bool insertion_sort_forward_maxima(extrema<Coordinate> * here) {
			if (*(here + 1) < *here)
				return insertion_sort_forward_maxima_continued(*here, here);
			else
				return((here + 1)->kind == extrema<Coordinate>::Sentinel);
		}

		///////////////////////////////////////////////////////////

			// During local-sort, it's possible for a min-max to cross one another
			// and cross a checkpoint in the mean time, so it's necessary to remember
			// unoverlaps that don't exist in the checkpoint. (This is
			// a problem when objects are moving further than their size in one cycle).
		std::vector<int> uncountedRemoves;
	public:
		ul_chunk(int _d, SAPType * _parent)
			: d(_d), parent(_parent), lastSentinelIndex(1), previousChunk(0), nextChunk(0) {
			elements[0] = extrema<Coordinate>::sentinel(extrema<Coordinate>::Minima);
			elements[1] = extrema<Coordinate>::sentinel(extrema<Coordinate>::Maxima);
		}

		inline void addCheckPoint(int datum) {
			int i;
			for (i = 0; i < (int)uncountedRemoves.size(); ++i)
				if (uncountedRemoves[i] == datum)
					break;
			if (i != (int)uncountedRemoves.size()) {
				uncountedRemoves[i] = uncountedRemoves.back();
				uncountedRemoves.pop_back();
			} else
				checkPoints.push_back(datum);
		}
		inline void removeCheckPoint(int datum) {
			int size = (int)checkPoints.size(), i;
			for (i = 0; i < size; ++i)
				if (checkPoints[i] == datum)
					break;
			if (i != size) {
				checkPoints[i] = checkPoints.back();
				checkPoints.pop_back();
			} else
				uncountedRemoves.push_back(datum);
		}

		void globallySortChunk() {
			int i = 1;
			for (;;) {
				unsigned int k = elements[i].kind;
				if (k == extrema<Coordinate>::Minima)
					insertion_sort_backward_minima_across(&elements[i]);
				else if (k == extrema<Coordinate>::Maxima)
					insertion_sort_backward_maxima_across(&elements[i]);
				else
					break;

				++i;
			}
		}

		inline void insertion_sort_backward_minima_across(extrema<Coordinate> * p) {

			if (insertion_sort_backward_minima(p) && previousChunk != 0) {
				extrema<Coordinate> * remotePoint = &(previousChunk->elements[previousChunk->lastSentinelIndex - 1]);
				if (elements[1] < *remotePoint) {
					if (d == 0) {
						// Swap them across here, and update the checkPoints in this dimension if necessary
						if (elements[1].kind == extrema<Coordinate>::Minima)
							previousChunk->addCheckPoint(elements[1].aabbIndex);
						else
							previousChunk->removeCheckPoint(elements[1].aabbIndex);
						if (remotePoint->kind == extrema<Coordinate>::Minima)
							previousChunk->removeCheckPoint(remotePoint->aabbIndex);
						else
							previousChunk->addCheckPoint(remotePoint->aabbIndex);
					}

					if (remotePoint->kind == extrema<Coordinate>::Maxima)
						check_overlap_expand(elements[1].aabbIndex, remotePoint->aabbIndex);

					extrema<Coordinate> temp = *remotePoint;
					previousChunk->place_extrema_remote(remotePoint, elements[1], previousChunk);
					place_extrema_remote(&elements[1], temp, this);

					previousChunk->insertion_sort_backward_minima_across(remotePoint);
				}
			}
		}

		inline void insertion_sort_backward_maxima_across(extrema<Coordinate> * p) {

			if (insertion_sort_backward_maxima(p) && previousChunk != 0) {
				extrema<Coordinate> * remotePoint = &(previousChunk->elements[previousChunk->lastSentinelIndex - 1]);
				if (elements[1] < *remotePoint) {
					if (d == 0) {
						// Update the checkPoints in this dimension if necessary
						if (elements[1].kind == extrema<Coordinate>::Minima)
							previousChunk->addCheckPoint(elements[1].aabbIndex);
						else
							previousChunk->removeCheckPoint(elements[1].aabbIndex);
						if (remotePoint->kind == extrema<Coordinate>::Minima)
							previousChunk->removeCheckPoint(remotePoint->aabbIndex);
						else
							previousChunk->addCheckPoint(remotePoint->aabbIndex);
					}

					if (remotePoint->kind == extrema<Coordinate>::Minima)
						check_overlap_contract(elements[1].aabbIndex, remotePoint->aabbIndex);

					extrema<Coordinate> temp = *remotePoint;
					previousChunk->place_extrema_remote(remotePoint, elements[1], previousChunk);
					place_extrema_remote(&elements[1], temp, this);

					previousChunk->insertion_sort_backward_maxima_across(remotePoint);
				}
			}
		}

		inline void insertion_sort_forward_minima_across(extrema<Coordinate> * p) {

			if (insertion_sort_forward_minima(p) && nextChunk != 0) {
				extrema<Coordinate> * remotePoint = &(nextChunk->elements[1]);
				if (*remotePoint < elements[lastSentinelIndex - 1]) {
					if (d == 0) {
						// Swap them across here, and update my checkPoints in this dimension if necessary
						if (elements[lastSentinelIndex - 1].kind == extrema<Coordinate>::Minima)
							removeCheckPoint(elements[lastSentinelIndex - 1].aabbIndex);
						else
							addCheckPoint(elements[lastSentinelIndex - 1].aabbIndex);
						if (remotePoint->kind == extrema<Coordinate>::Minima)
							addCheckPoint(remotePoint->aabbIndex);
						else
							removeCheckPoint(remotePoint->aabbIndex);
					}

					if (remotePoint->kind == extrema<Coordinate>::Maxima)
						check_overlap_contract(elements[lastSentinelIndex - 1].aabbIndex, remotePoint->aabbIndex);

					extrema<Coordinate> temp = *remotePoint;
					nextChunk->place_extrema_remote(remotePoint, elements[lastSentinelIndex - 1], nextChunk);
					place_extrema_remote(&elements[lastSentinelIndex - 1], temp, this);

					nextChunk->insertion_sort_forward_minima_across(remotePoint);
				}
			}
		}

		inline void insertion_sort_forward_maxima_across(extrema<Coordinate> * p) {

			if (insertion_sort_forward_maxima(p) && nextChunk != 0) {
				extrema<Coordinate> * remotePoint = &(nextChunk->elements[1]);
				if (*remotePoint < elements[lastSentinelIndex - 1]) {
					if (d == 0) {
						// Update my checkPoints in this dimension if necessary
						if (elements[lastSentinelIndex - 1].kind == extrema<Coordinate>::Minima)
							removeCheckPoint(elements[lastSentinelIndex - 1].aabbIndex);
						else
							addCheckPoint(elements[lastSentinelIndex - 1].aabbIndex);
						if (remotePoint->kind == extrema<Coordinate>::Minima)
							addCheckPoint(remotePoint->aabbIndex);
						else
							removeCheckPoint(remotePoint->aabbIndex);
					}

					if (remotePoint->kind == extrema<Coordinate>::Minima)
						check_overlap_expand(elements[lastSentinelIndex - 1].aabbIndex, remotePoint->aabbIndex);

					extrema<Coordinate> temp = *remotePoint;
					nextChunk->place_extrema_remote(remotePoint, elements[lastSentinelIndex - 1], nextChunk);
					place_extrema_remote(&elements[lastSentinelIndex - 1], temp, this);

					nextChunk->insertion_sort_forward_maxima_across(remotePoint);
				}
			}
		}

		inline void update_minima(extrema<Coordinate> * p, Coordinate newLocation) {
			// expanding, going left
			if (newLocation < p->location) {
				p->location = newLocation;
				insertion_sort_backward_minima_across(p);
			}
			// contracting, going right
			else {
				p->location = newLocation;
				insertion_sort_forward_minima_across(p);
			}
		}
		inline void update_maxima(extrema<Coordinate> * p, Coordinate newLocation) {
			// contracting, going left
			if (newLocation < p->location) {
				p->location = newLocation;
				insertion_sort_backward_maxima_across(p);
			}
			// expanding, going right
			else {
				p->location = newLocation;
				insertion_sort_forward_maxima_across(p);
			}
		}

		void insert_minima_or_maxima(int aabbIndex, Coordinate loc, typename extrema<Coordinate>::Which kind) {
			elements[lastSentinelIndex + 1] = elements[lastSentinelIndex];
			++lastSentinelIndex;

			extrema<Coordinate> p(loc, kind, aabbIndex);
			extrema<Coordinate> * src = &(elements[lastSentinelIndex - 2]);
			while (p < *src) {
				place_extrema(src + 1, src); // overlaps handled afterwards
				--src;
			}
			// New element goes at src+1
			place_extrema_remote(src + 1, p, this);
		}

		void insert_minima_and_maxima(int aabbIndex, Coordinate minLoc, Coordinate maxLoc) {
			elements[lastSentinelIndex + 2] = elements[lastSentinelIndex];
			lastSentinelIndex += 2;

			extrema<Coordinate> p(maxLoc, extrema<Coordinate>::Maxima, aabbIndex);
			extrema<Coordinate> * src = &(elements[lastSentinelIndex - 3]);
			while (p < *src) {
				place_extrema(src + 2, src);
				--src;
			}
			// New element goes at src+2
			place_extrema_remote(src + 2, p, this);

			// Start search for minima location
			extrema<Coordinate> p2(minLoc, extrema<Coordinate>::Minima, aabbIndex);
			while (p2 < *src) {
				place_extrema(src + 1, src);
				--src;
			}
			place_extrema_remote(src + 1, p2, this);
		}

		void erase_minima_or_maxima(extrema<Coordinate> * p) {
			extrema<Coordinate> * stop = &(elements[lastSentinelIndex - 1]);
			while (p != stop) {
				place_extrema(p, (p + 1));
				++p;
			}
			elements[lastSentinelIndex - 1] = elements[lastSentinelIndex];
			--lastSentinelIndex;
		}

		void erase_minima_and_maxima(extrema<Coordinate> * minP, extrema<Coordinate> * maxP) {
			extrema<Coordinate> * stop = maxP - 1;
			while (minP != stop) {
				place_extrema(minP, (minP + 1));
				++minP;
			}

			stop = &(elements[lastSentinelIndex - 2]);
			while (minP != stop) {
				place_extrema(minP, (minP + 2));
				++minP;
			}

			elements[lastSentinelIndex - 2] = elements[lastSentinelIndex];
			lastSentinelIndex -= 2;
		}

		inline ThisType * next() {
			return nextChunk;
		}

		bool room_for(int i) {
			return(lastSentinelIndex < (numElementsPerChunk - i));
		}

		// Assumes the check is sequential (the ones before were already checked)
		bool in_range(Coordinate val) {
			if (val < elements[lastSentinelIndex - 1].location || nextChunk == 0 || val < nextChunk->elements[1].location)
				return true;
			else
				return false;
		}

		void get_keys(extrema<Coordinate> * from, extrema<Coordinate> * to, typename extrema<Coordinate>::Which kind, std::vector<int>& keys) {
			while (from != to) {
				if (from->kind == (unsigned int)kind)
					keys.push_back(from->aabbIndex);
				from++;
			}
		}

		Coordinate get_loc(int index) {
			return elements[index].location;
		}

		extrema<Coordinate>& get_upperValue() {
			return elements[lastSentinelIndex - 1];
		}

		// Insert new chunk after current one
		ThisType * split_forward() {
			ThisType * newChunk = new ThisType(d, parent);

			newChunk->nextChunk = nextChunk;
			newChunk->previousChunk = this;
			if (nextChunk != 0)
				nextChunk->previousChunk = newChunk;
			nextChunk = newChunk;

			// Copy about half the elements over
			int srcStart = 1 << (chunkSizeP2 - 1), dest = 1, src = srcStart;
			if (d == 0) {
				// The new guy inherits my checkPoints (my former checkpoint position is his new one)
				newChunk->checkPoints.insert(newChunk->checkPoints.end(), checkPoints.begin(), checkPoints.end());

				// Effectively "move" my checkPoint position backward to my new point
				for (int i = lastSentinelIndex - 1; i >= srcStart; --i) {
					if (elements[i].kind == extrema<Coordinate>::Maxima)
						addCheckPoint(elements[i].aabbIndex);
					else
						removeCheckPoint(elements[i].aabbIndex);
				}
			}
			while (src != lastSentinelIndex) {
				newChunk->place_extrema_remote(&(newChunk->elements[dest]), elements[src], newChunk);
				++src; ++dest;
			}
			// Copy sentinel to proper location on both
			newChunk->elements[dest] = elements[src];
			newChunk->lastSentinelIndex = dest;
			elements[srcStart] = elements[src];
			lastSentinelIndex = srcStart;

			return newChunk;
		}

		// Merge two chunks by consuming the next guy from 'this'
		ThisType * merge_forward() {
			// Copy elements over
			int to = lastSentinelIndex, from = 1, stop = nextChunk->lastSentinelIndex;
			int newlastSentinelIndex = lastSentinelIndex + nextChunk->lastSentinelIndex - 1;
			elements[newlastSentinelIndex] = elements[lastSentinelIndex];
			lastSentinelIndex = newlastSentinelIndex;
			while (from < stop)
				place_extrema_remote(&(elements[to++]), nextChunk->elements[from++], this);

			// The combined chunk's checkPoints = the consumed guy's checkPoints
			if (d == 0)
				checkPoints.swap(nextChunk->checkPoints);

			// Excise victim
			ThisType * victim = nextChunk;
			nextChunk = nextChunk->nextChunk;
			if (nextChunk != 0)
				nextChunk->previousChunk = this;
			return(victim);
		}
	};


	// Class handles single dimension: D are used.
	template<typename Key, class SAPType, typename Coordinate>
	class ulist {
	public:
		typedef ul_chunk<SAPType, Coordinate, CHUNKSIZE> chunk_type;
	private:
		chunk_type * headChunk;
		std::vector<chunk_type*> chunkRefs;

		// Info from the base
		SAPType * parent;
		int d;

	public:
		ulist() : headChunk(0), parent(0) {
		}
		~ulist() {
			// Deallocate the chunks
			chunk_type * ptr, *p = headChunk;
			while (p != 0) {
				ptr = p->nextChunk;
				delete(p);
				p = ptr;
			}
		}

		void connectToParent(int _d, SAPType * _parent) {
			d = _d;
			parent = _parent;

			clear();
		}

		bool check_split(int i, int room_needed) {
			if (!chunkRefs[i]->room_for(room_needed)) {
				chunkRefs.resize(chunkRefs.size() + 1);
				for (int k = (int)chunkRefs.size() - 2; k != i; --k) {
					chunkRefs[k + 1] = chunkRefs[k];
				}

				chunkRefs[i + 1] = chunkRefs[i]->split_forward();
				return true;
			}
			return false;
		}

		chunk_type * check_merge(chunk_type * here) {
			// I CANNOT allow a chunk to be empty, or else swaps will not occur across neighbors
			chunk_type * deleted = 0;
			if (here->nextChunk != 0 && (here->nextChunk->lastSentinelIndex == 1 || (here->lastSentinelIndex + here->nextChunk->lastSentinelIndex) < (here->numElementsPerChunk >> 1)))
				deleted = here->merge_forward(); // "nextChunk" will be deleted
			else if (here->previousChunk != 0 && (here->lastSentinelIndex == 1 || (here->lastSentinelIndex + here->previousChunk->lastSentinelIndex) < (here->numElementsPerChunk >> 1)))
				deleted = here->previousChunk->merge_forward(); // "here" chunk will be deleted
			if (deleted != 0)
				chunkRefs.erase(std::find(chunkRefs.begin(), chunkRefs.end(), deleted));

			return deleted;
		}

		// This will only be called on dimension '0' after extrema are inserted and before erased
		void overlap_work_for_dynamics(int aabbIndex, bool insertion) {
			// Do overlap tests and checkpoint maintenance across segments
			std::vector<int> keys;
			chunk_type * minChunk = static_cast<chunk_type*>(parent->boxes[aabbIndex].mins[0].chunk);
			chunk_type * maxChunk = static_cast<chunk_type*>(parent->boxes[aabbIndex].maxs[0].chunk);

			if ((!insertion) && minChunk != maxChunk)
				minChunk->removeCheckPoint(aabbIndex);
			minChunk->get_keys( /*from*/ parent->boxes[aabbIndex].mins[0].p + 1, /*to*/ &(minChunk->elements[minChunk->lastSentinelIndex]), extrema<Coordinate>::Maxima, keys);
			keys.insert(keys.end(), minChunk->checkPoints.begin(), minChunk->checkPoints.end()); // Include checkpoints set for this one only
			std::sort(keys.begin(), keys.end()); keys.erase(std::unique(keys.begin(), keys.end()), keys.end()); // Unique the keys so far, the rest will be new
			int size = (int)keys.size();
			for (int i = 0; i < size; ++i) if (keys[i] == aabbIndex) {
				keys[i] = keys.back(); keys.pop_back(); break;
			} // Eliminate self collision on insert
			if (minChunk != maxChunk) {
				if (insertion)
					minChunk->addCheckPoint(aabbIndex);
				minChunk = minChunk->nextChunk;
				while (minChunk != maxChunk) {
					minChunk->get_keys(&(minChunk->elements[1]), &(minChunk->elements[minChunk->lastSentinelIndex]), extrema<Coordinate>::Minima, keys);
					if (insertion)
						minChunk->addCheckPoint(aabbIndex);
					else
						minChunk->removeCheckPoint(aabbIndex);
					minChunk = minChunk->nextChunk;
				}
				minChunk->get_keys(&(minChunk->elements[1]), parent->boxes[aabbIndex].maxs[0].p, extrema<Coordinate>::Minima, keys);
			}
			if (insertion)
				for (int i = 0; i < (int)keys.size(); ++i)
					chunkRefs[0]->check_overlap_fully_expand(aabbIndex, keys[i]); // Any chunk * will do. Should do something about this.
			else
				for (int i = 0; i < (int)keys.size(); ++i)
					chunkRefs[0]->check_overlap_fully_contract(aabbIndex, keys[i]); // These guys have to check *all* dimensions (for now) since
					// I'm not guaranteed to be overlapping in 'd'! (I could eliminate this problem with a better traversal during dynamics)
		}

		void insert(int aabbIndex, Coordinate min, Coordinate max) {
			assert(min < max);
			extrema<Coordinate> minPoint(min, extrema<Coordinate>::Minima, aabbIndex); // Must compare full extremas during insertion for correctness: consider passing these down instead of making more

			int i, largest = (int)chunkRefs.size() - 1;
			for (i = 0; i < largest; ++i)
				if (minPoint < chunkRefs[i]->get_upperValue())
					break;

			extrema<Coordinate> maxPoint(max, extrema<Coordinate>::Maxima, aabbIndex);
			int j, rSize = (int)chunkRefs.size() - 1;
			for (j = i; j < rSize; ++j)
				if (maxPoint < chunkRefs[j]->get_upperValue())
					break;

			if (j == i) {
				if (check_split(i, 2)) // Make sure we have room to insert another one
					insert(aabbIndex, min, max);
				else
					chunkRefs[i]->insert_minima_and_maxima(aabbIndex, min, max);
			} else {
				// Have to split backward to not change indices
				bool split = check_split(j, 1);
				split = split || check_split(i, 1);

				if (split)
					insert(aabbIndex, min, max);
				else {
					chunkRefs[i]->insert_minima_or_maxima(aabbIndex, min, extrema<Coordinate>::Minima);
					chunkRefs[j]->insert_minima_or_maxima(aabbIndex, max, extrema<Coordinate>::Maxima);
				}
			}
		}

		void erase(extremaLocator<Coordinate>& min, extremaLocator<Coordinate>& max) {
			chunk_type * minChunk = static_cast<chunk_type*>(min.chunk);
			chunk_type * maxChunk = static_cast<chunk_type*>(max.chunk);

			if (minChunk == maxChunk) {
				minChunk->erase_minima_and_maxima(min.p, max.p);
				delete(check_merge(minChunk));
			} else {
				minChunk->erase_minima_or_maxima(min.p);
				maxChunk->erase_minima_or_maxima(max.p);
				chunk_type * deleted = check_merge(minChunk);
				if (deleted != maxChunk)
					delete(check_merge(maxChunk));
				delete(deleted);
			}
		}

		void update(int aabbIndex, Coordinate min, Coordinate max) {
			chunk_type * minChunk = static_cast<chunk_type*>(parent->boxes[aabbIndex].mins[d].chunk);
			extrema<Coordinate> * minP = parent->boxes[aabbIndex].mins[d].p;
			minChunk->update_minima(minP, min);

			chunk_type * maxChunk = static_cast<chunk_type*>(parent->boxes[aabbIndex].maxs[d].chunk);
			extrema<Coordinate> * maxP = parent->boxes[aabbIndex].maxs[d].p;
			maxChunk->update_maxima(maxP, max);
		}

		void globalSort() {
			chunk_type * chPtr = headChunk;
			while (chPtr != 0) {
				chPtr->globallySortChunk();
				chPtr = chPtr->nextChunk;
			}
		}

		void clear() {
			// Deallocate the chunks
			chunk_type * ptr, *p = headChunk;
			while (p != 0) {
				ptr = p->nextChunk;
				delete(p);
				p = ptr;
			}
			chunkRefs.clear();

			headChunk = new chunk_type(d, parent);
			chunkRefs.push_back(headChunk);
		}
	};

	template<int D, typename Key, typename Coordinate, class Filter>
	class Segmented {
		typedef Segmented<D, Key, Coordinate, Filter> ThisType;
		typedef std::deque<AABB<D, Key, Coordinate> > BoxesType;
		typedef OverlapManager<D, Coordinate, int, Filter, ThisType> OverlapManagerType;
		friend struct OverlapManager<D, Coordinate, int, Filter, ThisType>;
		typedef ulist<Key, ThisType, Coordinate> IntervalArrayType;
		friend class ulist<Key, ThisType, Coordinate>;
		friend class ul_chunk<ThisType, Coordinate, CHUNKSIZE>;
		typedef typename IntervalArrayType::chunk_type chunk_type;
		std::vector<int> updatesBufferIndices;
		std::vector<Coordinate> updatesBufferLocations[D];

		std::vector<int> erasesBufferIndices;

		std::vector<int> insertsBufferIndices;
		std::vector<Key> insertsBufferKeys;
		std::vector<Coordinate> insertsBufferLocations[D];

		IntervalArrayType dimension[D];
		OverlapManagerType overlapManager;
		BoxesType boxes;
		std::vector<int> free_indexes, empty_indexes; // Manages the free indexes as keys are added/deleted
		bool debugging;
		Coordinate spatialBoundsMin[D], spatialBoundsMax[D];

		bool globalSort;
		int entityCount;

	public:
		static const bool event_based_output = true;
		static const bool threaded = false;
		Segmented() : overlapManager(this), globalSort(false), entityCount(0), debugging(false) {
			clear();
			for (int d = 0; d < D; ++d)
				dimension[d].connectToParent(d, this);
		}

		Segmented(Filter _filter) : overlapManager(_filter, this), globalSort(false), entityCount(0), debugging(false) {
			clear();
			for (int d = 0; d < D; ++d) {
				dimension[d].connectToParent(d, this);
			}
		}

		int numBoxes() {
			return boxes.size();
		}

		void AcquireAABB(int key, Coordinate min[], Coordinate max[]) {
			for (int d = 0; d < D; ++d) {
				min[d] = boxes[key].mins[d].p->location;
				max[d] = boxes[key].maxs[d].p->location;
			}
		}

		void LookupOverlapKeys(std::vector<std::pair<int, int> >& source, std::vector<std::pair<Key, Key> >& dest) {
			int size = (int)source.size();
			for (int i = 0; i < size; ++i) {
				int one = source[i].first;
				int two = source[i].second;
				Key keyOne = boxes[one].objectKey, keyTwo = boxes[two].objectKey;
				dest.push_back(std::make_pair(std::min(keyOne, keyTwo), std::max(keyOne, keyTwo)));
			}
		}

		void gather(std::vector<std::pair<Key, Key> >& overlapEvents, std::vector<std::pair<Key, Key> >& unoverlapEvents) {
			std::vector<std::pair<int, int> > overs, unovers;
			overlapManager.GatherEvents(overs, unovers);
			LookupOverlapKeys(overs, overlapEvents);
			LookupOverlapKeys(unovers, unoverlapEvents);

			free_indexes.insert(free_indexes.end(), empty_indexes.begin(), empty_indexes.end());
			empty_indexes.clear();
		}

		int insert(const Key& key, Coordinate min[], Coordinate max[]) {
			++entityCount;

			if (free_indexes.empty()) {
				free_indexes.push_back((int)boxes.size());
				boxes.resize(boxes.size() + 1);
			}
			int index = free_indexes.back();
			free_indexes.pop_back();

			insertsBufferIndices.push_back(index);
			insertsBufferKeys.push_back(key);
			for (int d = 0; d < D; ++d) {
				assert(min[d] < max[d]);
				insertsBufferLocations[d].push_back(min[d]);
				insertsBufferLocations[d].push_back(max[d]);
			}

			boxes[index].inUse = true;
			return index;
		}

		void FlushInsertions() {
			typename std::vector<int>::iterator indexPtr = insertsBufferIndices.begin(), endPtr = insertsBufferIndices.end();
			typename std::vector<Key>::iterator keyPtr = insertsBufferKeys.begin();
			typename std::vector<Coordinate>::iterator locPtr[D];
			for (int d = 0; d < D; ++d)
				locPtr[d] = insertsBufferLocations[d].begin();

			while (indexPtr != endPtr) {
				int index = *indexPtr++;
				Key key = *keyPtr++;
				boxes[index].objectKey = key;
				boxes[index].inUse = true;

				for (int d = 0; d < D; ++d) {
					Coordinate min = *locPtr[d]++;
					Coordinate max = *locPtr[d]++;

					dimension[d].insert(index, min, max);
				}

				// Checks overlaps, maintains checkpoints
				dimension[0].overlap_work_for_dynamics(index, true /*insertions*/);
			}

			insertsBufferIndices.clear();
			insertsBufferKeys.clear();
			for (int d = 0; d < D; ++d)
				insertsBufferLocations[d].clear();
		}

		void erase(int index) {
			// Make sure we're erasing a valid object
			assert(boxes[index].inUse);

			--entityCount;

			erasesBufferIndices.push_back(index);
			// Invalidate box: this really only needs to be done for debugging
			boxes[index].inUse = false; // The min/max pointers remain: must compare aabb's during processing
		}

		void FlushErases() {
			typename std::vector<int>::iterator indexPtr = erasesBufferIndices.begin(), endPtr = erasesBufferIndices.end();

			while (indexPtr != endPtr) {
				int index = *indexPtr++;
				empty_indexes.push_back(index);

				// Checks overlaps, maintains checkpoints
				dimension[0].overlap_work_for_dynamics(index, false /*erases*/);

				for (int d = 0; d < D; ++d)
					dimension[d].erase(boxes[index].mins[d], boxes[index].maxs[d]);

				// Invalidate box: this really only needs to be done for debugging
				boxes[index].inUse = false; // The min/max pointers remain: must compare aabb's during processing

			}

			erasesBufferIndices.clear();
		}

		void UpdateAABB(int index, Coordinate min[], Coordinate max[]) {
			// Make sure we're updating a valid object
			assert(boxes[index].inUse);

			updatesBufferIndices.push_back(index);
			for (int d = 0; d < D; ++d) {
				updatesBufferLocations[d].push_back(min[d]);
				updatesBufferLocations[d].push_back(max[d]);
			}
		}

		// This seems to be faster for global sort and slightly faster for local sort, rather
		// than going column by column. This necessitates the OverlapManager which tracks
		// overlap status explicitly
		void FlushUpdates() {
			// Calculate whether global sort should be used this cycle
			// Use a global sort if # of updates is at least a quarter # of entities?
			#if LOCALSORTONLY
			globalSort = false;
			#else
			globalSort = (float(updatesBufferIndices.size()) / float(entityCount)) > 0.25;
			#endif

			typename std::vector<int>::iterator indexPtr = updatesBufferIndices.begin(), endPtr = updatesBufferIndices.end();
			typename std::vector<Coordinate>::iterator locPtr[D];
			for (int d = 0; d < D; ++d)
				locPtr[d] = updatesBufferLocations[d].begin();

			if (globalSort) {
				while (indexPtr != endPtr) {
					int index = *indexPtr++;
					extremaLocator<Coordinate> * mins = boxes[index].mins, *maxs = boxes[index].maxs;
					for (int d = 0; d < D; ++d) {
						mins[d].p->location = *locPtr[d]++;
						maxs[d].p->location = *locPtr[d]++;
					}
				}
			} else
				while (indexPtr != endPtr) {
					int index = *indexPtr++;
					for (int d = 0; d < D; ++d) {
						Coordinate min = *locPtr[d]++;
						Coordinate max = *locPtr[d]++;
						dimension[d].update(index, min, max);
					}
				}
			updatesBufferIndices.clear();
			for (int d = 0; d < D; ++d)
				updatesBufferLocations[d].clear();

			if (globalSort)
				for (int d = 0; d < D; ++d)
					dimension[d].globalSort();

		}

		void SetConfiguration(Coordinate spatialBounds[], int objectCount, int _numThreads) {
			Coordinate min[D], max[D];
			for (int d = 0; d < D; ++d) {
				min[d] = spatialBounds[2 * d];
				max[d] = spatialBounds[2 * d + 1];
			}
			SetSpatialBounds(min, max);
		}

		void SetSpatialBounds(Coordinate boundsMin[], Coordinate boundsMax[]) {
			for (int d = 0; d < D; ++d) {
				spatialBoundsMin[d] = boundsMin[d];
				spatialBoundsMax[d] = boundsMax[d];
			}
		}

		bool PointBelongsToSpace(Coordinate point[]) {
			int d;
			for (d = 0; d < D; ++d)
				if (point[d] < spatialBoundsMin[d] || point[d] >= spatialBoundsMax[d])
					break;
			return(d == D);
		}

		bool SpaceFullyContainsAABB(Coordinate min[], Coordinate max[]) {
			for (int d = 0; d < D; ++d)
				if (min[d] <= spatialBoundsMin[d] || max[d] >= spatialBoundsMax[d])
					return false;
			return true;
		}

		void GatherEvents(std::vector<std::pair<Key, Key> >& overlapEvents, std::vector<std::pair<Key, Key> >& unoverlapEvents) {
			FlushInsertions();
			FlushUpdates();
			FlushErases();
			gather(overlapEvents, unoverlapEvents);
		}

		void clear() {
			for (unsigned int d = 0; d < D; ++d)
				dimension[d].clear();

			boxes.clear();
			free_indexes.clear();
			empty_indexes.clear();
		}
	};

}; // namespace SweepAndPruneCD

#endif
