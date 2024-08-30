#include "AXESerializer.hpp"
#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "imgui.h"
#include "json_impl.hpp"
#include <cstdint>
#include <fstream>
#include <utility>

#define EC_THIS "Song Serializer"

namespace Shadow::AXE {

bool serializeSong(const Song* song, const std::string& filepath) {
	json output;
	output["name"] = song->name;
	output["artist"] = song->artist;
	output["album"] = song->album;
	output["decodeOnLoad"] = song->decodeOnLoad;
	output["bpm"] = song->bpm;
	output["key"] = song->key;
	output["timeSignature"] = song->timeSignature;
	output["masterVolume"] = song->masterVolume;

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

			clipObj["balence"] = clip->balence;
			clipObj["volume"] = clip->volume;

			clipObj["muted"] = clip->muted;

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

		trackObj["balence"] = track.balence;
		trackObj["volume"] = track.volume;

		trackObj["muted"] = track.muted;

		output["tracks"].push_back(trackObj);
	}
	
	// Node Graphs
	output["nodeGraphs"] = json::array();
	for (auto& ng : song->nodeGraphs) {
		json ngObj;

		ngObj["name"] = ng.name;
		ngObj["lastKnownGraphId"] = ng.lastKnownGraphId;

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

	JSON::dumpJsonToFile(output, filepath + ".raw", true);

	// JSON::dumpJsonToBson(output, filepath);

	return true;
}

bool deserializeSong(Song* song, const std::string& filepath) {
	EC_NEWCAT(EC_THIS);

	std::ifstream infile(filepath);
	if (!infile) ERROUT("Failed to read song file, expect failure from here");
	json decodedSong = json::from_bson(infile);
	infile.close();

	song->name = decodedSong["name"];
	song->artist = decodedSong["artist"];
	song->album = decodedSong["album"];
	song->decodeOnLoad = decodedSong["decodeOnLoad"];
	song->bpm = decodedSong["bpm"];
	song->key = decodedSong["key"];
	song->timeSignature[0] = decodedSong["timeSignature"][0];
	song->timeSignature[1] = decodedSong["timeSignature"][1];
	song->masterVolume = decodedSong["masterVolume"];

	for (auto& track : decodedSong["tracks"]) {
		Track tempTrack;
		tempTrack.name = track["name"];

		for (auto& clip : track["clips"]) {
			std::shared_ptr<Clip> tempClip = std::make_shared<Clip>();
			tempClip->name = clip["name"];
			tempClip->baseAudioSource = clip["baseAudioSource"];

			tempClip->position = clip["position"];
			tempClip->length = clip["length"];

			tempClip->balence = clip["balence"];
			tempClip->volume = clip["volume"];

			tempClip->muted = clip["muted"];
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

		tempTrack.balence = track["balence"];
		tempTrack.volume = track["volume"];

		tempTrack.muted = track["muted"];

		EC_PRINT(EC_THIS, "Decoding track: %s", tempTrack.name.c_str());

		song->tracks.push_back(tempTrack);
	}

	// Node Graphs
	for (auto& ng: decodedSong["nodeGraphs"]) {
		NodeGraph tempNg;

		tempNg.name = ng["name"];
		tempNg.lastKnownGraphId = ng["lastKnownGraphId"];

		for (auto& node : ng["nodes"]) {
			Node tempNode;
			tempNode.name = node["name"];
			tempNode.size = ImVec2(node["size"][0], node["size"][1]);
			tempNode.id = (int)node["id"];

			tempNode.color = ImColor(
				(float)node["color"][0],
				(float)node["color"][1],
				(float)node["color"][2],
				(float)node["color"][3]);

			for (auto& input : node["inputs"]) {
				Pin tempInput;
				tempInput.name = input["name"];
				tempInput.kind = input["kind"];
				tempInput.id = (int)input["id"];

				tempNode.inputs.push_back(tempInput);
			}

			for (auto& output : node["outputs"]) {
				Pin tempOutput;
				tempOutput.name = output["name"];
				tempOutput.kind = output["kind"];
				tempOutput.id = (int)output["id"];

				tempNode.outputs.push_back(tempOutput);
			}
			
			tempNg.nodes.push_back(tempNode);
		}

		for (auto& link : ng["links"]) {
			Link tempLink;
			tempLink.id = (int)link["id"];
			tempLink.inputId = (int)link["inputId"];
			tempLink.outputId = (int)link["outputId"];

			tempNg.links.push_back(tempLink);
		}

		song->nodeGraphs.push_back(tempNg);
	}

	return true;
}

}