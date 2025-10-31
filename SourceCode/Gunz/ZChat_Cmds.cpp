#include "stdafx.h"

#include "ZChat.h"
#include "MChattingFilter.h"
#include "ZApplication.h"
#include "ZGameInterface.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "ZCombatChat.h"
#include "ZCombatInterface.h"
#include "ZIDLResource.h"
#include "MListBox.h"
#include "MLex.h"
#include "MTextArea.h"
#include "ZMyInfo.h"
#include "ZMessages.h"
#include "MUtil.h"
#include "ZConfiguration.h"
#include "ZInterfaceListener.h"
#include "ZNetRepository.h"
#include "ZApplication.h"
#include "ZChat_CmdID.h"
#include "MEventInfo.h"
#include "ZOptionInterface.h"
#include "ZCommandHack.h"

#include "RGMain.h"
#include "Config.h"

void ChatCmd_Test(const char* line, const int argc, char **const argv);				// Testing
void ChatCmd_Exit(const char* line, const int argc, char** const argv);
void ChatCmd_Help(const char* line, const int argc, char **const argv);				// µµøÚ∏ª
void ChatCmd_Go(const char* line, const int argc, char **const argv);					// ∞‘¿”πÅE¿Ãµø
void ChatCmd_Whisper(const char* line, const int argc, char **const argv);			// ±”º”∏ª
void ChatCmd_CreateChatRoom(const char* line, const int argc, char **const argv);		// √§∆√πÅE∞≥º≥
void ChatCmd_JoinChatRoom(const char* line, const int argc, char **const argv);		// √§∆√πÅE¬ÅE©
void ChatCmd_LeaveChatRoom(const char* line, const int argc, char **const argv);		// √§∆√πÅE≈ª≈ÅE
void ChatCmd_SelectChatRoom(const char* line, const int argc, char **const argv);		// √§∆√πÅEº±≈√
void ChatCmd_InviteChatRoom(const char* line, const int argc, char **const argv);		// √§∆√πÅE√ ¥ÅE
void ChatCmd_VisitChatRoom(const char* line, const int argc, char **const argv);		// √§∆√πÅE√ ¥ÅE¬ÅEÆ
void ChatCmd_ChatRoomChat(const char* line, const int argc, char **const argv);		// √§∆√
void ChatCmd_CopyToTestServer(const char* line, const int argc, char **const argv);	// ƒ≥∏Ø≈Õ¿ÅE€ - ≈◊Ω∫∆Æ º≠πˆ∑Œ ƒ≥∏Ø≈Õ ¿ÅE€
void ChatCmd_StageFollow(const char* line, const int argc, char **const argv);		// «√∑π¿ÃæÅE√ﬂ¿˚«ÿº≠ ∞‘¿”¬ÅE°
void ChatCmd_Friend(const char* line, const int argc, char **const argv);				// ƒ£±∏ ∞ÅE√
void ChatCmd_Clan(const char* line, const int argc, char **const argv);				// ≈¨∂ÅE∞ÅE√

void ChatCmd_RequestQuickJoin(const char* line, const int argc, char **const argv);		// ƒÅE∂¿Œ
void ChatCmd_Report119(const char* line, const int argc, char **const argv);				// Ω≈∞ÅE
void ChatCmd_RequestPlayerInfo(const char* line, const int argc, char **const argv);		// ªÁøÅE⁄ ¡§∫∏∫∏±ÅE
void ChatCmd_Macro(const char* line,const int argc, char **const argv);

