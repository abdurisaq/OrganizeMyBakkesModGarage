
#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"



RGB ConvertToRGB(float r, float g, float b)
{
	// Clamp values to the range [0, 255] and cast to uint8_t
	return RGB{
		static_cast<uint8_t>(std::clamp(r * 255.0f, 0.0f, 255.0f)),
		static_cast<uint8_t>(std::clamp(g * 255.0f, 0.0f, 255.0f)),
		static_cast<uint8_t>(std::clamp(b * 255.0f, 0.0f, 255.0f))
	};
}



BMLoadout OrganizeMyBakkesModGarage::ConvertToBMLoadout(const pluginsdk::Loadout& loadout, const CarInfo& car_info)
{
    BMLoadout bmLoadout;

    // Fill in Header information
    bmLoadout.header.version = CURRENT_LOADOUT_VERSION;
    bmLoadout.header.code_size = sizeof(BMLoadout); // Optionally calculate code size
    bmLoadout.header.crc = 0;  // You could calculate a CRC if needed

    // Initialize Body
    Body& body = bmLoadout.body;

    // Maps for blue and orange teams' loadouts
    std::map<uint8_t, Item> blueLoadout;
    std::map<uint8_t, Item> orangeLoadout;

    // Color overrides (if applicable)
    OverrideColor blueColor;
    OverrideColor orangeColor;

    // Process each loadout item and assign it to both blue and orange loadouts
    for (const auto& [slot, item] : loadout.items) {
        Item bmItem;
        bmItem.slot_index = static_cast<uint8_t>(slot); // Assuming the slot maps directly
        bmItem.product_id = item.product_id;

        // Process paint index (if available)
        for (const auto& attr : item.attributes) {
            if (attr.type == pluginsdk::ItemAttribute::AttributeType::PAINT) {
                bmItem.paint_index = static_cast<uint8_t>(attr.value);  // Map paint index
            }
        }

        // Assign the item to both blue and orange loadouts
        blueLoadout[static_cast<uint8_t>(slot)] = bmItem;
        orangeLoadout[static_cast<uint8_t>(slot)] = bmItem;
    }

    // Assign loadouts to Body struct
    body.blue_loadout = blueLoadout;
    body.orange_loadout = orangeLoadout;

    // Handle color overrides
    //
        //	const auto& colors = car_info.loadout.paint_finish;

    //	if (colors.team_paint.has_value())
    //	{
    //		LOG("    Team Paint: team {}, team_color_id {}, custom_color_id {}",
    //			colors.team_paint->team, colors.team_paint->team_color_id, colors.team_paint->custom_color_id);
    //	}



    if (loadout.paint_finish.team_paint.has_value()) {
        const auto& color = loadout.paint_finish;
        LOG("overriding color");
        std::vector<pluginsdk::PaintFinishColor> colors; 
        if (car_info.team == 0) {
            colors = LoadoutUtilities::GetBlueColorSet();
        }
        else if(car_info.team ==1) {
            colors = LoadoutUtilities::GetOrangeColorSet();
        }
        LOG(" primary color : R {}, G: {}, B: {}",
            			colors[color.team_paint->team_color_id].color.R, colors[color.team_paint->team_color_id].color.G, colors[color.team_paint->team_color_id].color.B);
        LOG("secondary color : R {}, G: {}, B: {}",
            						colors[color.team_paint->custom_color_id].color.R, colors[color.team_paint->custom_color_id].color.G, colors[color.team_paint->custom_color_id].color.B);
        LOG("    Team Paint: team {}, team_color_id {}, custom_color_id {}",
            			color.team_paint->team, color.team_paint->team_color_id, color.team_paint->custom_color_id);
        
        blueColor.primary_colors = ConvertToRGB(colors[color.team_paint->team_color_id].color.R, colors[color.team_paint->team_color_id].color.G, colors[color.team_paint->team_color_id].color.B);
        blueColor.secondary_colors = ConvertToRGB(colors[color.team_paint->custom_color_id].color.R, colors[color.team_paint->custom_color_id].color.G, colors[color.team_paint->custom_color_id].color.B);
    }

    //if bm overriding, this is primary color
    if (loadout.paint_finish.team_color_override.has_value()) {

        const auto& color = *loadout.paint_finish.team_color_override;
        blueColor.should_override = true;
        blueColor.secondary_colors = ConvertToRGB(color.R, color.G, color.B);
        orangeColor.should_override = true;
        orangeColor.secondary_colors = ConvertToRGB(color.R, color.G, color.B);
        LOG("overriding color");
        LOG("    Custom Color: r {}, g {}, b {}",
            			color.R, color.G, color.B);
    }

    //if bm overriding, this is secondary color
    if (loadout.paint_finish.custom_color_override.has_value()) {
		const auto& color = *loadout.paint_finish.custom_color_override;
		blueColor.should_override = true;
		blueColor.primary_colors = ConvertToRGB(color.R, color.G, color.B);
		orangeColor.should_override = true;
		orangeColor.primary_colors = ConvertToRGB(color.R, color.G, color.B);
		LOG("overriding color");
		LOG("    Custom Color Override: r {}, g {}, b {}",
            						color.R, color.G, color.B);
	}
    body.blueColor = blueColor;
    body.orangeColor = orangeColor;

    // Set other Body parameters (example: blue_wheel_team_id, etc.)
    body.blue_wheel_team_id = 0;  // Assign wheel team IDs
    body.orange_wheel_team_id = 1;

    return bmLoadout;
}
