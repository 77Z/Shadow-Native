#ifndef SHADOW_NATIVE_DEBUG_TASK_INDICATOR_HPP
#define SHADOW_NATIVE_DEBUG_TASK_INDICATOR_HPP

#include <string>
#include <unordered_map>

/// Adds a task indicator to the top of the editor, stays there until cleared with EC_TASKCLEAR
#define EC_TASK()

/// Clears a task using the return value of EC_TASK
#define EC_TASKCLEAR()

namespace Shadow::TaskIndicator {

int addTask(const std::string& text);
void endTask(int taskId);
std::unordered_map<int, std::string> getActiveTasks();

}

#endif /* SHADOW_NATIVE_DEBUG_TASK_INDICATOR_HPP */