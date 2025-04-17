#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


void OrganizeMyBakkesModGarage::RenderSettings()
{

	std::string binding = pastBinding;

	if (!reloadedCurrentGroup) {
		updateCurrentGroup();
		reloadedCurrentGroup = true;
	}
	ImGui::Text("Current binding: %s", binding.c_str());
	ImGui::InputText("##bind_key", &bind_key);
	ImGui::SameLine();
	if (ImGui::Button("Set bind")) {
		
		cvarManager->removeBind(binding);
		cvarManager->setBind(bind_key, "open_organizemybakkesmodgarage_ui");
		pastBinding = bind_key;
		
	}
	//swapCarBodyCapability = cvarManager->getCvar("swap_enabled").getBoolValue();
	ImGui::Checkbox("Swap car body capability", &swapCarBodyCapability);
	if (swapCarBodyCapability) {
		cvarManager->getCvar("swap_enabled").setValue(1);
	}
	else {
				cvarManager->getCvar("swap_enabled").setValue(0);
	}
	ImGui::Checkbox("Shuffle in Freeplay", &shuffleInFreeplay);
	if (shuffleInFreeplay) {
		cvarManager->getCvar("freeplay_shuffle_enabled").setValue(1);
	}
	else {
		cvarManager->getCvar("freeplay_shuffle_enabled").setValue(0);
	}
	ImGui::Checkbox("Shuffle in Online Game", &shuffleInOnlineGame);
	if (shuffleInOnlineGame) {
		cvarManager->getCvar("online_shuffle_enabled").setValue(1);
	}
	else {
		cvarManager->getCvar("online_shuffle_enabled").setValue(0);
	}
	ImGui::Text("Main preset group selected");
	if (currentGroup.first.empty()) {
		ImGui::Text("No group selected");
	}
	else {
		if (ImGui::TreeNodeEx(currentGroup.first.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_CollapsingHeader)) {
			for (const auto preset : currentGroup.second.presets) {
				ImGui::Text(preset.name.c_str());
			}
		}
	}
	if (inReplay) {
		ImGui::Text("Presets in replay");

		for (auto& [key, value] : carInfoBMString) {
			ImGui::Text("name : %s", key.c_str());

			ImGui::SameLine();
			std::string buttonLabel = "Copy##" + key; // Unique label per key

			if (ImGui::Button(buttonLabel.c_str())) {
				ImGui::SetClipboardText(value.c_str());
			}

			ImGui::SameLine();
			ImGui::Text("id : %s", value.c_str());
		}
	}


}





