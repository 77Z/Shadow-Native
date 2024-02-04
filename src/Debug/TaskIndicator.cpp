#include "Debug/TaskIndicator.hpp"
#include <string>
#include <unordered_map>

namespace Shadow::TaskIndicator {

static std::unordered_map<int, std::string> activeTasks;

static int taskIdIterator = 0;

int addTask(const std::string& text) {
	taskIdIterator++;

	activeTasks[taskIdIterator] = text;

	return taskIdIterator;
}

void endTask(int taskId) {
	activeTasks.erase(taskId);
}

std::unordered_map<int, std::string> getActiveTasks() {
	return activeTasks;
}

}