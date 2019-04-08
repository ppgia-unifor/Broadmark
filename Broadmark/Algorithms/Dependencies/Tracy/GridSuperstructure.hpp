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
 *  GridSuperstructure.hpp
 *  CollisionDetectionTestBed
 */

#ifndef GRIDSUBDIVIDER_HPP
#define GRIDSUBDIVIDER_HPP

#include "Utility.hpp"
#include <limits>
 //#include <pthread.h> // threadPolicy
#include <map>
#include <cmath>

namespace SweepAndPrune {

	struct space_space_predicate {
		int reservedKeySpace;
		bool operator()(const int first, const int second) const {
			return(first < reservedKeySpace && second < reservedKeySpace);
		}
	};

	// Clamp a value to min (inclusive) and max (exclusive)
	static int range_clamp(int value, int min, int max) {
		if (value < min)
			return min;
		else if (value >= max)
			return max - 1;
		else
			return value;
	}

	template< template<int sD, typename sClientKey, typename sCoordinate, class sFilter> class SubdomainAlgorithm, bool triggered_migrations, int D, typename ClientKey, typename Coordinate, class ThreadingPolicy, class ClientFilter = FilterNothing<ClientKey> >
	class GridSubdivider {

		struct avatar {
			int spatialDomain;
			int cellKey;
			avatar() : spatialDomain(0), cellKey(0) {
			}
			avatar(int domain, int key) : spatialDomain(domain), cellKey(key) {
			}
			bool operator<(const avatar& rhs) const {
				return spatialDomain < rhs.spatialDomain;
			}
		};

		struct objectInfo {
			std::vector<avatar> avatars; // Always sorted if *not* using triggered_migrations
			ClientKey clientKey;
		};

		struct FilterFunction {
			ClientFilter clientFilter;
			int reservedKeySpace;
			array_map<objectInfo>& objects;
			FilterFunction(array_map<objectInfo>& _objects, int _reservedKeySpace) : objects(_objects), reservedKeySpace(_reservedKeySpace) {
			}
			bool pass(int one, int two) {
				int result = ((one < reservedKeySpace) << 1) | (two < reservedKeySpace);
				if (result == 0)
					return(clientFilter.pass(objects[one - reservedKeySpace].clientKey, objects[two - reservedKeySpace].clientKey));
				else
					return(result != 3);
			}
		};

		struct spaceInfo {
			int index;
			int coordinates[D];
			spaceInfo(int _index, int _coordinates[]) : index(_index) {
				for (int d = 0; d < D; ++d) coordinates[d] = _coordinates[d];
			}
		};

		typedef SubdomainAlgorithm<D, int, Coordinate, FilterFunction> SubdomainAlgorithmType;
		typedef GridSubdivider<SubdomainAlgorithm, triggered_migrations, D, ClientKey, Coordinate, ThreadingPolicy, ClientFilter> ThisType;

		array_map<objectInfo> objects;
		SweepAndPrune::BatchedSet<std::pair<ClientKey, ClientKey> > candidates;

		Coordinate spatialMinima[D], spatialMaxima[D];

		float cellSize[D];
		float inverseCellSize[D];
		int totalCells;
		int numCells[D];
		std::vector<SubdomainAlgorithmType*> spaces;
		std::vector<spaceInfo> spaceCoordinates;

		std::vector<int> spaceToThread;
		std::vector<int> threadToSpacesMin;
		std::vector<int> threadToSpacesMax;
		int numThreads;

		int reservedKeySpace;

		std::vector<int> erasesBuffer;

		void ComputeSpatialBounds(const int range[], Coordinate min[], Coordinate max[]) {
			for (int d = 0; d < D; ++d) {
				min[d] = spatialMinima[d] + range[d] * cellSize[d];
				max[d] = min[d] + cellSize[d];
			}
		}

		void AddSpatialBounds(int space, int neighborIndex, const int neighborRange[]) {
			for (int d = 0; d < D; ++d)
				if (neighborRange[d] < 0 || neighborRange[d] == numCells[d])
					return;

			Coordinate min[D], max[D];
			ComputeSpatialBounds(neighborRange, min, max);

			spaces[space]->insert(neighborIndex, min, max);
		}

