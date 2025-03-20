#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


BAKKESMOD_PLUGIN(OrganizeMyBakkesModGarage, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

std::vector<std::pair<std::string, std::vector<Preset>>> groups;
std::string newGroupName;

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
		}, "Displays the window for bakkes garage", PERMISSION_ALL
	);

	gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", [this](std::string eventName) {
		if (!defaultHooked){
			defaultHooked = true;
			cvarManager->setBind("F4", "open_organizemybakkesmodgarage_ui");
			LOG("Default bind set to F4 on main menu load.");
		}		
	});

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


void OrganizeMyBakkesModGarage::RenderWindow() {
	_globalCvarManager = cvarManager;
	static std::vector<Preset> presets = this->readPresets("data\\presets.data");


	// create grups
	ImGui::Text("Create New Group:");
	ImGui::InputText("##GroupName", &newGroupName);
	if (ImGui::Button("Create Group") && !newGroupName.empty()) {
		groups.emplace_back(newGroupName, std::vector<Preset>{});
		newGroupName.clear();
	}

	ImGui::Separator();

	// group stuff
	for (size_t i = 0; i < groups.size(); ++i) {
		ImGui::Text("%s (%zu presets)", groups[i].first.c_str(), groups[i].second.size());

		ImGui::SameLine();
		if (ImGui::Button(("+##AddPreset" + std::to_string(i)).c_str())) {
			ImGui::OpenPopup(("AddPresetPopup##" + std::to_string(i)).c_str());
		}

		if (ImGui::BeginPopup(("AddPresetPopup##" + std::to_string(i)).c_str())) {
			for (const auto& preset : presets) {
				if (ImGui::Selectable(preset.name.c_str())) {
					groups[i].second.push_back(preset);
				}
			}
			ImGui::EndPopup();
		}

		if (!groups[i].second.empty()) {
			if (ImGui::CollapsingHeader(("Group: " + groups[i].first).c_str())) {
				for (size_t j = 0; j < groups[i].second.size(); ++j) {
					const auto& preset = groups[i].second[j];
					ImGui::Text("%s", preset.name.c_str());

					ImGui::SameLine();
					if (ImGui::Button(("-##RemovePreset" + std::to_string(i) + std::to_string(j)).c_str())) {
						groups[i].second.erase(groups[i].second.begin() + j);
					}

					ImGui::SameLine();
					if (ImGui::Button(("Run##RunPreset" + std::to_string(i) + std::to_string(j)).c_str())) {
						std::string command = "sleep 1;cl_itemmod_code " + preset.id;
						LOG("Executing preset: %s", preset.id.c_str());
						cvarManager->executeCommand(command);
						/*OnGameThread([this,command] {
		                    cvarManager->executeCommand(command);
							    });*/
					}
				}
			}
		}
	}

}