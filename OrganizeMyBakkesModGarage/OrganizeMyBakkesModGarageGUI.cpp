#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"

std::vector<std::pair<std::string, std::vector<Preset>>> groups;
std::string newGroupName;

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

//void OrganizeMyBakkesModGarage::RenderWindow()
//{
//	LOG("Rendering window in RenderWindow function");
//	ImGui::Text("Hello World in windows");
//
//	static std::vector<Preset> presets = this->readPresets("bakkesmod/data/presets.data");
//
//	ImGui::Begin("Preset Viewer");
//
//	if (presets.empty()) {
//		ImGui::Text("No presets found.");
//	}
//	else {
//		for (const auto& preset : presets) {
//			ImGui::Text("Name: %s", preset.name.c_str());
//			ImGui::Text("Encoded ID: %s", preset.id.c_str());
//			ImGui::Separator(); // Add a line for separation
//		}
//	}
//
//	ImGui::End();
//}


void OrganizeMyBakkesModGarage::RenderWindow() {
    static std::vector<Preset> presets = this->readPresets("data\\presets.data");
    ImGui::Begin("Preset Viewer");
    // Display Groups
    ImGui::Text("Groups:");
    for (size_t i = 0; i < groups.size(); ++i) {
        ImGui::Text("Group %s (%d presets)", groups[i].first.c_str(), (int)groups[i].second.size());

        if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
            groups.erase(groups.begin() + i);
        }
    }

    ImGui::Separator();

    // Create Group
    ImGui::InputText("Group Name", &newGroupName);
    if (ImGui::Button("Create Group") && !newGroupName.empty()) {
        groups.emplace_back(newGroupName, std::vector<Preset>{});
        newGroupName.clear();
    }

    ImGui::Separator();

    // Display Presets
    if (presets.empty()) {
        ImGui::Text("No presets found.");
    }
    else {
        ImGui::BeginChild("##PresetList", ImVec2(0, 400), true, ImGuiWindowFlags_HorizontalScrollbar);

        for (size_t i = 0; i < presets.size(); ++i) {
            const auto& preset = presets[i];
            ImGui::Text("Name: %s", preset.name.c_str());
            ImGui::Text("Encoded ID: %s", preset.id.c_str());

            for (size_t j = 0; j < groups.size(); ++j) {
                if (ImGui::Button(("Add to " + groups[j].first + "##" + std::to_string(i) + "_" + std::to_string(j)).c_str())) {
                    groups[j].second.push_back(preset);
                }
            }
            ImGui::Separator();
        }

        ImGui::EndChild();
    }

    ImGui::End();
}