// ∞‘¿”æ»ø°º≠∏∏ ∞°¥…«— ∏˙”…æÅE
void ChatCmd_EmotionTaunt(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionBow(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionWave(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionLaugh(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionCry(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionDance(const char* line,const int argc, char **const argv);
void ChatCmd_Suicide(const char* line,const int argc, char **const argv);
void ChatCmd_Callvote(const char* line,const int argc, char **const argv);
void ChatCmd_VoteYes(const char* line,const int argc, char **const argv);
void ChatCmd_VoteNo(const char* line,const int argc, char **const argv);
void ChatCmd_Kick(const char* line,const int argc, char **const argv);
void ChatCmd_MouseSensitivity(const char* line,const int argc, char **const argv);

// Custom: Command callbacks
void ChatCmd_AdminSummon(const char* line,const int argc, char **const argv);
void ChatCmd_AdminGoTo(const char* line,const int argc, char **const argv);
void ChatCmd_AdminSlap(const char* line,const int argc, char **const argv);
void ChatCmd_AdminSpawn(const char* line,const int argc, char **const argv);
void ChatCmd_AdminPopup(const char* line,const int argc, char **const argv);
void ChatCmd_AdminSilence(const char* line,const int argc, char **const argv);
void ChatCmd_AdminPKick(const char* line,const int argc, char **const argv);
void ChatCmd_AdminUKick(const char* line,const int argc, char **const argv);
void ChatCmd_AdminTeam(const char* line,const int argc, char **const argv);
void ChatCmd_AdminNAT(const char* line,const int argc, char **const argv);
void ChatCmd_LobbyChat(const char* line, const int argc, char **const argv);
void ChatCmd_PingServer(const char* line, const int argc, char** const argv);
void ChatCmd_GiveCoinsCash(const char* line, const int argc, char** const argv);
void ChatCmd_GiveCoinsMedals(const char* line, const int argc, char** const argv);
void ChatCmd_Senditem(const char* line, const int argc, char** const argv);
#ifdef _STAFFCHAT
void ChatCmd_StaffChat(const char* line, const int argc, char** const argv);
#endif
void ChatCmd_LadderInvite(const char* line,const int argc, char **const argv);
void ChatCmd_LadderTest(const char* line,const int argc, char **const argv);
void ChatCmd_LaunchTest(const char* line,const int argc, char **const argv);

// ƒ˘Ω∫∆Æ ≈◊Ω∫∆ÆøÅE∏˙”…æÅE
void ChatCmd_QUESTTEST_God(const char* line,const int argc, char **const argv);				// π´¿˚∏µÅE
void ChatCmd_QUESTTEST_SpawnNPC(const char* line,const int argc, char **const argv);		// NPC Ω∫∆ÅE
void ChatCmd_QUESTTEST_NPCClear(const char* line,const int argc, char **const argv);		// NPC ≈¨∏ÆæÅE
void ChatCmd_QUESTTEST_Reload(const char* line,const int argc, char **const argv);			// ∏Æº“Ω∫ ∏Æ∑ŒµÅE
void ChatCmd_QUESTTEST_SectorClear(const char* line,const int argc, char **const argv);		// ºΩ≈Õ ≈¨∏ÆæÅE
void ChatCmd_QUESTTEST_Finish(const char* line,const int argc, char **const argv);			// ƒ˘Ω∫∆Æ ≈¨∏ÆæÅE
void ChatCmd_QUESTTEST_LocalSpawnNPC(const char* line,const int argc, char **const argv);	// npc Ω∫∆ÅE(local)
void ChatCmd_QUESTTEST_WeakNPCs(const char* line,const int argc, char **const argv);		// NPC ø°≥ ¡ˆ∏¶ 1∑Œ

// admin ∏˙”…
void ChatCmd_AdminKickPlayer(const char* line, const int argc, char **const argv);		// µΩ∫ƒø≥ÿ∆Æ Ω√≈¥
void ChatCmd_AdminMutePlayer(const char* line, const int argc, char **const argv);		// √§∆√ ±›¡ÅEΩ√≈¥
void ChatCmd_AdminBlockPlayer(const char* line, const int argc, char **const argv);		// ¡¢º” ±›¡ÅEΩ√≈¥

#ifdef _GRADECHANGE
void ChatCmd_GetGrade(const char* line, const int argc, char** const argv);
#endif

void ChatCmd_AdminPingToAll(const char* line, const int argc, char **const argv);		// Garbage Connection √ªº“ »Æ¿Œ
void ChatCmd_AdminAnnounce(const char* line, const int argc, char **const argv);		// ¿ÅEº ∞¯¡ÅE
void ChatCmd_AdminServerHalt(const char* line, const int argc, char **const argv);		// º≠πÅE¡æ∑ÅE
void ChatCmd_AdminSwitchCreateLadderGame(const char* line, const int argc, char **const argv);		// ≈¨∑£¿ÅEø©∫Œ º≥¡§
void ChatCmd_AdminReloadClientHash(const char* line, const int argc, char **const argv);		// XTrap ≈¨∂Û¿Ãæ∆Æ Hash ∏Æ∑ŒµÅE
void ChatCmd_AdminResetAllHackingBlock( const char* line, const int argc, char **const argv );
void ChatCmd_AdminReloadGambleitem( const char* line, const int argc, char **const argv );
void ChatCmd_AdminDumpGambleitemLog( const char* line, const int argc, char **const argv );
void ChatCmd_AdminAssasin( const char* line, const int argc, char **const argv );
void ChatCmd_AdminFreeze(const char* line, const int argc, char **const argv);
void ChatCmd_AdminKickAll(const char* line, const int argc, char **const argv);
void ChatCmd_Killall(const char* line, const int argc, char** const argv);
void ChatCmd_AdminTeleport2(const char* line, const int argc, char** const argv);
void ChatCmd_AdminTeleport(const char* line, const int argc, char** const argv);
void ChatCmd_AdminTeleportAll(const char* line, const int argc, char** const argv);
void ChatCmd_AdminStage(const char* line, const int argc, char** const argv);
// event ¡¯«‡ø˚€µ¿⁄ ∏˙”…
void ChatCmd_ChangeMaster(const char* line, const int argc, char **const argv);			// πÊ¿Â±««— ª©æ—æ˚€»
void ChatCmd_ChangePassword(const char* line, const int argc, char **const argv);		// πÅE∆–Ω∫øˆµÅEπŸ≤ﬁ
void ChatCmd_AdminHide(const char* line, const int argc, char **const argv);			// ≈ı∏˙‹Œ∞£
void ChatCmd_RequestJjang(const char* line, const int argc, char **const argv);
void ChatCmd_RemoveJjang(const char* line, const int argc, char **const argv);
void ChatCmd_AdminSpeed(const char* line, const int argc, char** const argv);
void ChatCmd_AdminSBTest(const char* line, const int argc, char** const argv);
void ChatCmd_AdminEsp(const char* line, const int argc, char** const argv);
void ChatCmd_Staff(const char* line, const int argc, char** const argv);
///////////////////////////////////////////////////////////////////////////////////
void ChatCmd_FindPlayer(const char* line, const int argc, char** const argv);
void ChatCmd_Mods(const char* line, const int argc, char** const argv);
void ChatCmd_Commands(const char* line, const int argc, char** const argv);
void ChatCmd_IgnorePlayer(const char* line, const int argc, char** const argv);
void ChatCmd_UnIgnorePlayer(const char* line, const int argc, char** const argv);
void ChatCmd_IgnoreList(const char* line, const int argc, char** const argv);
void ChatCmd_HideJjang(const char* line, const int argc, char** const argv);
void ChatCmd_Ki(const char* line, const int argc, char** const argv);
void ChatCmd_ClearChat(const char* line, const int argc, char** const argv);
void ChatCmd_TeleSavePos(const char* line, const int argc, char** const argv);
void ChatCmd_TeleLoadPos(const char* line, const int argc, char** const argv);

#ifdef _REJOIN_NEW
void ChatCmd_StageRejoin(const char* line, const int argc, char** const argv);
#endif

/////////////////////////////////////////////////////////////////////////////////
void ChatCmd_Flipmower(const char* line, const int argc, char** const argv);
void ChatCmd_InsaneMassive(const char* line, const int argc, char** const argv);
void ChatCmd_LawnMower(const char* line, const int argc, char** const argv);
void ChatCmd_Respawn(const char* line, const int argc, char** const argv);
void ChatCmd_Enchant(const char* line, const int argc, char** const argv);
void ChatCmd_Shot(const char* line, const int argc, char** const argv);
void ChatCmd_NpcShot(const char* line, const int argc, char** const argv);
void ChatCmd_Explosion(const char* line, const int argc, char** const argv);
void ChatCmd_ExplosionUnder(const char* line, const int argc, char** const argv);
void ChatCmd_GrenadeThrow(const char* line, const int argc, char** const argv);
void ChatCmd_InfoPlayer(const char* line, const int argc, char** const argv);
#ifdef _RADIO
void ChatCmd_RadioCommand(const char* line, const int argc, char** const argv);
#endif
#ifdef _REPORT
void ChatCmd_Report(const char* line, const int argc, char** const argv);
void ChatCmd_StaffLists(const char* line, const int argc, char** const argv);
#endif
#ifdef _ASSITEN
void ChatCmd_AdminAssitent(const char* line, const int argc, char** const argv);
#endif

void ChatCmd_Pos(const char* line, const int argc, char** const argv);

#ifdef _CMD_ALL
void ChatCmd_Crash(const char* line, const int argc, char** const argv);
#endif
void _AddCmdFromXml(ZChatCmdManager* pCmdManager, ZCmdXmlParser* pParser,
	int nCmdID, ZChatCmdProc fnProc, u32 flag,
	int nMinArgs, int nMaxArgs, bool bRepeatEnabled)
{
	auto pCmdStr = pParser->GetStr(nCmdID);
	if (pCmdStr)
	{
		pCmdManager->AddCommand(nCmdID, pCmdStr->szName, fnProc, flag, nMinArgs, nMaxArgs, bRepeatEnabled,
			pCmdStr->szUsage, pCmdStr->szHelp);

		for (int i = 0; i < (int)pCmdStr->vecAliases.size(); i++)
		{
			pCmdManager->AddAlias(pCmdStr->vecAliases[i].c_str(), pCmdStr->szName);
		}
	}
}

#define _CC_AC(X1,X2,X3,X4,X5,X6,X7,X8)		m_CmdManager.AddCommand(0,X1,X2,X3,X4,X5,X6,X7,X8)
#define _CC_ALIAS(NEW,TAR)					m_CmdManager.AddAlias(NEW,TAR)
#define _CC_ACX(X1,X2,X3,X4,X5,X6)			_AddCmdFromXml(&m_CmdManager,&parser,X1,X2,X3,X4,X5,X6)

// √§∆√∏˙”…æÅEªÁøÅE˝¿∫ http://iworks.maietgames.com/mdn/wiki.php/∞«¡ÅE√§∆√∏˙”…æÅEø° ¿˚æ˚›÷ººøÅE. by bird

void ZChat::InitCmds()
{
	ZCmdXmlParser parser;
	if (!parser.ReadXml(ZApplication::GetFileSystem(), FILENAME_CHATCMDS))
	{
		MLog("Error while Read Item Descriptor %s", "system/chatcmds.xml");
	}

	LoadRGCommands(m_CmdManager);

	_CC_ACX(CCMD_ID_HELP,				&ChatCmd_Help,				CCF_ALL, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_WHISPER,			&ChatCmd_Whisper,			CCF_ALL, ARGVNoMin, 1, false);
	_CC_ACX(CCMD_ID_REPORT119,			&ChatCmd_Report119,			CCF_ALL, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_FRIEND,				&ChatCmd_Friend,			CCF_ALL, ARGVNoMin, 1, false);
	_CC_ACX(CCMD_ID_CLAN,				&ChatCmd_Clan,				CCF_ALL, ARGVNoMin, ARGVNoMax, false);
	_CC_ACX(CCMD_ID_STAGE_FOLLOW,		&ChatCmd_StageFollow,		CCF_LOBBY, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_REQUEST_QUICK_JOIN,	&ChatCmd_RequestQuickJoin,	CCF_LOBBY, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_EMOTION_TAUNT,		&ChatCmd_EmotionTaunt,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_BOW,		&ChatCmd_EmotionBow  ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_WAVE,		&ChatCmd_EmotionWave ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_LAUGH,		&ChatCmd_EmotionLaugh,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_CRY,		&ChatCmd_EmotionCry  ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_DANCE,		&ChatCmd_EmotionDance,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_MACRO,				&ChatCmd_Macro,				CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_SUICIDE,			&ChatCmd_Suicide,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_CALLVOTE,			&ChatCmd_Callvote,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_VOTEYES,			&ChatCmd_VoteYes,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_VOTENO,				&ChatCmd_VoteNo,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_KICK,				&ChatCmd_Kick,				CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_CREATE_CHATROOM,	&ChatCmd_CreateChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_JOIN_CHATROOM,		&ChatCmd_JoinChatRoom  ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_LEAVE_CHATROOM,		&ChatCmd_LeaveChatRoom ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_SELECT_CHATROOM,	&ChatCmd_SelectChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_INVITE_CHATROOM,	&ChatCmd_InviteChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_VISIT_CHATROOM,		&ChatCmd_VisitChatRoom ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_CHAT_CHATROOM,		&ChatCmd_ChatRoomChat  ,	CCF_ALL, ARGVNoMin, 1,false);
	_CC_ACX(CCMD_ID_MOUSE_SENSITIVITY,	&ChatCmd_MouseSensitivity,	CCF_GAME, ARGVNoMin, 1,true);

	// Custom: Commands
	_CC_ACX(CCMD_ID_FIND_PLAYER,		&ChatCmd_FindPlayer,		CCF_ALL, ARGVNoMin, 1, true);
	_CC_AC("tags",						&ChatCmd_Mods,				CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/tags", "Lists all room modifiers");
	_CC_AC("commands",					&ChatCmd_Commands,			CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/commands", "Lists all commands");
	_CC_AC("ignore",					&ChatCmd_IgnorePlayer,		CCF_ALL, ARGVNoMin, 1, true, "/ignore <charname>", "Ignores a player");
	_CC_AC("unignore",					&ChatCmd_UnIgnorePlayer,	CCF_ALL, ARGVNoMin, 1, true, "/unignore <charname>", "Removes a player from the ignore list");
	_CC_AC("ignorelist",				&ChatCmd_IgnoreList,		CCF_ALL, ARGVNoMin,	ARGVNoMax, true, "/ignorelist", "Display users in the ignore list");
	_CC_AC("nojjang",                   &ChatCmd_HideJjang,         CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/nojjang", "");
	_CC_AC("super", &ChatCmd_Ki, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/super", "");
	_CC_AC("clean", &ChatCmd_ClearChat, CCF_ALL, ARGVNoMin, 1, true, "/clean", "");
	_CC_AC("save", &ChatCmd_TeleSavePos, CCF_GAME, ARGVNoMin, 1, true, "/save", "");
	_CC_AC("back", &ChatCmd_TeleLoadPos, CCF_GAME, ARGVNoMin, 1, true, "/back", "");
	_CC_AC("exit", &ChatCmd_Exit, CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/exit", "");

#ifdef _REJOIN_NEW
	_CC_AC("rejoin", &ChatCmd_StageRejoin, CCF_LOBBY, ARGVNoMin, ARGVNoMax, true, "/rejoin", "");
#endif

#ifdef _REPORT
	_CC_AC("report", &ChatCmd_Report, CCF_ALL, ARGVNoMin, 2, true, "/report <name> <reason>", "");
#endif
#ifdef _PLAYERINO
	_CC_AC("perfil",                      &ChatCmd_InfoPlayer, CCF_STAGE, ARGVNoMin, 1, true, "/perfil <charname>", "");
#endif
#ifdef _RADIO
	_CC_AC("ra", &ChatCmd_RadioCommand, CCF_ALL, ARGVNoMin, 2, true, "/ra", "");
#endif
///////////////////////////////ADMIN COMMANDS/////////////////////
	_CC_AC("autoq", &ChatCmd_NpcShot, CCF_ADMIN | CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/autoq", "");
	_CC_AC("flip", &ChatCmd_Flipmower, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/flip", "");
	_CC_AC("masive", &ChatCmd_InsaneMassive, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/masive", "");
	_CC_AC("lawn", &ChatCmd_LawnMower, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/lawn", "");
	_CC_AC("spawn", &ChatCmd_Respawn, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/spawn", "");
	_CC_AC("enchant", &ChatCmd_Enchant, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/enchant", "");
	_CC_AC("shot", &ChatCmd_Shot, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/shot", "");
	_CC_AC("boom", &ChatCmd_Explosion, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/boom", "");
	_CC_AC("bomu", &ChatCmd_ExplosionUnder, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/bomu", "");
	_CC_AC("frag", &ChatCmd_GrenadeThrow, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/frag", "");
	_CC_AC("pos", &ChatCmd_Pos, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/pos", "");
	_CC_AC("admin_summon", &ChatCmd_AdminSummon, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_summon <charname>", "");
	_CC_AC("admin_goto", &ChatCmd_AdminGoTo, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_goto <charname>", "");
	_CC_AC("admin_slap", &ChatCmd_AdminSlap, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_slap <charname>", "");
	_CC_AC("admin_spawn", &ChatCmd_AdminSpawn, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_spawn <charname>", "");
	_CC_AC("admin_popup", &ChatCmd_AdminPopup, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_popup <msg>", "");
	_CC_AC("check_sb", &ChatCmd_AdminSBTest, CCF_ADMIN, ARGVNoMin, 1, true, "/check_sb <charname>", "");
	_CC_AC("admin_silence", &ChatCmd_AdminSilence, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_silence <minutes>", "");
	_CC_AC("pkick", &ChatCmd_AdminPKick, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, 1, true, "/pkick <player>", "");
	_CC_AC("unkick", &ChatCmd_AdminUKick, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, 1, true, "/unkick <player>", "");
	_CC_AC("admin_team", &ChatCmd_AdminTeam, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_team <red/blue>", "");
	_CC_AC("lobbychat", &ChatCmd_LobbyChat, CCF_ADMIN, ARGVNoMin, 1, true, "/lobbychat", "");
	_CC_AC("forcenat", &ChatCmd_AdminNAT, CCF_LOBBY |CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/forcenat", "Force NAT");
	_CC_AC("serverping", &ChatCmd_PingServer, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/serverping", "");
	_CC_AC("donate", &ChatCmd_GiveCoinsCash, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/donate", "");
	_CC_AC("ladder", &ChatCmd_GiveCoinsMedals, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/ladder", "");
	_CC_AC("send_item", &ChatCmd_Senditem, CCF_ADMIN | CCF_ALL, ARGVNoMin, 3, true, "/send_item <charname> <itemid> <rentdays>", "");
	_CC_AC("speed", &ChatCmd_AdminSpeed, CCF_ADMIN | CCF_GAME, ARGVNoMin, 1, true, "/speed", "");
	_CC_AC("esp", &ChatCmd_AdminEsp, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/esp", "");
	_CC_AC("kill", &ChatCmd_Killall, CCF_ADMIN | CCF_GAME, ARGVNoMin, 1, true, "/kill", "");
	_CC_AC("tp2", &ChatCmd_AdminTeleport2, CCF_ADMIN | CCF_GAME, ARGVNoMin, 1, true, "/tp2 <name>", "");
	_CC_AC("tp1", &ChatCmd_AdminTeleport, CCF_ADMIN | CCF_GAME, ARGVNoMin, 1, true, "/tp1 <player>", "");
	_CC_AC("tpall", &ChatCmd_AdminTeleportAll, CCF_ADMIN | CCF_GAME, ARGVNoMin, -1, true, "/tpall", "");
#ifdef _ASSITEN
	_CC_AC("aim_best", &ChatCmd_AdminAssitent, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/aim_best", "");
#endif
#ifdef _STAFFCHAT
	_CC_AC("staff", &ChatCmd_StaffChat, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, 1, true, "/staff <Msg>", "");
#endif
	_CC_AC("sta", &ChatCmd_Staff, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/sta", "");
	_CC_AC("stafflist", &ChatCmd_StaffLists, CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/stafflist", "Lists all staff");
#ifdef _GRADECHANGE
	_CC_AC("get_grade", &ChatCmd_GetGrade, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/get_grade", "");
#endif
	_CC_AC("admin_kick", &ChatCmd_AdminKickPlayer, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_kick <charname>", "");
	_CC_AC("admin_mute", &ChatCmd_AdminMutePlayer, CCF_ADMIN, ARGVNoMin, 2, true, "/admin_mute <charname> <due>", "");
	_CC_AC("admin_block", &ChatCmd_AdminBlockPlayer, CCF_ADMIN, ARGVNoMin, 2, true, "/admin_block <charname> <due>", "");
	_CC_AC("wa", &ChatCmd_AdminAnnounce, CCF_ADMIN, ARGVNoMin, 1, true, "/wa <∏ﬁΩ√¡ÅE", "");
	_CC_AC("changemaster", &ChatCmd_ChangeMaster, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/changemaster", "");
	_CC_AC("changepassword", &ChatCmd_ChangePassword, CCF_ALL | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/changepassword", "");
	_CC_AC("admin_hide", ChatCmd_AdminHide, CCF_STAGE | CCF_ADMIN | CCF_LOBBY, ARGVNoMin, ARGVNoMax, true, "/admin_hide", "");
	_CC_AC("jjang", &ChatCmd_RequestJjang, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/jjang", "");
	_CC_AC("removejjang", &ChatCmd_RemoveJjang, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/removejjang", "");
	_CC_AC("free", &ChatCmd_AdminFreeze, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/free <name>", "");
	_CC_AC("akick", &ChatCmd_AdminKickAll, CCF_ADMIN | CCF_STAGE | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/akick", "");
	_CC_AC("admin_reload_hash", &ChatCmd_AdminReloadClientHash, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reload_hash", "");
	_CC_AC("admin_switch_laddergame", &ChatCmd_AdminSwitchCreateLadderGame, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_switch_laddergame 1", "");
	_CC_AC("admin_reset_all_hacking_block", &ChatCmd_AdminResetAllHackingBlock, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reset_all_hacking_block", "");
	_CC_AC("admin_reload_gambleitem", &ChatCmd_AdminReloadGambleitem, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reload_gambleitem", "");
	_CC_AC("admin_dump_gambleitem_log", &ChatCmd_AdminDumpGambleitemLog, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_dump_gambleitem_log", "");
	_CC_AC("admin_commander", &ChatCmd_AdminAssasin, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/admin_commander", "");
	_CC_AC("pingtoall", &ChatCmd_AdminPingToAll, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/pingtoall", "");
	_CC_AC("server_halt", &ChatCmd_AdminServerHalt, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/server_halt", "");
	_CC_ALIAS("dc", "admin_kick");
	_CC_ALIAS("admin_stfu", "admin_silence");
#ifdef _CMD_ALL
	_CC_AC("crash", &ChatCmd_Crash, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/crash <player/all!>", "");
#endif
///////////////////////////////ADMIN COMMANDS/////////////////////


#ifdef _TESTCLANWAR
	//_CC_AC("ƒ≥∏Ø≈Õ¿ÅE€", &ChatCmd_CopyToTestServer, CCF_LOBBY, ARGVNoMin, 0, "/ƒ≥∏Ø≈Õ¿ÅE€", "ƒ≥∏Ø≈Õ ¡§∫∏∏¶ ≈◊Ω∫∆Æº≠πˆø° ∫πªÁ«’¥œ¥Ÿ.");
	_CC_AC("team", &ChatCmd_LadderInvite, CCF_LOBBY, ARGVNoMin, ARGVNoMax, true, "", "");
	_CC_AC("test", &ChatCmd_Test, CCF_ALL, ARGVNoMin, 1, true ,"/test <name>", "≈◊Ω∫∆Æ∏¶ ºˆ«‡«’¥œ¥Ÿ.");
	_CC_AC("laddertest", &ChatCmd_LadderTest, CCF_ADMIN|CCF_ALL, ARGVNoMin, ARGVNoMax, true ,"/laddertest", "∑°¥ı≈◊Ω∫∆Æ∏¶ ø‰√ª«’¥œ¥Ÿ.");
	_CC_AC("launchtest", &ChatCmd_LaunchTest, CCF_ADMIN|CCF_ALL, ARGVNoMin, ARGVNoMax, true ,"/laddertest", "Town");
#endif

	_CC_AC("go",		&ChatCmd_Go, CCF_LOBBY, ARGVNoMin, 1, true, "/go πÊπ¯»£", "∞‘¿”πÊ¿∏∑Œ πŸ∑Œ ¿Ãµø«’¥œ¥Ÿ.");

#ifndef _PUBLISH
{

	// æ∆¡ÅE¥Ÿ ±∏«ˆµ«¡ÅEæ æ“¿Ω
	_CC_AC("pf",		&ChatCmd_RequestPlayerInfo, CCF_LOBBY|CCF_STAGE, ARGVNoMin, 2, true ,"/pf <ƒ≥∏Ø≈Õ¿Ã∏ß>", "¥Ÿ∏• ªÁøÅE⁄¿« ¡§∫∏∏¶ ∫æ¥œ¥Ÿ.");
	_CC_ALIAS("§ƒ§©", "pf");
}
#endif

	// ≈◊Ω∫∆Æ ¿ÅEÅE
	_CC_AC("gtgod",			&ChatCmd_QUESTTEST_God,				CCF_TEST, ARGVNoMin, 1    , true,"/gtgod", "");
	_CC_AC("gtspn",			&ChatCmd_QUESTTEST_SpawnNPC,		CCF_TEST, ARGVNoMin, 2    , true,"/gtspn <NPC≈∏¿‘> <NPCºÅE", "");
	_CC_AC("gtclear",		&ChatCmd_QUESTTEST_NPCClear,		CCF_TEST, ARGVNoMin, 1    , true,"/gtclear", "");
	_CC_AC("gtreload",		&ChatCmd_QUESTTEST_Reload,			CCF_TEST, ARGVNoMin, 1    , true,"/gtreload", "");
	_CC_AC("gtsc",			&ChatCmd_QUESTTEST_SectorClear,		CCF_TEST, ARGVNoMin, 1    , true,"/gtsc", "");
	_CC_AC("gtfin",			&ChatCmd_QUESTTEST_Finish,			CCF_TEST, ARGVNoMin, 1    , true,"/gtfin", "");
	_CC_AC("gtlspn",		&ChatCmd_QUESTTEST_LocalSpawnNPC,	CCF_TEST, ARGVNoMin, 2    , true,"/gtlspn <NPC≈∏¿‘> <NPCºÅE", "");
	_CC_AC("gtweaknpcs",	&ChatCmd_QUESTTEST_WeakNPCs,		CCF_TEST, ARGVNoMin, 1    , true,"/gtweaknpcs", "");
}



void OutputCmdHelp(const char* cmd)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();
	ZChatCmd* pCmd = pCCM->GetCommandByName(cmd);
	if (pCmd == NULL) return;

	// Custom: Event Team
	if ( (pCmd->GetFlag() & CCF_ADMIN) && !ZGetMyInfo()->IsAdminGrade() && ZGetMyInfo()->GetUGradeID() != MMUG_EVENTTEAM)
		return;

#ifdef _EVENTCMD
	if ((pCmd->GetFlag() & CCF_EVENT) && !ZGetMyInfo()->IsEventMasterGrade())
		return;
#endif
    char szBuf[512];
	sprintf(szBuf, "%s: %s", pCmd->GetName(), pCmd->GetHelp());
	ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
}

void OutputCmdUsage(const char* cmd)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();
	ZChatCmd* pCmd = pCCM->GetCommandByName(cmd);
	if (pCmd == NULL) return;

	// Custom: Event Team
	if ( (pCmd->GetFlag() & CCF_ADMIN) && !ZGetMyInfo()->IsAdminGrade() && ZGetMyInfo()->GetUGradeID() != MMUG_EVENTTEAM)
		return;

#ifdef _EVENTCMD
	if ((pCmd->GetFlag() & CCF_EVENT) && !ZGetMyInfo()->IsEventMasterGrade())
		return;
#endif
    char szBuf[512];
	sprintf(szBuf, "%s: %s", ZMsg(MSG_WORD_USAGE), pCmd->GetUsage());
	ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
}

void OutputCmdWrongArgument(const char* cmd)
{
	// Custom: Made CMT_SYSTEM
	//ZChatOutput( ZMsg(MSG_WRONG_ARGUMENT), ZChat::CMT_SYSTEM );
	//OutputCmdUsage(cmd);
	ZChatOutput(ZMsg(MSG_WRONG_ARGUMENT));
	OutputCmdUsage(cmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ChatCmd_Help(const char* line, const int argc, char **const argv)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();

	char szBuf[1024] = "";

	//////////////////////////////////////////////////////////

	GunzState state = ZApplication::GetGameInterface()->GetState();

	if( state==GUNZ_GAME ) {
		// ¿⁄Ω≈¿« ƒ≥∏Ø≈Õ∞° ∑π∫ß¿Ã 1-10 ªÁ¿Ã¿Œ ∞ÊøÅE∏..
		if(ZGetMyInfo()) {
			if(ZGetMyInfo()->GetLevel() < 10) {
				if( ZGetGame() ) {
					ZGetGame()->m_HelpScreen.ChangeMode();
					return;
				}
			}
		}
	}

	if (argc == 1)
	{
		
		ZChatCmdFlag nCurrFlag = CCF_NONE;

		switch (state)
		{
			case GUNZ_LOBBY: nCurrFlag = CCF_LOBBY; break;
			case GUNZ_STAGE: nCurrFlag = CCF_STAGE; break;
			case GUNZ_GAME: nCurrFlag = CCF_GAME; break;
		}

		sprintf(szBuf, "%s: ", ZMsg(MSG_WORD_COMMANDS));

		int nCnt=0;
		int nCmdCount = pCCM->GetCmdCount();

		for (ZChatCmdMap::iterator itor = pCCM->GetCmdBegin(); itor != pCCM->GetCmdEnd(); ++itor)
		{
			nCnt++;
			ZChatCmd& Cmd = itor->second;

			if (Cmd.GetFlag() & CCF_ADMIN) continue;
			if (!(Cmd.GetFlag() & nCurrFlag)) continue;

			strcat_safe(szBuf, Cmd.GetName());

			if (nCnt != nCmdCount) strcat_safe(szBuf, ", ");
		}

		// æˆ√ª≥≠ «œµÂƒ⁄µÅE.. æ˚ﬁøºÅEæ¯¥Ÿ... -§∑-;
		switch (state)
		{
			case GUNZ_LOBBY:
				strcat( szBuf, "go");
				break;
			case GUNZ_STAGE:
				strcat( szBuf, "kick");
				break;
			case GUNZ_GAME:
				break;
		}

		ZChatOutput(szBuf, ZChat::CMT_SYSTEM);

		sprintf(szBuf, "%s: /h %s", ZMsg(MSG_WORD_HELP), ZMsg(MSG_WORD_COMMANDS));
		ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
	}
	else if (argc == 2)
	{
		OutputCmdHelp(argv[1]);
		OutputCmdUsage(argv[1]);
	}
}

void ChatCmd_Go(const char* line, const int argc, char **const argv)
{
	if (argc < 2) return;
	if (ZGetGameClient()->GetChannelType() == MCHANNEL_TYPE_CLAN) return;

	ZRoomListBox* pRoomList = (ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (pRoomList == NULL)
		return;

	int nRoomNo = atoi(argv[1]);

	ZPostStageGo(nRoomNo);
}

void ChatCmd_Whisper(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return;
	}


	char* pszSenderName = "Me";	// æ∆π´∞≈≥™ ∫∏≥ªµµ º≠πˆø°º≠ √§øˆ≥÷¿Ω

	char szName[512] = "";
	char szRName[512] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szName, szRName);

	if ( (int)strlen( pszMsg) > 127)
		return;

	// øÂ« ≈Õ∏µ
	if (!ZGetGameInterface()->GetChat()->CheckChatFilter(pszMsg)) return;

	//±”º”∏ª ƒ≥∏Ø≈Õ ¿Ã∏ß ±€¿⁄ ºÅE¡¶«—..jintriple3
	int nNameLen = (int)strlen(szName);
	if ( nNameLen < 1)		// Custom: Changed MIN_CHARNAME to 1 to fix whisper issues with short names.
	{
		const char *str = ZErrStr( MERR_TOO_SHORT_NAME );
		if(str)
		{
			char text[1024];
			sprintf(text, "%s (E%d)", str, MERR_TOO_SHORT_NAME);
			ZChatOutput(MCOLOR(96,96,168), text, ZChat::CL_CURRENT);
		}
	}
	// Custom: Changed > MAX_CHARNAME to 128, to fix whisper issues with long names.
	else if ( nNameLen > 128)		// ¿Ã∏ß¿Ã ¡¶«— ±€¿⁄ºˆ∏¶ ≥—æ˙¥Ÿ.
	{
		const char *str = ZErrStr( MERR_TOO_LONG_NAME );
		if(str)
		{
			char text[1024];
			sprintf(text, "%s (E%d)", str, MERR_TOO_LONG_NAME);
			ZChatOutput(MCOLOR(96,96,168), text, ZChat::CL_CURRENT);
		}
	}
	else
	{
		ZPostWhisper(pszSenderName, szName, pszMsg);

		// loop back
		char szMsg[512];
		sprintf(szMsg, "(To %s) : %s", szRName, pszMsg);	//jintriple3 ¿Ø¿ÅE≥◊¿”¿∫ ±◊¥ÅEŒ √‚∑¬µ«µµ∑œ...
		ZChatOutput(MCOLOR(96,96,168), szMsg, ZChat::CL_CURRENT);
	}
}

void ChatCmd_CreateChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return;
	}

	char* pszCharName = argv[1];

	if( !MGetChattingFilter()->IsValidStr( pszCharName, 1) ){
		char szMsg[ 256 ];
		ZTransMsg( szMsg, MSG_WRONG_WORD_NAME, 1, MGetChattingFilter()->GetLastFilteredStr().c_str());
		ZApplication::GetGameInterface()->ShowMessage( szMsg, NULL, MSG_WRONG_WORD_NAME );
	}
	else
	{
		ZChatOutput( 
			ZMsg(MSG_LOBBY_REQUESTING_CREATE_CHAT_ROOM), 
			ZChat::CMT_SYSTEM );

		ZPostChatRoomCreate(ZGetMyUID(), pszCharName);
	}
}
void ChatCmd_JoinChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_REQUESTING_JOIN_CAHT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszChatRoomName = argv[1];

	ZPostChatRoomJoin(pszChatRoomName);
}

void ChatCmd_LeaveChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_LEAVE_CHAT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszRoomName = argv[1];

	ZPostChatRoomLeave(pszRoomName);
}

void ChatCmd_SelectChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_CHOICE_CHAT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszRoomName = argv[1];

	ZPostSelectChatRoom(pszRoomName);
}

unsigned long spamtimer;
void ChatCmd_InviteChatRoom(const char* line, const int argc, char **const argv)
{
	if(timeGetTime() - spamtimer > 2000) // Custom: 2 second delay on spamming /chatinvite. thxmaiet
		{
			if (argc < 2) 
			{
				OutputCmdWrongArgument(argv[0]);
				return;
			}
	
			if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
			{
				// Custom: Made CMT_SYSTEM
				ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
				return;
			}
	
			char* pszPlayerName = argv[1];
		
			char szLog[128];
			
			ZTransMsg( szLog, MSG_LOBBY_INVITATION, 1, pszPlayerName );
		
			ZChatOutput(szLog, ZChat::CMT_SYSTEM);
		
			ZPostInviteChatRoom(pszPlayerName);
			spamtimer = timeGetTime();
		}

}

void ChatCmd_VisitChatRoom(const char* line, const int argc, char **const argv)
{
	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return;
	}

	char* pszRoomName = const_cast<char*>(ZGetGameClient()->GetChatRoomInvited());
	ZPostChatRoomJoin(pszRoomName);
}

void ChatCmd_ChatRoomChat(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return;
	}

	char* pszMsg = argv[1];

	// øÂ« ≈Õ∏µ
	if (!ZGetGameInterface()->GetChat()->CheckChatFilter(pszMsg)) return;

	ZPostChatRoomChat(pszMsg);
}


void ChatCmd_CopyToTestServer(const char* line, const int argc, char **const argv)
{
	// ªÁøÅEœ¡ÅEæ ¥¬¥Ÿ. - ≈◊Ω∫∆Æ º≠πˆ∑Œ ¡§∫∏ ∫πªÅE
	return;


	if (argc != 1) return;

	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();

#define DELAY_POST_COPY_TO_TESTSERVER		(1000 * 60)		// 5∫– µÙ∑π¿Ã

	if ((nNowTime - st_nLastTime) > DELAY_POST_COPY_TO_TESTSERVER)
	{
		ZPostRequestCopyToTestServer(ZGetGameClient()->GetPlayerUID());

		st_nLastTime = nNowTime;
	}
}


void ChatCmd_StageFollow(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostStageFollow(pszTarget);
}

void ChatCmd_Friend(const char* line, const int argc, char **const argv)
{
	//// ZFriendCommandHelper ////
	class ZFriendCommandHelper {
	public:
		enum ZFRIENDCMD {
			ZFRIENDCMD_ADD,
			ZFRIENDCMD_REMOVE,
			ZFRIENDCMD_LIST,
			ZFRIENDCMD_MSG,
			ZFRIENDCMD_UNKNOWN
		};
		ZFRIENDCMD GetSubCommand(const char* pszCmd) {
			if (stricmp(pszCmd, "Add") == 0)
				return ZFRIENDCMD_ADD;
			else if (stricmp(pszCmd, "√ﬂ∞°") == 0)
				return ZFRIENDCMD_ADD;
			else if (stricmp(pszCmd, "Remove") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (stricmp(pszCmd, "ªË¡¶") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (stricmp(pszCmd, "list") == 0)
				return ZFRIENDCMD_LIST;
			else if (stricmp(pszCmd, "∏Ò∑œ") == 0)
				return ZFRIENDCMD_LIST;
			else if (stricmp(pszCmd, "msg") == 0)
				return ZFRIENDCMD_MSG;
			else if (stricmp(pszCmd, "√§∆√") == 0)
				return ZFRIENDCMD_MSG;
			else return ZFRIENDCMD_UNKNOWN;
		}
	} friendHelper;

	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char szSubCmd[256] = "";
	char szArg[256] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szSubCmd);
	
	//// Sub Command Handler ////
	switch(friendHelper.GetSubCommand(szSubCmd)) {
	case ZFriendCommandHelper::ZFRIENDCMD_ADD:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendAdd(szArg);
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_REMOVE:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendRemove(szArg);
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_LIST:
		{
			ZPostFriendList();
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_MSG:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendMsg(szArg);
		}
		break;
	default:
		OutputDebugString("Unknown Friend Command \n");
		break;
	};
}

void ChatCmd_Clan(const char* line, const int argc, char **const argv)
{
	

	//// ZClanCommandHelper ////
	class ZClanCommandHelper {
	public:
		enum ZCLANCMD {
			ZCLANCMD_CREATE,		// ≈¨∑£ ª˝º∫
			ZCLANCMD_CLOSE,			// ≈¨∑£ ∆ÛºÅE
			ZCLANCMD_JOIN,
			ZCLANCMD_LEAVE,
			ZCLANCMD_EXPEL_MEMBER,	// ∞≠¡¶≈ª≈ÅE
			ZCLANCMD_LIST,
			ZCLANCMD_MSG,
			
			ZCLANCMD_CHANGE_GRADE,	// ∏‚πÅE±««— ∫Ø∞ÅE

			ZCLANCMD_UNKNOWN
		};
		ZCLANCMD GetSubCommand(const char* pszCmd) {
			GunzState nGameState = ZApplication::GetGameInterface()->GetState();

			if ((stricmp(pszCmd, "ª˝º∫") == 0) || (stricmp(pszCmd, "open") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CREATE;
			}
			else if ((stricmp(pszCmd, "∆ÛºÅE") == 0) || (stricmp(pszCmd, "«ÿ√º") == 0) || (stricmp(pszCmd, "close") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CLOSE;
			}
			else if ( (stricmp(pszCmd, "√ ¥ÅE") == 0) || (stricmp(pszCmd, "invite") == 0) )
			{
				if (nGameState == GUNZ_LOBBY || nGameState == GUNZ_STAGE) return ZCLANCMD_JOIN;
			}
			else if ( (stricmp(pszCmd, "≈ª≈ÅE") == 0) || (stricmp(pszCmd, "leave") == 0) )
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_LEAVE;
			}
			else if ( (stricmp(pszCmd, "±««—∫Ø∞ÅE") == 0) || (stricmp(pszCmd, "promote") == 0) )
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CHANGE_GRADE;
			}
			else if ((stricmp(pszCmd, "∞≠¡¶≈ª≈ÅE") == 0) || (stricmp(pszCmd, "πÊ√ÅE") == 0) || (stricmp(pszCmd, "dismiss") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_EXPEL_MEMBER;
			}
			else if ((stricmp(pszCmd, "list") == 0) || (stricmp(pszCmd, "∏Ò∑œ") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_LIST;
			}
			else if ((stricmp(pszCmd, "msg") == 0) || (stricmp(pszCmd, "√§∆√") == 0))
			{
				return ZCLANCMD_MSG;
			}
			
			return ZCLANCMD_UNKNOWN;
		}
	} clanHelper;

	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char szSubCmd[256] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szSubCmd);
	
	//// Sub Command Handler ////
	switch(clanHelper.GetSubCommand(szSubCmd)) {
	case ZClanCommandHelper::ZCLANCMD_CREATE:
		{
			// clan ª˝º∫ ≈¨∑£¿Ã∏ß πﬂ±‚¿Œ1 πﬂ±‚¿Œ2 πﬂ±‚¿Œ3 πﬂ±‚¿Œ4
			if (argc < 7)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			ZGetGameClient()->RequestCreateClan(argv[2], &argv[3]);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_CLOSE:
		{
			// clan ∆ÛºÅE≈¨∑£¿Ã∏ß
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (ZGetMyInfo()->GetClanGrade() != MCG_MASTER)
			{
				ZChatOutput( 
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER), 
					ZChat::CMT_SYSTEM );
				break;
			}

			// ≈¨∑£¿Ã∏ß »Æ¿Œ
			if (stricmp(ZGetMyInfo()->GetClanName(), argv[2]))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_WRONG_CLANNAME), 
					ZChat::CMT_SYSTEM );
				break;
			}

			ZApplication::GetGameInterface()->ShowConfirmMessage(ZMsg(MSG_CLAN_CONFIRM_CLOSE), ZGetClanCloseConfirmListenter()	);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_JOIN:
		{
			// clan √ ¥ÅE∞°¿‘¿⁄¿Ã∏ß
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_ADMIN))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER_AND_ADMIN), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szClanName[256];
			strcpy(szClanName, 	ZGetMyInfo()->GetClanName());
			ZPostRequestJoinClan(ZGetGameClient()->GetPlayerUID(), szClanName, argv[2]);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_LEAVE:
		{
			// clan ≈ª≈ÅE
			if (argc < 2)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			// ∏∂Ω∫≈Õ¥¬ ≈ª≈∞° æ»µ»¥Ÿ.
			if (IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_MASTER))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_MASTER_CANNOT_LEAVED), 
					ZChat::CMT_SYSTEM);
				break;
			}

			ZApplication::GetGameInterface()->ShowConfirmMessage(
				ZMsg(MSG_CLAN_CONFIRM_LEAVE), 
				ZGetClanLeaveConfirmListenter() );
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_CHANGE_GRADE:
		{
			// clan ±««—∫Ø∞ÅE∏‚πˆ¿Ã∏ß ±««—¿Ã∏ß
			if (argc < 4)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_MASTER))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szMember[256];
			int nClanGrade = 0;

			strcpy(szMember, argv[2]);
			if ((strlen(szMember) < 0) || (strlen(szMember) > CLAN_NAME_LENGTH))
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if ((!stricmp(argv[3], "≈¨∑£ø˚€µ¿⁄")) || (!stricmp(argv[3], "ø˚€µ¿⁄")) || (!stricmp(argv[3], "øµ¿⁄")) || (!stricmp(argv[3], "admin")))
			{
				nClanGrade = (int)MCG_ADMIN;
			}
			else if ((!stricmp(argv[3], "≈¨∑£∏‚πÅE")) || (!stricmp(argv[3], "∏‚πÅE")) || (!stricmp(argv[3], "≈¨∑£øÅE")) || (!stricmp(argv[3], "member")))
			{
				nClanGrade = (int)MCG_MEMBER;
			}
			else
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}


			ZPostRequestChangeClanGrade(ZGetGameClient()->GetPlayerUID(), szMember, nClanGrade);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_EXPEL_MEMBER:
		{
			// clan ∞≠¡¶≈ª≈ÅE≈¨∑£∏‚πÅE
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_ADMIN))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER_AND_ADMIN), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szMember[256];
			int nClanGrade = 0;

			strcpy(szMember, argv[2]);
			if ((strlen(szMember) < 0) || (strlen(szMember) > CLAN_NAME_LENGTH))
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			ZPostRequestExpelClanMember(ZGetGameClient()->GetPlayerUID(), szMember);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_LIST:
		{

		}
		break;
	case ZClanCommandHelper::ZCLANCMD_MSG:
		{
			if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
			{
				// Custom: Made CMT_SYSTEM
				ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
				break;
			}

			// clan msg «œ∞˙ŸÕ¿∫∏ª
			MLex lex;
			
			char szLine[512], szTemp1[256] = "", szTemp2[256] = "";
			strcpy(szLine, line);

			char* pszMsg = lex.GetTwoArgs(szLine, szTemp1, szTemp2);

			ZPostClanMsg(ZGetGameClient()->GetPlayerUID(), pszMsg);
		}
		break;
	default:
		// Custom: Changed clan sub command error message
		OutputCmdWrongArgument(argv[0]);
		// Custom: Made CMT_SYSTEM
		//ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		break;
	};
}

void ChatCmd_RequestQuickJoin(const char* line, const int argc, char **const argv)
{
	ZGetGameInterface()->RequestQuickJoin();
}

void ChatCmd_Report119(const char* line, const int argc, char **const argv)
{
	ZPostLocalReport119();
}

void ChatCmd_AdminKickPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];
	/*if (stricmp(pszPlayerName, "all")==0)
	{
		ZPostAdminKickAll();
	}
	else
	{*/
		ZPostAdminRequestKickPlayer(pszPlayerName);
	//}
}

int GetDueHour(char* pszDue)
{
	int nLength = (int)strlen(pszDue);

	for(int i = 0; i < nLength - 1; i++ ) {
		if( pszDue[i] > '9' || pszDue[i] < '0') {
			return -1;
		}
	}

	int nDueType = toupper(pszDue[nLength - 1]);
	if( nDueType == toupper('d') ) {
		int nDay = atoi(pszDue);
		if( nDay < 365 * 10)	return nDay * 24;
		else					return -1;		
	} 
	else if( nDueType == toupper('h')) {
		int nHour = atoi(pszDue);
		return nHour;
	} 
	else {
		return -1;
	}
}

void ChatCmd_AdminMutePlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 3) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];

	int nDueHour = GetDueHour(argv[2]);
	if( nDueHour < 0 ) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

#ifdef _DEBUG
	mlog("Request Mute on Player(%s) While %d Hour\n", pszPlayerName, nDueHour);
#endif

	ZPostAdminRequestMutePlayer(pszPlayerName, nDueHour);
}

// Custom: admin_kickall command.
void ChatCmd_AdminKickAll(const char* line, const int argc, char **const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade() && ZGetMyInfo()->GetUGradeID() != MMUG_EVENTTEAM) return;

	ZPostAdminKickAll(ZGetGameClient()->GetPlayerUID());
}

void ChatCmd_AdminBlockPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 3) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];

	int nDueHour = GetDueHour(argv[2]);
	if( nDueHour < 0 ) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

#ifdef _DEBUG
	mlog("Request Block on Player(%s) While %d Hour\n", pszPlayerName, nDueHour);
#endif

	ZPostAdminRequestBlockPlayer(pszPlayerName, nDueHour);
}

void ChatCmd_AdminPingToAll(const char* line, const int argc, char **const argv)
{
	ZPostAdminPingToAll();
}

void ChatCmd_AdminReloadClientHash(const char* line, const int argc, char **const argv)
{
	ZPostAdminReloadClientHash();
}


void ChatCmd_AdminResetAllHackingBlock( const char* line, const int argc, char **const argv )
{
	ZPostAdminResetAllHackingBlock();
}

void ChatCmd_AdminReloadGambleitem( const char* line, const int argc, char **const argv )
{
	ZPostAdminReloadGambleItem();
}


void ChatCmd_AdminDumpGambleitemLog( const char* line, const int argc, char **const argv )
{
	ZPostAdminDumpGambleItemLog();
}

void ChatCmd_AdminAssasin( const char* line, const int argc, char **const argv )
{
	ZPostAdminAssasin();
}


void ChatCmd_ChangeMaster(const char* line, const int argc, char **const argv)
{
	ZPostChangeMaster();
}

void ChatCmd_ChangePassword(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszPassword = argv[1];

	ZPostChangePassword(pszPassword);
}

void ChatCmd_AdminHide(const char* line, const int argc, char **const argv)
{
	ZPostAdminHide();
}

void ChatCmd_RequestJjang(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTargetName = argv[1];

	ZPostAdminRequestJjang(pszTargetName);
}

void ChatCmd_RemoveJjang(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTargetName = argv[1];

	ZPostAdminRemoveJjang(pszTargetName);
}

void ChatCmd_Test(const char* line, const int argc, char **const argv)
{
	ZChatOutput("Testing...", ZChat::CMT_SYSTEM);
	
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszCharName = argv[1];

	ZGameClient* pClient = ZGetGameClient();
	MMatchPeerInfoList* pList = pClient->GetPeers();
	for (MMatchPeerInfoList::iterator i=pList->begin(); i!= pList->end(); i++) {
		MMatchPeerInfo* pInfo = (*i).second;
		if(stricmp(pInfo->CharInfo.szName, pszCharName) == 0) {
			MCommand* pCmd = pClient->CreateCommand(MC_TEST_PEERTEST_PING, pInfo->uidChar);
			pClient->Post(pCmd);
		}
	}
}

void ChatCmd_Macro(const char* line, const int argc, char **const argv)
{
// config ø° µ˚”œ ¿˙¿ÅE- ≈∞¿‘∑¬ ¥©∏¶∂ß√≥∑≥
// 
	if(argc != 3)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	int mode = -1;

		 if( stricmp(argv[1], "1")==0 ) mode = 0;
	else if( stricmp(argv[1], "2")==0 ) mode = 1;
	else if( stricmp(argv[1], "3")==0 ) mode = 2;
	else if( stricmp(argv[1], "4")==0 ) mode = 3;
	else if( stricmp(argv[1], "5")==0 ) mode = 4;
	else if( stricmp(argv[1], "6")==0 ) mode = 5;
	else if( stricmp(argv[1], "7")==0 ) mode = 6;
	else if( stricmp(argv[1], "8")==0 ) mode = 7;
	else if( stricmp(argv[1], "9")==0 ) mode = 8;
	else 
		return;

	ZCONFIG_MACRO* pMacro = NULL;

	if(ZGetConfiguration())
		pMacro = ZGetConfiguration()->GetMacro();
	
	if( pMacro && argv[2] ) {
		strcpy( pMacro->szMacro[mode],argv[2] );
		ZGetConfiguration()->Save( Z_LOCALE_XML_HEADER);
	}
}

void ChatCmd_EmotionTaunt(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_TAUNT );
}

void ChatCmd_EmotionBow(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_BOW );
}

void ChatCmd_EmotionWave(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_WAVE );
}

void ChatCmd_EmotionLaugh(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_LAUGH );
}

void ChatCmd_EmotionCry(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_CRY );
}

void ChatCmd_EmotionDance(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_DANCE );
}


void ChatCmd_RequestPlayerInfo(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZPostRequestCharInfoDetail(ZGetGameClient()->GetPlayerUID(), argv[1]);
}

void ChatCmd_AdminAnnounce(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szMsg[256];
	strcpy(szMsg, argv[1]);
	ZPostAdminAnnounce(ZGetGameClient()->GetPlayerUID(), szMsg, ZAAT_CHAT);
}

void ChatCmd_AdminServerHalt(const char* line, const int argc, char **const argv)
{
	ZPostAdminHalt(ZGetGameClient()->GetPlayerUID());
}

void ChatCmd_AdminSwitchCreateLadderGame(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	bool bEnabled = true;
	if (!strcmp(argv[1], "0")) bEnabled = false;

	ZPostAdminRequestSwitchLadderGame(ZGetGameClient()->GetPlayerUID(), bEnabled);
}

void ChatCmd_Suicide(const char* line,const int argc, char **const argv)
{
	ZGetGameClient()->RequestGameSuicide();
}


void ChatCmd_LadderInvite(const char* line,const int argc, char **const argv)
{
	if (argc < 3) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	// ≈◊Ω∫∆Æ∑Œ øÅE± 2∏˙‹Ã «—∆¿
	char szNames[2][256];
	strcpy(szNames[0], argv[1]);
	strcpy(szNames[1], argv[2]);

	ZGetGameClient()->RequestProposal(MPROPOSAL_LADDER_INVITE, &argv[1], 2);
}

void ChatCmd_LadderTest(const char* line,const int argc, char **const argv)
{
	// ªÁøÅEœ¥¬ ∫Œ∫–¿Ã æ¯æ˚ÿ≠ µπˆ±◊øÅE∏∑Œ ºˆ¡§«‘. -by SungE 2007-04-02
#ifdef _DEBUG
	if (argc == 1)
	{
		char szPlayerName[MATCHOBJECT_NAME_LENGTH];
		strcpy(szPlayerName, ZGetMyInfo()->GetCharName());
		char* pName[1];
		pName[0] = szPlayerName;

//		ZPostLadderRequestChallenge(ppMember, nCount, 0, 0);
	} else if (argc == 2)
	{
		char szPlayerName[MATCHOBJECT_NAME_LENGTH], szTeamMember1[MATCHOBJECT_NAME_LENGTH];
		strcpy(szPlayerName, ZGetMyInfo()->GetCharName());
		strcpy(szTeamMember1, argv[1]);

		char*pName[2];
		pName[0] = szPlayerName;
		pName[1] = szTeamMember1;

//		ZPostLadderRequestChallenge(ppMember, nCount, 0, 0);
	}
#endif
}

void ChatCmd_LaunchTest(const char* line,const int argc, char **const argv)
{
	// ªÁøÅEœ¥¬ ∫Œ∫–¿Ã æ¯æ˚ÿ≠ µπˆ±◊øÅE∏∑Œ ºˆ¡§«‘. -by SungE 2007-04-02
#ifdef _TESTCLANWAR
	MCommand* pCmd = ZGetGameClient()->CreateCommand(MC_MATCH_LADDER_LAUNCH, ZGetMyUID());
	pCmd->AddParameter(new MCmdParamUID(MUID(0,0)));
	ZGetGameClient()->Post(pCmd);
#endif
}

void ChatCmd_Callvote(const char* line,const int argc, char **const argv)
{
	if ( (argv[1] == NULL) || (argv[2] == NULL) )
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZPOSTCMD2(MC_MATCH_CALLVOTE, MCmdParamStr(argv[1]), MCmdParamStr(argv[2]))
}

void ChatCmd_VoteYes(const char* line,const int argc, char **const argv)
{
	ZPOSTCMD0(MC_MATCH_VOTE_YES);
}

void ChatCmd_VoteNo(const char* line,const int argc, char **const argv)
{
	ZPOSTCMD0(MC_MATCH_VOTE_NO);
}

void ChatCmd_Kick(const char* line,const int argc, char **const argv)
{
	ZGetCombatInterface()->GetVoteInterface()->CallVote("kick");
}

void ChatCmd_MouseSensitivity(const char* line,const int argc, char **const argv)
{
	if (argc == 1) 
	{
		ZChatOutputMouseSensitivityCurrent( ZGetConfiguration()->GetMouseSensitivityInInt());
	}
	else if (argc == 2)
	{
		// ∞®µµ º≥¡§
		int original = ZGetConfiguration()->GetMouseSensitivityInInt();

		char* szParam = argv[1];
		int asked = atoi(szParam);
        int changed = ZGetConfiguration()->SetMouseSensitivityInInt(asked);

		ZChatOutputMouseSensitivityChanged(original, changed);
	}
	else
		OutputCmdWrongArgument(argv[0]);
}

void ChatCmd_FindPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostWhere(pszTarget);
}
void ChatCmd_AdminSpeed(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}
	if (!ZGetGame())
	{
		ZChatOutput("This command can only be used in-game!", ZChat::CMT_SYSTEM);
		return;
	}

	char szMsg[256];

	if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSpeed == 0)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSpeed = 1;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		sprintf(szMsg, "^2Admin Speed Mode: Activated");
	}
	else if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSpeed == 1)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSpeed = 0;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		sprintf(szMsg, "^1Admin Speed Mode: Deactivated");
	}
	ZChatOutput(szMsg);

}
void ChatCmd_Mods(const char* line, const int argc, char **const argv)
{
#define zco(x) ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), x );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_LOBBY_DEFALT), "> Staff List Streak" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), ">> Place these modifiers in your room name, Only in Channel Rooms" );
	zco( "* [LEAD] - Disables the Anti-Lead system" );
	zco("* [TELE] - Save position NUMPAD 1 - Load position NUMPAD 2");
	zco( "* [V] - Vanilla Mode. HP/AP is capped to default (125/100)." )
	zco( "* [R] / [IA] - Instant reloading / Infinite Ammo" )
	zco( "* [EX] - Ignore explosion damage, flashbangs and smoke grenades.")
	zco( "* [FPS] - FPS mode. Press shift to sprint!" )
	zco( "* [DMG2] / [DMG3] - Damage Multiplier for 'Infected' mode." )
	zco( "* [GLA] - Onle Melee")
	zco( "* [SGO] / [SNO] / [RVO] - Shotguns only / Snipers only / Revolvers only" )
	zco( "* [RTD] - Roll The Dice mode." )
	zco( "  -----------------NEW ROOMS ADD-----------------");
	zco( "* [VAMP] / [PAINT] Mode Vampire / Paint Mode.");
	zco( "* [F] - No Flip.")
	zco( "* [M] / [N] - No Massive / Ninja Jump")
	zco( "* [WIW] - No Spawn World Items, HP, AP and Bullets!")
	zco( "* [G ] - [S ] - [S ] Gravity / Speed / Jump!")
#undef zco
}
void ChatCmd_StaffLists(const char* line, const int argc, char** const argv)
{
#define zco(x) ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), x );
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ">> Gunz Personal Equipment List.");
	zco("* Emerald - Owner");
	zco("* Fantastic - Owner 2do)");
	zco("* Desperate - Developer (Client)");


#undef zco
}
void ChatCmd_Commands(const char* line, const int argc, char **const argv)
{
	static unsigned long int st_nLastTime = 0;

	if ((timeGetTime() - st_nLastTime) < 5000) {
		char Warning[128];
		sprintf(Warning, "[Warning]: Wait 5 seconds to use again...");
		ZChatOutput(MCOLOR(0xFFFFFF00), Warning);
		return;
	}
	st_nLastTime = timeGetTime();

	if (ZGetGame()) {
		ZGetGameInterface()->GetCombatInterface()->UpdatePlayerCommands("");
		return;
	}
	else 
	{
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_LOBBY_DEFALT), "In-Game Commands" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /extra - check HP/AP given damage and taken damage." );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /mods - View our room modifiers (Room Tags)" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /find <player> - Finds a player" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST),  "** /nojjang - Enable or Disable Jjang");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /stafflist - List Staff");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /ignore <player> - Ignore a player" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /rejoin (number stage) - rejoin match clan war.");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /save - save your position. Only work roomtag tele].");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /back - restore you position. Only work roomtag [tele].");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /unignore <player> - Unignore a player" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /ignorelist - View ignore list" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /clear - Clean Chat old chat");
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /clean - Clean Chat new chat");
	}
}

void ChatCmd_IgnorePlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	if( strlen( pszTarget ) > 32 )
		return;

	if( ZGetGameClient()->GetIgnoreListCount() >= 64 )
	{
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), "The ignore list is full." );
		return;
	}

	if( !stricmp( pszTarget, ZGetMyInfo()->GetCharName() ) )
	{
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), "You can't ignore yourself!" );
		return;
	}

	char szBuffer[256];

	if( ZGetGameClient()->IsUserIgnored( pszTarget ) )
	{
		sprintf( szBuffer, "'%s' is already ignored.", pszTarget );
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), szBuffer );
		return;
	}

	sprintf( szBuffer, "'%s' has been ignored.", pszTarget );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), szBuffer );

	ZGetGameClient()->SetIgnoreUser( pszTarget, true );
}

