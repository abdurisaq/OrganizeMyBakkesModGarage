#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


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



}


std::string OrganizeMyBakkesModGarage::toLowerCase(const std::string& str) {
	std::string lowerStr = str;
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lowerStr;
}



void OrganizeMyBakkesModGarage::RenderWindow() {
	
	static std::vector<Preset> presets = this->readPresets("data\\presets.data");
	_globalCvarManager = cvarManager;


	ImGui::Text("Create New Group:");
	ImGui::InputText("##GroupName", &newGroupName);
	if (ImGui::Button("Create Group") && !newGroupName.empty()) {
		groups.emplace_back(newGroupName, std::vector<Preset>{});
		newGroupName.clear();
	}

	ImGui::Separator();

	ImVec2 windowSize = ImGui::GetWindowSize();

	float topSectionHeight = ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing() + ImGui::GetFrameHeightWithSpacing();

	float remainingHeight = windowSize.y - topSectionHeight;

	if (!groups.empty()) {
		ImGui::BeginChild("Groups", ImVec2(0, remainingHeight), true);
		for (int i = 0; i < groups.size(); ++i) {

			ImGui::PushID(i);
			ImVec2 availSpace = ImGui::GetContentRegionAvail();
			ImGui::SetNextItemWidth(availSpace.x * 0.5f);
			if (ImGui::Button(("+##AddPreset" + std::to_string(i)).c_str())) {
				currentGroupIndex = i;
				showAddPresetWindow = true;
			}
			ImGui::SameLine();
			if (ImGui::Button(("Delete Group##" + std::to_string(i)).c_str())) {
				groups.erase(groups.begin() + i);
				ImGui::PopID();
				continue; // Skip rendering the deleted group
			}
			ImGui::SameLine();

			if (ImGui::CollapsingHeader((groups[i].first + " (" + std::to_string(groups[i].second.size()) + " presets)").c_str())) {


				ImVec2 groupWindowSize = ImGui::GetWindowSize();

				ImVec2 childSize = ImVec2(groupWindowSize.x, groupWindowSize.y * 0.4f);


				ImGui::BeginChild("PresetListInGroup", ImVec2(groupWindowSize.x, childSize.y), true);
				for (size_t j = 0; j < groups[i].second.size(); ++j) {
					const auto& preset = groups[i].second[j];
					ImGui::Text(preset.name.c_str());

					ImGui::SameLine();
					if (ImGui::Button(("Apply##" + std::to_string(i) + std::to_string(j)).c_str())) {
						std::string command = "sleep 1;cl_itemmod_code " + preset.id;
						LOG("Executing preset: %s", preset.id.c_str());
					;

						gameWrapper->Execute([this, command](GameWrapper* gw) {
							cvarManager->executeCommand(command, false);
							});
					}

					ImGui::SameLine();
					if (ImGui::Button(("Delete##" + std::to_string(i) + std::to_string(j)).c_str())) {
						groups[i].second.erase(groups[i].second.begin() + j);
					}
				}
				ImGui::EndChild();
			}

			ImGui::PopID();


			ImGui::Separator();
		}
		ImGui::EndChild();
	}


	if (showAddPresetWindow && currentGroupIndex != -1) {
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGui::Begin("Add Preset", &showAddPresetWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		ImGui::InputText("Search##SearchPreset", &searchQuery);

		// convert ot lower case
		std::string lowerSearchQuery = toLowerCase(searchQuery);

		ImGui::BeginChild("PresetList", ImVec2(380, 250), true);
		for (const auto& preset : presets) {

			//
			if (lowerSearchQuery.empty() || toLowerCase(preset.name).find(lowerSearchQuery) != std::string::npos) {

				bool presetInGroup = false;
				for (const auto& groupPreset : groups[currentGroupIndex].second) {
					if (groupPreset.id == preset.id) {
						presetInGroup = true;
						break;
					}
				}


				if (!presetInGroup) {
					if (ImGui::Selectable(preset.name.c_str())) {
						groups[currentGroupIndex].second.push_back(preset);
						showAddPresetWindow = false;
					}
				}
			}
		}
		ImGui::EndChild();

		ImGui::End();
	}

	
}