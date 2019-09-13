#pragma once


#include "Macros.h"
#include "Settings.h"
#include "Dependencies\json.hpp"
#include <iostream>
#include <fstream>


using nlohmann::json;

// Object that holds collected measurements in json format. 
class Results {
private:
	class ResultsInternal {
	public:
		json results;
		json currentFrame;

		ResultsInternal() = default;
	};

private:
	static ResultsInternal *m_instance;

	Results() = default;
public:
	static void Init(const InflatedSettings& settings) {
		m_instance = new ResultsInternal();
		m_instance->results["Settings"] = settings;
	}
	static void BeginFrame() {
		m_instance->currentFrame = json();
	}
	static void EndFrame() {
		m_instance->results["Frames"].push_back(m_instance->currentFrame);
	}

	template<typename T>
	static void Report(const char* name, T value) {
		m_instance->currentFrame[name] = value;
	}

	static void Finish() {
		// dumps the json to a string using 4 whitespaces as identation
		std::string dump = m_instance->results.dump(4);

		std::ofstream file;
		file.open(m_instance->results["Settings"]["m_outputLog"].get<std::string>());
		file.write(dump.c_str(), dump.length());
		file.close();
	}
};


