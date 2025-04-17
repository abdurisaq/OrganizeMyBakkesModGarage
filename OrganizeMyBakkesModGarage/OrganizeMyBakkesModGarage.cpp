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

	cvarManager->registerNotifier(
		"printLoadouts",
		[this](std::vector<std::string> args) {
			LogCarInfo(car_info_map);

		}, "print loadouts in replay", PERMISSION_ALL
	);
	cvarManager->setBind("F9", "printLoadouts");




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
	car_info_map.clear();
	carInfoBM.clear();
	paintFinishMap.clear();
	unclaimedEpicIds.clear();
	unclaimedPaintFinishes.clear();
	conversionTriggered = false;
	inReplay = true;
	currentPaintFinish= { 0,0 };
	previousEpicIdsUsed.clear();
	carInfoBMString.clear();
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

	//TAGame.GFxData_PRI_TA.SetPlayerIDString



	gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.GFxData_PRI_TA.SetPlayerIDString",
		[this](ActorWrapper cw, void* params, std::string eventName) {
			struct FString {
				wchar_t* Data;
				int32_t Length;
				int32_t Capacity;
			};

			FString* epicIdStr = reinterpret_cast<FString*>(params);

			if (epicIdStr && epicIdStr->Data) {
				// Convert UTF-16 to UTF-8
				std::wstring wideStr(epicIdStr->Data, epicIdStr->Length);

				// Get required buffer size WITHOUT null terminator
				int size = WideCharToMultiByte(
					CP_UTF8,
					0,
					wideStr.c_str(),
					wideStr.length(),
					nullptr,
					0,
					nullptr,
					nullptr
				);

				if (size > 0) {
					std::string epicId(size-1, '\0');
					WideCharToMultiByte(
						CP_UTF8,
						0,
						wideStr.c_str(),
						wideStr.length(),
						&epicId[0],
						size,
						nullptr,
						nullptr
					);
					LOG("uncleaned Epic ID: {}", epicId);
					// Platform ID extraction
					PlatformId extracted = ExtractPlatformId(epicId);
					if (extracted.type == "Epic") {
						bool found = false;
						for (const auto& id : previousEpicIdsUsed) {
							if (id == extracted.id) {
								found = true;
								break;
							}
						}
					
						for (const auto& id : unclaimedEpicIds) {
							if (id == extracted.id) {
								found = true;
								break;
							}
						}
						if (found) {
							LOG("Epic ID already used: {}", extracted.id);
							return;
						}
						LOG("Epic ID being added to current playerID String : {}", extracted.id);
						playerIdString = extracted.id;
						unclaimedEpicIds.push_back(playerIdString);
						LOG("Cleaned Epic ID: {}", playerIdString);
						if (unclaimedPaintFinishes.size() > 0) {
							paintFinishMap[unclaimedEpicIds.front()] = unclaimedPaintFinishes.back();
		
							unclaimedPaintFinishes.pop_back();
							unclaimedEpicIds.erase(unclaimedEpicIds.begin());
							checkAndConvert();
							LOG("Added unclaimed paint finish to player ID: {}", playerIdString);
							LOG("paint finish id's are {} and {}", paintFinishMap[playerIdString].first, paintFinishMap[playerIdString].second);
							previousEpicIdsUsed.push_back(playerIdString);
							playerIdString.clear();
						}
					}
				}
			}
		}
	);


	gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.PlayerVanity_TA.SetPlayerID", [this](ActorWrapper cw, void* params, std::string eventName) {
		
		tempCount++;
		if (tempCount % 2 == 0) return;
		currentPlayerId = *reinterpret_cast<long long*>(params);
		});
	gameWrapper->HookEventWithCaller<CarMeshComponentBaseWrapper>("Function TAGame.CarMeshComponentBase_TA.SetCustomFinishID", [this](CarMeshComponentBaseWrapper cw, void* params, std::string eventName) {
		if (cw.IsNull()) {
			LOG("ProductAssetWrapper is null");
			return;
		}
		int id = *reinterpret_cast<int*>(params);
		
		currentPaintFinish.second = id;
		LOG("current paint finish is {} and {}", currentPaintFinish.first, currentPaintFinish.second);
		if (currentPlayerId == 0) {
			if (unclaimedEpicIds.empty()) {
				LOG("player id is empty adding to unclaimed paint finishes");
				unclaimedPaintFinishes.push_back(currentPaintFinish);
			}
			else {
				LOG("in paint finish but no id yet, pullnig from unclaimed ids:  {}", unclaimedEpicIds.front());
				paintFinishMap[unclaimedEpicIds.front()] = currentPaintFinish;
				previousEpicIdsUsed.push_back(unclaimedEpicIds.front());
				//unclaimedEpicIds.pop_back();
				unclaimedEpicIds.erase(unclaimedEpicIds.begin());
			
				
				playerIdString.clear();
				checkAndConvert();
			}
		}
		else {
			LOG("user id is {}", currentPlayerId);
			paintFinishMap[std::to_string(currentPlayerId)] = currentPaintFinish;
			LOG("in user id, not set to 0");
			checkAndConvert();
		}
		
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
	
	inReplay = false;

}

const char* GetPaintName(pluginsdk::Itempaint paint)
{
	switch (paint)
	{
	case pluginsdk::Itempaint::NONE: return "None";
	case pluginsdk::Itempaint::CRIMSON: return "Crimson";
	case pluginsdk::Itempaint::LIME: return "Lime";
	case pluginsdk::Itempaint::BLACK: return "Black";
	case pluginsdk::Itempaint::SKYBLUE: return "Sky Blue";
	case pluginsdk::Itempaint::COBALT: return "Cobalt";
	case pluginsdk::Itempaint::BURNTSIENNA: return "Burnt Sienna";
	case pluginsdk::Itempaint::FORESTGREEN: return "Forest Green";
	case pluginsdk::Itempaint::PURPLE: return "Purple";
	case pluginsdk::Itempaint::PINK: return "Pink";
	case pluginsdk::Itempaint::ORANGE: return "Orange";
	case pluginsdk::Itempaint::GREY: return "Grey";
	case pluginsdk::Itempaint::TITANIUMWHITE: return "Titanium White";
	case pluginsdk::Itempaint::SAFFRON: return "Saffron";
	case pluginsdk::Itempaint::GOLD: return "Gold";
	case pluginsdk::Itempaint::ROSEGOLD: return "Rose Gold";
	case pluginsdk::Itempaint::WHITEGOLD: return "White Gold";
	case pluginsdk::Itempaint::ONYX: return "Onyx";
	case pluginsdk::Itempaint::PLATINUM: return "Platinum";
	default: return "Unknown";
	}
}


std::string EquipslotToString(pluginsdk::Equipslot slot) {
	switch (slot) {
	case pluginsdk::Equipslot::BODY: return "car body";
	case pluginsdk::Equipslot::DECAL: return "decal";
	case pluginsdk::Equipslot::WHEELS: return "wheels";
	case pluginsdk::Equipslot::ROCKETBOOST: return "rocket boost";
	case pluginsdk::Equipslot::ANTENNA: return "antenna";
	case pluginsdk::Equipslot::TOPPER: return "topper";
	case pluginsdk::Equipslot::BUMPER: return "bumper";
	case pluginsdk::Equipslot::PAINTFINISH: return "paint finish";
	case pluginsdk::Equipslot::ENGINEAUDIO: return "engine audio";
	case pluginsdk::Equipslot::TRAIL: return "trail";
	case pluginsdk::Equipslot::GOALEXPLOSION: return "goal explosion";
	case pluginsdk::Equipslot::PLAYERBANNER: return "player banner";
	case pluginsdk::Equipslot::GOALSTINGER: return "goal stinger";
	case pluginsdk::Equipslot::PLAYERAVATAR: return "player avatar";
	case pluginsdk::Equipslot::AVATARBORDER: return "avatar border";
	case pluginsdk::Equipslot::PLAYERTITLE: return "player title";
	case pluginsdk::Equipslot::ESPORTSTEAM: return "esports team";
	default: return "other";
	}
}

void OrganizeMyBakkesModGarage::LogCarInfo(const std::unordered_map<std::string, CarInfo>& car_info_map)
{

	for (const auto& [key, value] : car_info_map)
	{
		LOG("Car Info - Player: {} | Team: {}", value.player_name, value.team);
		carInfoBM[value.player_name] = ConvertToBMLoadout(value.loadout, value);
		carInfoBMString[value.player_name] = save(carInfoBM[value.player_name]);
		//load("test");
		print_loadout(carInfoBM[value.player_name]);
	}

	for (auto [key, value] : paintFinishMap) {
		LOG("Player Id :{}", key);
		LOG("Primary Paint Finish: {}", value.first);
		LOG("Secondary Paint Finish: {}", value.second);
	
	}
	
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





