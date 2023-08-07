#include "Editor/Project.hpp"

namespace Shadow::Editor {

static std::string projectName;
static std::string projectPath;

std::string getCurrentProjectName() { return projectName; }
std::string getCurrentProjectPath() { return projectPath; }

void setCurrentProjectName(std::string name) { projectName = name; }
void setCurrentProjectPath(std::string path) { projectPath = path; }

}