		void GenerateSpaceIndices(int minIndex[], int maxIndex[], std::vector<spaceInfo>& result) {
			int coordinates[D]; for (int d = 0; d < D; ++d) coordinates[d] = minIndex[d];
			int dIndex = 0; bool done = false;
			do {
				// Compute the next index using 'range' and numCells
				int index = 0; int factor = 1;
				for (int d = 0; d < D; ++d) {
					index += coordinates[d] * factor;
					factor *= numCells[d];
				}

				// Accumulate result
				result.push_back(spaceInfo(index, coordinates));

				// Increment coordinates for D dimensions
				coordinates[dIndex]++;
				if (coordinates[dIndex] == maxIndex[dIndex]) {
					do {
						coordinates[dIndex] = 0;
						dIndex++;
						if (dIndex == D) {
							done = true;
							break;
						}
						coordinates[dIndex]++;
					} while (coordinates[dIndex] == maxIndex[dIndex]);
					dIndex = 0;
				}
			} while (!done);
		}

		// Used to map spatial coordinates to Grid cells in a k-dimensional way
		template<int k>
		struct typeD {
		};
		void determineCells(const Coordinate min[], const Coordinate max[], const int offset, typeD<0>, std::vector<int>& cells) {
			int cmin = range_clamp(int((min[0] - spatialMinima[0]) * inverseCellSize[0]), 0, numCells[0]);
			int cmax = range_clamp(int((max[0] - spatialMinima[0]) * inverseCellSize[0]), 0, numCells[0]);
			for (int x = cmin; x <= cmax; ++x) {
				int cell = offset + x;
				cells.push_back(cell);
			}
		}
		template<int dim>
		void determineCells(const Coordinate min[], const Coordinate max[], const int offset, typeD<dim>, std::vector<int>& cells) {
			int cmin = range_clamp(int((min[dim] - spatialMinima[dim]) * inverseCellSize[dim]), 0, numCells[dim]);
			int cmax = range_clamp(int((max[dim] - spatialMinima[dim]) * inverseCellSize[dim]), 0, numCells[dim]);
			int factor = numCells[0];
			for (int d = 1; d < dim; ++d)
				factor *= numCells[d];

			int new_offset = offset + cmin * factor;
			for (int k = cmin; k <= cmax; ++k) {
				determineCells(min, max, new_offset, typeD<dim - 1>(), cells);
				new_offset += factor;
			}
		}
		void  determineCells(Coordinate min[], Coordinate max[], std::vector<int>& cells) {
			determineCells(min, max, 0, typeD<D - 1>(), cells);
		}

		void perform_spatial_overlap(std::pair<int, int>& overlap) {
			int space = overlap.first, entity = overlap.second - reservedKeySpace;

			if (entity < 0) // Space-space collision
				return;

			int s, avSize = (int)objects[entity].avatars.size();
			for (s = 0; s < avSize; ++s)
				if (objects[entity].avatars[s].spatialDomain == space)
					break;
			if (s == avSize && avSize != 0 /* This is for erased items that are also updated, causing empty avatars vector */) {
				// Get the position of this thing
				Coordinate min[D], max[D];
				spaces[objects[entity].avatars[0].spatialDomain]->AcquireAABB(objects[entity].avatars[0].cellKey, min, max);

				// Add to now-overlapping space
				int subKey = spaces[space]->insert(entity + reservedKeySpace, min, max);
				objects[entity].avatars.push_back(avatar(space, subKey));
			}
		}

		void perform_spatial_unoverlap(std::pair<int, int>& unoverlap) {
			int space = unoverlap.first, entity = unoverlap.second - reservedKeySpace;

			if (entity < 0) // Space-space collision
				return;

			int s;
			for (s = 0; s < (int)objects[entity].avatars.size(); ++s)
				if (objects[entity].avatars[s].spatialDomain == space)
					break;
			if (s != (int)objects[entity].avatars.size()) {
				spaces[objects[entity].avatars[s].spatialDomain]->erase(objects[entity].avatars[s].cellKey); // single-threaded for now
				objects[entity].avatars[s] = objects[entity].avatars.back();
				objects[entity].avatars.pop_back();
			}
		}