void ChatCmd_UnIgnorePlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	if( strlen( pszTarget ) > 32 )
		return;

	if( !stricmp( pszTarget, ZGetMyInfo()->GetCharName() ) )
	{
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), "You can't un-ignore yourself!" );
		return;
	}

	char szBuffer[256];

	if( !ZGetGameClient()->IsUserIgnored( pszTarget ) )
	{
		sprintf( szBuffer, "'%s' is not being ignored.", pszTarget );
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), szBuffer );
		return;
	}

	sprintf( szBuffer, "'%s' has been removed from the ignore list.", pszTarget );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM), szBuffer );

	ZGetGameClient()->SetIgnoreUser( pszTarget, false );
}

void ChatCmd_IgnoreList(const char* line, const int argc, char **const argv)
{
	list<string> lIgnore = ZGetGameClient()->GetIgnoreList();

	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "Ignore List:" );

	if( lIgnore.size() == 0 )
	{
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "> No users in the ignore list" );
		return;
	}

	int i = 1;
	for( list<string>::iterator it = lIgnore.begin(); it != lIgnore.end(); ++it, ++i )
	{
		char szBuf[128];
		sprintf( szBuf, "> [%d] %s", i, (*it).c_str() );
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), szBuf );
	}
}
void ChatCmd_InfoPlayer(const char* line, const int argc, char** const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTarget = argv[1];
#ifdef _PLAYERINO
	ZPostPlayerInformation(pszTarget);
#endif
}
void ChatCmd_AdminSummon(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostAdminSummon(ZGetGameClient()->GetPlayerUID(), pszTarget);
}


