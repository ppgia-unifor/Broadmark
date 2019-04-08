

#include "Core\Algorithms.h"
#include "Core\Assertions.h"
#include "Core\Clock.h"
#include "Core\Scene.h"
#include "Core\Settings.h"
#include <iostream>
#include <iomanip>

#undef min
#undef max
#undef fabs
#define DOCTEST_CONFIG_IMPLEMENT
#include "Dependencies/doctest.h"


#define DEBUG_ALGORITHM "GPU_SAP"

#if DEBUG
#define VALIDATION_ALGORITHM "SAP_AVX"
#else
#define VALIDATION_ALGORITHM "None"
#endif



double weightedExponentialMovingAverage(double current, double average) {
	const double alpha = 0.3;
	return alpha * current + (1 - alpha) * average;
}

void execute(const Settings& settings) {
	Scene scene(settings.m_inputScene.c_str());

	const InflatedSettings inflatedSettings(settings, scene.GetHeader());
	std::unique_ptr<BroadphaseInterface> algorithm = Algorithms::Create(inflatedSettings.m_algorithm.c_str());
	std::unique_ptr<BroadphaseInterface> validation = Algorithms::Create(VALIDATION_ALGORITHM);

	if (algorithm == nullptr) {
		std::cout << "The specified algorithm does not exists (" << inflatedSettings.m_algorithm << ")\n";
		return;
	} else if (inflatedSettings.m_algorithm == "GPU_SAP" && inflatedSettings.m_numberOfObjects > 512000) {
		std::cout << "GPU SAP does not work for more than 512k objects)\n";
		return;
	}

	std::cout << "Algorithm: " << inflatedSettings.m_algorithm_prettyName << '\n';
	std::cout << "Scene: " << inflatedSettings.m_inputScene << '\n';

	const SceneFrame& initialData = scene.GetNextFrame();
	double avgDt = Measure([&] {
		algorithm->Initialize(inflatedSettings, initialData);
		validation->Initialize(inflatedSettings, initialData);
	});

	Results::Init(inflatedSettings);
	for (size_t i = 0; i < scene.GetHeader().m_numberOfFrames; i++) {
		const SceneFrame& frameData = scene.GetNextFrame();

		const double dt = Measure([&] {
			Results::BeginFrame();
			algorithm->ExecuteAndMeasure(frameData);
			algorithm->GetOverlaps()->Validate();
			Results::EndFrame();
		});

		algorithm->Validate();
		validation->Execute(frameData);
		validation->GetOverlaps()->Validate();

		avgDt = weightedExponentialMovingAverage(dt, avgDt);
		std::cout << std::setprecision(4) << std::setw(5) << std::fixed
			<< "\r\t(" << i + 1 << "/" << scene.GetHeader().m_numberOfFrames << "): "
			<< avgDt << " s/frame "
			<< "(ETA: " << avgDt * (scene.GetHeader().m_numberOfFrames - i) / 60 << "m) - "
			<< algorithm->GetOverlaps()->Size() / (double)scene.GetHeader().m_numberOfObjects << " collisions/object"
			<< " ";

		// Some algorithms may report extra pairs due to AABB margins or other devices
		ASSERT(algorithm->GetOverlaps()->Size() >= validation->GetOverlaps()->Size());

#ifndef DEBUG
		// avoids wasting too much time on slow algorithms or on bad alg-scene combinations
		// some algorithms are really bad just at the beggining, so we give them up to 50 frames
		// to reach a < 0.75s time
		//if (//avgDt > 2.5f ||
		//	i > 5 && avgDt > 2.0f ||
		//	i > 15 && avgDt > 1.0f) {
		//	std::cout << "\n\tToo slow, aborting...\n";
		//	return;
		//}
#endif
	}

	Results::Finish();
}

void main_tests() {
	doctest::Context context;

	context.run();
	if (context.shouldExit()) {
		printf("\n\n");
		system("pause");
		exit(0);
	}
}
void main_debug() {
	Settings settings;
	settings.m_inputScene = "E:/Simulations/Recordings_AABBs/Brownian Cubes N768000 F1000frames.aabbs";
	settings.m_outputLog = "C:/Users/ygorr/Desktop/Temp/Nova pasta/log.json";
	settings.m_algorithm = DEBUG_ALGORITHM;
	settings.m_algorithm_prettyName = settings.m_algorithm;
	settings.m_KD_ObjectsPerLeaf = 500;
	settings.m_Grid_ObjectsPerCell = 200;
	settings.m_GPUSap_KernelType = 5;

	execute(settings);
}
void main_release(const char* path) {
	std::ifstream file(path);
	if (file.fail()) {
		std::cout << "The supplied JSON file does not exists or is unavailable. \n";
		std::cout << "\tFile: " << path << "\n\n";
		system("pause");
		exit(0);
	}

	json j;
	file >> j;
	Settings settings;
	try {
		settings = j.get<Settings>();
	} catch (json::exception& e) {
		std::cout << "The supplied JSON could not be parsed. \n";
		std::cout << "\tJson: " << e.what() << '\n' << "\tException id: " << e.id << std::endl;
		std::cout << "\tFile: " << path << "\n\n";
		system("pause");
		exit(0);
	}

	settings.Validate();
	execute(settings);
}
void main_help() {
	const std::vector<const char*> algorithms = Algorithms::EnumerateAlgorithms();

	std::cout << "Ygor Reboucas Serpa (2018)\n";
	std::cout << "\n";
	std::cout << "To run a simulation, pass as a simple command line argument a json-file with the simulation settings.\n";
	std::cout << "For reference, refer to the 'Settings.h' file or to any example json in the docs\n";
	std::cout << "In the following, we list all available algorithms\n";
	for (const char* algorithm : algorithms) {
		std::cout << "\t" << algorithm << "\n";
	}

	std::cout << "\n";
	std::cout << "Alternatively, if you wish to run all unit tests, use the -tests command\n";
	std::cout << "\n\n\n";
}


int main(int argc, char** argv) {
	#ifdef DEBUG
	main_tests();
	#endif
	initializeDocTestForAssertionsOutOfTestCases();
	
	#ifdef DEBUG
	main_debug();
	#else
	if (argc == 2) {
		if (strcmp(argv[1], "-tests") == 0) {
			main_tests();
		} else {
			main_release(argv[1]);
		}
	} else {
		main_help();
	}
	#endif
	
	#ifdef DEBUG
	std::cout << "\n\n\n";
	system("pause");
	#endif

	std::cout << "\n\n";
	return 0;
}


