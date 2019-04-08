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
 *  SweepAndPrune.hpp
 *  CollisionDetectionTestBed
 */

#ifndef SWEEPANDPRUNE_HPP
#define SWEEPANDPRUNE_HPP

#include "GridSuperstructure.hpp"
#include "SegSNP.hpp"

namespace SweepAndPrune {

	struct NoThreads {
		typedef int threadIdType;
		static void Create(threadIdType * id, void *(*routine)(void *), void * userData) {
			(*routine) (userData);
		}
		static void Join(threadIdType id) {
		}
	};

	template<typename Key>
	struct FilterNone {
		bool pass(Key one, Key two) {
			return true;
		}
	};

	template<int D, typename ClientKey, typename Coordinate = float, class ThreadPolicy = NoThreads, class FilterPolicy = FilterNone<ClientKey> >
	class Tracy_SweepAndPrune : public FullOutputAdapter<GridSubdivider<Segmented, false, D, ClientKey, Coordinate, ThreadPolicy, FilterPolicy> > {
	};

}; // namespace SweepAndPrune

#endif
