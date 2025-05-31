#include "AXEClipBrowser.hpp"
#include "AXEJobSystem.hpp"
#include "AXETypes.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/EditorConsole.hpp"
#include "ImGuiNotify.hpp"
#include "imgui.h"
#include "miniaudio.h"
#include <filesystem>
#include <string>
#include "ShadowIcons.hpp"

#define EC_THIS "Clip Browser"

namespace Shadow::AXE {

namespace fs = std::filesystem;

static void prettyPrintAudioEngineErr(ma_result r) {
	EC_ERROUT(EC_THIS, "%s", ma_result_description(r));
}

// TODO: Implement grabbing master volume level and implement it for sound file previews?
ClipBrowser::ClipBrowser() {
	EC_NEWCAT(EC_THIS);

	globalLibraryPath = EngineConfiguration::getConfigDir();

	globalLibraryPath = globalLibraryPath / "AXEProjects" / "GlobalLibrary";

	EC_PRINT(EC_THIS, "Global Library Path stringified!: %s", globalLibraryPath.string().c_str());

	clipBrowserAudioEngineConfig = ma_engine_config_init();
	ma_result result = ma_engine_init(&clipBrowserAudioEngineConfig, &clipBrowserAudioEngine);

	if (result != MA_SUCCESS) {
		EC_ERROUT(EC_THIS, "Failed to init ClipBrowser's audioEngine");
		prettyPrintAudioEngineErr(result);
	}

	refreshFiles();
}

ClipBrowser::~ClipBrowser() {
	ma_engine_uninit(&clipBrowserAudioEngine);
}

void ClipBrowser::onUpdate(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("Clip Browser", &p_open)) {
		End();
		return;
	}

	if (SmallButton("Refresh")) {
		refreshFiles();
		ImGui::InsertNotification({ImGuiToastType::Success, 3000, "Clip browser refreshed"});
	}

	int it = 0;
	for (auto& clip : clips) {
		PushID(it);

		if (clip.isDirectory) {
			PopID();
			continue;
		}

		SetNextItemAllowOverlap();
		Selectable((SHADOW_ICON_MUSIC " " + clip.clipPath.filename().string()).c_str());
		if (BeginItemTooltip()) {
			Text("%s", audioFileTypes_PrettyNames[clip.audioFileType]);
			Separator();
			TextUnformatted("Drag me onto the Timeline!");
			Separator();
			if (GetIO().KeyCtrl) {
				Text("%s", clip.clipPath.string().c_str());
			} else {
				TextUnformatted("Hold CTRL for full path");
			}
			EndTooltip();
		}
		if (BeginDragDropSource()) {
			SetDragDropPayload("AXE_CLIP_PATH_PAYLOAD", clip.clipPath.string().c_str(), clip.clipPath.string().size() + 1); // +1 for NULL!!!
			Text( SHADOW_ICON_MUSIC " %s", clip.clipPath.filename().string().c_str());
			EndDragDropSource();
		}
		SameLine();
		SetCursorPosX(GetWindowWidth() - 60);
		if (SmallButton(SHADOW_ICON_PLAY)) {
			aeResult = ma_engine_play_sound(&clipBrowserAudioEngine, clip.clipPath.string().c_str(), nullptr);
			if (aeResult != MA_SUCCESS) prettyPrintAudioEngineErr(aeResult);
		}
		
		PopID();
		it++;
	}

	End();
}

void ClipBrowser::shutdown() {}

void ClipBrowser::refreshFiles() {
	clips.clear();

	JobSystem::submitJob("Sort through clips", [this]() -> bool {

		if (!fs::exists(globalLibraryPath)) {
			std::string msg = "Failed to read GlobalLibrary directory in AXEProjects!! I'm gonna complain instead of making the dir!!";
			JobSystem::degradeEditorWithMessage("GlobalLibrary doesn't exist!", msg);
			EC_ERROUT(EC_THIS, msg.c_str());
			return false;
		}

		for (const fs::directory_entry& file : fs::recursive_directory_iterator(globalLibraryPath)) {
			ClipBrowserItemInfo item;
			item.clipPath = file.path();
			item.isDirectory = file.is_directory();

			if (file.is_directory() || !file.path().has_extension()) {
				item.audioFileType = AudioFileTypes_Unknown;
				clips.push_back(item);
				continue;
			}

			for (int i = 0; i < AudioFileTypes_Count; i++) {
				if (file.path().extension().string() == "." + std::string(audioFileTypes_FileExtensions[i])) {
					item.audioFileType = static_cast<AudioFileTypes_>(i);
				}
			}

			clips.push_back(item);
		}

		return true;
	});

}

void ClipBrowser::addFileToLibrary(const fs::path filepath) {
	if (!fs::exists(filepath)) {
		ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Specified file doesn't exist??"});
		EC_ERROUT(EC_THIS, "Caller tried to add file that doesn't exist?\nFile in question: %s", filepath.c_str());
		return;
	}

	if (fs::is_directory(filepath)) {
		ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Can't add folders to the Global Library!"});
		EC_ERROUT(EC_THIS, "Can't add dirs to GlobalLibrary!!\nDir in question: %s", filepath.c_str());
		return;
	}

	if (!fs::is_regular_file(filepath)) {
		ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Can't add non-regular files!"});
		EC_ERROUT(EC_THIS, "Can't add non-regular files!\nFP in question: %s", filepath.c_str());
		return;
	}

	try {
		fs::copy_file(filepath, globalLibraryPath / filepath.filename());
	} catch (const std::exception& e) {
		ImGui::InsertNotification({
			ImGuiToastType::Error,
			5000,
			"Failed to import audio file :/",
		});
	}

	refreshFiles();
};

}