void ChatCmd_AdminFreeze(const char* line, const int argc, char **const argv)
{
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTarget = argv[1];
	ZPostAdminFreeze(ZGetGameClient()->GetPlayerUID(),pszTarget);
}

void ChatCmd_AdminGoTo(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostAdminGoTo(ZGetGameClient()->GetPlayerUID(), pszTarget);
}

void ChatCmd_AdminSlap(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostAdminSlap(ZGetGameClient()->GetPlayerUID(), pszTarget);
}

void ChatCmd_AdminSpawn(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostAdminSpawn(ZGetGameClient()->GetPlayerUID(), pszTarget);
}

void ChatCmd_AdminPopup(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szMsg[256];
	strcpy(szMsg, argv[1]);
	ZPostAdminAnnounce(ZGetGameClient()->GetPlayerUID(), szMsg, ZAAT_MSGBOX);
}

void ChatCmd_AdminSilence(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	int nDurationMin = atoi(argv[1]);
	ZPostAdminSilence(ZGetGameClient()->GetPlayerUID(), nDurationMin);
}

void ChatCmd_AdminPKick(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szTarget[256];
	strcpy(szTarget, argv[1]);
	ZPostAdminPKick(ZGetGameClient()->GetPlayerUID(), szTarget, true);
}

