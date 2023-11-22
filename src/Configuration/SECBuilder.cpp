#include "Configuration/SECBuilder.hpp"
#include "ppk_assert_impl.hpp"
#include <fstream>

namespace Shadow {
namespace Configuration {

	SECBuilder::SECBuilder(std::string fileName)
		: fileName(fileName) { }

	SECBuilder::~SECBuilder() { }

	void SECBuilder::add(std::string key, std::string value) {
		PPK_ASSERT(!closed, "SEC file already written to! You can't add more");
		out.append(key + ":" + value + "\n");
	}

	void SECBuilder::write() {
		std::ofstream file(fileName);
		file << out;
		file.close();
		closed = true;
	}

}
}