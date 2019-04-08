#pragma once


#include "Configs.h"
#include "Macros.h"
#include "Core/Math/Vec3.h"
#include "Core/Settings.h"
#include "Core/Scene.h"
#include "Broadphase\BroadphaseInterface.h"
#include "Broadphase/OverlapChaches/NullCache.h"
#include "None.h"

#include <thread>
#include <chrono>
#include <type_traits>


class TimeValidator : public None {
public:
	virtual ~TimeValidator() = default;


	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	void SearchOverlaps() override {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
};