void ChatCmd_AdminUKick(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szTarget[256];
	strcpy(szTarget, argv[1]);
	ZPostAdminPKick(ZGetGameClient()->GetPlayerUID(), szTarget, false);
}

void ChatCmd_AdminTeam(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTeam = argv[1];

	if (stricmp(pszTeam, "red") && stricmp(pszTeam, "blue"))
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	int nTeam = stricmp(pszTeam, "red") == 0 ? MMT_RED : MMT_BLUE;
	ZPostAdminTeam(ZGetGameClient()->GetPlayerUID(), nTeam);
}

// Custom: Lobby Chat toggle
void ChatCmd_LobbyChat(const char* line, const int argc, char **const argv)
{
	ZGetGameClient()->m_LobbyChat = !ZGetGameClient()->m_LobbyChat;
	ZChatOutput(ZGetGameClient()->m_LobbyChat == true ? "^2Lobby chat will now display." : "^2Lobby chat will no longer display.");
}

void ChatCmd_AdminNAT(const char* line, const int argc, char **const argv)
{
	if (ZGetGameInterface()->GetState() == GUNZ_LOBBY)
	{
		ZGetGameClient()->m_bAdminNAT = !ZGetGameClient()->m_bAdminNAT;
		//ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), "This command can only be used in the lobby.");
		//ZChatOutput(ZGetGameClient()->m_bAdminNAT == true ? "^2Local admin NAT enabled." : "^2Local admin NAT disabled.");
		ZPostAdminNAT();
	}
	else
	{
		ZChatOutput("This command can only be used in the lobby.", ZChat::CMT_SYSTEM);
	}
}

