#ifndef SHADOW_NATIVE_AXE_AXE_JOB_SYSTEM_HPP
#define SHADOW_NATIVE_AXE_AXE_JOB_SYSTEM_HPP

#include <ctime>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Shadow::AXE::JobSystem {

struct Job {
	int id;
	std::string name;
	std::time_t startTime;

	// Retired jobs only
	bool success = false;
	std::string miscInfo; // C++ exceptions usually go here
	std::time_t endTime;
};

// Submit a job to be executed on a different thread
void submitJob(const std::string& name, std::function<bool()> jobMethod);

// Draws a small clickable widget to indicate the user of active jobs
void onUpdateStatusBar();

}

#endif /* SHADOW_NATIVE_AXE_AXE_JOB_SYSTEM_HPP */