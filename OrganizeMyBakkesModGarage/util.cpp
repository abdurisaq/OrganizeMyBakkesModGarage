#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"
#include <regex>

void OrganizeMyBakkesModGarage::SaveGroupsToFile(const std::filesystem::path& filePath) {
	std::filesystem::create_directories(filePath.parent_path());

	std::ofstream outFile(filePath);
	if (!outFile.is_open()) {
		std::cerr << "Failed to open file for writing: " << filePath << std::endl;
		return;
	}

	for (const auto& group : groups) {
		// Write group name and timestamp
		outFile << group.first << " (Modified: " << group.second.timeModified << "):\n";

		// Write presets in the group
		for (const auto& preset : group.second.presets) {
			outFile << "    - " << preset.name << ": " << preset.id << "\n";
		}

		outFile << "\n"; // Add a blank line between groups
	}

	outFile.close();
	std::cout << "Groups saved to file: " << filePath << std::endl;
}

void OrganizeMyBakkesModGarage::LoadGroupsFromFile(const std::filesystem::path& filePath) {
	std::ifstream inFile(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Failed to open file for reading: " << filePath << std::endl;
		return;
	}

	std::string line;
	std::string currentGroupName;
	std::vector<Preset> currentPresets;
	time_t currentTimestamp = time(nullptr);

	while (std::getline(inFile, line)) {
		if (line.empty()) continue;

		// Extract group name and timestamp using regex or parsing
		if (line.back() == ':') {
			if (!currentGroupName.empty()) {
				// Save the previous group
				groups.push_back({ currentGroupName, PresetGroup(currentPresets, currentTimestamp) });
			}

			// Parse the group name and timestamp
			size_t pos = line.find(" (Modified: ");
			if (pos != std::string::npos) {
				currentGroupName = line.substr(0, pos);
				std::string timestampStr = line.substr(pos + 11, line.size() - pos - 12);
				currentTimestamp = std::stol(timestampStr);
			}
			else {
				currentGroupName = line.substr(0, line.size() - 1); // Remove ":"
				currentTimestamp = time(nullptr);
			}

			currentPresets.clear();
		}
		// Parse preset data
		else if (line.find("    - ") == 0) {
			std::string presetLine = line.substr(6);
			std::istringstream ss(presetLine);
			std::string presetName, presetId;
			if (std::getline(ss, presetName, ':') && std::getline(ss, presetId)) {
				currentPresets.push_back({ presetName, presetId });
			}
		}
	}

	if (!currentGroupName.empty()) {
		groups.push_back({ currentGroupName, PresetGroup(currentPresets, currentTimestamp) });
	}

	inFile.close();
	std::cout << "Groups loaded from file: " << filePath << std::endl;
}

std::vector<Preset> OrganizeMyBakkesModGarage::readPresets(const std::string& file_path) {
	const char* appdata = std::getenv("APPDATA");
	if (appdata == nullptr) {
		std::cerr << "Failed to get APPDATA environment variable\n";
		return {};
	}
	std::string path = std::string(appdata) + "\\bakkesmod\\bakkesmod\\" + file_path;

	//LOG("Reading presets from: {}", path);

	std::vector<Preset> presets;

	std::ifstream file(path);

	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::string name, encoded_id;
			if (std::getline(iss, name, '\t') && std::getline(iss, encoded_id)) {

				presets.push_back({ name, encoded_id });
			}
		}
	}
	else {
		std::cerr << "Failed to open presets.data\n";
	}

	return presets;
}


std::string OrganizeMyBakkesModGarage::toLowerCase(const std::string& str) {
	std::string lowerStr = str;
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lowerStr;
}


void  OrganizeMyBakkesModGarage::readCurrentBakkesModPreset(const std::string& file_path) {
	const char* appdata = std::getenv("APPDATA");
	if (appdata == nullptr) {
		std::cerr << "Failed to get APPDATA environment variable\n";
		return;
	}
	std::string path = std::string(appdata) + "\\bakkesmod\\bakkesmod\\" + file_path;
	
		//LOG("Reading presets from: {}", path);


	std::ifstream file(path);

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return ;
	}

	std::string line;
	std::regex codeRegex(R"(cl_itemmod_code\s+\"([^\"]+)\")");

	while (std::getline(file, line)) {
		std::smatch match;
		if (std::regex_search(line, match, codeRegex) && match.size() > 1) {
			currentBakkesModPreset = match[1].str(); 
			//LOG("Current BakkesMod preset: {}", currentBakkesModPreset);
			 return;
		}
	}

	std::cerr << "cl_itemmod_code not found in the file." << std::endl;
}	


void OrganizeMyBakkesModGarage::reSortGroups() {

	bool direction = sortDirection;

	switch (currentSortOption) {
	case 0:
		std::sort(groups.begin(), groups.end(), [direction](const std::pair<std::string, PresetGroup>& a, const std::pair<std::string, PresetGroup>& b) {
			if (direction) {
				return a.first < b.first;
			}
			return a.first > b.first;
			});
		break;
	case 1:
		std::sort(groups.begin(), groups.end(), [direction](const std::pair<std::string, PresetGroup>& a, const std::pair<std::string, PresetGroup>& b) {
			if (direction) {
				return a.second.timeModified < b.second.timeModified;
			}
			return a.second.timeModified > b.second.timeModified;
			});

		break;

	case 2:
		std::sort(groups.begin(), groups.end(), [direction](const std::pair<std::string, PresetGroup>& a, const std::pair<std::string, PresetGroup>& b) {
			if (direction) {
				return a.second.presets.size() > b.second.presets.size();
			}
			return a.second.presets.size() < b.second.presets.size();
			});
		break;

	}
	pastSortOption = currentSortOption;

}


void OrganizeMyBakkesModGarage::readCurrentBinding() {
	const char* appdata = std::getenv("APPDATA");
	if (appdata == nullptr) {
		std::cerr << "Failed to get APPDATA environment variable\n";
		return;
	}
	std::string path = std::string(appdata) + "\\bakkesmod\\bakkesmod\\" + "cfg\\binds.cfg";

	std::ifstream file(path);

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return;
	}

	std::string line;
	std::regex pattern(R"(bind\s+(\S+)\s+\"open_organizemybakkesmodgarage_ui\")"); // Capture key

	while (std::getline(file, line)) {
		std::smatch match;
		if (std::regex_search(line, match, pattern)) {
			pastBinding = bind_key = match[1].str();
			return ; 
		}
	}
	//default
	cvarManager->setBind("F4", "open_organizemybakkesmodgarage_ui");
	pastBinding = bind_key = "F4";

	return;


}