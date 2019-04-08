


#include "Bullet3GPUAlgorithms.h"


#if USE_GPU
#pragma comment(lib, "OpenCL.lib")
#include "Bullet3OpenCL\Initialize\b3OpenCLInclude.h"
#include "Bullet3OpenCL\Initialize\b3OpenCLUtils.h"
#include "Bullet3OpenCL\BroadphaseCollision\b3GpuBroadphaseInterface.h"

#include "Bullet3OpenCL\BroadphaseCollision\b3GpuSapBroadphase.h"

class b3AlignedObjectb3Int4Array : public b3AlignedObjectArray<b3Int4> {};


cl_context Bullet3GPUAlgorithm::sClContext;
cl_device_id Bullet3GPUAlgorithm::sClDevice;
cl_command_queue Bullet3GPUAlgorithm::sClQueue;
bool Bullet3GPUAlgorithm::initialized = false;
#define Vec3Conv(v) {v[0], v[1], v[2]}


Bullet3GPUAlgorithm::Bullet3GPUAlgorithm() {
	int ciErrNum = 0;
	if (!initialized) {
		std::cout << "Initializing OpenCL...\n";
		sClContext = b3OpenCLUtils::createContextFromType(CL_DEVICE_TYPE_GPU, &ciErrNum);

		oclCHECKERROR(ciErrNum, CL_SUCCESS);
		const int numDev = b3OpenCLUtils::getNumDevices(sClContext);

		std::cout << " -Found " << numDev << " OpenCL-Compatible GPUs.\n";
		if (numDev > 0) {
			sClDevice = b3OpenCLUtils::getDevice(sClContext, 0);
			sClQueue = clCreateCommandQueue(sClContext, sClDevice, 0, &ciErrNum);

			b3OpenCLDeviceInfo info;
			b3OpenCLUtils::getDeviceInfo(sClDevice, &info);
			
			std::cout << " -Using '" << info.m_deviceName << "' (Driver Version: " << info.m_driverVersion << ")\n";
			std::cout << " -" << info.m_computeUnits << " compute units available at " << info.m_clockFrequency << "MHz \n";
			std::cout << "\n";
			initialized = true;
		} else {
			std::cout << " -No GPU was found, all OpenCL algorithms will silently fail\n\n";
		}
	}

	m_algorithm = nullptr;
	m_pairs = new b3AlignedObjectb3Int4Array();
}
Bullet3GPUAlgorithm::~Bullet3GPUAlgorithm() {
	delete m_algorithm;
	delete m_pairs;
}

void Bullet3GPUAlgorithm::Initialize(InflatedSettings settings, const SceneFrame& frameData) {
	if (!initialized) {
		return;
	}

	BaseAlgorithm<Object, SimpleCache>::Initialize(settings, frameData);

	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		const Aabb& aabb = frameData.m_aabbs[i];

		m_algorithm->createProxy(Vec3Conv(aabb.m_min), Vec3Conv(aabb.m_max), i, 1, 1);
	}
}
void Bullet3GPUAlgorithm::UpdateObjects(const SceneFrame& frameData) {
	if (!initialized) {
		return;
	}

	BaseAlgorithm<Object, SimpleCache>::UpdateObjects(frameData);
	for (size_t i = 0; i < m_settings.m_numberOfObjects; i++) {
		const Aabb& aabb = frameData.m_aabbs[i];

		m_algorithm->getAllAabbsCPU()[i].m_minVec = Vec3Conv(aabb.m_min);
		m_algorithm->getAllAabbsCPU()[i].m_maxVec = Vec3Conv(aabb.m_max);
		m_algorithm->getAllAabbsCPU()[i].m_minIndices[3] = i;
		m_algorithm->getAllAabbsCPU()[i].m_signedMaxIndices[3] = i;
	}
}
void Bullet3GPUAlgorithm::UpdateStructures() {
	if (!initialized) {
		return;
	}

	m_algorithm->writeAabbsToGpu();
}
void Bullet3GPUAlgorithm::SearchOverlaps() {
	if (!initialized) {
		return;
	}

	m_algorithm->calculateOverlappingPairs(m_settings.m_numberOfObjects * 18);
	m_algorithm->getOverlappingPairsGPU().copyToHost(*m_pairs, true);

	for (size_t i = 0; i < m_pairs->size(); i++) {
		Object *p1, *p2;
		if (m_pairs->at(i).x < m_pairs->at(i).y) {
			p1 = &m_objects[m_pairs->at(i).x];
			p2 = &m_objects[m_pairs->at(i).y];
		} else {
			p1 = &m_objects[m_pairs->at(i).y];
			p2 = &m_objects[m_pairs->at(i).x];
			
		}

		if (p1 != p2) {
			m_cache.AddPair(p1, p2);
		}
	}
	m_pairs->clear();
}
#endif