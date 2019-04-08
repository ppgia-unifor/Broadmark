#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Object.h"
#include "Core/Math/Vec3.h"
#include "Core/Clock.h"
#include "Core/Settings.h"
#include "Core\Results.h"
#include "Core/Scene.h"
#include "Broadphase/OverlapCache.h"


// Basic interface for all broad phase algorithms. 
// Each method is defined in the order they are called during execution
class BroadphaseInterface {
public:
	virtual ~BroadphaseInterface() = default;


	// Use this method to allocate memory for each object and for custom structures
	virtual void Initialize(InflatedSettings settings, const SceneFrame& frameData) = 0;

	virtual void PreUpdateObjects() {}
	// Updates each object's AABBs with the new frameData. 
	virtual void UpdateObjects(const SceneFrame& frameData) = 0;
	virtual void PostUpdateObjects() {}

	// Dedicated method for overlapping pairs cleansing. Useful for incremental algorithms
	virtual void CleanCache() = 0;

	virtual void PreUpdateStructures() {}
	// Updates any custom structure used by the algorithm. Occurs after all objects were updated
	virtual void UpdateStructures() {}
	virtual void PostUpdateStructures() {}

	virtual void PreSearchOverlaps() {}
	// Performs the broad phase search. 
	virtual void SearchOverlaps() = 0;
	virtual void PostSearchOverlaps() {}


	virtual OverlapCache* GetOverlaps() = 0;

	// Use this method to perform validation code. Useful for debugging.
	// This method will only be called on debug builds
	virtual void Validate() const {};
	// Use this method to print data to the console after a full simulation frame. Useful for debugging
	// Printed data using this method won't be copied to the results log
	virtual void Trace() const {};

	void Execute(const SceneFrame& frameData) {
		PreUpdateObjects();
		UpdateObjects(frameData);
		PostUpdateObjects();

		CleanCache();

		PreUpdateStructures();
		UpdateStructures();
		PostUpdateStructures();

		PreSearchOverlaps();
		SearchOverlaps();
		PostSearchOverlaps();
	}
	void ExecuteAndMeasure(const SceneFrame& frameData) {
		MeasureAndReport("Total", [=] {
			MeasureAndReport("UpdateObjects", [=]() {
				PreUpdateObjects();
				UpdateObjects(frameData);
				PostUpdateObjects();
			});
			MeasureAndReport("CleanCache", [=]() { CleanCache(); });
			MeasureAndReport("UpdateStructures", [=]() {
				PreUpdateStructures();
				UpdateStructures();
				PostUpdateStructures();
			});
			MeasureAndReport("SearchOverlaps", [=]() {
				PreSearchOverlaps();
				SearchOverlaps();
				PostSearchOverlaps();
			});
		});
	}
};
