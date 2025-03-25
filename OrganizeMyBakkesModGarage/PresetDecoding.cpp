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
	
	print_loadout(currentLoadout);

}


void OrganizeMyBakkesModGarage::validateCorrectCarBody(BMLoadout loadout) {

	Item requestedCarBody = loadout.body.blue_loadout[0];
	LOG("Requested car body product id: {}" ,std::to_string(requestedCarBody.product_id));

	BMLoadout currentLoadout = load(trim(currentBakkesModPreset));

	Item currentCarBody = currentLoadout.body.blue_loadout[0];

	LOG("Current car body product id: {}", std::to_string(currentCarBody.product_id));
	if (requestedCarBody.product_id != currentCarBody.product_id) {
		LOG("Requested car body does not match what you currently have on");
	}

}


