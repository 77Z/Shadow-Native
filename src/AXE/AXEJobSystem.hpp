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

/// Artificially submit a faulty job with a specified message. You'd usually
/// want to do this when you handle an error yourself (hopefully without
/// crashing), but you want to indicate to the user via the toolbar that
/// AXE is degraded due to a handled error.
void degradeEditorWithMessage(const std::string& name, const std::string& message);

}

#endif /* SHADOW_NATIVE_AXE_AXE_JOB_SYSTEM_HPP */