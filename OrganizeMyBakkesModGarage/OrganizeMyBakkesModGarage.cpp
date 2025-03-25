#include "pch.h"
#include "OrganizeMyBakkesModGarage.h"


BAKKESMOD_PLUGIN(OrganizeMyBakkesModGarage, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;



void OrganizeMyBakkesModGarage::onLoad()
{
	_globalCvarManager = cvarManager;	


	cvarManager->registerNotifier(
		"open_organizemybakkesmodgarage_ui",
		[this](std::vector<std::string> args) {
			if (!isWindowOpen_) {
				LOG("Opening OrganizeMyBakkesModGarage window");
				
				cvarManager->executeCommand("togglemenu " + GetMenuName());
				
				
			}
		}, "Displays the window for bakkes garage.", PERMISSION_ALL
	);
	readCurrentBakkesModPreset("cfg\\config.cfg");
	

	std::filesystem::path myDataFolder = gameWrapper->GetDataFolder() / "OrganizeMyBakkesModGarage";
	groupFilePath = myDataFolder / "groups.txt";
	LoadGroupsFromFile(groupFilePath);

	//TAGame.Mutator_Freeplay_TA.Init
	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", [this](std::string eventName) {
		if (currentGroup.first.empty()) return;
		if (!shuffleInFreeplay) {
			LOG("Not shuffling in freeplay");
			return;
		}
		//readCurrentBakkesModPreset("cfg\\config.cfg");
		LOG("Found current preset: {}", currentBakkesModPreset);
		std::string idChoice = currentBakkesModPreset;
		while (idChoice == currentBakkesModPreset) {
			int randomFromGroup = currentGroup.second.size() > 0 ? rand() % currentGroup.second.size() : 0;
			idChoice = currentGroup.second[randomFromGroup].id;
		}


		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
		LOG("Executing preset swap");
		if (swapCarBodyCapability) {
			decodePresetId(idChoice);
		}
		gameWrapper->Execute([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command, false);
			});
		currentBakkesModPreset = idChoice;



		});

	gameWrapper->HookEvent("Function TAGame.OnlineGameJoinGame_TA.GetLoadout", [this](std::string eventName) {
		if (currentGroup.first.empty()) return;
		if (!shuffleInOnlineGame) {
			LOG("Not shuffling in online game");
			return;
		}
		//readCurrentBakkesModPreset("cfg\\config.cfg");
		LOG("Found current preset: {}", currentBakkesModPreset);
		std::string idChoice = currentBakkesModPreset;
		while (idChoice == currentBakkesModPreset) {
			int randomFromGroup = currentGroup.second.size() > 0 ? rand() % currentGroup.second.size() : 0;
			idChoice = currentGroup.second[randomFromGroup].id;
		}


		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
		LOG("Executing preset swap");
		gameWrapper->Execute([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command, false);
			});
		currentBakkesModPreset = idChoice;



		});


	//gameWrapper->HookEvent("Function TAGame.LoadingScreen_TA.HandlePreLoadMap",			
	//	[this](std::string eventName) {
	//		/*ServerWrapper sw = gameWrapper->GetCurrentGameState();
	//		if (!sw) return;
	//		if (gameWrapper->IsInGame()) {
	//			LOG("In game, saving groups to file.");

	//		}*/
	//		if(currentGroup.first.empty()) return;
	//		if (!shuffleInFreeplay && gameWrapper->IsInFreeplay()) {
	//			LOG("Not shuffling in freeplay");
	//			return;
	//		}
	//		if (!shuffleInOnlineGame && gameWrapper->IsInOnlineGame()) {
	//			LOG("Not shuffling in online game");
	//			return;
	//		}
	//		readCurrentBakkesModPreset("cfg\\config.cfg");
	//		LOG("Found current preset: {}", currentBakkesModPreset);	
	//		std::string idChoice = currentBakkesModPreset;
	//		while (idChoice == currentBakkesModPreset) {
	//			int randomFromGroup = currentGroup.second.size() > 0 ? rand() % currentGroup.second.size() : 0;
	//			idChoice = currentGroup.second[randomFromGroup].id;
	//		}

	//		
	//		std::string command = "sleep 1;cl_itemmod_code " + idChoice;
	//		LOG("Executing preset swap");
	//		gameWrapper->Execute([this, command](GameWrapper* gw) {
	//			cvarManager->executeCommand(command, false);
	//			});
	//		//currentGroup
	//		

	//	}
	//);

	

	/*gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", [this](std::string eventName) {
		if (!defaultHooked){
			defaultHooked = true;
			cvarManager->setBind("F4", "open_organizemybakkesmodgarage_ui");
			LOG("Default bind set to F4 on main menu load.");
		}		
	});*/

	//LOG("Plugin loaded!");
	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("OrganizeMyBakkesModGarage debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	/*cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		LOG("the cvar with name: {} changed", cvarName);
		LOG("the new value is: {}", newCvar.getStringValue());
	});*/

	//cvar2.addOnValueChanged(std::bind(&OrganizeMyBakkesModGarage::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&OrganizeMyBakkesModGarage::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&OrganizeMyBakkesModGarage::YourPluginMethod, this);
}




void OrganizeMyBakkesModGarage::onUnload() {
	LOG("Plugin  organizemygarage unloaded!");
	SaveGroupsToFile(groupFilePath);
}

void OrganizeMyBakkesModGarage::OnClose()
{
	PluginWindowBase::OnClose(); // Call base class for any default behavior
	SaveGroupsToFile(groupFilePath);
	LOG("Groups saved to file on window close.");
}

void OrganizeMyBakkesModGarage::OnOpen() {

	PluginWindowBase::OnOpen(); // Call base class for any default behavior
	LOG("Groups loaded from file on window open.");
	presets = readPresets("data\\presets.data");
}





