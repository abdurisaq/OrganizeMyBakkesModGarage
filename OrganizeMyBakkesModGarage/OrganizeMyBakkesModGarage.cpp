#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


BAKKESMOD_PLUGIN(OrganizeMyBakkesModGarage, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;



void OrganizeMyBakkesModGarage::onLoad()
{
	_globalCvarManager = cvarManager;	


	cvarManager->registerNotifier(
		"open_organizemybakkesmodgarage_ui",
		[this](std::vector<std::string> args) {
			if (!isWindowOpen_) {
				LOG("Opening OrganizeMyBakkesModGarage window");
				
				cvarManager->executeCommand("togglemenu " + GetMenuName());
				
				
			}
		}, "Displays the window for bakkes garage.", PERMISSION_ALL
	);

	

	std::filesystem::path myDataFolder = gameWrapper->GetDataFolder() / "OrganizeMyBakkesModGarage";
	groupFilePath = myDataFolder / "groups.txt";
	LoadGroupsFromFile(groupFilePath);


	


	

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
	LOG("Plugin  organizemygarage unloaded!");
	SaveGroupsToFile(groupFilePath);
}

void OrganizeMyBakkesModGarage::OnClose()
{
	PluginWindowBase::OnClose(); // Call base class for any default behavior
	SaveGroupsToFile(groupFilePath);
	LOG("Groups saved to file on window close.");
}


