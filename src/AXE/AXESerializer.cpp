#include "AXESerializer.hpp"
#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "imgui.h"
#include "json_impl.hpp"
#include <cstdint>
#include <exception>
#include <fstream>
#include <memory>
#include <utility>
#include "GoDownInFlames.hpp"

#define EC_THIS "Song Serializer"

namespace Shadow::AXE {

bool serializeSong(const Song* song, const std::string& filepath) {
	json output;
	output["songFileVersion"] = song->songFileVersion;
	output["name"] = song->name;
	output["artist"] = song->artist;
	output["album"] = song->album;
	output["decodeOnLoad"] = song->decodeOnLoad;
	output["bpm"] = song->bpm;
	output["key"] = song->key;
	output["timelineUnits"] = song->timelineUnits;
	output["timeSignature"] = song->timeSignature;
	output["masterVolume"] = song->masterVolume;
	output["lastKnownGraphId"] = song->lastKnownGraphId;
	output["songLength"] = song->songLength;

	output["tracks"] = json::array();
	for (auto& track : song->tracks) {
		json trackObj;

		trackObj["name"] = track.name;

		trackObj["clips"] = json::array();
		for (auto& clip : track.clips) {
			json clipObj;
			clipObj["name"] = clip->name;
			clipObj["baseAudioSource"] = clip->baseAudioSource;

			clipObj["position"] = clip->position;
			clipObj["length"] = clip->length;

			clipObj["balance"] = clip->balance;
			clipObj["volume"] = clip->volume;

			clipObj["muted"] = clip->muted;

			clipObj["clipType"] = clip->clipType;

			// TODO: this feels like the point where breaking out into
			// subroutines might be beneficial to the readability of the code
			if (clip->drumData != nullptr) {
				for (auto& drumTrack : clip->drumData->drumTracks) {
					json drumTrackObj;

					drumTrackObj["samplePath"] = drumTrack.samplePath;
					drumTrackObj["beats"] = json::array();
					for (size_t i = 0; i < drumTrack.beats.size(); i++) {
						drumTrackObj["beats"].push_back(static_cast<bool>(drumTrack.beats[i]));
					}

					clipObj["drumData"]["drumTracks"].push_back(drumTrackObj);
				}
				clipObj["drumData"]["measures"] = clip->drumData->measures;
			}

			if (clip->pianoRollData != nullptr) {
				// placeholder to let the deserializer know that there is piano
				// data
				clipObj["pianoData"] = 1;
			}

			trackObj["clips"].push_back(clipObj);
		}

		trackObj["automations"] = json::array();
		for (auto& automation : track.automations) {
			json autoObj;

			autoObj["startRail"] = automation.startRail;
			autoObj["endRail"] = automation.endRail;

			autoObj["points"] = json::array();
			for (auto& point : automation.points) {
				json pointObj;
				pointObj.push_back(point.first);
				pointObj.push_back(point.second);

				autoObj["points"].push_back(pointObj);
			}

			autoObj["color"].push_back(automation.color.Value.x);
			autoObj["color"].push_back(automation.color.Value.y);
			autoObj["color"].push_back(automation.color.Value.z);
			autoObj["color"].push_back(automation.color.Value.w);

			autoObj["visible"] = automation.visible;

			trackObj["automations"].push_back(autoObj);
		}

		trackObj["balance"] = track.balance;
		trackObj["volume"] = track.volume;

		trackObj["muted"] = track.muted;

		trackObj["color"].push_back(track.color.Value.x);
		trackObj["color"].push_back(track.color.Value.y);
		trackObj["color"].push_back(track.color.Value.z);
		trackObj["color"].push_back(track.color.Value.w);

		output["tracks"].push_back(trackObj);
	}
	
	// Node Graphs
	output["nodeGraphs"] = json::array();
	for (auto& ng : song->nodeGraphs) {
		json ngObj;

		ngObj["name"] = ng.name;
		ed::SetCurrentEditor(ng.editorContext);
		ngObj["editorContextData"] = json::parse(ed::VTGetManualSaveState());
		ed::SetCurrentEditor(nullptr);
		// ngObj["lastKnownGraphId"] = ng.lastKnownGraphId;

		ngObj["nodes"] = json::array();
		for (auto& node : ng.nodes) {
			json nodeObj;
			nodeObj["name"] = node.name;
			nodeObj["size"].push_back(node.size.x);
			nodeObj["size"].push_back(node.size.y);
			nodeObj["id"] = node.id.Get();
			
			nodeObj["color"].push_back(node.color.Value.x);
			nodeObj["color"].push_back(node.color.Value.y);
			nodeObj["color"].push_back(node.color.Value.z);
			nodeObj["color"].push_back(node.color.Value.w);

			nodeObj["inputs"] = json::array();
			for (auto& input : node.inputs) {
				json inputObj;
				inputObj["name"] = input.name;
				inputObj["kind"] = input.kind;
				inputObj["id"] = input.id.Get();
				
				nodeObj["inputs"].push_back(inputObj);
			}

			nodeObj["outputs"] = json::array();
			for (auto& output : node.outputs) {
				json outputObj;
				outputObj["name"] = output.name;
				outputObj["kind"] = output.kind;
				outputObj["id"] = output.id.Get();
				
				nodeObj["outputs"].push_back(outputObj);
			}
			ngObj["nodes"].push_back(nodeObj);
		}

		ngObj["links"] = json::array();
		for (auto& link : ng.links) {
			json linkObj;
			linkObj["id"] = link.id.Get();
			linkObj["inputId"] = link.inputId.Get();
			linkObj["outputId"] = link.outputId.Get();

			ngObj["links"].push_back(linkObj);
		}

		output["nodeGraphs"].push_back(ngObj);
	}

	// Bookmarks
	output["bookmarks"] = json::array();
	for (auto& b : song->bookmarks) {
		json bmObj;

		bmObj["name"] = b.name;
		bmObj["color"].push_back(b.color.Value.x);
		bmObj["color"].push_back(b.color.Value.y);
		bmObj["color"].push_back(b.color.Value.z);
		bmObj["color"].push_back(b.color.Value.w);
		bmObj["position"] = b.position;

		output["bookmarks"].push_back(bmObj);
	}

	// JSON::dumpJsonToFile(output, filepath + ".raw", true);

	JSON::dumpJsonToBson(output, filepath);

	return true;
}

bool deserializeSong(Song* song, const std::string& filepath) {
	EC_NEWCAT(EC_THIS);

	std::ifstream infile(filepath);
	if (!infile) {
		ERROUT("Failed to read song file!");
		ERROUT("Attempted to read path %s", filepath.c_str());

		BAILOUT("Failed to read song file, talk to Vince and check stdout");
		return false;
	}
	try {
		json decodedSong = json::from_bson(infile);
		infile.close();

		song->songFileVersion = decodedSong["songFileVersion"];
		song->name = static_cast<std::string>(decodedSong["name"]);
		song->artist = static_cast<std::string>(decodedSong["artist"]);
		song->album = static_cast<std::string>(decodedSong["album"]);
		song->decodeOnLoad = decodedSong["decodeOnLoad"];
		song->bpm = decodedSong["bpm"];
		song->key = decodedSong["key"];
		song->timelineUnits = decodedSong["timelineUnits"];
		song->timeSignature[0] = decodedSong["timeSignature"][0];
		song->timeSignature[1] = decodedSong["timeSignature"][1];
		song->masterVolume = decodedSong["masterVolume"];
		song->lastKnownGraphId = decodedSong["lastKnownGraphId"];
		song->songLength = decodedSong["songLength"];

		for (auto& track : decodedSong["tracks"]) {
			Track tempTrack;
			tempTrack.name = static_cast<std::string>(track["name"]);

			for (auto& clip : track["clips"]) {
				std::shared_ptr<Clip> tempClip = std::make_shared<Clip>();
				tempClip->name = static_cast<std::string>(clip["name"]);
				tempClip->baseAudioSource = static_cast<std::string>(clip["baseAudioSource"]);

				tempClip->position = clip["position"];
				tempClip->length = clip["length"];

				tempClip->balance = clip["balance"];
				tempClip->volume = clip["volume"];

				tempClip->muted = clip["muted"];

				tempClip->clipType = clip["clipType"];
				// TODO: this feels like the point where breaking out into
				// subroutines might be beneficial to the readability of the code
				if (clip["drumData"] != nullptr) {
					tempClip->drumData = std::make_shared<DrumMachineData>();

					for (auto& drumTrack : clip["drumData"]["drumTracks"]) {
						DrumTrack tempDrumTrack;

						tempDrumTrack.samplePath = (std::string)drumTrack["samplePath"];

						for (auto& beat : drumTrack["beats"]) {
							tempDrumTrack.beats.emplace_back((bool)beat);
						}

						tempClip->drumData->drumTracks.emplace_back(tempDrumTrack);
					}

					tempClip->drumData->measures = clip["drumData"]["measures"];
				}

				if (clip["pianoData"] != nullptr) {
					tempClip->pianoRollData = std::make_shared<PianoRollData>();


				}


				EC_PRINT(EC_THIS, "Decoded clip: %s at position %.3f", tempClip->name.c_str(), (float)tempClip->position);
				tempTrack.clips.push_back(tempClip);
			}

			for (auto& automation : track["automations"]) {
				Automation tempAuto;

				tempAuto.startRail = automation["startRail"];
				tempAuto.endRail = automation["endRail"];

				for (auto& point : automation["points"]) {
					tempAuto.points.push_back(
						std::pair<uint64_t, float>(point[0], point[1])
					);
				}

				tempAuto.color = ImColor(
					(float)automation["color"][0],
					(float)automation["color"][1],
					(float)automation["color"][2],
					(float)automation["color"][3]
				);

				tempAuto.visible = automation["visible"];

				tempTrack.automations.push_back(tempAuto);
			}

			tempTrack.balance = track["balance"];
			tempTrack.volume = track["volume"];

			tempTrack.muted = track["muted"];

			tempTrack.color = ImColor(
				(float)track["color"][0],
				(float)track["color"][1],
				(float)track["color"][2],
				(float)track["color"][3] // Technically unused
			);

			EC_PRINT(EC_THIS, "Decoding track: %s", tempTrack.name.c_str());

			song->tracks.push_back(tempTrack);
		}

		// Node Graphs
		for (auto& ng: decodedSong["nodeGraphs"]) {
			NodeGraph tempNg;

			tempNg.name = static_cast<std::string>(ng["name"]);
			tempNg.config.SettingsFile = nullptr;
			tempNg.config.NavigateButtonIndex = 2;
			tempNg.editorContext = ed::CreateEditor(&tempNg.config);

			ed::SetCurrentEditor(tempNg.editorContext);
			ed::VTManualStateLoad(json(ng["editorContextData"]).dump());

			auto& colors = ed::GetStyle().Colors;
			colors[ed::StyleColor_Bg]                = ImColor(0, 0, 0, 255);
			colors[ed::StyleColor_HovNodeBorder]     = ImColor(255, 69, 0, 255);
			colors[ed::StyleColor_SelNodeBorder]     = ImColor(255, 0, 0, 255);
			colors[ed::StyleColor_PinRect]           = ImColor(255, 0, 0, 255);
		
			colors[ed::StyleColor_LinkSelRect]       = ImColor(255, 0, 0, 50);
			colors[ed::StyleColor_LinkSelRectBorder] = ImColor(255, 0, 0, 255);
			colors[ed::StyleColor_NodeSelRect]       = ImColor(255, 0, 0, 50);
			colors[ed::StyleColor_NodeSelRectBorder] = ImColor(255, 0, 0, 255);
		
			ed::SetCurrentEditor(nullptr);
			// tempNg.lastKnownGraphId = ng["lastKnownGraphId"];

			for (auto& node : ng["nodes"]) {
				Node tempNode;
				tempNode.name = static_cast<std::string>(node["name"]);
				tempNode.size = ImVec2(node["size"][0], node["size"][1]);
				tempNode.id = static_cast<int>(node["id"]);

				tempNode.color = ImColor(
					(float)node["color"][0],
					(float)node["color"][1],
					(float)node["color"][2],
					(float)node["color"][3]);

				for (auto& input : node["inputs"]) {
					Pin tempInput;
					tempInput.name = static_cast<std::string>(input["name"]);
					tempInput.kind = input["kind"];
					tempInput.id = static_cast<int>(input["id"]);

					tempNode.inputs.push_back(tempInput);
				}

				for (auto& output : node["outputs"]) {
					Pin tempOutput;
					tempOutput.name = static_cast<std::string>(output["name"]);
					tempOutput.kind = output["kind"];
					tempOutput.id = static_cast<int>(output["id"]);

					tempNode.outputs.push_back(tempOutput);
				}
				
				tempNg.nodes.push_back(tempNode);
			}

			for (auto& link : ng["links"]) {
				Link tempLink;
				tempLink.id = static_cast<int>(link["id"]);
				tempLink.inputId = static_cast<int>(link["inputId"]);
				tempLink.outputId = static_cast<int>(link["outputId"]);

				tempNg.links.push_back(tempLink);
			}

			song->nodeGraphs.emplace_back(std::move(tempNg));
		}

		for (auto& bm: decodedSong["bookmarks"]) {
			Bookmark tempBm;

			tempBm.name = static_cast<std::string>(bm["name"]);
			tempBm.color = ImColor(
				(float)bm["color"][0],
				(float)bm["color"][1],
				(float)bm["color"][2],
				(float)bm["color"][3]);
			tempBm.position = bm["position"];

			song->bookmarks.emplace_back(std::move(tempBm));
		}
	} catch (const std::exception& e) {
		BAILOUT("Failed to read song file after stdin stage.\n"
			"Talk to vince to send your song file and he can manually decode it.\n"
			"Exception thrown: " + std::string(e.what()));
	}

	return true;
}

}