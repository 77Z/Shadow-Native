#include "AXESerializer.hpp"
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
		
		for (auto& automation : track.automations) {
			
		}

		trackObj["balence"] = track.balence;
		trackObj["volume"] = track.volume;

		trackObj["muted"] = track.muted;

		output["tracks"].push_back(trackObj);
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

		tempTrack.balence = track["balence"];
		tempTrack.volume = track["volume"];

		tempTrack.muted = track["muted"];

		EC_PRINT(EC_THIS, "Decoding track: %s", tempTrack.name.c_str());

		song->tracks.push_back(tempTrack);
	}

	return true;
}

}