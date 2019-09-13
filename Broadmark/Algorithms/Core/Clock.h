

#pragma once


#include <Windows.h>
#include "Core/Results.h"


// High-resolution clock implementation
// This is one of the few windows-only parts of the code
class Clock {
private:
	LARGE_INTEGER m_freq;
	LARGE_INTEGER m_reference;

public:
	Clock() {
		QueryPerformanceFrequency(&m_freq);
		reset();
	}

	void reset() {
		QueryPerformanceCounter(&m_reference);
	}
	double getAndReset() {
		const LARGE_INTEGER reference = m_reference;
		QueryPerformanceCounter(&m_reference);

		// No need to bloat log files with smaller than 10^{-6} values
		const double timeInSeconds = (double)((m_reference.QuadPart - reference.QuadPart) / (double)m_freq.QuadPart);
		return timeInSeconds > 0.000001f ? timeInSeconds : 0;
	}
};

template<typename Action>
inline double Measure(const Action action) {
	Clock clock;
	action();

	return clock.getAndReset();
}

template<typename Action>
inline void MeasureAndReport(const char* actionName, const Action action) {
	Results::Report(actionName, Measure(action));
}
