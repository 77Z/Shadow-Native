#ifndef SHADOW_NATIVE_GO_DOWN_IN_FLAMES_HPP
#define SHADOW_NATIVE_GO_DOWN_IN_FLAMES_HPP

#include <string>

// This will kill the process and try its best to alert the user that something
// has gone wrong beyond repair
#define BAILOUT(message) Shadow::internalGoDownInFlames(message, __FILE_NAME__, __LINE__)

namespace Shadow {

/// Careful with the message you pass in here as it's really not sanitized well
/// at all.
void internalGoDownInFlames(const std::string& message, const std::string& file, int line);

}

#endif /* SHADOW_NATIVE_GO_DOWN_IN_FLAMES_HPP */