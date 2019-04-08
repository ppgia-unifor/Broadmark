


#include "Algorithms.h"
#include "Broadphase/Algorithms/BF/BF.h"
#include "Broadphase/Algorithms/SAP/SAP.h"
#include "Broadphase/Algorithms/CGAL/CGAL.h"
#include "Broadphase/Algorithms/iSAP/AxisSweep.h"
#include "Broadphase/Algorithms/Grid/Grid.h"
#include "Broadphase/Algorithms/Tracy/Tracy.h"
#include "Broadphase/Algorithms/Tracy/Tracy_Parallel.h"
#include "Broadphase/Algorithms/DBVT/DBVT.h"
#include "Broadphase/Algorithms/GPU/Bullet3GPUAlgorithms.h"
#include "Broadphase/Algorithms/Misc/None.h"
#include "Broadphase/Algorithms/Misc/Inspectors.h"
#include "Broadphase/Algorithms/KD/KD.h"


#define REGISTER(name, type) if (strcmp(name, algorithmName) == 0) { return std::make_unique<type>(); }
std::unique_ptr<BroadphaseInterface> Algorithms::Create(const char* algorithmName) {
	REGISTER("BF", BF);
	REGISTER("BF_Parallel", BF_Parallel);
	REGISTER("BF_SSE", BF_SSE);
	REGISTER("BF_AVX", BF_AVX);
	REGISTER("BF_SSE_Parallel", BF_SSE_Parallel);
	REGISTER("BF_AVX_Parallel", BF_AVX_Parallel);
	REGISTER("SAP", SAP);
	REGISTER("SAP_Parallel", SAP_Parallel);
	REGISTER("SAP_SSE", SAP_SSE);
	REGISTER("SAP_AVX", SAP_AVX);
	REGISTER("SAP_SSE_Parallel", SAP_SSE_Parallel);
	REGISTER("SAP_AVX_Parallel", SAP_AVX_Parallel);
	REGISTER("CGAL", CGAL_Internal);
	REGISTER("AxisSweep", AxisSweep);
	REGISTER("Grid_2D", Grid_2D);
	REGISTER("Grid_3D", Grid_3D);
	REGISTER("Grid_2D_SAP", Grid_2D_SAP);
	REGISTER("Grid_3D_SAP", Grid_3D_SAP);
	REGISTER("Grid_2D_Parallel", Grid_2D_Parallel);
	REGISTER("Grid_3D_Parallel", Grid_3D_Parallel);
	REGISTER("Grid_2D_SAP_Parallel", Grid_2D_SAP_Parallel);
	REGISTER("Grid_3D_SAP_Parallel", Grid_3D_SAP_Parallel);
	REGISTER("Tracy", Tracy);
	REGISTER("Tracy_Parallel", Tracy_Parallel);
	REGISTER("DBVT D", DBVT_D);
	REGISTER("DBVT F", DBVT_F);
	REGISTER("GPU_Grid", GPU_Grid);
	REGISTER("GPU_LBVH", GPU_LBVH);
	REGISTER("GPU_SAP", GPU_SAP);
	REGISTER("KD", KD);

	// Misc algorithms used to validate the system
	REGISTER("None", None);
	REGISTER("TimeValidator", TimeValidator);

	return nullptr;
}

std::vector<const char*> Algorithms::EnumerateAlgorithms() {
	return {
		"BF",
		"BF_Parallel",
		"BF_SSE",
		"BF_AVX",
		"BF_SSE_Parallel",
		"BF_AVX_Parallel",
		"SAP",
		"SAP_Parallel",
		"SAP_SSE",
		"SAP_AVX",
		"SAP_SSE_Parallel",
		"SAP_AVX_Parallel",
		"CGAL",
		"AxisSweep",
		"Grid_2D",
		"Grid_2D_SAP",
		"Grid_2D_Parallel",
		"Grid_2D_SAP_Parallel",
		"Grid_3D",
		"Grid_3D_SAP",
		"Grid_3D_Parallel",
		"Grid_3D_SAP_Parallel",
		"Tracy",
		"Tracy_Parallel",
		"DBVT D",
		"DBVT F",
		"GPU_Grid",
		"GPU_LBVH",
		"GPU_SAP",
		"KD",

		"None",
		"TimeValidator",
	};
}