#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"


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
#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class Preset {
	public:
	std::string name;
	std::string id;
};


class GuiFeatureBase;

class OrganizeMyBakkesModGarage: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	std::vector<std::pair<std::string, std::vector<Preset>>> groups;
	std::unordered_map<std::string,Preset>choices;
	std::vector<int>choicesBool;
	
	std::string newGroupName;
	std::string queriedGroupName;
	std::vector<std::string> sortOptions = { "Name", "Date", "Size" };
	int currentSortOption = 0;
	std::vector<Preset> presets;
	bool showAddPresetWindow = false;
	bool multiSelect = false;
	int currentGroupIndex = -1;
	std::string searchQuery;


	std::filesystem::path groupFilePath;

	//std::shared_ptr<bool> enabled;
	std::string bind_key = "F4";
	bool defaultHooked = false;
	//Boilerplate
	void onLoad() override;
	std::vector<Preset> readPresets(const std::string& file_path);

	void onUnload() override; // Uncomment and implement if you need a unload method
	void SaveGroupsToFile(const std::filesystem::path& filePath);
	void LoadGroupsFromFile(const std::filesystem::path& filePath);

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



public:
	
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	void RenderWindow() override; // Uncomment if you want to render your own plugin window
	void OnClose() override;
};
