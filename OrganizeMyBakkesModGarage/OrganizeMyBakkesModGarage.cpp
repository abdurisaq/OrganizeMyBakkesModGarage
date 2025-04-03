#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


BAKKESMOD_PLUGIN(OrganizeMyBakkesModGarage, "Organize BM Presets", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;



void OrganizeMyBakkesModGarage::onLoad()
{
	_globalCvarManager = cvarManager;	


	cvarManager->registerNotifier(
		"open_organizemybakkesmodgarage_ui",
		[this](std::vector<std::string> args) {
			if (!isWindowOpen_) {
				//LOG("Opening OrganizeMyBakkesModGarage window");
				
				cvarManager->executeCommand("togglemenu " + GetMenuName());
				
				
			}
		}, "Displays the window for bakkes garage.", PERMISSION_ALL
	);
	readCurrentBakkesModPreset("cfg\\config.cfg");
	
	readCurrentBinding();

	

	std::filesystem::path myDataFolder = gameWrapper->GetDataFolder() / "OrganizeMyBakkesModGarage";
	groupFilePath = myDataFolder / "groups.txt";

	

	LoadGroupsFromFile(groupFilePath);

	//TAGame.Mutator_Freeplay_TA.Init

	cvarManager->registerCvar("swap_enabled", "0", "Enable car bodySwapping", true, true, 0, true, 1,true)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		swapCarBodyCapability = cvar.getBoolValue();
			});
	
	cvarManager->registerCvar("freeplay_shuffle_enabled", "0", "Enable shuffling in freeplay", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		shuffleInFreeplay = cvar.getBoolValue();
			});
	cvarManager->registerCvar("online_shuffle_enabled", "0", "Enable shuffling in online games", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
			shuffleInOnlineGame = cvar.getBoolValue();
			});
	cvarManager->registerCvar("mainPresetGroupName", "", "group name for main preset group", true, false, 0, false, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		currentGroupNameCvar = cvar.getStringValue();
			});
	

	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", [this](std::string eventName) {
		if (currentGroup.first.empty()) {
			if (!reloadedCurrentGroup) {
				updateCurrentGroup();
				reloadedCurrentGroup = true;
			}
			if (currentGroup.first.empty()) {
				return;
			}
			return;
		}
		if (!shuffleInFreeplay) {
			//LOG("Not shuffling in freeplay");
			return;
		}
		std::string idChoice = currentBakkesModPreset;
		while (idChoice == currentBakkesModPreset) {
			int randomFromGroup = currentGroup.second.presets.size() > 0 ? rand() % currentGroup.second.presets.size() : 0;
			idChoice = currentGroup.second.presets[randomFromGroup].id;
		}


		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
		//LOG("Executing preset swap");
		if (swapCarBodyCapability) {
			decodePresetId(idChoice);
		}
		gameWrapper->Execute([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command, false);
			});
		currentBakkesModPreset = idChoice;



		});

	
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", [this](std::string eventName) {
		if (currentGroup.first.empty()) {
			if (!reloadedCurrentGroup) {
				updateCurrentGroup();
				reloadedCurrentGroup = true;
			}
			if (currentGroup.first.empty()) {
				return;
			}
		}
		//if (gameWrapper->IsInOnlineGame())return;
		if (!shuffleInOnlineGame) {
			//LOG("Not shuffling in online game");
			return;
		}
		std::string idChoice = currentBakkesModPreset;
		while (idChoice == currentBakkesModPreset) {
			int randomFromGroup = currentGroup.second.presets.size() > 0 ? rand() % currentGroup.second.presets.size() : 0;
			idChoice = currentGroup.second.presets[randomFromGroup].id;
		}


		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
		//LOG("Executing preset swap");
		if (swapCarBodyCapability) {
			decodePresetId(idChoice);
		}
		gameWrapper->Execute([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command, false);
			});
		currentBakkesModPreset = idChoice;
		
		});
	gameWrapper->HookEvent("Function TAGame.GFxShell_TA.LeaveMatch", [this](std::string eventName) {
		if (currentGroup.first.empty()) return;
		//if (gameWrapper->IsInOnlineGame())return;
		if (!shuffleInOnlineGame) {
			//LOG("Not shuffling in online game");
			return;
		}
		std::string idChoice = currentBakkesModPreset;
		while (idChoice == currentBakkesModPreset) {
			int randomFromGroup = currentGroup.second.presets.size() > 0 ? rand() % currentGroup.second.presets.size() : 0;
			idChoice = currentGroup.second.presets[randomFromGroup].id;
		}


		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
		//LOG("Executing preset swap");
		if (swapCarBodyCapability) {
			decodePresetId(idChoice);
		}
		gameWrapper->Execute([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command, false);
			});
		currentBakkesModPreset = idChoice;

		});

	

	
}




void OrganizeMyBakkesModGarage::onUnload() {
	//LOG("Plugin  organizemygarage unloaded!");
	SaveGroupsToFile(groupFilePath);
}

void OrganizeMyBakkesModGarage::OnClose()
{
	PluginWindowBase::OnClose(); // Call base class for any default behavior
	SaveGroupsToFile(groupFilePath);
	//LOG("Groups saved to file on window close.");
}

void OrganizeMyBakkesModGarage::OnOpen() {

	PluginWindowBase::OnOpen(); // Call base class for any default behavior
	//LOG("Groups loaded from file on window open.");
	presets = readPresets("data\\presets.data");
}





