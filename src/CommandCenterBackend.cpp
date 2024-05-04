#include "CommandCenterBackend.hpp"
#include <string>
#include <vector>

namespace Shadow::CommandCenter {

static commandList commands;

void registerCommand(std::string name, std::string description, void (*func)()) {
	commands.push_back(std::make_tuple(name, description, func));
}

commandList getCommands() {
	return commands;
}

}