// ƒ˘Ω∫∆Æ ≈◊Ω∫∆ÆøÅE∏˙”…æÅE/////////////////////////////////////////////////////
void ChatCmd_QUESTTEST_God(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	bool bNowGod = ZGetQuest()->GetCheet(ZQUEST_CHEET_GOD);
	bNowGod = !bNowGod;

	ZGetQuest()->SetCheet(ZQUEST_CHEET_GOD, bNowGod);

	if (bNowGod)
	{
		ZChatOutput( "God mode enabled" );
	}
	else
	{
		ZChatOutput( "God mode disabled" );
	}
#endif
}


void ChatCmd_QUESTTEST_SpawnNPC(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;
	if (argc < 2) return;

	int nNPCID = 0;
	int nCount = 1;

	nNPCID = atoi(argv[1]);
	if(argv[2])
		nCount = atoi(argv[2]);

	ZPostQuestTestNPCSpawn(nNPCID, nCount);
#endif
}



void ChatCmd_QUESTTEST_LocalSpawnNPC(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	int nNPCID = 0;
	int nCount = 1;

	nNPCID = atoi(argv[1]);
	if(argv[2]) nCount = atoi(argv[2]);

	MCommand* pCmd = ZNewCmd(MC_QUEST_NPC_LOCAL_SPAWN);
	pCmd->AddParameter(new MCmdParamUID(ZGetMyUID()));
	
	MUID uidLocal;
	uidLocal.High = 10000;
	uidLocal.Low = (unsigned long)ZGetObjectManager()->size();

	pCmd->AddParameter(new MCmdParamUID(uidLocal));
	pCmd->AddParameter(new MCmdParamUChar((unsigned char)nNPCID));
	pCmd->AddParameter(new MCmdParamUChar((unsigned char)0));

	ZPostCommand(pCmd);
#endif
}


