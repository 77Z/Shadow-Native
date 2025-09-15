//
// Created by vince on 9/12/25.
//

#ifndef SHADOW_NATIVE_AXEVSTPLUGINS_HPP
#define SHADOW_NATIVE_AXEVSTPLUGINS_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace Shadow::AXE {

	struct BasicVstEntry {
		std::string prettyName;
		std::string name;
		std::string vendor;
		std::string version;
		std::string url;
		std::string email;
		std::filesystem::path path;
	};

	class AXEVSTPlugins {
	public:
		AXEVSTPlugins();
		~AXEVSTPlugins();

		void onUpdate(bool& p_open);

		static std::filesystem::path getVSTStorageLocation();

		/// Scans VST path to index plugins
		void reloadVSTPlugins();

	private:
		std::vector<BasicVstEntry> indexedVsts;
	};

} // Shadow::AXE

#endif // SHADOW_NATIVE_AXEVSTPLUGINS_HPP
