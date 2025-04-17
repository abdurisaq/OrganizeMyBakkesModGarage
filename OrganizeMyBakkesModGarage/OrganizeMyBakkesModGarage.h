#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/utilities/LoadoutUtilities.h"
#include "bakkesmod/wrappers/items/assets/ProductAssetWrapper.h"
#include "bakkesmod/wrappers/GameObject/MeshComponents/CarMeshComponentBaseWrapper.h"
#include <Windows.h>
#include <filesystem>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <time.h>
#include "bmloadout.h"
#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


template <typename T, typename std::enable_if<std::is_base_of<ObjectWrapper, T>::value>::type*>
void GameWrapper::HookEventWithCallerPost(std::string eventtName, std::function<void(T caller, void* params, std::string eventName)> callback)
{
	auto wrapped_callback = [callback](ActorWrapper caller, void* params, std::string eventName) {
		callback(T(caller.memory_address), params, eventName);

		};
	HookEventWithCaller<ActorWrapper>(eventtName, wrapped_callback);

}

template <typename T, typename std::enable_if<std::is_base_of<ObjectWrapper, T>::value>::type*>
void GameWrapper::HookEventWithCaller(std::string eventName,
	std::function<void(T caller, void* params, std::string eventName)> callback)
{
	auto wrapped_callback = [callback](ActorWrapper caller, void* params, std::string eventName)
		{
			callback(T(caller.memory_address), params, eventName);
		};
	HookEventWithCaller<ActorWrapper>(eventName, wrapped_callback);
}


class Preset {
	public:
	std::string name;
	std::string id;
};

class PresetGroup {
	public:
	std::vector<Preset> presets;
	time_t timeModified;
	PresetGroup() {
		timeModified = time(0);
	}
	PresetGroup(std::vector<Preset> presets) {
		this->presets = presets;
		timeModified = time(0);
	}
	PresetGroup(std::vector<Preset> presets, time_t timestamp) {
		this->presets = presets;
		timeModified = timestamp;
	}
};

struct CarInfo {
	pluginsdk::Loadout loadout;
	int team;
	std::string player_name;
	std::string playerId;
	// add whatever else you want (e.g., score, boost usage, etc.)
};
struct PlatformId {
	std::string type;  
	std::string id;    
};

class GuiFeatureBase;

class OrganizeMyBakkesModGarage: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::vector<std::pair<std::string, std::vector<Preset>>> groups;
	std::vector<std::pair<std::string, PresetGroup>> groups;
	std::unordered_map<std::string,Preset>choices;
	std::vector<bool>choicesBool;//stupid bool vector optimization means i cant access the bool's address

	bool * boolArray = nullptr;
	
	int tempCount = 0;
	long long currentPlayerId = 0;
	std::string playerIdString;
	std::vector<std::string> previousEpicIdsUsed;

	std::unordered_map<std::string,std::pair<int,int>> paintFinishMap;
	std::vector<std::pair<int, int>> unclaimedPaintFinishes;
	std::vector<std::string> unclaimedEpicIds;
	std::pair<int,int> currentPaintFinish;


	std::string newGroupName;
	std::string queriedGroupName;
	std::vector<std::string> sortOptions = { "Name", "Date", "Size" };
	int currentSortOption = 0;
	bool sortDirection = true;
	int pastSortOption = 0;
	std::vector<Preset> presets;
	bool showAddPresetWindow = false;
	bool showEditGroupWindow = false;
	bool multiSelect = false;
	int currentGroupIndex = -1;
	std::string currentGroupNameCvar;
	std::string searchQuery;

	std::pair<std::string, PresetGroup> currentGroup;
	std::string currentBakkesModPreset;
	bool shuffleInFreeplay = false;
	bool shuffleInOnlineGame = false;
	bool swapCarBodyCapability = false;

	std::filesystem::path groupFilePath;

	//std::shared_ptr<bool> enabled;
	std::string pastBinding;
	std::string bind_key;
	void readCurrentBinding();
	bool defaultHooked = false;
	//Boilerplate
	void onLoad() override;
	std::vector<Preset> readPresets(const std::string& file_path);
	void readCurrentBakkesModPreset(const std::string& file_path);

	PlatformId ExtractPlatformId(const std::string& platformString);

	void onUnload() override; // Uncomment and implement if you need a unload method
	void SaveGroupsToFile(const std::filesystem::path& filePath);
	void LoadGroupsFromFile(const std::filesystem::path& filePath);
	void updateCurrentGroup();
	bool reloadedCurrentGroup = false;

	template <typename T, typename... Args>
	[[nodiscard]] std::shared_ptr<T> CreateModule(Args&&... args)
	{
		std::shared_ptr<T> created = std::make_shared<T>(std::forward<Args>(args)...);
		if (auto gui_feature = std::dynamic_pointer_cast<GuiFeatureBase>(created))
		{
			gui_features_.emplace_back(gui_feature);
		}
		return created;
	}

	std::vector<std::shared_ptr<GuiFeatureBase>> gui_features_;
	std::string toLowerCase(const std::string& str);
	void addPresetWindow();
	void editGroupWindow();
	void decodePresetId(const std::string& presetId);
	void validateCorrectCarBody(BMLoadout loadout);
	void reSortGroups();
	
	BMLoadout ConvertToBMLoadout(const pluginsdk::Loadout& loadout, const CarInfo& car_info);
	
	//std::vector< std::optional<pluginsdk::Loadout>> allLoadouts;
	std::unordered_map<std::string, CarInfo> car_info_map;
	std::unordered_map<std::string, BMLoadout> carInfoBM;
	std::unordered_map<std::string, std::string> carInfoBMString;
	bool inReplay = false;

	void checkAndConvert();
	bool conversionTriggered = false;

public:
	
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	void RenderWindow() override; // Uncomment if you want to render your own plugin window
	void OnClose() override;
	void OnOpen() override;

	void OnReplayOpen();
	
	void OnReplayClose() ;
	void LogCarInfo(const std::unordered_map<std::string, CarInfo>& car_info_map);
	
};
