#include "AXESerializer.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "json_impl.hpp"
#include <fstream>

#define EC_THIS "Song Serializer"

namespace Shadow::AXE {

bool serializeSong(const Song* song) {
	json output;
	output["bpm"] = song->bpm;
	output["key"] = song->key;
	output["timeSignature"] = song->timeSignature;
	output["tracks"] = json::array();
	for (auto& track : song->tracks) {
		json trackObj;

		trackObj["name"] = track.name;
		
		for (auto& automation : track.automations) {
			
		}

		output["tracks"].push_back(trackObj);
	}

	JSON::dumpJsonToFile(output, "./AXEproject.json", true);


	JSON::dumpJsonToBson(output, "./AXEproject.bson");

	return true;
}

bool deserializeSong(Song* song) {
	EC_NEWCAT(EC_THIS);

	std::ifstream infile("./AXEproject.bson");
	if (!infile) ERROUT("Failed to read song file, expect failure from here");
	json decodedSong = json::from_bson(infile);
	infile.close();

	song->bpm = decodedSong["bpm"];
	song->key = decodedSong["key"];
	song->timeSignature[0] = decodedSong["timeSignature"][0];
	song->timeSignature[1] = decodedSong["timeSignature"][1];

	for (auto& track : decodedSong["tracks"]) {
		Track tempTrack;
		tempTrack.name = track["name"];

		EC_PRINT(EC_THIS, "Decoding track: %s", tempTrack.name.c_str());

		song->tracks.push_back(tempTrack);
	}

	return true;
}

}