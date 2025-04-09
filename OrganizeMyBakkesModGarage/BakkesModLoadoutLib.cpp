// BakkesModLoadoutLib.cpp : Defines the entry point for the console application.
//gotten from https://github.com/bakkesmodorg/BakkesModLoadoutLib

#include "pch.h"
#include <string>
#include "bmloadout.h"
#include <iostream>
#include "helper_classes.h"

#include <fstream>

/*
	Slots supported by BakkesMod
*/
enum EQUIPSLOT {
	SLOT_BODY = 0, //Body won't be applied when loading in BakkesMod, user must have it equipped
	SLOT_SKIN = 1,
	SLOT_WHEELS = 2,
	SLOT_BOOST = 3,
	SLOT_ANTENNA = 4,
	SLOT_HAT = 5,

	SLOT_PAINTFINISH = 7,
	SLOT_PAINTFINISH_SECONDARY = 12,

	SLOT_ENGINE_AUDIO = 13,
	SLOT_SUPERSONIC_TRAIL = 14,
	SLOT_GOALEXPLOSION = 15,
	SLOT_ANTHEM = 18
};

enum ITEMPAINT {
	PAINT_NONE = 0,
	PAINT_CRIMSON = 1,
	PAINT_LIME = 2,
	PAINT_BLACK = 3,
	PAINT_SKYBLUE = 4,
	PAINT_COBALT = 5,
	PAINT_BURNTSIENNA = 6,
	PAINT_FORESTGREEN = 7,
	PAINT_PURPLE = 8,
	PAINT_PINK = 9,
	PAINT_ORANGE = 10,
	PAINT_GREY = 11,
	PAINT_TITANIUMWHITE = 12,
	PAINT_SAFFRON = 13,
	PAINT_GOLD = 14,
	PAINT_ROSEGOLD = 15,
	PAINT_WHITEGOLD = 16,
	PAINT_ONYX = 17,
	PAINT_PLATINUM = 18
};

const char* getSlotName(EQUIPSLOT slot) {
	static const std::unordered_map<EQUIPSLOT, const char*> slotNames = {
		{SLOT_BODY, "Body"},//
		{SLOT_SKIN, "Skin"},//
		{SLOT_WHEELS, "Wheels"},//
		{SLOT_BOOST, "Boost"},//
		{SLOT_ANTENNA, "Antenna"},//
		{SLOT_HAT, "Hat"},//
		{SLOT_PAINTFINISH, "Paint Finish"},
		{SLOT_PAINTFINISH_SECONDARY, "Secondary Paint Finish"},
		{SLOT_ENGINE_AUDIO, "Engine Audio"},
		{SLOT_SUPERSONIC_TRAIL, "Supersonic Trail"},
		{SLOT_GOALEXPLOSION, "Goal Explosion"},
		{SLOT_ANTHEM, "Anthem"}
	};

	auto it = slotNames.find(slot);
	if (it != slotNames.end()) {
		return it->second;
	}

	return "Unknown Slot";
}

// Function to print the loadout
void print_loadout(BMLoadout loadout)
{
	LOG("HEADER");
	LOG("\tVersion: {}", unsigned(loadout.header.version));
	LOG("\tSize in bytes: {}", loadout.header.code_size);
	LOG("\tCRC: {}", unsigned(loadout.header.crc));
	LOG("");  // Empty line for separation

	LOG("Blue is orange: {}", loadout.body.blue_is_orange ? "true" : "false");

	LOG("Blue:");
	for (auto body : loadout.body.blue_loadout)
	{
		const char* slotName = getSlotName(static_cast<EQUIPSLOT>(body.first));
		if (slotName != "Unknown Slot") {
			LOG("\tSlot: {}, Name: {}, ID: {}, paint: {}", unsigned(body.first), slotName, body.second.product_id, unsigned(body.second.paint_index));
			if (loadout.body.blue_wheel_team_id != 0 && body.second.slot_index == SLOT_WHEELS) {
				LOG("\t\tTeamID: {}", unsigned(loadout.body.blue_wheel_team_id));
			}
		}
	}

	//if (loadout.body.blueColor.should_override) {
		LOG("Color Primary ({}, {}, {})", unsigned(loadout.body.blueColor.primary_colors.r), unsigned(loadout.body.blueColor.primary_colors.g), unsigned(loadout.body.blueColor.primary_colors.b));
		LOG("Secondary ({}, {}, {})", unsigned(loadout.body.blueColor.secondary_colors.r), unsigned(loadout.body.blueColor.secondary_colors.g), unsigned(loadout.body.blueColor.secondary_colors.b));
	//}

	if (!loadout.body.blue_is_orange)
	{
		LOG("");  // Empty line for separation
		LOG("Orange:");
		for (auto body : loadout.body.orange_loadout)
		{

			const char* slotName = getSlotName(static_cast<EQUIPSLOT>(body.first));
			if (slotName != "Unknown Slot") {
				LOG("\tSlot: {}, Name: {}, ID: {}, paint: {}", unsigned(body.first), slotName, body.second.product_id, unsigned(body.second.paint_index));
				if (loadout.body.orange_wheel_team_id != 0 && body.second.slot_index == SLOT_WHEELS) {
					LOG("\t\tTeamID: {}", unsigned(loadout.body.orange_wheel_team_id));
				}
			}
		}

		if (loadout.body.orangeColor.should_override) {
			LOG("Color Primary ({}, {}, {})", unsigned(loadout.body.orangeColor.primary_colors.r), unsigned(loadout.body.orangeColor.primary_colors.g), unsigned(loadout.body.orangeColor.primary_colors.b));
			LOG("Secondary ({}, {}, {})", unsigned(loadout.body.orangeColor.secondary_colors.r), unsigned(loadout.body.orangeColor.secondary_colors.g), unsigned(loadout.body.orangeColor.secondary_colors.b));
		}
	}

	LOG("");  // Empty line for separation
}
std::map<uint8_t, Item> read_items_from_buffer(BitBinaryReader<unsigned char>& reader, const int loadoutVersion)
{
	std::map<uint8_t, Item> items;
	int itemsSize = reader.ReadBits<int>(4); //Read the length of the item array
	std::cout << "itemsSize: " << itemsSize << std::endl;
	if (itemsSize == 0) {
		int itemsSize = reader.ReadBits<int>(4); //Read the length of the item array
		std::cout << "itemsSize: " << itemsSize << std::endl;
	}
	for (int i = 0; i < itemsSize; i++)
	{
		Item option;
		int slotIndex = reader.ReadBits<int>(5); //Read slot of item
		int productId = reader.ReadBits<int>(loadoutVersion >= 4 ? 16 : 13); //Read product ID
		bool isPaintable = reader.ReadBool(); //Read whether item is paintable or not

		if (isPaintable)
		{
			int paintID = reader.ReadBits<int>(6); //Read paint index
			option.paint_index = paintID;
		}
		option.product_id = productId;
		option.slot_index = slotIndex;
		items.insert_or_assign(slotIndex, option); //Add item to loadout at its selected slot
	}
	return items;
}

