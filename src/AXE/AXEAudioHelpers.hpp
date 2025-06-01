#include "AXETypes.hpp"

namespace Shadow::AXE {

/// Checks an ma_result and will print to stdout and the EditorConsole if
/// the operation wasn't successful. If the result is successful, this method
/// will stay quiet.
void checkResult(ma_result r);

}