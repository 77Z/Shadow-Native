#include "AXESerializer.hpp"
#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "json_impl.hpp"
#include <fstream>

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
		
		// for (auto& automation : track.automations) {
			
		// }

		trackObj["clips"] = json::array();
		for (auto& clip : track.clips) {
			json clipObj;
			clipObj["name"] = clip.name;
			clipObj["baseAudioSource"] = clip.baseAudioSource;

			clipObj["position"] = clip.position;
			clipObj["length"] = clip.length;

			clipObj["balence"] = clip.balence;
			clipObj["volume"] = clip.volume;

			clipObj["muted"] = clip.muted;

			trackObj["clips"].push_back(clipObj);
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

		ngObj["nodes"] = json::array();
		for (auto& node : ng.nodes) {
			json nodeObj;
			nodeObj["name"] = node.name;
			nodeObj["size"].push_back(node.size.x);
			nodeObj["size"].push_back(node.size.y);

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

	JSON::dumpJsonToBson(output, filepath);

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
			Clip tempClip;
			tempClip.name = clip["name"];
			tempClip.baseAudioSource = clip["baseAudioSource"];

			tempClip.position = clip["position"];
			tempClip.length = clip["length"];

			tempClip.balence = clip["balence"];
			tempClip.volume = clip["volume"];

			tempClip.muted = clip["muted"];
			EC_PRINT(EC_THIS, "Decoded clip: %s at position %.3f", tempClip.name.c_str(), (float)tempClip.position);
			tempTrack.clips.push_back(tempClip);
		}

		tempTrack.balence = track["balence"];
		tempTrack.volume = track["volume"];

		tempTrack.muted = track["muted"];

		EC_PRINT(EC_THIS, "Decoding track: %s", tempTrack.name.c_str());

		song->tracks.push_back(tempTrack);
	}

	for (auto& ng: decodedSong["nodeGraphs"]) {
		NodeGraph tempNg;

		tempNg.name = ng["name"];

		song->nodeGraphs.push_back(tempNg);
	}

	return true;
}

}