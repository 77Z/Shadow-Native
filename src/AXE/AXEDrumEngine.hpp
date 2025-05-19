#ifndef SHADOW_NATIVE_AXE_AXE_DRUM_ENGINE
#define SHADOW_NATIVE_AXE_AXE_DRUM_ENGINE

#include "AXETypes.hpp"
#include <vector>
namespace Shadow::AXE {

class AXEDrumEngine {
public:
	AXEDrumEngine();
	~AXEDrumEngine();

	void onUpdate();

	void openDrumEditor(Clip* clip);

private:

	// Each opened drum collection in this vector will have its own window for
	// the user to edit the beat.
	std::vector<Clip*> openedDrumCollections;

};

}

#endif /* SHADOW_NATIVE_AXE_AXE_DRUM_ENGINE */