RGB read_colors_from_buffer(BitBinaryReader<unsigned char>& reader)
{
	RGB col;
	col.r = reader.ReadBits<uint8_t>(8);
	col.g = reader.ReadBits<uint8_t>(8);
	col.b = reader.ReadBits<uint8_t>(8);
	return col;
}

BMLoadout load(std::string loadoutString)
{
	
	BitBinaryReader<unsigned char> reader(loadoutString);
	BMLoadout loadout;
	
	/*
	Reads header
		VERSION (6 bits)
		SIZE_IN_BYTES (10 bits)
		CRC (8 BITS)
	*/
	loadout.header.version = reader.ReadBits<uint8_t>(6);
	loadout.header.code_size = reader.ReadBits<uint16_t>(10);
	loadout.header.crc = reader.ReadBits<uint8_t>(8);

	
	/* Verification (can be skipped if you already know the code is correct) */

	/*
	Calculate whether code_size converted to base64 is actually equal to the given input string
	Mostly done so we don't end up with invalid buffers, but this step is not required.
	*/
	int stringSizeCalc = ((int)ceil((4 * (float)loadout.header.code_size / 3)) + 3) & ~3;
	int stringSize = loadoutString.size();
	
	if (abs(stringSizeCalc - stringSize) > 6) //Diff may be at most 4 (?) because of base64 padding, but we check > 6 because IDK
	{
		//Given input string is probably invalid, handle
		std::cout << "Invalid input string size!";
		exit(0);
	}
	

	/*
	Verify CRC, aka check if user didn't mess with the input string to create invalid loadouts
	*/
	
	if (!reader.VerifyCRC(loadout.header.crc, 3, loadout.header.code_size))
	{
		//User changed characters in input string, items isn't valid! handle here
		std::cout << "Invalid input string! CRC check failed";
		exit(0);
	}
	

	//At this point we know the input string is probably correct, time to parse the body

	loadout.body.blue_is_orange = reader.ReadBool(); //Read single bit indicating whether blue = orange
	loadout.body.blue_loadout = read_items_from_buffer(reader, loadout.header.version); //Read loadout

	loadout.body.blueColor.should_override = reader.ReadBool(); //Read whether custom colors is on
	if (loadout.body.blueColor.should_override) {
		/* Read rgb for primary colors (0-255)*/
		loadout.body.blueColor.primary_colors = read_colors_from_buffer(reader);

		/* Read rgb for secondary colors (0-255)*/
		loadout.body.blueColor.secondary_colors = read_colors_from_buffer(reader);
	}

	if (loadout.header.version > 2) {
		loadout.body.blue_wheel_team_id = reader.ReadBits<int>(6);
	}

	if (loadout.body.blue_is_orange) //User has same loadout for both teams
	{
		loadout.body.orange_loadout = loadout.body.blue_loadout;
		loadout.body.orange_wheel_team_id = loadout.body.blue_wheel_team_id;
	}
	else {
		loadout.body.orange_loadout = read_items_from_buffer(reader, loadout.header.version);
		loadout.body.orangeColor.should_override = reader.ReadBool(); //Read whether custom colors is on
		if (loadout.body.blueColor.should_override) {
			/* Read rgb for primary colors (0-255)*/
			loadout.body.orangeColor.primary_colors = read_colors_from_buffer(reader);

			/* Read rgb for secondary colors (0-255)*/
			loadout.body.orangeColor.secondary_colors = read_colors_from_buffer(reader);
		}

		if (loadout.header.version > 2) {
			loadout.body.orange_wheel_team_id = reader.ReadBits<int>(6);
		}
	}

	return loadout;
}