void ChatCmd_QUESTTEST_NPCClear(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestClearNPC();
#endif
}


void ChatCmd_QUESTTEST_Reload(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG

	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;

	ZGetObjectManager()->ClearNPC();	// ∏’¿ÅENPC∞° ≈¨∏Ææ˚—«æ˚⁄ﬂ µ⁄≈ª¿Ã æ¯¥Ÿ.
	ZGetQuest()->Reload();

	ZChatOutput( "Reloaded" );
#endif
}


void ChatCmd_QUESTTEST_SectorClear(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestSectorClear();
#endif
}

void ChatCmd_QUESTTEST_Finish(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestFinish();
#endif
}

void ChatCmd_QUESTTEST_WeakNPCs(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	bool bNow = ZGetQuest()->GetCheet(ZQUEST_CHEET_WEAKNPCS);
	bNow = !bNow;

	ZGetQuest()->SetCheet(ZQUEST_CHEET_WEAKNPCS, bNow);

	if (bNow)
	{
		ZChatOutput( "WeakNPC mode enabled" );

		// ¡ˆ±› ¿÷¥¬ NPCµÈ¿« HP∏¶ 1∑Œ ºº∆√
		for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
			itor != ZGetObjectManager()->end(); ++itor)
		{
			ZObject* pObject = (*itor).second;
			if (pObject->IsNPC())
			{
				ZActor* pActor = (ZActor*)pObject;
				ZModule_HPAP* pModule = (ZModule_HPAP*)pActor->GetModule(ZMID_HPAP);
				if (pModule)
				{
					pModule->SetHP(1);
				}
			}
		}
	}
	else
	{
		ZChatOutput( "WeakNPC mode disabled" );
	}
