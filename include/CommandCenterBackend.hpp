#ifndef SHADOW_NATIVE_COMMAND_CENTER_BACKEND_HPP
#define SHADOW_NATIVE_COMMAND_CENTER_BACKEND_HPP

#include <string>
#include <vector>

#define CMD_CTR_NEWCMD(name, desc, func) \
	Shadow::CommandCenter::registerCommand(name, desc, func);

namespace Shadow::CommandCenter {

typedef std::vector<std::tuple<std::string, std::string, void (*)()>> commandList;

void registerCommand(std::string name, std::string description, void (*func)());

commandList getCommands();

}

#endif /* SHADOW_NATIVE_COMMAND_CENTER_BACKEND_HPP */