		void strip_spatial_entries(std::vector<std::pair<int, int> >& vec) {
			int to = 0;
			int from = 0;
			while (from < vec.size()) {
				if (vec[from].first >= reservedKeySpace)
					vec[to++] = vec[from];
				++from;
			}
			vec.resize(to);
		}

		void NewPair(std::vector<std::pair<int, int> >& cand, std::vector<std::pair<ClientKey, ClientKey> >& pairs, const std::pair<int, int> p) {
			cand.push_back(p);
			pairs.push_back(std::make_pair(std::min(objects[p.first - reservedKeySpace].clientKey, objects[p.second - reservedKeySpace].clientKey), std::max(objects[p.first - reservedKeySpace].clientKey, objects[p.second - reservedKeySpace].clientKey)));
		}

		// Event-based back end that is witness-count based
		std::map<std::pair<int, int>, int> witnesses;

		void ProcessWitnesses(bool processMigrations, int beginO, int beginU, std::vector<std::pair<int, int> >& overlaps, std::vector<std::pair<int, int> >& unoverlaps) {
			if (triggered_migrations) {
				std::vector<std::pair<int, int> > spatialOverlaps, spatialUnoverlaps;
				// Perform migrations using data from movement
				for (int o = beginO; o < (int)overlaps.size(); )
					if (overlaps[o].first < reservedKeySpace) {
						if (processMigrations && overlaps[o].second >= reservedKeySpace) {
							spatialOverlaps.push_back(overlaps[o]);
						}
						overlaps[o] = overlaps.back();
						overlaps.pop_back();
					} else
						++o;

					for (int u = beginU; u < (int)unoverlaps.size(); )
						if (unoverlaps[u].first < reservedKeySpace) {
							if (processMigrations && unoverlaps[u].second >= reservedKeySpace) {
								spatialUnoverlaps.push_back(unoverlaps[u]);
							}
							unoverlaps[u] = unoverlaps.back();
							unoverlaps.pop_back();
						} else
							++u;

						// Triggered migrations can cause insert+erases,
						// so for now just strip them in pairs before performing migrations
						for (int i = 0; i < (int)spatialOverlaps.size(); ++i)
							for (int j = 0; j < (int)spatialUnoverlaps.size(); ++j) {
								if (spatialOverlaps[i] == spatialUnoverlaps[j]) {
									spatialOverlaps[i] = spatialOverlaps.back();
									spatialOverlaps.pop_back();
									spatialUnoverlaps[j] = spatialUnoverlaps.back();
									spatialUnoverlaps.pop_back();
									--i; --j;
									if (i < (int)spatialOverlaps.size()) // this can happen if i runs out before j
										break;
								}
							}

						int size = (int)spatialOverlaps.size();
						for (int i = 0; i < size; ++i)
							perform_spatial_overlap(spatialOverlaps[i]);
						size = (int)spatialUnoverlaps.size();
						for (int i = 0; i < size; ++i)
							perform_spatial_unoverlap(spatialUnoverlaps[i]);
			}

			// Leave behind entries that correspond to events
			for (int o = beginO; o < (int)overlaps.size(); ) {
				int& i = ++witnesses[overlaps[o]];
				if (i == 1)
					++o;
				else {
					overlaps[o] = overlaps.back();
					overlaps.pop_back();
				}
			}

			for (int u = beginU; u < (int)unoverlaps.size(); ) {
				typename std::map<std::pair<int, int>, int>::iterator ptr = witnesses.find(unoverlaps[u]);
				if (ptr->second == 1) {
					witnesses.erase(ptr);
					u++;
				} else {
					(ptr->second)--;
					unoverlaps[u] = unoverlaps.back();
					unoverlaps.pop_back();
				}
			}
		}

		// threadPolicy
		struct ThreadData {
			ThisType * object;
			int threadNum;
			typename ThreadingPolicy::threadIdType thread;
			static void * ProcessSpaces(void * ths) {
				ThreadData * threadData = (ThreadData *)ths;
				threadData->object->ProcessSpaces(threadData->threadNum);
				return 0;
			}
		};

		std::vector<std::pair<int, int> > * gatherOverlapBuffers, *gatherUnoverlapBuffers;

