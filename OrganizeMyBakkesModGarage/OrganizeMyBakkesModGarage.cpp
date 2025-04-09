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
	
}

void OrganizeMyBakkesModGarage::OnReplayOpen() {
	car_info_map.clear();
	carInfoBM.clear();

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
	//TAGame.Car_TA.UpdateTeamLoadout
	gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.HandleLoadoutSelected",
		[this](CarWrapper&& car, ...) {
			if (auto pri = car.GetPRI()) {
				
				
				/*LOG("Paint Finish: {}",);*/
				std::string id = pri.GetUniqueIdWrapper().GetIdString();

				// Only insert if not already present
				if (!car_info_map.contains(id)) {
					auto loadout_maybe = LoadoutUtilities::GetLoadoutFromPri(pri, pri.GetTeamNum2());

					if (!loadout_maybe) return;

					CarInfo info{
						.loadout = *loadout_maybe,
						.team = pri.GetTeamNum2(),
						.player_name = pri.GetPlayerName().ToString()
					};

					car_info_map[id] = std::move(info);
				}
			}
		});
}

void OrganizeMyBakkesModGarage::OnReplayClose() 
{

	//LOG("Closing replay");
	//not getting called
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.UpdateTeamLoadout");
	/*car_info_map.clear();
	carInfoBM.clear();*/
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
	}

	for (const auto& [key, value] : carInfoBM)
	{
		print_loadout(value);
	}
	// Iterate over each car's info in the map
	for (const auto& [car_key, car_info] : car_info_map)
	{

		LOG("Car Info - Player: {} | Team: {}", car_info.player_name, car_info.team);

		// Iterate over each item in the car's loadout
		for (const auto& [slot, item] : car_info.loadout.items)
		{
			LOG("Slot {} => Product ID: {}", EquipslotToString(slot), item.product_id);

			// Iterate over the item's attributes
			for (const auto& attr : item.attributes)
			{
				if (attr.type == pluginsdk::ItemAttribute::AttributeType::PAINT)
				{
					LOG("        Paint: {} (ID: {})", GetPaintName(static_cast<pluginsdk::Itempaint>(attr.value)), attr.value);
				}
				
				
			}
		}

		LOG("==========================================");
	}
}

void OrganizeMyBakkesModGarage::OnPriLoadoutSet(PriWrapper& pri) {
	if (!pri)
	{
		return;
	}

	auto car = pri.GetCar();
	//if no car they're spectating. Don't care about those
	if (!car)
	{
		return;
	}
	

	auto loadout_maybe = LoadoutUtilities::GetLoadoutFromPri(pri, pri.GetTeamNum2());
	auto colors = LoadoutUtilities::GetPaintFinishColors(car);
	if (!loadout_maybe)
	{
		return;
	}
	auto& [items, paint_finish] = *loadout_maybe;
	for (const auto& [slot, item] : items) {
		std::string slotName = EquipslotToString(slot);
		if (slotName != "other") {
			LOG("{}: Product ID {}", slotName, item.product_id);
		}
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





