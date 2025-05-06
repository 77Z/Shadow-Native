#include "AXEClipBrowser.hpp"
#include "AXEJobSystem.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "ImGuiNotify.hpp"
#include "imgui.h"
#include "miniaudio.h"
#include <filesystem>
#include "IconsCodicons.h"

#define EC_THIS "Clip Browser"

namespace Shadow::AXE {

ClipBrowser::ClipBrowser() {
	EC_NEWCAT(EC_THIS);

	globalLibraryPath = EngineConfiguration::getConfigDir() + "/AXEProjects/GlobalLibrary";
	// globalLibraryPath = "AXEProjects"/"GlobalLibrary";

	//TODO: This fails but has no reason to :P
	// ma_engine_config engineConfig = ma_engine_config_init();
	// ma_result result = ma_engine_init(&engineConfig, audioEngine);

	// if (result != MA_SUCCESS) {
	// 	EC_ERROUT(EC_THIS, "Failed to init ClipBrowser's audioEngine");
	// 	EC_ERROUT(EC_THIS, "Faulty result enum (%i)", result);
	// }

	refreshFiles();
}

ClipBrowser::~ClipBrowser() {
	// ma_engine_uninit(audioEngine);
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

	for (auto& clip : clips) {

		if (clip.isDirectory) continue;

		SetNextItemAllowOverlap();
		Selectable((ICON_CI_MUSIC " " + clip.prettyName).c_str());
		if (BeginItemTooltip()) {
			Text("%s", clip.prettyType.c_str());
			Separator();
			TextUnformatted("Drag me onto the Timeline!");
			Separator();
			if (GetIO().KeyCtrl) {
				Text("%s", clip.fullpath.c_str());
			} else {
				TextUnformatted("Hold CTRL for full path");
			}
			EndTooltip();
		}
		if (BeginDragDropSource()) {
			SetDragDropPayload("AXE_CLIP_PATH_PAYLOAD", clip.fullpath.data(), clip.fullpath.size());
			Text("\xee\xb0\x9b %s", clip.prettyName.c_str());
			EndDragDropSource();
		}
		SameLine();
		if (SmallButton((ICON_CI_PLAY "##" + clip.relativePath).c_str())) {
			ma_engine_play_sound(audioEngine, clip.fullpath.c_str(), nullptr);
		}
		
	}

	End();
}

void ClipBrowser::shutdown() {}

void ClipBrowser::refreshFiles() {
	clips.clear();

	JobSystem::submitJob("Sort through clips", [this]() -> bool {

		if (!std::filesystem::exists(globalLibraryPath)) {
			ERROUT("Failed to read GlobalLibrary directory in AXEProjects!! I'm gonna complain instead of making the dir!!");
			EC_ERROUT(EC_THIS, "Failed to read GlobalLibrary directory in AXEProjects!! I'm gonna complain instead of making the dir!!");
			return false;
		}

		for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(globalLibraryPath)) {
			std::string fp = file.path().string();
			// EC_PRINT(EC_THIS, "%s", fp.substr().c_str());

			ClipBrowserItemInfo item;
			item.fullpath = fp;
			item.relativePath = fp.substr(globalLibraryPath.length() + 1);
			item.prettyName = fp.substr(fp.find_last_of("/") + 1);
			item.isDirectory = std::filesystem::is_directory(fp);

			if (fp.ends_with(".wav")) {
				item.prettyType = "Waveform Audio";
			} else if (fp.ends_with(".flac")) {
				item.prettyType = "Free Lossless Audio Codec";
			} else if (fp.ends_with(".mp3")) {
				item.prettyType = "MPEG-1 Audio Layer III";
			} else if (fp.ends_with(".ogg")) {
				item.prettyType = "OGG / Vorbis Audio Codec";
			} else {
				item.prettyType = "?? Unknown other file type ??";
			}

			clips.push_back(item);
		}

		return true;
	});

}

void ClipBrowser::addFileToLibrary(const std::string& filepath) {
	if (!std::filesystem::exists(filepath)) {
		ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Specified file doesn't exist??"});
		EC_ERROUT(EC_THIS, "Caller tried to add file that doesn't exist?\nFile in question: %s", filepath.c_str());
		return;
	}

	if (std::filesystem::is_directory(filepath)) {
		ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Can't add folders to the Global Library!"});
		EC_ERROUT(EC_THIS, "Can't add dirs to GlobalLibrary!!\nDir in question: %s", filepath.c_str());
		return;
	}

	if (!std::filesystem::is_regular_file(filepath)) {
		ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Can't add non-regular files!"});
		EC_ERROUT(EC_THIS, "Can't add non-regular files!\nFP in question: %s", filepath.c_str());
		return;
	}

	try {
		std::filesystem::copy_file(filepath, globalLibraryPath + "/" + filepath.substr(filepath.find_last_of("/") + 1));
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