void OrganizeMyBakkesModGarage::RenderWindow() {
	

	_globalCvarManager = cvarManager;

	if (!reloadedCurrentGroup) {
		updateCurrentGroup();
		reloadedCurrentGroup = true;
	}
	ImGui::InputTextWithHint("##GroupName", "Search Groups", &queriedGroupName);
	/*if (ImGui::Button("Create Group") && !newGroupName.empty()) {
		groups.emplace_back(newGroupName, std::vector<Preset>{});
		newGroupName.clear();
	}*/

	ImGui::SameLine();
	if (ImGui::Button("Sort")) {
		ImGui::OpenPopup("SortOptions");
	}
	if (ImGui::BeginPopup("SortOptions")) {
		for (int i = 0; i < sortOptions.size(); ++i) {
			std::string option = sortOptions[i];
			if (i == currentSortOption) {
				if (sortDirection) {
					option += " ^";
				}
				else {
					option += " v";
				}
			}
			if (ImGui::Selectable(option.c_str())) {//sortOptions[i].c_str()
				currentSortOption = i;
				if (currentSortOption == pastSortOption) {
					sortDirection = !sortDirection;
				}
				reSortGroups();
				/*bool direction = sortDirection;
				switch(currentSortOption) {
				case 0:
					std::sort(groups.begin(), groups.end(), [direction](const std::pair<std::string, PresetGroup>& a, const std::pair<std::string, PresetGroup>& b) {
						if (direction) {
							return a.first < b.first;
						}
						return a.first > b.first;
						});
					break;
				case 1:
					std::sort(groups.begin(), groups.end(), [direction](const std::pair<std::string, PresetGroup>& a, const std::pair<std::string, PresetGroup>& b) {
						if (direction) {
							return a.second.timeModified < b.second.timeModified;
						}
						return a.second.timeModified > b.second.timeModified;
						});

					break;

				case 2:
					std::sort(groups.begin(), groups.end(), [direction](const std::pair<std::string, PresetGroup>& a, const std::pair<std::string, PresetGroup>& b) {
						if (direction) {
							return a.second.presets.size() > b.second.presets.size();
						}
						return a.second.presets.size() < b.second.presets.size();
						});
					break;

				}
				pastSortOption = currentSortOption;*/
			}
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Create Group")) {
		ImGui::OpenPopup("CreateGroup");
	}
	if (ImGui::BeginPopup("CreateGroup")) {
		ImGui::InputTextWithHint("##GroupName", "Enter new group name", &newGroupName);
		if (ImGui::Button("Create")) {
			groups.emplace_back(newGroupName, PresetGroup());
			newGroupName.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();

	ImVec2 windowSize = ImGui::GetWindowSize();

	float topSectionHeight = ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing() + ImGui::GetFrameHeightWithSpacing();

	float remainingHeight = windowSize.y - topSectionHeight;

	queriedGroupName = toLowerCase(queriedGroupName);

	if (!groups.empty()) {
		ImGui::BeginChild("Groups", ImVec2(0, remainingHeight), true);
		for (int i = 0; i < groups.size(); ++i) {
			//lowerSearchQuery.empty() || toLowerCase(preset.name).find(lowerSearchQuery) != std::string::npos
			

			if (queriedGroupName.empty() || toLowerCase(groups[i].first).find(queriedGroupName) != std::string::npos) {
				ImGui::PushID(i);
				ImVec2 availSpace = ImGui::GetContentRegionAvail();
				

				//ImGui::CollapsingHeader((groups[i].first + " (" + std::to_string(groups[i].second.size()) + " presets)").c_str())

				
				std::string headerLabel = groups[i].first + " (" + std::to_string(groups[i].second.presets.size()) + " presets)";
				
				ImGui::SetNextItemWidth(200);
				//ImGui::PushStyleVar(ImGuiStyleVar_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
				ImVec2 cursorPosBeforeButton = ImGui::GetCursorPos();
				//availSpace.y * 0.05
				
				if (ImGui::TreeNodeEx(headerLabel.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap| ImGuiTreeNodeFlags_CollapsingHeader)) {//ImGuiTreeNodeFlags_SpanAvailWidth  //ImGuiTreeNodeFlags_AllowItemOverlap |ImGuiTreeNodeFlags_NoTreePushOnOpen
					
					ImVec2 groupWindowSize = ImGui::GetWindowSize();

					ImVec2 childSize = ImVec2(groupWindowSize.x, groupWindowSize.y * 0.4f);


					ImGui::BeginChild("PresetListInGroup", ImVec2(groupWindowSize.x, childSize.y), true);
					for (size_t j = 0; j < groups[i].second.presets.size(); ++j) {
						const auto& preset = groups[i].second.presets[j];
						/*ImGui::Text(preset.name.c_str());*/

						//ImGui::SameLine();
						if (ImGui::Button(preset.name.c_str())) {//ImGui::Button(("Apply##" + std::to_string(i) + std::to_string(j)).c_str())
							std::string command = "sleep 1;cl_itemmod_code " + preset.id;
							/*LOG("Executing preset: {}", preset.id.c_str());
							
							LOG("FULL COMMAND: {}", command.c_str());*/
							if (swapCarBodyCapability) {
								decodePresetId(preset.id);
							}
							gameWrapper->Execute([this, command](GameWrapper* gw) {
								cvarManager->executeCommand(command, false);
								});
							currentBakkesModPreset = preset.id;
						}
						/*ImGui::Text(preset.id.c_str());*/

						/*ImGui::SameLine();
						if (ImGui::Button(("Delete##" + std::to_string(i) + std::to_string(j)).c_str())) {
							groups[i].second.erase(groups[i].second.begin() + j);
						}*/
					}
					ImGui::EndChild();
					
				}
				ImGui::PopStyleColor();
				
				ImVec2 cursorPosAfterButton = ImGui::GetCursorPos();
				ImGui::SameLine();//availSpace 
				int buttonSizeX = availSpace.x * 0.15f;
				if(buttonSizeX < 80) buttonSizeX = 80;
				ImVec2 buttonSize = ImVec2(buttonSizeX, 20);
				ImGui::SetCursorPosX(availSpace.x - 2*buttonSize.x - 10);//cursorPosBeforeButton.x+400
				ImGui::SetCursorPosY(cursorPosBeforeButton.y);
				if (ImGui::Button(("Add Preset##AddPreset" + std::to_string(i)).c_str(), ImVec2(buttonSize.x*0.90, buttonSize.y))) {
					currentGroupIndex = i;
					choicesBool = std::vector<bool>(presets.size(), false);
					searchQuery.clear();
					choices.clear();
					showAddPresetWindow = true;
				}
				ImGui::SameLine();
				std::string popupId = "Options##" + std::to_string(i);
				if (ImGui::Button(popupId.c_str(),buttonSize)) {
					ImGui::OpenPopup(popupId.c_str());
				}
				if (ImGui::BeginPopup(popupId.c_str())) {
					
					std::string buttonLabel = "Set as Main";
					if(currentGroup.first == groups[i].first) buttonLabel = "Main";
					if (ImGui::Button(buttonLabel.c_str())) {
						currentGroup = groups[i];

						cvarManager->getCvar("mainPresetGroupName").setValue(groups[i].first);
						ImGui::CloseCurrentPopup();
					}
					if (ImGui::Button("Edit")) {
						showEditGroupWindow = true;
						currentGroupIndex = i;
						
						choicesBool = std::vector<bool>(presets.size(), false);
						searchQuery.clear();
						choices.clear();
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::Button("Delete")) {
						groups.erase(groups.begin() + i);
						ImGui::CloseCurrentPopup();
						
					}

					ImGui::EndPopup();
				}
				if (currentGroup.first == groups[i].first) {
					ImGui::SameLine();
					ImGui::Text("*");
				}
				ImGui::SetCursorPos(cursorPosAfterButton);
				

				ImGui::PopID();

			}
			ImGui::Separator();
		}
		ImGui::EndChild();
	}


	if (showAddPresetWindow && currentGroupIndex != -1) {
		addPresetWindow();
	}

	if (showEditGroupWindow && currentGroupIndex != -1) {
		editGroupWindow();
	}
	
}
void OrganizeMyBakkesModGarage::editGroupWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 350));

	ImGui::Begin("Edit Group", &showEditGroupWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
	ImGui::InputTextWithHint("##InputWithHint", "Enter a new name for this group", &groups[currentGroupIndex].first);
	/*ImGui::InputText("Search##SearchPreset", &searchQuery);*/
	ImGui::InputTextWithHint("##SearchPreset", "Search Presets", &searchQuery);
	//ImGui::InputTextWithHint("##GroupName", "Search Groups", &queriedGroupName);
	std::string lowerSearchQuery = toLowerCase(searchQuery);

	ImGui::BeginChild("PresetList", ImVec2(380, 250), true);
	ImVec2 availSpaceAdd = ImGui::GetContentRegionAvail();
	if (lowerSearchQuery.empty()) {
		for (size_t j = 0; j < groups[currentGroupIndex].second.presets.size(); ++j) {
			const auto& preset = groups[currentGroupIndex].second.presets[j];
			
			if (choicesBool[j]) {
				bool tempBool = choicesBool[j];
				if (ImGui::Checkbox(preset.name.c_str(),&tempBool)) {
					choicesBool[j] = tempBool;
					if (choicesBool[j]) {
						//LOG("Added: %s from choices map in edit", preset.name.c_str());
						choices[preset.name] = preset;
					}
					else {
						choices.erase(preset.name);
						//LOG("Erased: %s from choices map in edit", preset.name.c_str());
					}
				}
			}
		}
		ImGui::Separator();
	}
	for (size_t j = 0; j < groups[currentGroupIndex].second.presets.size(); ++j) {
		

		const auto& preset = groups[currentGroupIndex].second.presets[j];
		if (lowerSearchQuery.empty() || toLowerCase(preset.name).find(lowerSearchQuery) != std::string::npos) {
			
			if (lowerSearchQuery.empty() &&choicesBool[j]) continue;
			bool tempBool = choicesBool[j];
			
			if (ImGui::Checkbox(preset.name.c_str(), &tempBool)) {
				choicesBool[j] = tempBool;
				
				if (choicesBool[j]) {
					//LOG("Added: %s from choices map in edit", preset.name.c_str());
					choices[preset.name] = preset;
				}
				else {
					choices.erase(preset.name);
					//LOG("Erased: %s from choices map in edit", preset.name.c_str());
				}
			}
			
				
			
		}
	
	}
	ImGui::EndChild();

	ImVec2 buttonSize = ImVec2(45, 20);
	ImGui::SetCursorPosX(availSpaceAdd.x - (2 * buttonSize.x) - 10);
	if (ImGui::Button("Delete ", buttonSize)) {
		for (auto& [key, value] : choices) {
			//groups[currentGroupIndex].second.push_back(value);
			groups[currentGroupIndex].second.presets.erase(std::remove_if(groups[currentGroupIndex].second.presets.begin(), groups[currentGroupIndex].second.presets.end(), [&value](const Preset& preset) {
				return preset.id == value.id;
				}), groups[currentGroupIndex].second.presets.end());
			choicesBool = std::vector<bool>(presets.size(), false);
		}
		groups[currentGroupIndex].second.timeModified = time(0);
		choices.clear();
		//showEditGroupWindow = false;
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(availSpaceAdd.x - (buttonSize.x));
	if (ImGui::Button("Close", buttonSize)) showEditGroupWindow = false;

	ImGui::End();

}

void OrganizeMyBakkesModGarage::addPresetWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 350));

	ImGui::Begin("Add Preset", &showAddPresetWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

	//ImGui::InputText("Search##SearchPreset", &searchQuery);
	ImGui::InputTextWithHint("##SearchPreset", "Search Presets", &searchQuery);
	std::string lowerSearchQuery = toLowerCase(searchQuery);

	ImGui::BeginChild("PresetList", ImVec2(380, 250), true);
	ImVec2 availSpaceAdd = ImGui::GetContentRegionAvail();
	if (lowerSearchQuery.empty()) {
		for (int i = 0; i < presets.size(); i++) {
			const auto& preset = presets[i];
			bool presetInGroup = false;

			for (const auto& groupPreset : groups[currentGroupIndex].second.presets) {
				if (groupPreset.id == preset.id) {
					presetInGroup = true;
					break;
				}
			}

			if (!presetInGroup && choicesBool[i]) {
				bool tempBool = choicesBool[i];
				if (ImGui::Checkbox(preset.name.c_str(), &tempBool)) {
					choicesBool[i] = tempBool;
					if (choicesBool[i]) {
						//LOG("Added: %s from choices map in edit", preset.name.c_str());
						choices[preset.name] = preset;
					}
					else {
						choices.erase(preset.name);
						//LOG("Erased: %s from choices map in edit", preset.name.c_str());
					}
				}
			}
		}
		ImGui::Separator();
	}
	for (int i = 0; i < presets.size(); i++) {
		//if (choicesBool[i]) continue;
		const auto& preset = presets[i];

		if (lowerSearchQuery.empty() || toLowerCase(preset.name).find(lowerSearchQuery) != std::string::npos) {
			
			if (lowerSearchQuery.empty() && choicesBool[i]) continue;
			bool presetInGroup = false;
			for (const auto& groupPreset : groups[currentGroupIndex].second.presets) {
				if (groupPreset.id == preset.id) {
					presetInGroup = true;
					break;
				}
			}

			if (!presetInGroup) {
				if (!multiSelect) {
					ImGuiIO& io = ImGui::GetIO();
					float originalFontSize = io.FontGlobalScale;
					io.FontGlobalScale = 1.5f;

					if (ImGui::Selectable(preset.name.c_str())) {
						groups[currentGroupIndex].second.presets.push_back(preset);
						groups[currentGroupIndex].second.timeModified = time(0);
						showAddPresetWindow = false;
						multiSelect = false;
					}
					io.FontGlobalScale = originalFontSize;
				}
				else {
					bool tempBool = choicesBool[i];
					if (ImGui::Checkbox(preset.name.c_str(), &tempBool)) {
						choicesBool[i] = tempBool; 
						if (choicesBool[i]) {
							choices[preset.name] = preset;
						}
						else {
							choices.erase(preset.name);
						}
					}
				}
			}
		}
	}

	ImGui::EndChild();

	// Toggle between multi and ingle
	if (!multiSelect) {
		if (ImGui::Button("Multi Select")) {
			multiSelect = !multiSelect;
		}
	}
	else {
		if (ImGui::Button("Single Select")) {
			multiSelect = !multiSelect;
			choicesBool = std::vector<bool>(presets.size(), false);
			choices.clear();
		}
	}
	ImGui::SameLine();
	//

	ImVec2 buttonSize = ImVec2(40, 20);
	ImGui::SetCursorPosX(availSpaceAdd.x - (2 * buttonSize.x) - 10);

	if (ImGui::Button("Close", buttonSize)) {
		showAddPresetWindow = false;
		multiSelect = false;
	}
	ImGui::SameLine();
	ImGui::SetCursorPosX(availSpaceAdd.x - (buttonSize.x));
	if (multiSelect && ImGui::Button("Add", buttonSize)) {
		for (auto& [key, value] : choices) {
			groups[currentGroupIndex].second.presets.push_back(value);
		}
		groups[currentGroupIndex].second.timeModified = time(0);
		showAddPresetWindow = false;
		multiSelect = false;
	}
	ImGui::End();

}