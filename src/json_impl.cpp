#include "json_impl.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace Shadow::JSON {

void dumpJsonToBson(json& obj, const std::string& filepath) {
	std::vector<uint8_t> bson = json::to_bson(obj);
	std::ofstream outfile(filepath);
	for (int i = 0; i < bson.size(); i++) {
		outfile << bson[i];
	}
	outfile.close();
}

void dumpJsonToFile(json& obj, const std::string& filepath, bool formatted) {
	std::ofstream outfile(filepath);

	if (formatted) outfile << obj.dump(2);
	else outfile << obj.dump();
	
	outfile.close();
}

json readBsonFile(std::string filepath) {
	std::ifstream file(filepath);
	json decoded = json::from_bson(file);
	file.close();

	return decoded;
}

}