#endif
}
void ChatCmd_PingServer(const char* line, const int argc, char** const argv)
{
	ZPostServerPing(timeGetTime(), 0);
}
void ChatCmd_GiveCoinsCash(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}

	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (strlen(argv[1]) > 20)
	{
		ZChatOutput("El nombre de usuario debe de 20 caracteres o menos.");
		return;
	}

	char szPlayerName[20];
	strcpy(szPlayerName, argv[1]);

	int nCash = atoi(argv[2]);

	ZPostGiveCoinsCash(szPlayerName, nCash);
}
void ChatCmd_GiveCoinsMedals(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}

	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (strlen(argv[1]) > 20)
	{
		ZChatOutput("El nombre de usuario debe de 20 caracteres o menos.");
		return;
	}

	char szPlayerName[20];
	strcpy(szPlayerName, argv[1]);

	int nLC = atoi(argv[2]);

	ZPostGiveCoinsMedals(szPlayerName, nLC);
}
void ChatCmd_Senditem(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}

	if (argc < 4) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	int nItemId = atoi(argv[2]);
	int nDays = atoi(argv[3]);

	if (nItemId == 0 || nDays < 0) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZPostAdminSendItem(argv[1], nItemId, nDays);
}
#ifdef _STAFFCHAT
void ChatCmd_StaffChat(const char* line, const int argc, char** const argv)
{
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szMsg[256];

	strcpy(szMsg, argv[1]);
	ZPostStaffChat(szMsg);
}
#endif
void ChatCmd_AdminEsp(const char* line, const int argc, char** const argv)
{
	char szMsg[256];
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}
	if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isESP == 0)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isESP = 1;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		sprintf(szMsg, "^2ESP has been enabled!");
	}
	else if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isESP == 1)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isESP = 0;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		sprintf(szMsg, "^2ESP has been disabled!");
	}
	ZChatOutput(szMsg);
}
void ChatCmd_Killall(const char* line, const int argc, char **const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're not in game.", ZChat::CMT_SYSTEM);
		return;
	}
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}
	char szAdminName[100] = "";
	char szBuffer[128] = "";


	if (argc > 2){
		OutputCmdWrongArgument(argv[0]);


		return;
	}
	else if (argc < 2){
		strcpy(szAdminName, "");
		strcpy(szBuffer, "Room killed.");
	}
	else{
		strcpy(szAdminName, argv[1]);
		sprintf(szBuffer, "Room killed.", szAdminName);
	}


	ZPOSTCMD1(MC_KILL_ALL, MCmdParamStr(szAdminName));
	ZChatOutput(szBuffer, ZChat::CMT_SYSTEM);
}
void ChatCmd_AdminTeleport2(const char* line, const int argc, char** const argv) {
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;
	}
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}

	if (!ZGetGame())
	{
		ZChatOutput("You need to be in-game!", ZChat::CMT_SYSTEM);
		return;
	}


	if (argc < 2)
	{
		ZChatOutput("Enter a character name!", ZChat::CMT_SYSTEM);
		return;
	}
	ZPOSTCMD2(MC_ADMIN_TELEPORT, MCmdParamStr(""), MCmdParamStr(argv[1]));
}
void ChatCmd_AdminTeleport(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;
	}

	if (!ZGetGame())
	{
		ZChatOutput("You need to be in-game!", ZChat::CMT_SYSTEM);
		return;
	}

	if (argc < 2)
	{
		ZChatOutput("Enter a character name!", ZChat::CMT_SYSTEM);
		return;
	}

	ZCharacterManager* pZCharacterManager = ZGetCharacterManager();

	if (pZCharacterManager != NULL)
	{
		for (ZCharacterManager::iterator itor = pZCharacterManager->begin(); itor != pZCharacterManager->end(); ++itor)
		{
			ZCharacter* pCharacter = (ZCharacter*)(*itor).second;

			if (strcmp(pCharacter->GetProperty()->GetName(), argv[1]) == 0)
				ZGetGame()->m_pMyCharacter->SetPosition(pCharacter->GetPosition());
		}
	}
}
void ChatCmd_AdminTeleportAll(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) 
	{
		return;
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're not in game.", ZChat::CMT_SYSTEM);
		return;
	}

	ZPOSTCMD1(MC_TELEPORT_ALL, MCmdParamStr(""));
}
void ChatCmd_Ki(const char* line, const int argc, char** const argv)
{
	{
	if (!ZGetGame())
	{
		ZChatOutput("This command can only be used in-game!", ZChat::CMT_SYSTEM);
		return;
	}
	char szMsg[256];
	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();
    #define DELAY 15000

	if ((nNowTime - st_nLastTime) < DELAY) return;
	st_nLastTime = nNowTime;
	if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSuper == 0)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSuper = 1;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();

		if (ZGetGame())
			ZGetGame()->PostSpMotion(ZC_SPMOTION_TAUNT);

		if (ZGetGame())
			ZGetGame()->BerserkerKI();

		ZCharacter* pChar = ZGetGameInterface()->GetMyCharacter();
		rvector dir = -RealSpace2::RCameraDirection;
		ZGetEffectManager()->AddTrapFireEffect((D3DXVECTOR3)pChar->GetPosition(), dir);
		ZGetSoundEngine()->PlaySound("fx_ki", pChar->GetPosition());
		sprintf(szMsg, "^2You have transformed into a Super Warrior!");
	}
	else if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSuper == 1)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isSuper = 0;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		ZCharacter* pChar = ZGetGameInterface()->GetMyCharacter();
		ZGetEffectManager()->Clear();
		rvector dir = -RealSpace2::RCameraDirection;
		ZGetEffectManager()->AddTrapFireEffect((D3DXVECTOR3)pChar->GetPosition(), dir);
		ZGetSoundEngine()->PlaySound("fx_ki", pChar->GetPosition());
		sprintf(szMsg, "^1You are no longer a Super Warrior!");
	}
	ZChatOutput(szMsg);
   }
}
void ChatCmd_HideJjang(const char* line, const int argc, char** const argv)
{
	char szMsg[256];
	if (!ZGetMyInfo()->IsEventGrade())
	{
		//return;
	}
	if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isJjang == 0)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isJjang = 1;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		ZGetEffectManager()->Clear();
		ZGetSoundEngine()->PlaySound("fx_whoosh02");
		sprintf(szMsg, "^2Hide Jjang: Off");
	}
	else if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isJjang == 1)
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().isJjang = 0;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		if (ZGetGame())
			ZGetGame()->GetJjang();
		ZGetSoundEngine()->PlaySound("fx_whoosh02");
		sprintf(szMsg, "^1Hide Jjang: On");
	}
	ZChatOutput(szMsg);
}
void ChatCmd_ClearChat(const char* line, const int argc, char** const argv)
{
	ZGetGameInterface()->GetChat()->Clear();
}
#ifdef _REPORT
void ChatCmd_Report(const char* line, const int argc, char** const argv)
{
	char szMsg[256];
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	ZPostReport(argv[1], argv[2]);
}
#endif
rvector PlayerPosHotKeyMacro;
void ChatCmd_TeleSavePos(const char* line, const int argc, char** const argv)
{
	if (!ZGetGame())
	{
		ZChatOutput("You need to be in-game!", ZChat::CMT_SYSTEM);
		return;
	}
	// Custom: No Teleport
	ZMyCharacter* pMyCharacter = ZGetGameInterface()->GetGame()->m_pMyCharacter;	
	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_TELEPORT) && !ZGetGameClient()->GetMatchStageSetting()->IsTeamPlay()
		&& ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_DEATHMATCH_SOLO)
	{
		PlayerPosHotKeyMacro = pMyCharacter->GetPosition();
		ZChatOutput(MCOLOR(0, 255, 0), "Position Saved!");
	}
}
void ChatCmd_TeleLoadPos(const char* line, const int argc, char** const argv)
{
	if (!ZGetGame())
	{
		ZChatOutput("You need to be in-game!", ZChat::CMT_SYSTEM);
		return;
	}
	ZMyCharacter* pMyCharacter = ZGetGameInterface()->GetGame()->m_pMyCharacter;
	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_TELEPORT) && !ZGetGameClient()->GetMatchStageSetting()->IsTeamPlay()
		&& ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_DEATHMATCH_SOLO)
	{
		pMyCharacter->SetPosition(PlayerPosHotKeyMacro);
		ZChatOutput(MCOLOR(255, 0, 0), "Position Loaded!");
	}
}
void ChatCmd_Exit(const char* line, const int argc, char** const argv)
{
	PostQuitMessage(0);
}
#ifdef _GRADECHANGE
void ChatCmd_GetGrade(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade())
	{
		return;
	}
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	if (strlen(argv[1]) > 20)
	{
		ZChatOutput("The username should have less than 20 characteristics.");
		return;
	}
	char szPlayerName[20];
	strcpy(szPlayerName, argv[1]);
	int nUGradeID = atoi(argv[2]);
	ZPostGetGrade(szPlayerName, nUGradeID);
}

void ChatCmd_Flipmower(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&fm, 0, 0, 0);
}
void ChatCmd_InsaneMassive(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&im, 0, 0, 0);
}
void ChatCmd_LawnMower(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&lm, 0, 0, 0);
}
void ChatCmd_Respawn(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&rp, 0, 0, 0);
}
void ChatCmd_Enchant(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&en, 0, 0, 0);
}
void ChatCmd_Shot(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&sh, 0, 0, 0);
}
void ChatCmd_NpcShot(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&ns, 0, 0, 0);
}
void ChatCmd_Explosion(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&ex, 0, 0, 0);
}
void ChatCmd_ExplosionUnder(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&ex2, 0, 0, 0);
}
void ChatCmd_GrenadeThrow(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;//By Desperate
	}
	if (!ZGetGame())
	{
		ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
		return;
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&gt, 0, 0, 0);
}

void ChatCmd_AdminSBTest(const char* line, const int argc, char** const argv)
{
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostAdminSBTest(ZGetGameClient()->GetPlayerUID(), pszTarget);
}
#ifdef _ASSITEN
void ChatCmd_AdminAssitent(const char* line, const int argc, char** const argv)
{
	if (!ZGetGame())
	{
		ZChatOutput("You need to be in-game!", ZChat::CMT_SYSTEM);
		return;
	}
	if (ZGetGame()->m_pMyCharacter && !ZGetGame()->m_pMyCharacter->IsDie())
		ZGetCamera()->m_bAutoAiming = !ZGetCamera()->m_bAutoAiming;
}
#endif
void ChatCmd_Pos(const char* line, const int argc, char** const argv)
{
	if (!ZGetGame())
	{
		ZChatOutput("You need to be in-game!", ZChat::CMT_SYSTEM);
		return;
	}
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szText[128];
	PositionXYZ = static_cast<int>(atoi(argv[1]));
	sprintf(szText, "Get Pos: %d", PositionXYZ);
	ZChatOutput(szText, ZChat::CMT_SYSTEM);
}
#endif
#ifdef _CMD_ALL
void ChatCmd_Crash(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) 
	{
		return;
	}

	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (!ZGetGame())
	{
		ZChatOutput("You are not inside the room.", ZChat::CMT_SYSTEM);
		return;
	}

	ZPostAllCmd("crash", argv[1], "");
}
void ChatCmd_Staff(const char* line, const int argc, char** const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade()) {
		return;
	}

	if (ZGetGame()) {
		ZGetGameInterface()->GetCombatInterface()->UpdateStaffCommands("");
		return;
	}
	else 
	{
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_LOBBY_DEFALT), "In-Game Staff Commands" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /wa" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /tpall" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /tp1" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST),  "** /tp2");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /kill");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /admin_team" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /unkick");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /pkick");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /admin_silence");
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /changemaster" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /akick" );
	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), "** /staff");
	}
}
#endif
#ifdef _RADIO
void ChatCmd_RadioCommand(const char* line, const int argc, char** const argv)
{
	if (!ZGetGame())
	{
		ZChatOutput("You are not inside the room.", ZChat::CMT_SYSTEM);
		return;
	}
	ZGetCombatInterface()->GetVoteInterface()->CallRadioCommandList();
}
#endif
#ifdef _REJOIN_NEW
void ChatCmd_StageRejoin(const char* line, const int argc, char** const argv)
{
	ZGetGameClient()->IsRejoin = true;
	ZPostLadderRejoin();
}
#endif