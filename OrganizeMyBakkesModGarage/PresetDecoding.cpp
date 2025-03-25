#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r\f\v");
	if (first == std::string::npos) {
		return ""; // String is all whitespace
	}
	size_t last = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(first, (last - first + 1));
}


void OrganizeMyBakkesModGarage::decodePresetId(const std::string& presetId) {
	
	BMLoadout currentLoadout = load(trim(presetId));
	
	validateCorrectCarBody(currentLoadout);
	
	

}

void OrganizeMyBakkesModGarage::check() {

	//gameWrapper->Execute([this, command](GameWrapper* gw) {
	//	cvarManager->executeCommand(command, false);
	//	});

	gameWrapper->Execute([this](GameWrapper* gw) {
		
		LoadoutSaveWrapper lsw = gameWrapper->GetUserLoadoutSave();

		LoadoutSetWrapper equippedLoadout = lsw.GetEquippedLoadout();

		LoadoutSetData loadoutData = equippedLoadout.GetLoadoutData();

		LoadoutWrapper blueLoadout = loadoutData.blue;
		ArrayWrapper<int> blueLoadoutArray = blueLoadout.GetLoadout();
		for (int i = 0; i < blueLoadoutArray.Count(); i++) {
			LOG("Blue loadout item: {}", std::to_string(blueLoadoutArray.Get(i)));
		}
		
		});
	/*LoadoutSaveWrapper lsw = gameWrapper->GetUserLoadoutSave();

	LoadoutSetWrapper equippedLoadout = lsw.GetEquippedLoadout();

	LoadoutSetData loadoutData = equippedLoadout.GetLoadoutData();

	LoadoutWrapper blueLoadout = loadoutData.blue;
	ArrayWrapper<int> blueLoadoutArray = blueLoadout.GetLoadout();
	for (int i = 0; i < blueLoadoutArray.Count(); i++) {
		LOG("Blue loadout item: {}", std::to_string(blueLoadoutArray.Get(i)));
	}*/
}
void OrganizeMyBakkesModGarage::validateCorrectCarBody(BMLoadout loadout) {

	Item requestedCarBody = loadout.body.blue_loadout[0];
	LOG("Requested car body product id: {}" ,std::to_string(requestedCarBody.product_id));

	gameWrapper->Execute([this, requestedCarBody](GameWrapper* gw) {

		LoadoutSaveWrapper lsw = gameWrapper->GetUserLoadoutSave();

		LoadoutSetWrapper equippedLoadout = lsw.GetEquippedLoadout();

		LoadoutSetData loadoutData = equippedLoadout.GetLoadoutData();

		LoadoutWrapper blueLoadout = loadoutData.blue;
		ArrayWrapper<int> blueLoadoutArray = blueLoadout.GetLoadout();
		int currentCarBody = blueLoadoutArray.Get(0);
	/*	for (int i = 0; i < blueLoadoutArray.Count(); i++) {
			LOG("Blue loadout item: {}", std::to_string(blueLoadoutArray.Get(i)));
		}*/
		if (currentCarBody != requestedCarBody.product_id) {
			LOG("Car body mismatch. Requested: {}, Current: {}", std::to_string(requestedCarBody.product_id), std::to_string(currentCarBody));
			ArrayWrapper<LoadoutSetWrapper> presets = lsw.GetPresets();
			int currentIndex = lsw.GetIndex(equippedLoadout);
			for (int i = 0; i < presets.Count(); i++) {
				LoadoutSetWrapper preset = presets.Get(i);
				int presetCarBody = preset.GetLoadoutData().blue.GetLoadout().Get(0);
				if (presetCarBody == requestedCarBody.product_id) {
					LOG("Found preset with requested car body at index: {}", std::to_string(i));
					lsw.EquipPreset(preset);
					break;
				}
			}
		}
		else {
			LOG("Car body match. Requested: {}, Current: {}", std::to_string(requestedCarBody.product_id), std::to_string(currentCarBody));
		}
		});

	

}


