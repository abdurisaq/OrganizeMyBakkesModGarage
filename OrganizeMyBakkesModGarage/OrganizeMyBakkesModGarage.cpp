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
			cvarManager->executeCommand("togglemenu " + GetMenuName());
		}, "Displays the window for bakkes garage.", PERMISSION_ALL
	);

	
	cvarManager->registerNotifier(
		"testPrint",
		[this](std::vector<std::string> args) {
			for (auto [name, map] : itemNameMap) {
				LOG("printing for : {}", name);
				for (auto [slot, value] : map) {
					std::string slotName = EquipslotToString(slot);
					if (slotName == "other") continue;
					LOG("for slot: {}, name is : {}", slotName, value);
				}
			}
		}, "Displays the itemLoadouts", PERMISSION_ALL
	);

	readCurrentBakkesModPreset("cfg\\config.cfg");
	
	readCurrentBinding();

	

	std::filesystem::path myDataFolder = gameWrapper->GetDataFolder() / "OrganizeMyBakkesModGarage";
	groupFilePath = myDataFolder / "groups.txt";

	

	LoadGroupsFromFile(groupFilePath);

	

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
			return;
		}
		std::string idChoice = currentBakkesModPreset;
		while (idChoice == currentBakkesModPreset) {
			int randomFromGroup = currentGroup.second.presets.size() > 0 ? rand() % currentGroup.second.presets.size() : 0;
			idChoice = currentGroup.second.presets[randomFromGroup].id;
		}


		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
		
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
		
		if (!shuffleInOnlineGame) {
			
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

	
	gameWrapper->HookEventPost("Function TAGame.GameInfo_Replay_TA.HandleReplayImported", [this](...) {
		gameWrapper->HookEventPost("Function TAGame.GameInfo_Replay_TA.EventGameEventSet", [this](...) {
			OnReplayOpen();
			gameWrapper->UnhookEventPost("Function TAGame.GameInfo_Replay_TA.EventGameEventSet");
			});
		});
	gameWrapper->HookEvent("Function TAGame.GFxHUD_Replay_TA.Destroyed", [this](...) {
		OnReplayClose();
		});
	
}



void OrganizeMyBakkesModGarage::OnReplayOpen() {
	
	conversionTriggered = false;
	inReplay = true;
	//items = gameWrapper->GetItemsWrapper();
	auto game_event = gameWrapper->GetGameEventAsReplay();
	if (!game_event)
	{
		return;
	}

	auto replay = game_event.GetReplay();
	if (!replay)
	{
		return;
	}
	auto replay_id = replay.GetId().ToString();
	
	gameWrapper->HookEventWithCaller<PriWrapper>("Function TAGame.PRI_TA.HandleLoadoutLoaded", [this](PriWrapper cw, void* params, std::string eventName) {
		if (cw.IsNull()) return;
		std::string id = cw.GetUniqueIdWrapper().GetIdString();
		LOG("id is {}", id);

		auto loadout_maybe = LoadoutUtilities::GetLoadoutFromPri(cw, cw.GetTeamNum2());

		if (!loadout_maybe) return;

		CarInfo info{
			.loadout = *loadout_maybe,
			.team = cw.GetTeamNum2(),
			.player_name = cw.GetPlayerName().ToString(),
			.playerId = id,
		};

		car_info_map[id] = std::move(info);
		LOG("from car info map");
		checkAndConvert();

		});

	gameWrapper->HookEventWithCallerPost<CarMeshComponentBaseWrapper>("Function TAGame.CarMeshComponentBase_TA.SetTeamFinishID", [this](CarMeshComponentBaseWrapper cw, void* params, std::string eventName) {
		if (cw.IsNull()) {
			LOG("ProductAssetWrapper is null");
			return;
		}

		//first 4 bytes is the product id of params;
		int id = *reinterpret_cast<int*>(params);
		currentPaintFinish.first = id;

		});

	

	gameWrapper->HookEventWithCaller<PriWrapper>("Function TAGame.PRI_TA.SetCar", [this](PriWrapper cw, void* params, std::string eventName) {
		if (cw.IsNull()) return;
		auto id = cw.GetUniqueIdWrapper();
		LOG("found car id");
		playerIdString = id.str();
		LOG("id is {}", playerIdString);

		
		
	});

	gameWrapper->HookEventWithCaller<CarMeshComponentBaseWrapper>("Function TAGame.CarMeshComponentBase_TA.SetCustomFinishID", [this](CarMeshComponentBaseWrapper cw, void* params, std::string eventName) {
		if (cw.IsNull()) {
			LOG("ProductAssetWrapper is null");
			return;
		}
		int id = *reinterpret_cast<int*>(params);
		
		currentPaintFinish.second = id;

		paintFinishMap[playerIdString] = currentPaintFinish;
		LOG("setting player id {} to paint finish id {} and secondary {}", playerIdString, currentPaintFinish.first, id);
		
	});
	
}

void OrganizeMyBakkesModGarage::OnReplayClose() 
{

	gameWrapper->UnhookEvent("Function TAGame.Car_TA.UpdateTeamLoadout");
	gameWrapper->UnhookEvent("Function TAGame.PlayerVanity_TA.SetPlayerID");
	gameWrapper->UnhookEvent("Function TAGame.CarMeshComponentBase_TA.SetCustomFinishID");
	gameWrapper->UnhookEvent("Function TAGame.GFxData_PRI_TA.SetPlayerIDString");
	gameWrapper->UnhookEvent("Function TAGame.CarMeshComponentBase_TA.SetTeamFinishID");
	gameWrapper->UnhookEvent("Function TAGame.PRI_TA.HandleLoadoutLoaded");
	

	car_info_map.clear();
	carInfoBM.clear();
	paintFinishMap.clear();
	currentPaintFinish = { 0,0 };
	carInfoBMString.clear();

	inReplay = false;

}



void OrganizeMyBakkesModGarage::onUnload() {
	//LOG("Plugin  organizemygarage unloaded!");
	SaveGroupsToFile(groupFilePath);
}

void OrganizeMyBakkesModGarage::OnClose()
{
	PluginWindowBase::OnClose(); 
	SaveGroupsToFile(groupFilePath);
	
}

void OrganizeMyBakkesModGarage::OnOpen() {

	PluginWindowBase::OnOpen(); 
	
	presets = readPresets("data\\presets.data");
}






