#ifndef SHADOW_NATIVE_AXE_AXE_GLOBAL_SETTINGS_WINDOW_HPP
#define SHADOW_NATIVE_AXE_AXE_GLOBAL_SETTINGS_WINDOW_HPP

namespace Shadow::AXE {

struct GlobalSettings {
	int samplesPerPixel = 256;
	bool wavyRender = false;
};

void loadGlobalSettingsFromDisk();
GlobalSettings* getGlobalSettings();
void onUpdateGlobalSettingsWindow(bool& p_open);

}

#endif /* SHADOW_NATIVE_AXE_AXE_GLOBAL_SETTINGS_WINDOW_HPP */