#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


BAKKESMOD_PLUGIN(OrganizeMyBakkesModGarage, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

//std::vector<std::pair<std::string, std::vector<Preset>>> groups;
//std::string newGroupName;


void OrganizeMyBakkesModGarage::onLoad()
{
	_globalCvarManager = cvarManager;

	//std::vector<Preset> presets = this->readPresets("data\\presets.data");
	


	cvarManager->registerNotifier(
		"open_organizemybakkesmodgarage_ui",
		[this](std::vector<std::string> args) {
			if (!isWindowOpen_) {
				Render();
			}
		}, "Displays the window for bakkes garage.", PERMISSION_ALL
	);

	std::filesystem::path myDataFolder = gameWrapper->GetDataFolder() / "OrganizeMyBakkesModGarage";
	groupFilePath = myDataFolder / "groups.txt";
	LoadGroupsFromFile(groupFilePath);


	/*cvarManager->registerNotifier("get_map_name",
		[this](std::vector<std::string> args) {
			std::string mapName = gameWrapper->GetCurrentMap();
			unsigned long long id =gameWrapper->GetSteamID();
			LOG("Current map: {}", mapName);
			LOG("Steam ID: {}", id);
		}, "Logs the current map name to the console", PERMISSION_ALL);*/


	

	/*gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", [this](std::string eventName) {
		if (!defaultHooked){
			defaultHooked = true;
			cvarManager->setBind("F4", "open_organizemybakkesmodgarage_ui");
			LOG("Default bind set to F4 on main menu load.");
		}		
	});*/

	//LOG("Plugin loaded!");
	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("OrganizeMyBakkesModGarage debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	/*cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		LOG("the cvar with name: {} changed", cvarName);
		LOG("the new value is: {}", newCvar.getStringValue());
	});*/

	//cvar2.addOnValueChanged(std::bind(&OrganizeMyBakkesModGarage::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&OrganizeMyBakkesModGarage::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&OrganizeMyBakkesModGarage::YourPluginMethod, this);
}


