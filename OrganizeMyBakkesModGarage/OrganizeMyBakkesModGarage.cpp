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
				//LOG("Opening OrganizeMyBakkesModGarage window");
				
				cvarManager->executeCommand("togglemenu " + GetMenuName());
				
				
			}
		}, "Displays the window for bakkes garage.", PERMISSION_ALL
	);
	readCurrentBakkesModPreset("cfg\\config.cfg");
	

	std::filesystem::path myDataFolder = gameWrapper->GetDataFolder() / "OrganizeMyBakkesModGarage";
	groupFilePath = myDataFolder / "groups.txt";
	LoadGroupsFromFile(groupFilePath);

	//TAGame.Mutator_Freeplay_TA.Init
	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", [this](std::string eventName) {
		if (currentGroup.first.empty()) return;
		if (!shuffleInFreeplay) {
			//LOG("Not shuffling in freeplay");
			return;
		}
		//readCurrentBakkesModPreset("cfg\\config.cfg");
		//LOG("Found current preset: {}", currentBakkesModPreset);
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

	gameWrapper->HookEvent("Function TAGame.OnlineGameJoinGame_TA.GetLoadout", [this](std::string eventName) {
		if (currentGroup.first.empty()) return;
		if (!shuffleInOnlineGame) {
			//LOG("Not shuffling in online game");
			return;
		}
		//readCurrentBakkesModPreset("cfg\\config.cfg");
		//LOG("Found current preset: {}", currentBakkesModPreset);
		std::string idChoice = currentBakkesModPreset;
		while (idChoice == currentBakkesModPreset) {
			int randomFromGroup = currentGroup.second.presets.size() > 0 ? rand() % currentGroup.second.presets.size() : 0;
			idChoice = currentGroup.second.presets[randomFromGroup].id;
		}


		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
		//LOG("Executing preset swap");
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