		void ProcessSpaces(int threadNum) {
			for (int c = threadToSpacesMin[threadNum]; c < threadToSpacesMax[threadNum]; ++c)
				spaces[c]->GatherEvents(gatherOverlapBuffers[threadNum], gatherUnoverlapBuffers[threadNum]);
		}

		/**************************************************** Public Interface ***********************************************/

	public:
		static const bool threaded = true;
		static const bool event_based_output = true;
		typedef Coordinate Coord;
		typedef ClientKey CKey;
		GridSubdivider() : spaces(0), totalCells(0), reservedKeySpace(0), gatherOverlapBuffers(0), gatherUnoverlapBuffers(0) {
		}

		void SetGridParameters(Coordinate boundsMin[], Coordinate boundsMax[], int _numCells[], int _numThreads = 1) {
			totalCells = 1;
			for (int d = 0; d < D; ++d) {
				spatialMinima[d] = boundsMin[d];
				spatialMaxima[d] = boundsMax[d];
				numCells[d] = _numCells[d];
				totalCells *= _numCells[d];

				cellSize[d] = (spatialMaxima[d] - spatialMinima[d]) / numCells[d];
				inverseCellSize[d] = Coordinate(1.0) / cellSize[d];
			}
			numThreads = _numThreads;

			std::cout << "Cells: " << numCells[0];
			for (int d = 1; d < D; ++d)
				std::cout << " by " << numCells[d];
			std::cout << std::endl;
			std::cout << "Total Cells: " << totalCells << std::endl;;

			if (triggered_migrations)
				reservedKeySpace = totalCells;
			else
				reservedKeySpace = 0;

			if (spaces.size() > 0)
				spaces.clear();

			for (int i = 0; i < totalCells; ++i)
				spaces.push_back(new SubdomainAlgorithmType(FilterFunction(objects, reservedKeySpace)));

			// Generate spatial data for all cells
			int minCoordinate[D]; for (int d = 0; d < D; ++d) minCoordinate[d] = 0;
			GenerateSpaceIndices(minCoordinate, numCells, spaceCoordinates);

			// Set the spatial bounds & other criteria for cells
			for (int i = 0; i < (int)spaceCoordinates.size(); ++i) {
				Coordinate min[D], max[D];
				ComputeSpatialBounds(spaceCoordinates[i].coordinates, min, max);
				spaces[spaceCoordinates[i].index]->SetSpatialBounds(min, max);
			}

			if (triggered_migrations) {
				// For every space, add its neighbors as AABBs
				for (int i = 0; i < (int)spaceCoordinates.size(); ++i) {
					int min[D], max[D];
					for (int d = 0; d < D; ++d) {
						min[d] = spaceCoordinates[i].coordinates[d] - 1;
						max[d] = spaceCoordinates[i].coordinates[d] + 2; // max is exclusive
					}
					std::vector<spaceInfo> neighbors;
					GenerateSpaceIndices(min, max, neighbors);
					for (int j = 0; j < (int)neighbors.size(); ++j) {
						// Do not add a space to itself!
						if (spaceCoordinates[i].index != neighbors[j].index)
							AddSpatialBounds(spaceCoordinates[i].index, neighbors[j].index, neighbors[j].coordinates);
					}
				}
			}

			numThreads = _numThreads;

			if (gatherOverlapBuffers != 0)
				delete[](gatherOverlapBuffers);
			gatherOverlapBuffers = new std::vector<std::pair<int, int> >[numThreads]();
			if (gatherUnoverlapBuffers != 0)
				delete[](gatherUnoverlapBuffers);
			gatherUnoverlapBuffers = new std::vector<std::pair<int, int> >[numThreads]();

			threadToSpacesMin.resize(numThreads);
			threadToSpacesMax.resize(numThreads);
			int spacesPerThread = totalCells / numThreads;
			int nextCell = 0;
			int i;
			for (i = 0; i < numThreads; ++i) {
				threadToSpacesMin[i] = nextCell;
				threadToSpacesMax[i] = nextCell + spacesPerThread;
				nextCell += spacesPerThread;
			}
			threadToSpacesMax[i - 1] = totalCells;
			for (int i = 0; i < numThreads; ++i)
				std::cout << "Thread " << i << " has spaces [" << threadToSpacesMin[i] << "-" << threadToSpacesMax[i] << ")" << std::endl;

			spaceToThread.resize(totalCells);
			for (int i = 0; i < numThreads; ++i)
				for (int c = threadToSpacesMin[i]; c < threadToSpacesMax[i]; ++c)
					spaceToThread[c] = i;
		}

