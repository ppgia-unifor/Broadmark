

#undef min
#undef max
#undef fabs
#include "Dependencies/doctest.h"

#include "Core\Settings.h"
#include <iostream>
#include <string>

using namespace std;


TEST_CASE("Settings_Json_Input_and_Output") {
	Settings startingParams;
	startingParams.m_inputScene = "Input";
	startingParams.m_outputLog = "Output/File";
	startingParams.m_algorithm = "DBVT D";
	startingParams.m_algorithm_prettyName = "DBVT Deferred";
	startingParams.m_margin = Vec3(10, 20, -30);
	startingParams.m_numThreads = 6;
	startingParams.m_Grid_ObjectsPerCell = 256;
	startingParams.m_Tracy_ObjectsPerCell = 150;
	startingParams.m_KD_ObjectsPerLeaf = 1024;
	startingParams.m_GPUSap_KernelType = 2;

	json serializedParams = startingParams;
	const string output = serializedParams.dump();

	json desserializedParams = json::parse(output);
	const Settings readParams = desserializedParams.get<Settings>();

	CHECK(readParams.m_inputScene == "Input");
	CHECK(readParams.m_outputLog == "Output/File");
	CHECK(readParams.m_algorithm == "DBVT D");
	CHECK(readParams.m_algorithm_prettyName == "DBVT Deferred");
	CHECK(readParams.m_margin == Vec3(10, 20, -30));
	CHECK(readParams.m_numThreads == 6);
	CHECK(readParams.m_Grid_ObjectsPerCell == 256);
	CHECK(readParams.m_Tracy_ObjectsPerCell == 150);
	CHECK(readParams.m_KD_ObjectsPerLeaf == 1024);
	CHECK(readParams.m_GPUSap_KernelType == 2);
}