void OrganizeMyBakkesModGarage::SaveGroupsToFile(const std::filesystem::path& filePath) {

	std::filesystem::create_directories(filePath.parent_path());

	std::ofstream outFile(filePath);
	if (!outFile.is_open()) {
		std::cerr << "Failed to open file for writing: " << filePath << std::endl;
		return;
	}

	for (const auto& group : groups) {
		// Write group name
		outFile << group.first << ":\n";  // group.first is the group name

		// Write presets in the group
		for (const auto& preset : group.second) {
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

	while (std::getline(inFile, line)) {
		if (line.empty()) continue;

		// If it's a group name line (e.g., "GroupName:")
		if (line.back() == ':') {
			if (!currentGroupName.empty()) {
				// Save the previous group if it exists
				groups.push_back({ currentGroupName, currentPresets });
			}

			currentGroupName = line.substr(0, line.size() - 1);  // Remove the ":"
			currentPresets.clear();
		}
		// If it's a preset line (e.g., "    - PresetName: preset_id")
		else if (line.find("    - ") == 0) {
			std::string presetLine = line.substr(6);  // Remove the "    - " part
			std::istringstream ss(presetLine);
			std::string presetName, presetId;
			if (std::getline(ss, presetName, ':') && std::getline(ss, presetId)) {
				currentPresets.push_back({ presetName, presetId });
			}
		}
	}

	// Add the last group
	if (!currentGroupName.empty()) {
		groups.push_back({ currentGroupName, currentPresets });
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

	LOG("Reading presets from: {}", path);

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

void OrganizeMyBakkesModGarage::onUnload() {
	SaveGroupsToFile(groupFilePath);
}

void OrganizeMyBakkesModGarage::OnGameThread(std::function<void()>&& func) const
{
	gameWrapper->Execute([func = std::move(func)](...) {
		func();
		});
}

//
//
void OrganizeMyBakkesModGarage::RenderSettings()
{

	static char input_buffer[16] = "F4";
	ImGui::Text("Current binding: %s", bind_key);
	ImGui::InputText("##bind_key", input_buffer, IM_ARRAYSIZE(input_buffer));
	ImGui::SameLine();
	if (ImGui::Button("Set bind")) {
		cvarManager->removeBind(bind_key);
		cvarManager->setBind(input_buffer, "open_organizemybakkesmodgarage_ui");
		bind_key = input_buffer;
		LOG("Key bind changed to: {}", input_buffer);
	}


	ImGui::Text("Hello World in settings");
}

std::string toLowerCase(const std::string& str) {
	std::string lowerStr = str;
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lowerStr;
}



void OrganizeMyBakkesModGarage::RenderWindow() {
	static std::vector<Preset> presets = this->readPresets("data\\presets.data");
	_globalCvarManager = cvarManager;


	ImGui::Text("Create New Group:");
	ImGui::InputText("##GroupName", &newGroupName);
	if (ImGui::Button("Create Group") && !newGroupName.empty()) {
		groups.emplace_back(newGroupName, std::vector<Preset>{});
		newGroupName.clear();
	}

	ImGui::Separator();

	ImVec2 windowSize = ImGui::GetWindowSize();

	float topSectionHeight = ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing() + ImGui::GetFrameHeightWithSpacing();

	float remainingHeight = windowSize.y - topSectionHeight;

	if (!groups.empty()) {
		ImGui::BeginChild("Groups", ImVec2(0, remainingHeight), true); 
		for (int i = 0; i < groups.size(); ++i) {
			
			ImGui::PushID(i);  
			ImVec2 availSpace = ImGui::GetContentRegionAvail(); 
			ImGui::SetNextItemWidth(availSpace.x * 0.5f);
			if (ImGui::Button(("+##AddPreset" + std::to_string(i)).c_str())) {
				currentGroupIndex = i;
				showAddPresetWindow = true;
			}
			ImGui::SameLine();

			if (ImGui::CollapsingHeader((groups[i].first + " (" + std::to_string(groups[i].second.size()) + " presets)").c_str())) {
			
				
				ImVec2 groupWindowSize = ImGui::GetWindowSize();

				ImVec2 childSize = ImVec2(groupWindowSize.x, groupWindowSize.y * 0.2f); 

				
				ImGui::BeginChild("PresetListInGroup", ImVec2(groupWindowSize.x, childSize.y), true);  
				for (size_t j = 0; j < groups[i].second.size(); ++j) {
					const auto& preset = groups[i].second[j];
					ImGui::Button(preset.name.c_str());

					ImGui::SameLine();
					if (ImGui::Button(("Apply##" + std::to_string(i) + std::to_string(j)).c_str())) {
						std::string command = "sleep 1;cl_itemmod_code " + preset.id;
						LOG("Executing preset: %s", preset.id.c_str());
						_globalCvarManager->executeCommand(command);
					}

					ImGui::SameLine();
					if (ImGui::Button(("Delete##" + std::to_string(i) + std::to_string(j)).c_str())) {
						groups[i].second.erase(groups[i].second.begin() + j);
					}
				}
				ImGui::EndChild();  
			}

			ImGui::PopID(); 

			
			ImGui::Separator();  
		}
		ImGui::EndChild();
	}

	
	if (showAddPresetWindow && currentGroupIndex != -1) {
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGui::Begin("Add Preset", &showAddPresetWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		ImGui::InputText("Search##SearchPreset", &searchQuery);

		// convert ot lower case
		std::string lowerSearchQuery = toLowerCase(searchQuery);

		ImGui::BeginChild("PresetList", ImVec2(380, 250), true);  
		for (const auto& preset : presets) {
			
			//
			if (lowerSearchQuery.empty() || toLowerCase(preset.name).find(lowerSearchQuery) != std::string::npos) {
				
				bool presetInGroup = false;
				for (const auto& groupPreset : groups[currentGroupIndex].second) {
					if (groupPreset.id == preset.id) {
						presetInGroup = true;
						break;
					}
				}

				
				if (!presetInGroup) {
					if (ImGui::Selectable(preset.name.c_str())) {
						groups[currentGroupIndex].second.push_back(preset);
						showAddPresetWindow = false;
					}
				}
			}
		}
		ImGui::EndChild(); 

		ImGui::End();
	}
}
