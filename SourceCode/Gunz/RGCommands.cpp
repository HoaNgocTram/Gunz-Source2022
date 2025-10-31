#include "stdafx.h"
#include "ZConfiguration.h"
#include "NewChat.h"
#include "RGMain.h"
#include "VoiceChat.h"
#include "Config.h"
#include <cstdint>
#include "ZOptionInterface.h"
#include "ZTestGame.h"

bool CheckDeveloperMode(const char* Name)
{
	if (ZApplication::GetInstance()->GetLaunchMode() != ZApplication::ZLAUNCH_MODE_STANDALONE_GAME)
	{
		ZChatOutputF("%s can only be used in developer mode", Name);
		return false;
	}

	return true;
}
struct BoolResult
{
	bool Success;
	bool Value;
};
static BoolResult ParseBool(int argc, char** argv)
{
	auto Argument = argv[1];
	if (!_stricmp(Argument, "true") || !strcmp(Argument, "1")) {
		return{ true, true };
	}
	else if (!_stricmp(Argument, "false") || !strcmp(Argument, "0")) {
		return{ true, false };
	}

	return{ false, false };
}

// Sets the value of a bool according to the given arguments, and outputs feedback to user in chat.
//
// If no arguments are given, the value is toggled.
// If one argument is given, it's parsed by ParseBool: 
// "true" or "1" sets the bool to true,
// and "false" or "0" sets it to false.
//
// Returns false on invalid arguments. No change to the variable is applied in this case.
static bool SetBool(const char* Name, bool& Value, int argc, char** argv)
{
	if (argc < 1 || argc > 2) {
		assert(false);
		return false;
	}

	if (argc > 1)
	{
		// We got an argument. Parse it into a bool.
		auto ret = ParseBool(argc, argv);
		if (!ret.Success)
		{
			ZChatOutputF("%s is not a valid bool argument.", argv[1]);
			return false;
		}

		Value = ret.Value;
	}
	else
	{
		// No arguments. Toggle the value.
		Value = !Value;
	}

	ZChatOutputF("%s %s.", Name, Value ? "enabled" : "disabled");
	return true;
}


void LoadRGCommands(ZChatCmdManager& CmdManager)
{
	CmdManager.AddCommand(0, "argv", [](const char* line, int argc, char** const argv) {
		for (int i = 0; i < argc; i++)
			ZChatOutputF("%s", argv[i]);
		}, CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/argv", "");

#ifdef _VOICE_CHAT
	CmdManager.AddCommand(0, "mute", [](const char* line, int argc, char** const argv) {
		auto ret = FindSinglePlayer(argv[1]);

		if (!ret.second)
		{
			switch (ret.first)
			{
			case PlayerFoundStatus::NotFound:
				ZChatOutputF("No player with %s in their name was found", argv[1]);
				break;

			case PlayerFoundStatus::TooManyFound:
				ZChatOutputF("Too many players with %s in their name was found", argv[1]);
				break;

			default:
				ZChatOutputF("Unknown error %d", static_cast<int>(ret.first));
			};

			return;
		}

		bool b = GetRGMain().MutePlayer(ret.second->GetUID());

		ZChatOutputF("%s has been %s", ret.second->GetUserName(), b ? "muted" : "unmuted");
	}, CCF_ALL, 1, 1, true, "/mute <nameuser>", "");
#endif

#ifdef _SPEC 1
	CmdManager.AddCommand(0, "spec", [](const char* line, int argc, char** const argv) 
		{
		if (ZGetGameInterface()->GetState() != GUNZ_GAME)
			return;

		bool IsSpec = ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_SPECTATOR;
		ZPostSpec(!IsSpec); }, CCF_ALL, 0, 0, true, "/spec", "");
#endif

	//CmdManager.AddCommand(0, "clear", [](const char* line, int argc, char** const argv) {
	//	GetRGMain().GetChat().ClearHistory();
	//	},
	//	CCF_ALL, 0, 0, true, "/clear", "");

#ifdef _FREELOOK
	//CmdManager.AddCommand(0, "freelook", [](const char* line, int argc, char** const argv) {
	//	if (ZGetGameInterface()->GetState() != GUNZ_GAME)
	//		return;

	//	bool Value = ZGetCamera()->GetLookMode() == ZCAMERA_FREELOOK;
	//	if (SetBool("Freelook", Value, argc, argv)) {
	//		ZGetCamera()->SetLookMode(Value ? ZCAMERA_FREELOOK : ZCAMERA_DEFAULT);
	//	}
	//	},
	//	CCF_ALL, 1, 1, true, "/freelook <0/1>", "");
#endif


}