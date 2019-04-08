


#pragma once

#include "Configs.h"
#include "Broadphase\Algorithms\BaseAlgorithm.h"
#include "Broadphase\Object.h"
#include "Broadphase\OverlapChaches\SimpleCache.h"


#if USE_GPU
#pragma comment(lib, "OpenCL.lib")
#include <CL/cl.h>
class b3GpuBroadphaseInterface;
class b3AlignedObjectb3Int4Array;

class Bullet3GPUAlgorithm : public BaseAlgorithm<Object, SimpleCache> {
public:
	b3GpuBroadphaseInterface *m_algorithm;
	b3AlignedObjectb3Int4Array *m_pairs;
	static cl_context sClContext;
	static cl_device_id sClDevice;
	static cl_command_queue sClQueue;
	static bool initialized;

	Bullet3GPUAlgorithm();
	virtual ~Bullet3GPUAlgorithm();

	void Initialize(InflatedSettings settings, const SceneFrame& frameData) override;
	void UpdateObjects(const SceneFrame& frameData) override;
	void UpdateStructures() override;
	void SearchOverlaps() override;
};



#include "Bullet3OpenCL\BroadphaseCollision\b3GpuGridBroadphase.h"
#include "Bullet3OpenCL\BroadphaseCollision\b3GpuParallelLinearBvhBroadphase.h"
#include "Bullet3OpenCL\BroadphaseCollision\b3GpuSapBroadphase.h"


class GPU_Grid : public Bullet3GPUAlgorithm {
public:
	GPU_Grid() {
		m_algorithm = new b3GpuGridBroadphase(sClContext, sClDevice, sClQueue);
	}
};
class GPU_LBVH : public Bullet3GPUAlgorithm {
public:
	GPU_LBVH() {
		m_algorithm = new b3GpuParallelLinearBvhBroadphase(sClContext, sClDevice, sClQueue);
	}
};
class GPU_SAP : public Bullet3GPUAlgorithm {
public:
	GPU_SAP() {
		m_algorithm = new b3GpuSapBroadphase(sClContext, sClDevice, sClQueue,
			(b3GpuSapBroadphase::b3GpuSapKernelType)m_settings.m_GPUSap_KernelType);
	}
};

#else
#include "Broadphase\Algorithms\Misc\None.h"

class GPU_Grid : public None {};
class GPU_LBVH : public None {};
class GPU_SAP : public None {};
#endif