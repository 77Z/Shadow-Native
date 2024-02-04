#ifndef SHADOW_NATIVE_DEBUG_TASK_INDICATOR_HPP
#define SHADOW_NATIVE_DEBUG_TASK_INDICATOR_HPP

#include <string>
#include <unordered_map>

namespace Shadow::TaskIndicator {

/// Adds a task indicator to the top of the editor, stays there until cleared with endTask
int addTask(const std::string& text);
/// Clears a task using the return value of addTask
void endTask(int taskId);

std::unordered_map<int, std::string> getActiveTasks();

}

#endif /* SHADOW_NATIVE_DEBUG_TASK_INDICATOR_HPP */