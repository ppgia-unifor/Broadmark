


#include "Settings.h"
#include <string>


using namespace std;

void to_json(json& j, const Settings& p) {
	j = json{
		{"m_inputScene", p.m_inputScene },
		{"m_outputLog", p.m_outputLog },
		{"m_algorithm", p.m_algorithm },
		{"m_algorithm_prettyName", p.m_algorithm_prettyName },

		{"m_margin", p.m_margin },
		{"m_numThreads", p.m_numThreads },
		{"m_Grid_ObjectsPerCell", p.m_Grid_ObjectsPerCell },
		{"m_Tracy_ObjectsPerCell", p.m_Tracy_ObjectsPerCell },
		{"m_KD_ObjectsPerLeaf", p.m_KD_ObjectsPerLeaf },
		{"m_GPUSap_KernelType", p.m_GPUSap_KernelType },
	};
}
void to_json(json& j, const InflatedSettings& p) {
	j = (Settings)p;

	j["m_numberOfObjects"] = p.m_numberOfObjects;
}

#define OPTIONAL(prop, type) if (j.find(#prop) != j.end()) { p.prop = j[#prop].get<type>(); }
void from_json(const json& j, Settings& p) {
	p.m_inputScene = j["m_inputScene"].get<std::string>();
	p.m_outputLog = j["m_outputLog"].get<std::string>();
	p.m_algorithm = j["m_algorithm"].get<std::string>();
	
	OPTIONAL(m_algorithm_prettyName, std::string);
	OPTIONAL(m_margin, Vec3);
	OPTIONAL(m_numThreads, int);
	OPTIONAL(m_Grid_ObjectsPerCell, int);
	OPTIONAL(m_Tracy_ObjectsPerCell, int);
	OPTIONAL(m_KD_ObjectsPerLeaf, int);
	OPTIONAL(m_GPUSap_KernelType, int);

	if (p.m_algorithm_prettyName.empty()) {
		p.m_algorithm_prettyName = p.m_algorithm;
	}
}