		void clear() {
			for (int c = 0; c < totalCells; ++c)
				spaces[c]->clear();
			objects.clear();
			candidates.clear();
			for (int t = 0; t < numThreads; ++t) {
				gatherOverlapBuffers[t].clear();
				gatherUnoverlapBuffers[t].clear();
			}
		}

		int insert(ClientKey clientKey, Coordinate min[], Coordinate max[]) {
			objectInfo obj;
			obj.clientKey = clientKey;
			int pos = objects.insert(obj);

			// Now insert it into each applicable domain
			std::vector<int> cells;
			determineCells(min, max, cells);

			int size = (int)cells.size();
			for (int i = 0; i < size; ++i) {
				int cell = cells[i];
				int key = spaces[cell]->insert(pos + reservedKeySpace, min, max);
				objects[pos].avatars.push_back(avatar(cell, key)); // Will be in sorted order because determineCells gives in ascending order
			}

			return pos;
		}

		void erase(int key) {
			for (int i = 0; i < (int)objects[key].avatars.size(); ++i)
				spaces[objects[key].avatars[i].spatialDomain]->erase(objects[key].avatars[i].cellKey);

			// Handles must be freed AFTER inserts so as not to re-use handles on the same simulation cycle.
			erasesBuffer.push_back(key);
		}

		void UpdateAABB(int key, Coordinate min[], Coordinate max[]) {
			assert(objects.present(key));

			if (!triggered_migrations) {
				typename std::vector<avatar>& avatars = objects[key].avatars;
				// Provides a fast early-out for objects fully contained within a single cell
				if (avatars.size() == 1 && spaces[avatars[0].spatialDomain]->SpaceFullyContainsAABB(min, max)) {
					spaces[avatars[0].spatialDomain]->UpdateAABB(avatars[0].cellKey, min, max);
				} else {
					std::vector<int> cells;
					determineCells(min, max, cells); // Vector filled in sorted order
					int currentAvatarIndex = 0;
					int size = (int)cells.size();
					for (int i = 0; i < size; ++i) {

						int cell = cells[i];
						while (currentAvatarIndex < (int)avatars.size() && avatars[currentAvatarIndex].spatialDomain < cell) // deletions
						{
							spaces[avatars[currentAvatarIndex].spatialDomain]->erase(avatars[currentAvatarIndex].cellKey);
							int i = currentAvatarIndex + 1;
							int aSize = (int)avatars.size();
							while (i < aSize) {
								avatars[i - 1] = avatars[i];
								++i;
							}
							avatars.pop_back();
							// Don't have to decrement currentAvatarsIndex: new item has filled that slot
						}

						if (currentAvatarIndex < (int)avatars.size() && avatars[currentAvatarIndex].spatialDomain == cell) // Update: common case
						{
							spaces[avatars[currentAvatarIndex].spatialDomain]->UpdateAABB(avatars[currentAvatarIndex].cellKey, min, max);
							++currentAvatarIndex;
						} else // Insertion
						{
							int i = (int)avatars.size() - 1;
							avatars.resize(avatars.size() + 1);
							while (i >= currentAvatarIndex) {
								avatars[i + 1] = avatars[i];
								--i;
							}
							int subkey = spaces[cell]->insert(key + reservedKeySpace, min, max);
							avatars[currentAvatarIndex].spatialDomain = cell;
							avatars[currentAvatarIndex].cellKey = subkey;
							++currentAvatarIndex;
						}
					}
					// Handle dangling deletions at end of avatars not handled by "too few" current cells
					if (currentAvatarIndex < (int)avatars.size()) {
						int size = (int)avatars.size();
						for (int i = currentAvatarIndex; i < size; ++i)
							spaces[avatars[i].spatialDomain]->erase(avatars[i].cellKey);
						avatars.resize(currentAvatarIndex);
					}
				}
			} else {
				typename std::vector<avatar>::iterator ptr = objects[key].avatars.begin(), end = objects[key].avatars.end();
				while (ptr != end) {
					spaces[ptr->spatialDomain]->UpdateAABB(ptr->cellKey, min, max);
					++ptr;
				}
			}
		}

