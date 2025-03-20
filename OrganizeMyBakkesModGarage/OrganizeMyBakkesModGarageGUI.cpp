#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"
//
//std::vector<std::pair<std::string, std::vector<Preset>>> groups;
//std::string newGroupName;

//void OrganizeMyBakkesModGarage::RenderSettings()
//{
//
//	static char input_buffer[16] = "F4";
//	ImGui::Text("Current binding: %s", bind_key);
//	ImGui::InputText("##bind_key", input_buffer, IM_ARRAYSIZE(input_buffer));
//	ImGui::SameLine();
//	if (ImGui::Button("Set bind")) {
//		cvarManager->removeBind(bind_key);
//		cvarManager->setBind(input_buffer, "open_organizemybakkesmodgarage_ui");
//		bind_key = input_buffer;
//		LOG("Key bind changed to: {}", input_buffer);
//	}
//
//
//	ImGui::Text("Hello World in settings");
//}

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


//void OrganizeMyBakkesModGarage::RenderWindow() {
//    static std::vector<Preset> presets = this->readPresets("data\\presets.data");
//    ImGui::Begin("Preset Viewer");
//
//    // Display Groups
//    ImGui::Text("Groups:");
//    for (size_t i = 0; i < groups.size(); ++i) {
//        if (ImGui::CollapsingHeader((groups[i].first + " (" + std::to_string(groups[i].second.size()) + " presets)").c_str())) {
//
//            // Add a scrollable section for presets in this group
//            ImGui::BeginChild(("GroupScroll##" + std::to_string(i)).c_str(), ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
//
//            for (const auto& preset : groups[i].second) {
//                if (ImGui::Button(preset.name.c_str())) {
//                    //std::string command = "cl_itemmod_code " + preset.id;
//                    std::string command = "plugin reload organizemybakkesmodgarage";
//                    LOG("id is: -{}-", preset.id);
//                    //if (command != "cl_itemmod_code RA4oDPQXHALIR0D8BSwGCADHeRB4WSHmCP0BRPwDBvoLDgHkIyD+AgYDBIDjPAg8rBD/cnL+AQAA") {//eekso
//                    //    LOG("command is processing wrong");
//                    //}
//                    //else {
//                    //    cvarManager->executeCommand(command);
//                    //}
//                    OnGameThread([this] {
//                        cvarManager->executeCommand("plugin reload organizemybakkesmodgarage");
//                        });//std::format("openmenu {}", GetMenuName())
//                    /*cvarManager->executeCommand(command);*/
//                    LOG("Executing command: {}", command);
//                    //cvarManager->executeCommand("cl_itemmod_code " + preset.id);
//                    //RA4oDPQXHALIR0D8BSwGCADHeRB4WSHmCP0BRPwDBvoLDgHkIyD+AgYDBIDjPAg8rBD/cnL+AQAA
//                }
//                //ImGui::Text("Name: %s", preset.name.c_str());
//                ImGui::Text("Encoded ID: %s", preset.id.c_str());
//                ImGui::Separator();
//            }
//
//            ImGui::EndChild();
//
//            // Delete Group
//            if (ImGui::Button(("Delete Group##" + std::to_string(i)).c_str())) {
//                groups.erase(groups.begin() + i);
//            }
//        }
//    }
//
//    ImGui::Separator();
//
//    // Create Group
//    ImGui::InputText("Group Name", &newGroupName);
//    if (ImGui::Button("Create Group") && !newGroupName.empty()) {
//        groups.emplace_back(newGroupName, std::vector<Preset>{});
//        newGroupName.clear();
//    }
//
//    ImGui::Separator();
//
//    // Display Presets
//    if (presets.empty()) {
//        ImGui::Text("No presets found.");
//    }
//    else {
//        ImGui::BeginChild("##PresetList", ImVec2(0, 400), true, ImGuiWindowFlags_HorizontalScrollbar);
//
//        for (size_t i = 0; i < presets.size(); ++i) {
//            const auto& preset = presets[i];
//            ImGui::Text("Name: %s", preset.name.c_str());
//            ImGui::Text("Encoded ID: %s", preset.id.c_str());
//
//            // Buttons to add to groups
//            for (size_t j = 0; j < groups.size(); ++j) {
//                if (ImGui::Button(("Add to " + groups[j].first + "##" + std::to_string(i) + "_" + std::to_string(j)).c_str())) {
//                    groups[j].second.push_back(preset);
//                }
//            }
//            ImGui::Separator();
//        }
//
//        ImGui::EndChild();
//    }
//
//    ImGui::End();
//}






//void OrganizeMyBakkesModGarage::RenderWindow() {
//    _globalCvarManager = cvarManager;
//    static std::vector<Preset> presets = this->readPresets("data\\presets.data");
//    ImGui::Begin("Preset Viewer");
//
//    ImGui::Text("Groups:");
//    for (size_t i = 0; i < groups.size(); ++i) {
//        if (ImGui::CollapsingHeader((groups[i].first + " (" + std::to_string(groups[i].second.size()) + " presets)").c_str())) {
//
//            ImGui::BeginChild(("GroupScroll##" + std::to_string(i)).c_str(), ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
//
//            for (const auto& preset : groups[i].second) {
//                if (ImGui::Button(preset.name.c_str())) {
//                    std::string command = "cl_itemmod_code " + preset.id;
//                    //even just hardcoding the preset.id
//                    std::string command3 = "cl_itemmod_code RA4oDPQXHALIR0D8BSwGCADHeRB4WSHmCP0BRPwDBvoLDgHkIyD+AgYDBIDjPAg8rBD/cnL+AQAA";
//                    std::string command2 = "sv_training_enabled 1";
//                    LOG("id is: -{}-", preset.id);
//
//
//                    cvarManager->executeCommand(command2);
//
//
//                    cvarManager->executeCommand(command3.c_str());
//                    LOG("command2: {}", command2);
//                    LOG("command: {}", command);
//                    //cvarManager->executeCommand(command);
//
//                }
//                ImGui::Text("Encoded ID: %s", preset.id.c_str());
//                ImGui::Separator();
//            }
//
//            ImGui::EndChild();
//
//            if (ImGui::Button(("Delete Group##" + std::to_string(i)).c_str())) {
//                groups.erase(groups.begin() + i);
//            }
//        }
//    }
//
//    ImGui::Separator();
//
//    ImGui::InputText("Group Name", &newGroupName);
//    if (ImGui::Button("Create Group") && !newGroupName.empty()) {
//        groups.emplace_back(newGroupName, std::vector<Preset>{});
//        newGroupName.clear();
//    }
//
//    ImGui::Separator();
//
//    if (presets.empty()) {
//        ImGui::Text("No presets found.");
//    }
//    else {
//        ImGui::BeginChild("##PresetList", ImVec2(0, 400), true, ImGuiWindowFlags_HorizontalScrollbar);
//
//        for (size_t i = 0; i < presets.size(); ++i) {
//            const auto& preset = presets[i];
//            ImGui::Text("Name: %s", preset.name.c_str());
//            ImGui::Text("Encoded ID: %s", preset.id.c_str());
//
//            for (size_t j = 0; j < groups.size(); ++j) {
//                if (ImGui::Button(("Add to " + groups[j].first + "##" + std::to_string(i) + "_" + std::to_string(j)).c_str())) {
//                    groups[j].second.push_back(preset);
//                }
//            }
//            ImGui::Separator();
//        }
//
//        ImGui::EndChild();
//    }
//
//    ImGui::End();
//}