		void GatherEvents(std::vector<std::pair<ClientKey, ClientKey> >& overlapEvents, std::vector<std::pair<ClientKey, ClientKey> >& unoverlapEvents) {
			// Prep multithreaded processing
			std::vector<ThreadData> threadData(numThreads);
			for (int t = 0; t < numThreads; ++t) {
				threadData[t].object = this;
				threadData[t].threadNum = t;
				gatherOverlapBuffers[t].clear();
				gatherUnoverlapBuffers[t].clear();
			}

			// Process subdomains in parallel, which also gathers the events
			for (int i = 0; i < numThreads; ++i)
				ThreadingPolicy::Create(&(threadData[i].thread), &(ThreadData::ProcessSpaces), &threadData[i]);
			for (int i = 0; i < numThreads; ++i)
				ThreadingPolicy::Join(threadData[i].thread);

			// Clear avatars so updates don't affect erased items in the spaces
			// Works provided we early out on empty avatars vector so as not to query AABB
			std::vector<int>::iterator ptr = erasesBuffer.begin(), endPtr = erasesBuffer.end();
			while (ptr != endPtr)
				objects[*ptr++].avatars.clear();

			for (int t = 0; t < numThreads; ++t)
				ProcessWitnesses(true, 0, 0, gatherOverlapBuffers[t], gatherUnoverlapBuffers[t]);

			// Can't actually erase from object list until after inserts so as not to re-use ids on same cycle.
			ptr = erasesBuffer.begin(), endPtr = erasesBuffer.end();
			while (ptr != endPtr) {
				objects.erase(*ptr++);
			}
			erasesBuffer.clear();

			if (triggered_migrations) {
				std::vector<int> beginO(numThreads);
				std::vector<int> beginU(numThreads);
				for (int t = 0; t < numThreads; ++t) {
					beginO[t] = (int)gatherOverlapBuffers[t].size();
					beginU[t] = (int)gatherUnoverlapBuffers[t].size();
				}

				// Tell spaces to perform migration events: thread parallel implementation.
				for (int i = 0; i < numThreads; ++i)
					ThreadingPolicy::Create(&(threadData[i].thread), &(ThreadData::ProcessSpaces), &threadData[i]);
				for (int i = 0; i < numThreads; ++i)
					ThreadingPolicy::Join(threadData[i].thread);

				for (int t = 0; t < numThreads; ++t)
					ProcessWitnesses(false, beginO[t], beginU[t], gatherOverlapBuffers[t], gatherUnoverlapBuffers[t]);
			}

			// Translate new guys into the client's keys for "candidates" set
			for (int t = 0; t < numThreads; ++t) {
				int size = (int)gatherOverlapBuffers[t].size();
				for (int i = 0; i < size; ++i) {
					ClientKey key1 = objects[gatherOverlapBuffers[t][i].first - reservedKeySpace].clientKey, key2 = objects[gatherOverlapBuffers[t][i].second - reservedKeySpace].clientKey;
					std::pair<ClientKey, ClientKey> p;
					if (key1 < key2)
						p = std::make_pair(key1, key2);
					else
						p = std::make_pair(key2, key1);

					overlapEvents.push_back(p);
				}
				gatherOverlapBuffers[t].clear();
			}
			for (int t = 0; t < numThreads; ++t) {
				int size = (int)gatherUnoverlapBuffers[t].size();
				for (int i = 0; i < size; ++i) {
					ClientKey key1 = objects[gatherUnoverlapBuffers[t][i].first - reservedKeySpace].clientKey, key2 = objects[gatherUnoverlapBuffers[t][i].second - reservedKeySpace].clientKey;
					std::pair<ClientKey, ClientKey> p;
					if (key1 < key2)
						p = std::make_pair(key1, key2);
					else
						p = std::make_pair(key2, key1);

					unoverlapEvents.push_back(p);
				}
				gatherUnoverlapBuffers[t].clear();
			}
		}
	};

}; // namespace SweepAndPruneCD

#endif
