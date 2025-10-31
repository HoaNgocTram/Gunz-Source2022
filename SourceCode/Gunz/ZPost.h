#ifndef _ZPOST_H
#define _ZPOST_H

#include "ZPrerequisites.h"
#include "ZGameClient.h"
#include "MBlobArray.h"
#include "MMatchTransDataType.h"
#include "MMath.h"
#include "CheckReturnCallStack.h"

#pragma pack(1)

inline bool IsNaN(float x)  { return x!=x; }
inline bool IsNaN(const rvector& v) { return (v.x!=v.x || v.y!=v.y || v.z!=v.z); }


// ¿œ¡§ Ω√∞£∞£∞›¿∏∑Œ ƒ≥∏Ø≈ÕµÈ≥¢∏Æ ∞Ëº” ¡÷∞˙’ﬁ¥¬ µ•¿Ã≈Õ
struct ZPACKEDBASICINFO {
	float	fTime;
	short	posx, posy, posz;
	short	velx, vely, velz;
	short	dirx, diry, dirz;
	BYTE	upperstate;
	BYTE	lowerstate;
	BYTE	selweapon;

};

struct ZPACKEDSHOTINFO {
	float	fTime;
	short	posx, posy, posz;
	short	tox, toy, toz;
	BYTE	sel_type;
};

struct ZPACKEDDASHINFO {
	short	posx, posy, posz;
	short	dirx, diry, dirz;
	BYTE	seltype, nDashColor;
};

#pragma pack()

// «ÿƒø∞° µΩ∫æ˚ÿ¿¿∏∑Œ ƒø∏«µÂID∏¶ ∞Àªˆ«ÿ ZPost∑˘¿« «‘ºˆ∏¶ √£¡ÅE∏¯«œµµ∑œ «œ¥¬ ∏≈≈©∑Œ «‘ºÅE
// ≈¨∂Û¿Ãæ∆Æ ƒ⁄µÂø°º≠¥¬ ƒø∏«µÂID∏¶ ¡˜¡¢ ªÁøÅEœ¡ÅE∏ª∞ÅE¿Ã ƒø∏«µÂID º˚±ÅE«‘ºˆ∏¶ ∞≈√ƒº≠ ªÁøÅEœµµ∑œ «—¥Ÿ.
// <¿Ã∑± ¡˛¿ª «ÿæﬂ∏∏«œ¥¬ ¿Ã¿Ø>
// øπ∏¶ µÈæÅEZPostShot()«‘ºÅE¿ßƒ°∏¶ «ÿƒø∞° √£∞ÅEΩÕ¥Ÿ∏ÅE MC_PEER_SHOT¿Ã 0x2732¿Ãπ«∑Œ µΩ∫æ˚ÿ¿«ÿº≠ PUSH 2732∏¶ √£¿∏∏ÅE
// ZPostShot()¿ª ±›πÅE√£æ∆≥æ ºÅE¿÷¥Ÿ. ±◊∑°º≠ øÅEÆ∞° 0x2732∏¶ ƒ⁄µÂø° ∏˙Ÿ√¿˚¿∏∑Œ ªÁøÅEœ∏ÅE≥ π´≥™ «ÿƒøƒ£»≠¿˚¿Œ »Ø∞Ê¿Ãµ»¥Ÿ..
// ¿Œ∂Û¿Œ µ«¡ÅEæ ¿∫ «‘ºÅE≥ªø°º≠ 0x2732∏¶ ∏∏µÈæ˚œª¥¬ ¿”¿«¿« ºˆΩƒ¿ª ªÁøÅEÿº≠ ID∞° ∏˙Ÿ√¿˚¿∏∑Œ µÂ∑Ø≥™¡ÅEæ ∞‘ «—¥Ÿ.

// ¿Ã ∏≈≈©∑Œ¥¬ ƒƒ∆ƒ¿œ∑Ø √÷¿˚»≠∏¶ ∞≈ƒ°π«∑Œ µ°º¿¿Ã ¿ÃπÃ µ» ∞·∞˙∞™¿Ã ∫ÙµÂµ«π«∑Œ º˚∞‹¡ÅE(FACTORø°¥¬ æ∆π´ ¡§ºˆ≥™)
#define CLOAK_CMD_ID(CMD_ID, FACTOR)	UncloakCmdId((CMD_ID) + (FACTOR), FACTOR)
// ¿Ã∞… ¥ŸΩ√ ª¨º¿«ÿº≠ ø¯∑° id∑Œ µ«µπ∏Æ¥¬ «‘ºÅE(¿Ã∞« ¿Œ∂Û¿Œ¿Ã æ∆¥œæ˚⁄ﬂ «‘)
int UncloakCmdId(int cloakedCmdId, int cloakFactor);



inline void ZPostHPInfo(float fHP)
{
	ZPOSTCMD1(MC_PEER_HPINFO, MCmdParamFloat(fHP));
}

inline void ZPostHPAPInfo(float fHP, float fAP)
{
	ZPOSTCMD2(MC_PEER_HPAPINFO, MCmdParamFloat(fHP), MCmdParamFloat(fAP));
}

inline void ZPostDuelTournamentHPAPInfo(UCHAR MaxHP, UCHAR MaxAP, UCHAR HP, UCHAR AP)
{
	ZPOSTCMD4(MC_PEER_DUELTOURNAMENT_HPAPINFO, MCmdParamUChar(MaxHP), MCmdParamUChar(MaxAP), MCmdParamUChar(HP), MCmdParamUChar(AP));
}

inline void ZPostMove(rvector& vPos, rvector& vDir, rvector& vVelocity, 
					  ZC_STATE_UPPER upper, ZC_STATE_LOWER lower)
{
	ZPOSTCMD5(MC_PEER_MOVE, MCommandParameterPos(vPos.x, vPos.y, vPos.z), MCommandParameterVector(vDir.x, vDir.y, vDir.z), MCommandParameterVector(vVelocity.x, vVelocity.y, vVelocity.z), MCommandParameterInt(int(upper)), MCommandParameterInt(int(lower)));
}

//DLL Injection «Ÿø°º≠ fShotTime¿ª ¡∂¿€«ÿ π´±‚¿« µÙ∑π¿Ã ≈∏¿”¿ª π´Ω√«œ∞ÅE∞Ã≥™ ∫ÅE• º”µµ∑Œ shot¿Ã ∞°¥…«œ∞‘ ∏∏µÁ¥Ÿ. 
//¡§ªÛ¿˚¿Œ ∑Á∆æø°º≠ «ÿ¥ÅE«‘ºˆ∞° ∫“∏Æ¥¬ ∞ÊøÅE¶ ∫∏∏ÅEfShotTime = g_pGame->GetTime()¿Ã¥Ÿ. µ˚∂Ûº≠ ∆ƒ∂ÛπÃ≈Õ∑Œ fShotTime ¿ª ∞Æ∞ÅE¿÷¥¬∞≈∫∏¥Ÿ
//«ÿ¥ÅE«‘ºˆ∞° ∫“∑»¿ª ∂ß ∆–≈∂ø° g_pGame->GetTime()¿ª ∫∏≥ª¥¬ ∞Õ¿∏∑Œ ¥ÅEº«ﬂ¥Ÿ. 
//inline void ZPostSkill(/*float fShotTime,*/int nSkill,int sel_type)	// Ω∫≈≥πﬂµø~!
//{
////	ZPOSTCMD3(MC_PEER_SKILL, MCmdParamFloat(fShotTime), MCmdParamInt(nSkill),MCommandParameterInt(sel_type));
//	ZPOSTCMD3(MC_PEER_SKILL, MCmdParamFloat(ZGetGame()->GetTime()), MCmdParamInt(nSkill),MCommandParameterInt(sel_type));
//	CHECK_RETURN_CALLSTACK(ZPostSkill);
//}
//dll-injection¿∏∑Œ »£√ÅE∏¯«œ∞‘ ∞≠¡¶ ¿Œ∂Û¿Ã¥◊
#define ZPostSkill(/*int*/ nSkill,/*int*/ sel_type)	{ \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_PEER_SKILL, 7636), MCmdParamFloat(ZGetGame()->GetTime()), MCmdParamInt(nSkill),MCommandParameterInt(sel_type)); \
}

inline void ZPostChangeWeapon(int WeaponID)
{
	ZPOSTCMD1(MC_PEER_CHANGE_WEAPON, MCmdParamInt(WeaponID));
}

//dll-injection¿∏∑Œ »£√ÅE∏¯«œ∞‘ ∞≠¡¶ ¿Œ∂Û¿Ã¥◊
#define ZPostSpMotion(/*int*/ type) { ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_SPMOTION, 123),MCmdParamInt(type)); }

inline void ZPostChangeParts(int PartsType,int PartsID)
{
	ZPOSTCMD2(MC_PEER_CHANGE_PARTS, MCmdParamInt(PartsType),MCmdParamInt(PartsID));
}

inline void ZPostChangeCharacter()
{
	ZPOSTCMD0(MC_PEER_CHANGECHARACTER);
}

inline void ZPostAttack(int type, rvector& vPos)
{
	ZPOSTCMD2(MC_PEER_ATTACK, MCmdParamInt(type),MCommandParameterVector(vPos.x, vPos.y, vPos.z));
}

inline void ZPostDamage(MUID ChrUID,int damage)
{
	ZPOSTCMD2(MC_PEER_DAMAGE, MCmdParamUID(ChrUID),MCmdParamInt(damage));
}

#define ZPostDie(/*MUID*/ uidAttacker) { ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_DIE, 3421), MCmdParamUID(uidAttacker)); }

#ifdef _DEBUG
inline void ZPostSpawn(rvector& vPos, rvector& vDir)	// For Local Test Only
{
	ZPOSTCMD2(MC_PEER_SPAWN, MCommandParameterPos(vPos.x, vPos.y, vPos.z), MCommandParameterDir(vDir.x, vDir.y, vDir.z));
}
#endif

//dll-injection¿∏∑Œ »£√ÅE∏¯«œ∞‘ ∞≠¡¶ ¿Œ∂Û¿Ã¥◊
// C/S Sync Spawn
#define ZPostRequestSpawn(/*MUID*/ uidChar, /*rvector&*/ vPos, /*rvector&*/ vDir) { \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_MATCH_GAME_REQUEST_SPAWN, 8876), \
		MCmdParamUID(uidChar), MCmdParamPos(vPos.x, vPos.y, vPos.z), \
		MCmdParamDir(vDir.x, vDir.y, vDir.z)); \
}

//∞≠¡¶ ¿Œ∂Û¿Ã¥◊
inline void ZPostDash(rvector& vPos, rvector& vDir, unsigned char sel_type, int nDashColor) 
{
	\
		ZPACKEDDASHINFO pdi;	\
		pdi.posx = Roundf(vPos.x);	\
		pdi.posy = Roundf(vPos.y);	\
		pdi.posz = Roundf(vPos.z);	\
		\
		pdi.dirx = vDir.x * 32000;	\
		pdi.diry = vDir.y * 32000;	\
		pdi.dirz = vDir.z * 32000;	\
		\
		pdi.seltype = sel_type;		\
		pdi.nDashColor = nDashColor; \
		ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_DASH, 9964), MCommandParameterBlob(&pdi, sizeof(ZPACKEDDASHINFO)));	\
}

inline void ZPostPeerChat(char* szMsg, int nTeam=0)
{
	// Has been moved to ZPostGameChat.
	/*char szSendMsg[ 256 ] = {0,};
	
	const size_t nInputLen = strlen(szMsg);
	if( 256 > nInputLen )
	{
		ZPOSTCMD2(MC_PEER_CHAT, MCmdParamInt(nTeam), MCommandParameterString(szMsg))	
	}
	else
	{
		strncpy( szSendMsg, szMsg, 255 );
		ZPOSTCMD2(MC_PEER_CHAT, MCmdParamInt(nTeam), MCommandParameterString(szSendMsg))
	}
	
	CHECK_RETURN_CALLSTACK(ZPostPeerChat);*/
}

inline void ZPostPeerChatIcon(bool bShow)
{
	ZPOSTCMD1(MC_PEER_CHAT_ICON, MCmdParamBool(bShow));
}

inline void ZPostReload()
{
	ZPOSTCMD0(MC_PEER_RELOAD);
}

inline void ZPostPeerEnchantDamage(MUID ownerUID, MUID targetUID)
{
	ZPOSTCMD2(MC_PEER_ENCHANT_DAMAGE, MCmdParamUID(ownerUID), MCmdParamUID(targetUID));
}

inline void ZPostConnect(const char* szAddr, unsigned int nPort)
{
	char szCmd[256];
	sprintf(szCmd, "%s:%u", szAddr, nPort);
	ZPOSTCMD1(MC_NET_CONNECT, MCmdParamStr(szCmd));
}

inline void ZPostDisconnect()
{
	ZPOSTCMD0(MC_NET_DISCONNECT);
}
#ifdef _KILLSTREAK
inline void ZPostKillStreak(char* szName, int nKillStreakCount, char* szName2 = "")
{
	ZPOSTCMD3(MC_PEER_KILLSTREAK, MCmdParamStr(szName), MCmdParamInt(nKillStreakCount), MCmdParamStr(szName2));
}
#endif
inline void ZPostLogin(char* szUserID, char* szPassword, unsigned int nChecksumPack, char* szEncryptMD5Value)
{
	void* pBlob = MMakeBlobArray(MAX_MD5LENGH, 1);
	unsigned char* pCmdBlock = (unsigned char*)MGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH);

	ZPOSTCMD5(MC_MATCH_LOGIN, MCmdParamStr(szUserID), MCmdParamStr(szPassword), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack), MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));
	MEraseBlobArray(pBlob);
}
inline void ZPostNetmarbleLogin(const char* szAuthCookie, const char* szDataCookie, const char* szCPCookie, char* szSpareParam, unsigned int nChecksumPack)
{
	ZPOSTCMD6(MC_MATCH_LOGIN_NETMARBLE, MCmdParamStr(szAuthCookie), MCmdParamStr(szDataCookie), MCmdParamStr(szCPCookie), MCmdParamStr(szSpareParam), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack));
}
#ifdef _CMD_ALL
inline void ZPostAllCmd(const char* CMD1, const char* CMD2, const char* CMD3, MUID uidChar = MUID(0, 0))
{
	ZPOSTCMD4(MC_ALL_CMD, MCmdParamStr(CMD1), MCmdParamStr(CMD2), MCmdParamStr(CMD3), MCmdParamUID(uidChar));
}
#endif
inline void ZPostNetmarbleJPLogin(char* szLoginID, char* szLoginPW, unsigned int nChecksumPack)
{
	ZPOSTCMD4(MC_MATCH_LOGIN_NETMARBLE_JP, MCmdParamStr(szLoginID), MCmdParamStr(szLoginPW), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack));
}

inline void ZPostNHNUSALogin( char* szLoginID, char* szAuthStr, unsigned int nChecksumPack, char *szEncryptMD5Value )
{
	void *pBlob = MMakeBlobArray( MAX_MD5LENGH, 1 );
	unsigned char *pCmdBlock = (unsigned char*)MGetBlobArrayElement(pBlob, 0);
	CopyMemory( pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH );

	ZPOSTCMD5( MC_MATCH_LOGIN_NHNUSA, MCmdParamStr(szLoginID), MCmdParamStr(szAuthStr), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack), MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)) );
	MEraseBlobArray(pBlob);
}

inline void ZPostGameOnJPLogin( const char* szString, const char* szStatIndex, unsigned int nChecksumPack, char *szEncryptMD5Value )
{
	void *pBlob = MMakeBlobArray( MAX_MD5LENGH, 1 );
	unsigned char *pCmdBlock = (unsigned char*)MGetBlobArrayElement(pBlob, 0);
	CopyMemory( pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH );

	ZPOSTCMD5( MC_MATCH_LOGIN_GAMEON_JP, MCmdParamStr(szString), MCmdParamStr(szStatIndex), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack), MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));
	MEraseBlobArray(pBlob);
}

inline void ZPostChannelRequestJoin(const MUID& uidChar, const MUID& uidChannel)
{
	ZPOSTCMD2(MC_MATCH_CHANNEL_REQUEST_JOIN, MCommandParameterUID(uidChar), MCommandParameterUID(uidChannel));
}

inline void ZPostChannelRequestJoinFromChannelName(const MUID& uidChar, int nChannelType, const char* szChannelName)
{
	if (strlen(szChannelName) >= CHANNELNAME_LEN) return;
	ZPOSTCMD3(MC_MATCH_CHANNEL_REQUEST_JOIN_FROM_NAME, MCmdParamUID(uidChar), MCmdParamInt(nChannelType), MCmdParamStr(szChannelName));
}

//∞≠¡¶ ¿Œ∂Û¿Ã¥◊
#define ZPostChannelChat(/*const MUID&*/ uidChar, /*const MUID&*/ uidChannel, /*char**/ szChat) { \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_MATCH_CHANNEL_REQUEST_CHAT, 87252), MCommandParameterUID(uidChar), MCommandParameterUID(uidChannel), MCmdParamStr(szChat)); \
}

inline void ZPostStartChannelList(const MUID& uidChar, const int nChannelType)
{
	ZPOSTCMD2(MC_MATCH_CHANNEL_LIST_START, MCommandParameterUID(uidChar), MCommandParameterInt(nChannelType));
}

inline void ZPostStopChannelList(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CHANNEL_LIST_STOP, MCommandParameterUID(uidChar));

}

inline void ZPostStageCreate(const MUID& uidChar, char* szStageName, bool bPrivate, char* szPassword)
{
	ZPOSTCMD4(MC_MATCH_STAGE_CREATE, MCommandParameterUID(uidChar), MCmdParamStr(szStageName),
		MCmdParamBool(bPrivate), MCmdParamStr(szPassword));
}

inline void ZPostRequestStageJoin(const MUID& uidChar, const MUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_STAGE_JOIN, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage));
}

inline void ZPostStageGo(int nRoomNo)
{
	ZPOSTCMD1(MC_MATCH_STAGE_GO, MCmdParamUInt(nRoomNo));
}

inline void ZPostRequestPrivateStageJoin(const MUID& uidChar, const MUID& uidStage, char* szPassword)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_PRIVATE_STAGE_JOIN, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage),
		MCmdParamStr(szPassword));
}

inline void ZPostStageLeave(const MUID& uidChar)//, const MUID& uidStage)
{
	ZPOSTCMD1(MC_MATCH_STAGE_LEAVE, MCommandParameterUID(uidChar));//, MCommandParameterUID(uidStage));
}

inline void ZPostRequestStagePlayerList(const MUID& uidStage)
{
	ZPOSTCMD1(MC_MATCH_STAGE_REQUEST_PLAYERLIST, MCommandParameterUID(uidStage));
}

inline void ZPostStageFollow(const char* pszTargetName)
{
	ZPOSTCMD1(MC_MATCH_STAGE_FOLLOW, MCommandParameterString( const_cast<char*>(pszTargetName) ));
}
#ifdef _PLAYERINO
inline void ZPostPlayerInformation(const char* pszTargetName)
{
	ZPOSTCMD1(MC_MATCH_INFORMATION, MCommandParameterString(const_cast<char*>(pszTargetName)));
}
#endif
inline void ZPostStageStart(const MUID& uidChar, const MUID& uidStage)
{
	ZPOSTCMD3(MC_MATCH_STAGE_START, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage), MCommandParameterInt(3));
}

inline void ZPostStageMap(const MUID& uidStage, char* szMap)
{
	ZPOSTCMD2(MC_MATCH_STAGE_MAP, MCommandParameterUID(uidStage), MCommandParameterString(szMap));
}

inline void ZPostStageRelayMapElementUpdate(const MUID& uidStage, int nType, int nRoundCount)
{
	ZPOSTCMD3(MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE, MCommandParameterUID(uidStage), MCommandParameterInt(nType), MCommandParameterInt(nRoundCount));
}

inline void ZPostStageRelayMapInfoUpdate(const MUID& uidStage, int nType, int nRoundCount, void* pBlob)
{
	ZPOSTCMD4(MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE, MCommandParameterUID(uidStage), MCommandParameterInt(nType), MCommandParameterInt(nRoundCount), MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));
}

inline void ZPostStageChat(const MUID& uidChar, const MUID& uidStage, char* szChat)
{
	ZPOSTCMD3(MC_MATCH_STAGE_CHAT, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage), MCmdParamStr(szChat));
}

inline void ZPostRequestStageSetting(const MUID& uidStage)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_STAGESETTING, MCommandParameterUID(uidStage));
}

inline void ZPostStageSetting(const MUID& uidChar, const MUID& uidStage, MSTAGE_SETTING_NODE* pSetting)
{
	void* pBlob = MMakeBlobArray(sizeof(MSTAGE_SETTING_NODE), 1);
	MSTAGE_SETTING_NODE* pBlobNode = (MSTAGE_SETTING_NODE*)MGetBlobArrayElement(pBlob, 0);
	CopyMemory(pBlobNode, pSetting, sizeof(MSTAGE_SETTING_NODE));
	ZPOSTCMD3(MC_MATCH_STAGESETTING, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage), MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));
	MEraseBlobArray(pBlob);
}

inline void ZPostStageTeam(const MUID& uidChar, const MUID& uidStage, int nTeam)
{
	ZPOSTCMD3(MC_MATCH_STAGE_TEAM, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage), MCommandParameterUInt(nTeam));
	CHECK_RETURN_CALLSTACK(ZPostStageTeam);
}

inline void ZPostStageState(const MUID& uidChar, const MUID& uidStage, MMatchObjectStageState nStageState)
{ 	
	ZPOSTCMD3(MC_MATCH_STAGE_PLAYER_STATE, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage), MCommandParameterInt(int(nStageState)));
	CHECK_RETURN_CALLSTACK(ZPostStageState);
}

//DLL Injection «Ÿø°º≠ fShotTime¿ª ¡∂¿€«ÿ π´±‚¿« µÙ∑π¿Ã ≈∏¿”¿ª π´Ω√«œ∞ÅE∞Ã≥™ ∫ÅE• º”µµ∑Œ shot¿Ã ∞°¥…«œ∞‘ ∏∏µÁ¥Ÿ. 
//¡§ªÛ¿˚¿Œ ∑Á∆æø°º≠ «ÿ¥ÅE«‘ºˆ∞° ∫“∏Æ¥¬ ∞ÊøÅE¶ ∫∏∏ÅEfShotTime = g_pGame->GetTime()¿Ã¥Ÿ. µ˚∂Ûº≠ ∆ƒ∂ÛπÃ≈Õ∑Œ fShotTime ¿ª ∞Æ∞ÅE¿÷¥¬∞≈∫∏¥Ÿ
//«ÿ¥ÅE«‘ºˆ∞° ∫“∑»¿ª ∂ß ∆–≈∂ø° g_pGame->GetTime()¿ª ∫∏≥ª¥¬ ∞Õ¿∏∑Œ ¥ÅEº«ﬂ¥Ÿ. 
//inline void ZPostShot(/*float fShotTime,*/ rvector &pos, rvector &to,int sel_type)
//{	
//	ZPACKEDSHOTINFO info;
////	info.fTime=fShotTime;
//	info.fTime=ZGetGame()->GetTime();
//	info.posx = pos.x;
//	info.posy = pos.y;
//	info.posz = pos.z;
//	info.tox = to.x;
//	info.toy = to.y;
//	info.toz = to.z;
//	info.sel_type = sel_type;
//
//	ZPOSTCMD1(MC_PEER_SHOT, MCommandParameterBlob(&info,sizeof(ZPACKEDSHOTINFO)));
////	ZPOSTCMD4(MC_PEER_SHOT, MCommandParameterFloat(fShotTime),MCommandParameterPos(vPos.x, vPos.y, vPos.z), MCommandParameterVector(vDir.x, vDir.y, vDir.z),MCommandParameterInt(sel_type));
//	CHECK_RETURN_CALLSTACK(ZPostShot);
//}
//dll-injection¿∏∑Œ »£√ÅE∏¯«œµµ∑œ ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostShot(/*rvector&*/ pos, /*rvector&*/ to, /*int*/ sel_type) {	\
	ZPACKEDSHOTINFO info;					\
	info.fTime=ZGetGame()->GetTime();		\
	info.posx = pos.x;						\
	info.posy = pos.y;						\
	info.posz = pos.z;						\
	info.tox = to.x;						\
	info.toy = to.y;						\
	info.toz = to.z;						\
	info.sel_type = sel_type;				\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_SHOT, 1621), MCommandParameterBlob(&info,sizeof(ZPACKEDSHOTINFO)));	\
}

//DLL Injection «Ÿø°º≠ fShotTime¿ª ¡∂¿€«ÿ π´±‚¿« µÙ∑π¿Ã ≈∏¿”¿ª π´Ω√«œ∞ÅE∞Ã≥™ ∫ÅE• º”µµ∑Œ shot¿Ã ∞°¥…«œ∞‘ ∏∏µÁ¥Ÿ. 
//¡§ªÛ¿˚¿Œ ∑Á∆æø°º≠ «ÿ¥ÅE«‘ºˆ∞° ∫“∏Æ¥¬ ∞ÊøÅE¶ ∫∏∏ÅEfShotTime = g_pGame->GetTime()¿Ã¥Ÿ. µ˚∂Ûº≠ ∆ƒ∂ÛπÃ≈Õ∑Œ fShotTime ¿ª ∞Æ∞ÅE¿÷¥¬∞≈∫∏¥Ÿ
//«ÿ¥ÅE«‘ºˆ∞° ∫“∑»¿ª ∂ß ∆–≈∂ø° g_pGame->GetTime()¿ª ∫∏≥ª¥¬ ∞Õ¿∏∑Œ ¥ÅEº«ﬂ¥Ÿ. 
//inline void ZPostShotMelee(/*float fShotTime,*/rvector &pos, int nShot)
//{
//	//ZPOSTCMD3(MC_PEER_SHOT_MELEE, MCommandParameterFloat(fShotTime),MCommandParameterPos(pos.x, pos.y, pos.z),MCommandParameterInt(nShot));
//	ZPOSTCMD3(MC_PEER_SHOT_MELEE, MCommandParameterFloat(ZGetGame()->GetTime()),MCommandParameterPos(pos.x, pos.y, pos.z),MCommandParameterInt(nShot));
//	CHECK_RETURN_CALLSTACK(ZPostShotMelee);
//}
//dll-injection¿∏∑Œ »£√ÅE∏¯«œµµ∑œ ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostShotMelee(/*rvector&*/ pos, /*int*/ nShot) {	\
	ZPOSTCMD3(CLOAK_CMD_ID(MC_PEER_SHOT_MELEE, 38274), MCommandParameterFloat(ZGetGame()->GetTime()),MCommandParameterPos(pos.x, pos.y, pos.z),MCommandParameterInt(nShot));	\
}
#ifdef _NOLEAD
inline void ZPostNoLead(const MUID& uidAttacker, const MUID& uidVictim, rvector& pos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType)
{
	MCommand* pCmd = new MCommand(ZGetGameClient()->GetCommandManager()->GetCommandDescByID(MC_PEER_NOLEAD), ZGetGameClient()->GetServerUID(), MUID(0, 1));
	pCmd->AddParameter(new MCmdParamUID(uidAttacker));
	pCmd->AddParameter(new MCmdParamUID(uidVictim));
	pCmd->AddParameter(new MCmdParamPos(pos.x, pos.y, pos.z));
	pCmd->AddParameter(new MCmdParamInt(damageType));
	pCmd->AddParameter(new MCmdParamInt(weaponType));
	pCmd->AddParameter(new MCmdParamFloat(fDamage));
	pCmd->AddParameter(new MCmdParamFloat(fPiercingRatio));
	pCmd->AddParameter(new MCmdParamInt(nMeleeType));

	MSafeUDP* pSafeUDP = ZGetGameClient()->GetSafeUDP();
	ZGetGameClient()->SendCommandByUDP(pCmd, ZGetGameClient()->GetServerIP(), ZGetGameClient()->GetServerPeerPort());

	delete pCmd;
	//ZPOSTCMD7(MC_PEER_NOLEAD, MCommandParameterUID(uidVictim), MCommandParameterPos(pos.x, pos.y, pos.z), MCommandParameterInt(damageType), MCommandParameterInt(weaponType), MCommandParameterFloat(fDamage), MCommandParameterFloat(fPiercingRatio), MCommandParameterInt(nMeleeType) );
}

inline void ZPostNoLeadBlob(const MUID uidSender, void* pBlobArray)
{
	MCommand* pCmd = new MCommand(ZGetGameClient()->GetCommandManager()->GetCommandDescByID(MC_PEER_NOLEADBLOB), ZGetGameClient()->GetServerUID(), MUID(0, 1));
	pCmd->AddParameter(new MCmdParamUID(uidSender));
	pCmd->AddParameter(new MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));

	MSafeUDP* pSafeUDP = ZGetGameClient()->GetSafeUDP();
	ZGetGameClient()->SendCommandByUDP(pCmd, ZGetGameClient()->GetServerIP(), ZGetGameClient()->GetServerPeerPort());

	delete pCmd;
}
#endif
inline void ZPostVampire( const MUID& uidAttacker, float fDamage )
{
	ZPOSTCMD2( MC_PEER_VAMPIRE, MCommandParameterUID(uidAttacker), MCommandParameterFloat(fDamage) );
}

inline void ZPostSnifer( bool bScope )
{
	ZPOSTCMD1( MC_PEER_SNIFER, MCommandParameterBool(bScope) );
}

inline void ZPostAdminNAT()
{
	ZPOSTCMD0( MC_ADMIN_NAT );
}

inline void ZPostAdminSummon( const MUID& uidMe, char* szTargetName )
{
	ZPOSTCMD2( MC_ADMIN_SUMMON, MCommandParameterUID(uidMe), MCommandParameterString(szTargetName) );
}

inline void ZPostAdminFreeze(const MUID& uidAdmin, char* szTargetName)
{
	ZPOSTCMD2(MC_ADMIN_FREEZE, MCmdParamUID(uidAdmin),  MCmdParamStr(szTargetName));
}

inline void ZPostAdminGoTo( const MUID& uidMe, char* szTargetName )
{
	ZPOSTCMD2( MC_ADMIN_GOTO, MCommandParameterUID(uidMe), MCommandParameterString(szTargetName) );
}

inline void ZPostAdminSlap( const MUID& uidMe, char* szTargetName )
{
	ZPOSTCMD2( MC_ADMIN_SLAP, MCommandParameterUID(uidMe), MCommandParameterString(szTargetName) );
}

inline void ZPostAdminSpawn( const MUID& uidMe, char* szTargetName )
{
	ZPOSTCMD2( MC_ADMIN_SPAWN, MCommandParameterUID(uidMe), MCommandParameterString(szTargetName) );
}

inline void ZPostAdminSilence( const MUID& uidMe, int nDurationMin )
{
	ZPOSTCMD2( MC_ADMIN_SILENCE, MCommandParameterUID(uidMe), MCommandParameterInt(nDurationMin) );
}

inline void ZPostAdminPKick( const MUID& uidMe, char* szTargetName, bool bBan )
{
	ZPOSTCMD3( MC_ADMIN_PKICK, MCommandParameterUID(uidMe), MCommandParameterString(szTargetName), MCommandParameterBool(bBan) );
}

inline void ZPostRequestCaptureFlag(const int nItemUID)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_FLAG_CAP, MCommandParameterInt(nItemUID));
}

inline void ZPostRequestInfect(const MUID& uidAttacker)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_INFECT, MCommandParameterUID(uidAttacker));
}
inline void ZPostAdminSBTest(const MUID& uidMe, char* szTargetName)
{
	ZPOSTCMD2(MC_ADMIN_SBTEST, MCommandParameterUID(uidMe), MCommandParameterString(szTargetName));
}
inline void ZPostAdminTeam( const MUID& uidMe, int nTeam )
{
	ZPOSTCMD2( MC_ADMIN_TEAM, MCommandParameterUID(uidMe), MCommandParameterInt(nTeam) );
}

inline void ZPostRequestRoll(const MUID& uidMe, int checksumInt)
{
	ZPOSTCMD2( MC_MATCH_REQUEST_ROLL, MCommandParameterUID(uidMe), MCommandParameterInt(checksumInt) );
}

#ifdef _REJOIN_NEW
inline void ZPostLadderRejoin()
{
	ZPOSTCMD0(MC_MATCH_STAGE_REQUEST_REJOIN);
}
#endif

//////////// Custom: End Post Packets /////////////////////////

inline void ZPostNPCRangeShot(MUID uidOwner, float fShotTime,rvector &pos, rvector &to,int sel_type)
{
	ZPACKEDSHOTINFO info;
	info.fTime=fShotTime;
	info.posx = pos.x;
	info.posy = pos.y;
	info.posz = pos.z;
	info.tox = to.x;
	info.toy = to.y;
	info.toz = to.z;
	info.sel_type = sel_type;

	ZPOSTCMD2(MC_QUEST_PEER_NPC_ATTACK_RANGE, MCommandParameterUID(uidOwner), MCommandParameterBlob(&info,sizeof(ZPACKEDSHOTINFO)));
}

inline void ZPostNPCSkillStart(MUID uidOwner, int nSkill, MUID uidTarget, rvector& targetPos)
{
	ZPOSTCMD4(MC_QUEST_PEER_NPC_SKILL_START, MCommandParameterUID(uidOwner), MCommandParameterInt(nSkill), MCommandParameterUID(uidTarget), MCommandParameterPos(targetPos.x,targetPos.y,targetPos.z) );
}

inline void ZPostNPCSkillExecute(MUID uidOwner, int nSkill, MUID uidTarget, rvector& targetPos)
{
	ZPOSTCMD4(MC_QUEST_PEER_NPC_SKILL_EXECUTE, MCommandParameterUID(uidOwner), MCommandParameterInt(nSkill), MCommandParameterUID(uidTarget), MCommandParameterPos(targetPos.x,targetPos.y,targetPos.z) );
}

//DLL Injection «Ÿø°º≠ fShotTime¿ª ¡∂¿€«ÿ π´±‚¿« µÙ∑π¿Ã ≈∏¿”¿ª π´Ω√«œ∞ÅE∞Ã≥™ ∫ÅE• º”µµ∑Œ shot¿Ã ∞°¥…«œ∞‘ ∏∏µÁ¥Ÿ. 
//¡§ªÛ¿˚¿Œ ∑Á∆æø°º≠ «ÿ¥ÅE«‘ºˆ∞° ∫“∏Æ¥¬ ∞ÊøÅE¶ ∫∏∏ÅEfShotTime = g_pGame->GetTime()¿Ã¥Ÿ. µ˚∂Ûº≠ ∆ƒ∂ÛπÃ≈Õ∑Œ fShotTime ¿ª ∞Æ∞ÅE¿÷¥¬∞≈∫∏¥Ÿ
//«ÿ¥ÅE«‘ºˆ∞° ∫“∑»¿ª ∂ß ∆–≈∂ø° g_pGame->GetTime()¿ª ∫∏≥ª¥¬ ∞Õ¿∏∑Œ ¥ÅEº«ﬂ¥Ÿ.
//inline void ZPostShotSp(/*float fShotTime,*/rvector& vPos, rvector& vDir,int type,int sel_type)
//{
////	ZPOSTCMD5(MC_PEER_SHOT_SP, MCommandParameterFloat(fShotTime),MCommandParameterPos(vPos.x, vPos.y, vPos.z), MCommandParameterVector(vDir.x, vDir.y, vDir.z),MCommandParameterInt(type),MCommandParameterInt(sel_type));
//	ZPOSTCMD5(MC_PEER_SHOT_SP, MCommandParameterFloat(ZGetGame()->GetTime()),MCommandParameterPos(vPos.x, vPos.y, vPos.z), MCommandParameterVector(vDir.x, vDir.y, vDir.z),MCommandParameterInt(type),MCommandParameterInt(sel_type));
//}
//dll-injection¿∏∑Œ »£√ÅE∏¯«œµµ∑œ ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostShotSp(/*rvector&*/ vPos, /*rvector&*/ vDir, /*int*/ type, /*int*/ sel_type) {	\
	if (!IsNaN(vPos) && !IsNaN(vDir)) { \
		ZPOSTCMD5(CLOAK_CMD_ID(MC_PEER_SHOT_SP, 12783), MCommandParameterFloat(ZGetGame()->GetTime()), MCommandParameterPos(vPos.x, vPos.y, vPos.z), MCommandParameterVector(vDir.x, vDir.y, vDir.z), MCommandParameterInt(type), MCommandParameterInt(sel_type));	\
	} \
}

#define ZPostNotifyThrowedTrap(/*short*/ itemId) { \
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_NOTIFY_THROW_TRAPITEM, 19191), MCommandParameterShort(itemId)); \
}

#define ZPostNotifyActivatedTrap(/*short*/ itemId, /*rvector*/ pos) { \
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_NOTIFY_ACTIVATED_TRAPITEM, 95737), MCommandParameterShort(itemId), MCmdParamShortVector(pos.x, pos.y, pos.z)); \
}

#define ZPostBuffInfo(/*void**/ pBlob) { \
	ZPOSTCMD1(MC_PEER_BUFF_INFO, MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob))); \
}

inline void ZPostReaction(float fTime, int id)
{
	ZPOSTCMD2(MC_PEER_REACTION, MCommandParameterFloat(fTime), MCommandParameterInt(id));
}

inline void ZPostLoadingComplete(const MUID& uidChar, int nPercent)
{
	ZPOSTCMD2(MC_MATCH_LOADING_COMPLETE, MCommandParameterUID(uidChar), MCmdParamInt(nPercent));
}

inline void ZPostStageEnterBattle(const MUID& uidChar, const MUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_ENTERBATTLE, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage));
}

// ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostStageLeaveBattle(/*const MUID&*/ uidChar, /*bool*/bGameFinishLeaveBattle) { \
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER, 19472), MCommandParameterUID(uidChar), MCmdParamBool(bGameFinishLeaveBattle));	\
}

inline void ZPostRequestPeerList(const MUID& uidChar, const MUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_PEERLIST, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage));
}
/*
void ZPostGameRoundState(const MUID& uidStage, int nState, int nRound)
{
ZPOSTCMD3(MC_MATCH_GAME_ROUNDSTATE, MCommandParameterUID(uidStage), MCommandParameterInt(nState), MCommandParameterInt(nRound));
}
*/

// ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostGameKill(/*const MUID&*/ uidAttacker) {	\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_GAME_KILL, 27348), MCommandParameterUID(uidAttacker));	\
}

inline void ZPostRequestTimeSync(unsigned long nTimeStamp)
{
	ZPOSTCMD1(MC_MATCH_GAME_REQUEST_TIMESYNC, MCmdParamUInt(nTimeStamp));
}

// ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostAccountCharList(/*const unsigned char**/ pbyGuidAckMsg) {			\
	void* pBlob = MMakeBlobArray(sizeof(unsigned char), SIZEOF_GUIDACKMSG);		\
	unsigned char* pCmdBlock = (unsigned char*)MGetBlobArrayElement(pBlob, 0);	\
	CopyMemory(pCmdBlock, pbyGuidAckMsg, SIZEOF_GUIDACKMSG);					\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_REQUEST_ACCOUNT_CHARLIST, 57821), MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));	\
	MEraseBlobArray(pBlob);	\
}

inline void ZPostAccountCharInfo(int nCharNum)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_ACCOUNT_CHARINFO, MCommandParameterChar((char)nCharNum));
}

// ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostSelectMyChar(/*const MUID&*/ uidChar, /*const int*/ nCharIndex) {	\
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_REQUEST_SELECT_CHAR, 33333), MCommandParameterUID(uidChar), MCommandParameterUInt(nCharIndex));	\
}

inline void ZPostDeleteMyChar(const MUID& uidChar, const int nCharIndex, char* szCharName)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_DELETE_CHAR, MCommandParameterUID(uidChar), MCommandParameterUInt(nCharIndex),
		MCommandParameterString(szCharName));
}

inline void ZPostCreateMyChar(const MUID& uidChar, const int nCharIndex, char* szCharName,
							  const int nSex, const int nHair, const int nFace, const int nCostume)
{	
	ZPOSTCMD7(MC_MATCH_REQUEST_CREATE_CHAR, MCommandParameterUID(uidChar), MCommandParameterUInt(nCharIndex),
		MCommandParameterString(szCharName), MCommandParameterUInt(nSex), MCommandParameterUInt(nHair),
		MCommandParameterUInt(nFace), MCommandParameterUInt(nCostume));

	CHECK_RETURN_CALLSTACK(ZPostCreateMyChar);
}

inline void ZPostSimpleCharInfo(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_SIMPLE_CHARINFO, MCommandParameterUID(uidChar));
}

inline void ZPostFinishedRoundInfo(const MUID& uidStage, MUID& uidChar, MTD_RoundPeerInfo* pPeerInfo, 
								   int nPeerInfoCount, MTD_RoundKillInfo* pKillInfo, int nKillInfoCount)
{
	void* pBlobPeerInfo = MMakeBlobArray(sizeof(MTD_RoundPeerInfo), nPeerInfoCount);
	for (int i = 0; i < nPeerInfoCount; i++)
	{
		MTD_RoundPeerInfo* pNodePeerInfo = (MTD_RoundPeerInfo*)MGetBlobArrayElement(pBlobPeerInfo, i);
		CopyMemory(pNodePeerInfo, &pPeerInfo[i], sizeof(MTD_RoundPeerInfo));
	}

	void* pBlobKillInfo = MMakeBlobArray(sizeof(MTD_RoundKillInfo), nKillInfoCount);
	for (int i = 0; i < nKillInfoCount; i++)
	{
		MTD_RoundKillInfo* pNodeKillInfo = (MTD_RoundKillInfo*)MGetBlobArrayElement(pBlobKillInfo, i);
		CopyMemory(pNodeKillInfo, &pKillInfo[i], sizeof(MTD_RoundKillInfo));
	}

	ZPOSTCMD4(MC_MATCH_ROUND_FINISHINFO, MCommandParameterUID(uidStage), MCommandParameterUID(uidChar), 
		MCommandParameterBlob(pBlobPeerInfo, MGetBlobArraySize(pBlobPeerInfo)),
		MCommandParameterBlob(pBlobKillInfo, MGetBlobArraySize(pBlobKillInfo)));

	MEraseBlobArray(pBlobPeerInfo);
	MEraseBlobArray(pBlobKillInfo);
}

inline void ZPostRequestBuyItem(const MUID& uidChar, unsigned int nItemID, unsigned int nItemCnt = 1)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_BUY_ITEM, MCommandParameterUID(uidChar), MCommandParameterUInt(nItemID), MCommandParameterUInt(nItemCnt));
}

inline void ZPostRequestSellItem(const MUID& uidChar, const MUID& uidItem, unsigned int nItemCnt = 1)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_SELL_ITEM, MCommandParameterUID(uidChar), MCommandParameterUID(uidItem), MCommandParameterUInt(nItemCnt));
}

inline void ZPostRequestForcedEntry(const MUID& uidChar, const MUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_FORCED_ENTRY, MCommandParameterUID(uidChar), MCommandParameterUID(uidStage));
}

inline void ZPostRequestShopItemList(const MUID& uidChar, const int nFirstItemIndex, const int nItemCount)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_SHOP_ITEMLIST, MCommandParameterUID(uidChar), 
		MCommandParameterInt(nFirstItemIndex), MCommandParameterInt(nItemCount));
}

inline void ZPostRequestCharacterItemList(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_CHARACTER_ITEMLIST, MCommandParameterUID(uidChar));
}

inline void ZPostRequestCharacterItemListForce(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_CHARACTER_ITEMLIST_FORCE, MCommandParameterUID(uidChar));
}

inline void ZPostRequestAccountItemList(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_ACCOUNT_ITEMLIST, MCommandParameterUID(uidChar));
}

inline void ZPostRequestBringAccountItem(const MUID& uidChar, const int nAIID, const int nItemID, const int nItemCnt)
{
	ZPOSTCMD4(MC_MATCH_REQUEST_BRING_ACCOUNTITEM, MCommandParameterUID(uidChar), MCommandParameterInt(nAIID), MCommandParameterInt(nItemID), MCommandParameterInt(nItemCnt));
}
inline void ZPostRequestSendAccountItem(const MUID& uidChar, const MUID& uidItem, const int nItemCnt=1)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_BRING_BACK_ACCOUNTITEM, MCommandParameterUID(uidChar), MCommandParameterUID(uidItem), MCommandParameterUInt(nItemCnt));
}

inline void ZPostRequestEquipItem(const MUID& uidChar, const MUID& uidItem, const MMatchCharItemParts parts)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_EQUIP_ITEM, MCommandParameterUID(uidChar), 
		MCommandParameterUID(uidItem), MCommandParameterUInt(unsigned long int(parts))); 
}

inline void ZPostRequestTakeoffItem(const MUID& uidChar, const MMatchCharItemParts parts)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_TAKEOFF_ITEM, MCommandParameterUID(uidChar), 
		MCommandParameterUInt(unsigned long int(parts))); 
}

// ¥Ÿ∏• ªÁ∂˜¿« «√∑π¿ÃæÅE¡§∫∏ ø‰√ª
inline void ZPostRequestCharInfoDetail(const MUID& uidChar, const char* pszCharName)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_CHARINFO_DETAIL, MCommandParameterUID(uidChar), MCmdParamStr(const_cast<char*>(pszCharName)));
}


inline void ZPostFriendAdd(const char* pszName)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_ADD, MCmdParamStr(const_cast<char*>(pszName)));
}

inline void ZPostFriendRemove(const char* pszName)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_REMOVE, MCmdParamStr(const_cast<char*>(pszName)));
}

inline void ZPostFriendList()
{
	ZPOSTCMD0(MC_MATCH_FRIEND_LIST);
}

inline void ZPostFriendMsg(const char* pszMsg)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_MSG, MCmdParamStr(const_cast<char*>(pszMsg)));
}

// ¿⁄ªÅE‰√ª - ¡˜¡¢»£√‚«œ¡ˆ∏ª∞ÅEZGameClient::RequestGameSuicide() ∏¶ »£√‚«œµµ∑œ «œ¿⁄.
inline void ZPostRequestSuicide(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_SUICIDE, MCommandParameterUID(uidChar));
}

inline void ZPostAdminTerminal(const MUID& uidChar, char* szMsg)
{
	ZPOSTCMD2(MC_ADMIN_TERMINAL, MCommandParameterUID(uidChar), MCommandParameterString(szMsg));
}

inline void ZPostRequestGameInfo(const MUID& uidChar, const MUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_GAME_INFO, MCommandParameterUID(uidChar), 
		MCommandParameterUID(uidStage));
}

inline void ZPostRequestRecommendChannel()
{
	ZPOSTCMD0(MC_MATCH_REQUEST_RECOMMANDED_CHANNEL);
}

inline void ZPostRequestStageList(const MUID& uidChar, const MUID& uidChannel, int nStageCursor)
{
	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();
	if ((nNowTime - st_nLastTime) > 500)
	{
		ZPOSTCMD3(MC_MATCH_REQUEST_STAGE_LIST, MCommandParameterUID(uidChar),
			MCommandParameterUID(uidChannel), MCommandParameterInt(nStageCursor));

		st_nLastTime = nNowTime;
	}
}

inline void ZPostRequestChannelPlayerList(const MUID& uidChar, const MUID& uidChannel, const int nPage)
{
	ZPOSTCMD3(MC_MATCH_CHANNEL_REQUEST_PLAYER_LIST, MCommandParameterUID(uidChar),
		MCommandParameterUID(uidChannel), MCommandParameterInt(nPage));
}

inline void ZPostRequestObtainWorldItem(const MUID& uidChar, const int nItemUID)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_OBTAIN_WORLDITEM, MCommandParameterUID(uidChar),
		MCommandParameterInt(nItemUID));
}

inline void ZPostRequestSpawnWorldItem(const MUID& uidChar, const int nItemID, const rvector& pos, float fDropDelayTime)
{
	ZPOSTCMD4(MC_MATCH_REQUEST_SPAWN_WORLDITEM, MCommandParameterUID(uidChar),
		MCommandParameterInt(nItemID), MCommandParameterPos(pos.x, pos.y, pos.z), MCommandParameterFloat(fDropDelayTime));
}

inline void ZPostLadderInvite(const MUID& uidChar, const int nRequestID, char* szTeamName,
							char** ppMemberCharNames, int nMemberCharNamesCount)
{
}

inline void ZPostWhisper(char* pszSenderName, char* pszTargetName, char* pszMessage)
{
	ZPOSTCMD3(MC_MATCH_USER_WHISPER, MCmdParamStr(pszSenderName), 
			MCmdParamStr(pszTargetName), MCmdParamStr(pszMessage));
}

inline void ZPostWhere(char* pszTargetName)
{
	ZPOSTCMD1(MC_MATCH_USER_WHERE, MCmdParamStr(pszTargetName));
}

void ZPostUserOption();

inline void ZPostChatRoomCreate(const MUID& uidPlayer, char* pszChatRoomName)
{
	ZPOSTCMD2(MC_MATCH_CHATROOM_CREATE, MCmdParamUID(uidPlayer),
			MCmdParamStr(pszChatRoomName));
}

inline void ZPostChatRoomJoin(char* pszChatRoomName)
{
	ZPOSTCMD2( MC_MATCH_CHATROOM_JOIN, MCmdParamStr(""),
			MCmdParamStr(pszChatRoomName) );
}

inline void ZPostChatRoomLeave(char* pszChatRoomName)
{
	ZPOSTCMD2( MC_MATCH_CHATROOM_LEAVE, MCmdParamStr(""),
			MCmdParamStr(pszChatRoomName) );
}

inline void ZPostSelectChatRoom(char* pszChatRoomName)
{
	ZPOSTCMD1( MC_MATCH_CHATROOM_SELECT_WRITE, MCmdParamStr(pszChatRoomName) );
}

inline void ZPostInviteChatRoom(char* pszPlayerName)
{
	ZPOSTCMD3( MC_MATCH_CHATROOM_INVITE, MCmdParamStr(""), MCmdParamStr(pszPlayerName),
			MCmdParamStr("") );
}

inline void ZPostChatRoomChat(const char* pszChat)
{
	ZPOSTCMD3( MC_MATCH_CHATROOM_CHAT, MCmdParamStr(""), MCmdParamStr(""), MCmdParamStr(pszChat) );
}

inline void ZPostRequestMySimpleCharInfo(const MUID& uidChar)
{
	ZPOSTCMD1( MC_MATCH_REQUEST_MY_SIMPLE_CHARINFO, MCmdParamUID(uidChar) );
}


inline void ZPostRequestCopyToTestServer(const MUID& uidChar)
{
	return;	// ªÁøÅEœ¡ÅEæ ¿Ω
	ZPOSTCMD1(MC_MATCH_REQUEST_COPY_TO_TESTSERVER, MCmdParamUID(uidChar) );

}


// ƒÅE∂¿Œ ø‰√ª
inline void ZPostRequestQuickJoin(const MUID& uidChar, MTD_QuickJoinParam* pParam)
{
	void* pBlob = MMakeBlobArray(sizeof(MTD_QuickJoinParam), 1);
	MTD_QuickJoinParam* pBlobNode = (MTD_QuickJoinParam*)MGetBlobArrayElement(pBlob, 0);
	CopyMemory(pBlobNode, pParam, sizeof(MTD_QuickJoinParam));

	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_QUICKJOIN, MCommandParameterUID(uidChar), MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));
	MEraseBlobArray(pBlob);
}


// ≈¨∑£∞ÅE√ //////////////////////////////////////////////////////////////////////////////////////////
#ifndef _2PCLAN
inline void ZPostRequestCreateClan(const MUID& uidChar, const int nRequestID, char* szClanName,
	char** ppMemberCharNames, int nMemberCharNamesCount)
{
	if (nMemberCharNamesCount != CLAN_SPONSORS_COUNT) return;
	ZPOSTCMD7(MC_MATCH_CLAN_REQUEST_CREATE_CLAN, MCmdParamUID(uidChar), MCmdParamInt(nRequestID), MCmdParamStr(szClanName),
		MCmdParamStr(ppMemberCharNames[0]), MCmdParamStr(ppMemberCharNames[1]),
		MCmdParamStr(ppMemberCharNames[2]), MCmdParamStr(ppMemberCharNames[3]));

}
#else
inline void ZPostRequestCreateClan(const MUID& uidChar, const int nRequestID, char* szClanName)
{
	ZPOSTCMD3(MC_MATCH_CLAN_REQUEST_CREATE_CLAN, MCmdParamUID(uidChar), MCmdParamInt(nRequestID), MCmdParamStr(szClanName));
}
#endif // !_2PCLAN
//Banderas
#ifdef _COUNTRYFLAG
inline void ZPostSendCountryCode(const char* pszPlayerName, const int nCountryCode)
{
	ZPOSTCMD2(MC_GUNZ_SEND_COUNTRYCODE, MCmdParamStr(pszPlayerName), MCmdParamInt(nCountryCode));
}
#endif
#ifdef _CW_VOTE
inline void ZPostLadderWarVote(int Map)
{
	ZPOSTCMD1(MC_MATCH_VOTE, MCmdParamInt(Map));
}
#endif // _CW_VOTE
inline void ZPostJoinLadderWar(int Arg1, int Arg2)
{
	ZPOSTCMD2(MC_MATCH_JOIN_LADDERWARS, MCmdParamInt(Arg1), MCmdParamInt(Arg2));
}
inline void ZPostAnswerSponsorAgreement(const int nRequestID, const MUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer)
{
	ZPOSTCMD4(MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT, MCmdParamInt(nRequestID), MCmdParamUID(uidClanMaster), 
		MCmdParamStr(szSponsorCharName), MCmdParamBool(bAnswer));
}

inline void ZPostRequestAgreedCreateClan(const MUID& uidChar, char* szClanName, char** ppMemberCharNames, int nMemberCharNamesCount)
{
	// Custom: New IJJI clan changes.
	//if (nMemberCharNamesCount != CLAN_SPONSORS_COUNT) return;
	ZPOSTCMD6(MC_MATCH_CLAN_REQUEST_AGREED_CREATE_CLAN, MCmdParamUID(uidChar), MCmdParamStr(szClanName), 
				MCmdParamStr(ppMemberCharNames[0]), MCmdParamStr(ppMemberCharNames[1]),
				MCmdParamStr(ppMemberCharNames[2]), MCmdParamStr(ppMemberCharNames[3]));
}

// ≈¨∑£ ∆ÛºÅEø‰√ª
inline void ZPostRequestCloseClan(const MUID& uidChar, char* szClanName)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_CLOSE_CLAN, MCmdParamUID(uidChar), MCmdParamStr(szClanName));
}

// ≈¨∑£ ∞°¿‘ √≥∏Æ
inline void ZPostRequestJoinClan(const MUID& uidChar, const char* szClanName, const char* szJoiner)
{
	ZPOSTCMD3(MC_MATCH_CLAN_REQUEST_JOIN_CLAN, MCmdParamUID(uidChar), MCmdParamStr(szClanName), MCmdParamStr(szJoiner));
}

// ≈¨∑£ ∞°¿‘ µø¿« ¿¿¥ÅE
inline void ZPostAnswerJoinAgreement(const MUID& uidClanAdmin, const char* szJoiner, const bool bAnswer)
{
	ZPOSTCMD3(MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT, MCmdParamUID(uidClanAdmin), MCmdParamStr(szJoiner), MCmdParamBool(bAnswer));
}

// ≈¨∑£ µø¿««— ∞°¿‘√≥∏Æ ø‰√ª
inline void ZPostRequestAgreedJoinClan(const MUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	ZPOSTCMD3(MC_MATCH_CLAN_REQUEST_AGREED_JOIN_CLAN, MCmdParamUID(uidClanAdmin), MCmdParamStr(szClanName), MCmdParamStr(szJoiner));
}

// ≈¨∑£ ≈ª≈ÅEø‰√ª
inline void ZPostRequestLeaveClan(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_LEAVE_CLAN, MCmdParamUID(uidChar));
}

// ≈¨∑£ ∏‚πÅE±««— ∫Ø∞ÅEø‰√ª
inline void ZPostRequestChangeClanGrade(const MUID& uidClanAdmin, const char* szMember, int nClanGrade)
{
	ZPOSTCMD3(MC_MATCH_CLAN_MASTER_REQUEST_CHANGE_GRADE, MCmdParamUID(uidClanAdmin), MCmdParamStr(szMember), MCmdParamInt(nClanGrade));

}

// ≈¨∑£ ∏‚πÅE≈ª≈ÅEø‰√ª
inline void ZPostRequestExpelClanMember(const MUID& uidClanAdmin, const char* szMember)
{
	ZPOSTCMD2(MC_MATCH_CLAN_ADMIN_REQUEST_EXPEL_MEMBER, MCmdParamUID(uidClanAdmin), MCmdParamStr(szMember));
}


// √§≥Œ¿« ∏µÅE«√∑π¿ÃæÅE∏ÆΩ∫∆Æ ø‰√ª
inline void ZPostRequestChannelAllPlayerList(const MUID& uidChar, const MUID& uidChannel, const unsigned long int nPlaceFilter,
											 const unsigned long int nOptions)
{
	ZPOSTCMD4(MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST, MCommandParameterUID(uidChar),
		MCommandParameterUID(uidChannel), MCommandParameterUInt(nPlaceFilter), MCommandParameterUInt(nOptions));
}

// ≈¨∑£ ∏ﬁºº¡ÅE
inline void ZPostClanMsg(const MUID& uidSender, const char* pszMsg)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_MSG, MCmdParamUID(uidSender), MCmdParamStr(const_cast<char*>(pszMsg)));
}

inline void ZPostRequestClanMemberList(const MUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_MEMBER_LIST, MCmdParamUID(uidChar));
}

// ≈¨∑£ ¡§∫∏ ø‰√ª - ≈¨∑£¿ÅE≠πˆø°º≠∏∏ ªÁøÅE—¥Ÿ.
inline void ZPostRequestClanInfo(const MUID& uidChar, const char* szClanName)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_CLAN_INFO, MCmdParamUID(uidChar), MCmdParamStr(szClanName));
}


// ¥Ÿ∏•ªÁ∂˜¿« µø¿« ø‰√ª - ¿Ã«‘ºˆ¥¬ ¡˜¡¢ªÁøÅEœ¡ˆ∏ª∞ÅEZGameClient::RequestProposal∏¶ ¿ÃøÅEÿæﬂ«—¥Ÿ.
inline void ZPostRequestProposal(const MUID& uidChar, const int nProposalMode, const int nRequestID,
								 char** ppReplierCharNames, const int nReplierCount)
{
	void* pBlobRepliersName = MMakeBlobArray(sizeof(MTD_ReplierNode), nReplierCount);
	for (int i = 0; i < nReplierCount; i++)
	{
		MTD_ReplierNode* pReplierNode = (MTD_ReplierNode*)MGetBlobArrayElement(pBlobRepliersName, i);
		strcpy(pReplierNode->szName, ppReplierCharNames[i]);
	}

	ZPOSTCMD5(MC_MATCH_REQUEST_PROPOSAL, MCmdParamUID(uidChar), MCmdParamInt(nProposalMode),
		MCmdParamInt(nRequestID), MCmdParamInt(nReplierCount), 
		MCmdParamBlob(pBlobRepliersName, MGetBlobArraySize(pBlobRepliersName)));

	MEraseBlobArray(pBlobRepliersName);
}


// µø¿« ¿¿¥ÅE- ¿Ã«‘ºˆ¥¬ ¡˜¡¢ªÁøÅEœ¡ˆ∏ª∞ÅEZGameClient::ReplyAgreement∏¶ ¿ÃøÅEÿæﬂ«—¥Ÿ.
inline void ZPostReplyAgreement(const MUID& uidProposer, const MUID& uidChar, char* szReplierName, int nProposalMode, 
								int nRequestID, bool bAgreement)
{
	ZPOSTCMD6(MC_MATCH_REPLY_AGREEMENT, MCmdParamUID(uidProposer), MCmdParamUID(uidChar),
		MCmdParamStr(szReplierName), MCmdParamInt(nProposalMode), MCmdParamInt(nRequestID), MCmdParamBool(bAgreement));
}


// ∑°¥ı∞‘¿” Ω≈√ª
inline void ZPostLadderRequestChallenge(char** ppMemberCharNames, const int nMemberCount, unsigned long int nOptions, unsigned long int nGameType)
{
	void* pBlobMembersName = MMakeBlobArray(sizeof(MTD_ReplierNode), nMemberCount);
	for (int i = 0; i < nMemberCount; i++)
	{
		MTD_LadderTeamMemberNode* pMemberNode = (MTD_LadderTeamMemberNode*)MGetBlobArrayElement(pBlobMembersName, i);
		strcpy(pMemberNode->szName, ppMemberCharNames[i]);
	}

	ZPOSTCMD4(MC_MATCH_LADDER_REQUEST_CHALLENGE, MCmdParamInt(nMemberCount), MCmdParamUInt(nOptions),
		MCmdParamBlob(pBlobMembersName, MGetBlobArraySize(pBlobMembersName)), MCmdParamUInt(nGameType));


	MEraseBlobArray(pBlobMembersName);
}

inline void ZPostLadderCancel(bool BlitzKrieg)
{
	ZPOSTCMD1(MC_MATCH_LADDER_REQUEST_CANCEL_CHALLENGE, MCmdParamBool(BlitzKrieg));
}

inline void ZPostBlitzKriegsInvite(char* name)
{
	ZPOSTCMD1(MC_MATCH_LADDERWARS_FRIENDINVITE, MCmdParamStr(name));
}

inline void ZPostBlitzKriegsAccept()
{
	ZPOSTCMD0(MC_MATCH_LADDERWARS_FRIENDACCEPT);
}

inline void ZPostBlitzKriegsLeave()
{
	ZPOSTCMD0(MC_MATCH_LADDERWARS_CHARINFO);
}

// Duel Tournament ///////////////////////////////////////////////////////////////////////////////////
#ifdef _DUELTOURNAMENT
inline void ZPostDuelTournamentRequestJoinGame(const MUID& uidChar, MDUELTOURNAMENTTYPE nType)
{
	ZPOSTCMD2( MC_MATCH_DUELTOURNAMENT_REQUEST_JOINGAME, MCmdParamUID(uidChar), MCmdParamInt((int)nType));
}

inline void ZPostDuelTournamentRequestCancelGame(const MUID& uidChar, MDUELTOURNAMENTTYPE nType)
{
	ZPOSTCMD2( MC_MATCH_DUELTOURNAMENT_REQUEST_CANCELGAME, MCmdParamUID(uidChar), MCmdParamInt((int)nType));
}

inline void ZPostDuelTournamentRequestSideRankingInfo(const MUID& uidChar)
{
	static char szCharName[MATCHOBJECT_NAME_LENGTH] = { -1, 0, };	// ZGetMyInfo()->GetCharName()∞° ""∏¶ ∏Æ≈œ«“ ºÅE¿÷¿∏π«∑Œ ¿Ã∫Øºˆ∏¶ ""∑Œ √ ±‚»≠«œ∏ÅEø¿¿€µø ¿œæ˚œ≤
	static DWORD timeLastReqeust = 0;
	DWORD currTime = timeGetTime();

	// ≥ π´ ¿⁄¡÷ ø‰√ª«œ¡ÅEæ µµ∑œ ¡¶«—
	// ƒ≥∏Ø≈Õ ∫Ø∞Ê¿ª «— ∞ÊøÅE¬ ±◊≥… ø‰√ª
	if (timeLastReqeust==0 || currTime-timeLastReqeust > 10*1000 || stricmp(ZGetMyInfo()->GetCharName(), szCharName)!=0)
	{
		timeLastReqeust = currTime;
		strcpy(szCharName, ZGetMyInfo()->GetCharName());
		ZPOSTCMD1( MC_MATCH_DUELTOURNAMENT_REQUEST_SIDERANKING_INFO, MCmdParamUID(uidChar));
	}
}

inline void ZPostDuelTournamentGamePlayerStatus(const MUID& uidChar, float fAccumulationDamage, float fHP, float fAP)
{
	ZPOSTCMD4( MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS, MCmdParamUID(uidChar), MCmdParamFloat(fAccumulationDamage), MCmdParamFloat(fHP), MCmdParamFloat(fAP));
}
#endif //_DUELTOURNAMENT


// Admin /////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostAdminPingToAll()
{
	ZPOSTCMD0(MC_ADMIN_PING_TO_ALL);
}


inline void ZPostAdminRequestKickPlayer(char* pszTargetPlayerName)
{
	ZPOSTCMD1(MC_ADMIN_REQUEST_KICK_PLAYER, MCmdParamStr(pszTargetPlayerName));
}

/*inline void ZPostAdminKickAll()
{
	ZPOSTCMD0(MC_ADMIN_KICK_ALL);
}*/

inline void ZPostAdminRequestMutePlayer(char* pszTargetPlayerName, int nPunishHour)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_MUTE_PLAYER, MCmdParamStr(pszTargetPlayerName), MCmdParamInt(nPunishHour));
}

inline void ZPostAdminRequestBlockPlayer(char* pszTargetPlayerName, int nPunishHour)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_BLOCK_PLAYER, MCmdParamStr(pszTargetPlayerName), MCmdParamInt(nPunishHour));
}

inline void ZPostAdminRequestSwitchLadderGame(const MUID& uidChar, bool bEnabled)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_SWITCH_LADDER_GAME, MCmdParamUID(uidChar), MCmdParamBool(bEnabled));
}


inline void ZPostAdminAnnounce(const MUID& uidChar, char* szMsg, ZAdminAnnounceType nType)
{
	ZPOSTCMD3(MC_ADMIN_ANNOUNCE, MCommandParameterUID(uidChar), 
		MCommandParameterString(szMsg), MCommandParameterUInt(nType));
}

inline void ZPostAdminHalt(const MUID& uidChar)
{
	ZPOSTCMD1(MC_ADMIN_SERVER_HALT, MCommandParameterUID(uidChar) );
}

inline void ZPostAdminReloadClientHash()
{
	ZPOSTCMD0(MC_ADMIN_RELOAD_CLIENT_HASH);
}

inline void ZPostAdminResetAllHackingBlock()
{
	ZPOSTCMD0(MC_ADMIN_RESET_ALL_HACKING_BLOCK);
}

inline void ZPostAdminReloadGambleItem()
{
	ZPOSTCMD0(MC_ADMIN_RELOAD_GAMBLEITEM);
}

inline void ZPostAdminDumpGambleItemLog()
{
	ZPOSTCMD0(MC_ADMIN_DUMP_GAMBLEITEM_LOG);
}

inline void ZPostAdminAssasin()
{
	ZPOSTCMD0(MC_ADMIN_ASSASIN);
}

inline void ZPostVoiceChat(const void* Buffer, int Length)
{
	ZPOSTCMD1(MC_MATCH_SEND_VOICE_CHAT, MCmdParamBlob(Buffer, Length));
}

// Event /////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostChangeMaster()
{
	ZPOSTCMD0(MC_EVENT_CHANGE_MASTER);
}

inline void ZPostChangePassword(char* pszPassword)
{
	ZPOSTCMD1(MC_EVENT_CHANGE_PASSWORD, MCmdParamStr(pszPassword));
}

inline void ZPostAdminHide()
{
	ZPOSTCMD0(MC_ADMIN_HIDE);
}

inline void ZPostAdminRequestJjang(char* pszTargetName)
{
	ZPOSTCMD1(MC_EVENT_REQUEST_JJANG, MCmdParamStr(pszTargetName));
}

inline void ZPostAdminRemoveJjang(char* pszTargetName)
{
	ZPOSTCMD1(MC_EVENT_REMOVE_JJANG, MCmdParamStr(pszTargetName));
}


// Emblem ////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostRequestEmblemURL(void* pBlob)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_EMBLEMURL, MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));
}

inline void ZPostClanEmblemReady(unsigned int nCLID, char* pszEmblemURL)
{
	ZPOSTCMD2(MC_MATCH_CLAN_LOCAL_EMBLEMREADY, MCmdParamInt(nCLID), MCmdParamStr(pszEmblemURL));
}

// Quest /////////////////////////////////////////////////////////////////////////////////////////////
// ∞≠¡¶¿Œ∂Û¿Ã¥◊
#define ZPostQuestRequestNPCDead(/*const MUID&*/ uidKiller, /*const MUID&*/ uidNPC, /*rvector&*/ vPos) {	\
	ZPOSTCMD3(CLOAK_CMD_ID(MC_QUEST_REQUEST_NPC_DEAD, 24781), MCmdParamUID(uidKiller), MCmdParamUID(uidNPC), MCmdParamShortVector(vPos.x, vPos.y, vPos.z));	\
}

inline void ZPostQuestPeerNPCDead(const MUID& uidKiller, const MUID& uidNPC)
{
	ZPOSTCMD2(MC_QUEST_PEER_NPC_DEAD, MCmdParamUID(uidKiller), MCmdParamUID(uidNPC));
}


inline void ZPostQuestGameKill()
{
	ZPOSTCMD0(MC_MATCH_QUEST_REQUEST_DEAD);
}

#ifdef _DEBUG
inline void ZPostQuestTestNPCSpawn(const int nNPCType, const int nNPCCount)
{
	ZPOSTCMD2(MC_QUEST_TEST_REQUEST_NPC_SPAWN, MCmdParamInt(nNPCType), MCmdParamInt(nNPCCount));
}
#endif


#ifdef _DEBUG
inline void ZPostQuestTestClearNPC()
{
	ZPOSTCMD0(MC_QUEST_TEST_REQUEST_CLEAR_NPC);
}
#endif


#ifdef _DEBUG
inline void ZPostQuestTestSectorClear()
{
	ZPOSTCMD0(MC_QUEST_TEST_REQUEST_SECTOR_CLEAR);
}
#endif


#ifdef _DEBUG
inline void ZPostQuestTestFinish()
{
	ZPOSTCMD0(MC_QUEST_TEST_REQUEST_FINISH);
}
#endif


inline void ZPostQuestRequestMovetoPortal(const char nCurrSectorIndex)
{
	ZPOSTCMD1(MC_QUEST_REQUEST_MOVETO_PORTAL, MCmdParamChar(nCurrSectorIndex));
}

inline void ZPostQuestReadyToNewSector(const MUID& uidPlayer)
{
	ZPOSTCMD1(MC_QUEST_READYTO_NEWSECTOR, MCmdParamUID(uidPlayer));
}
#ifdef _SPEC 1
inline void ZPostSpec(bool Value)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_SPEC, MCmdParamBool(Value));
}
#endif
inline void ZPostQuestPong(unsigned long int nTime)
{
	ZPOSTCMD1(MC_QUEST_PONG, MCmdParamUInt(nTime));
}
#ifdef _SRVRPNG
inline void ZPostServerPing(const unsigned int nTimeStamp, int nType)
{
	ZPOSTCMD2(MC_SERVER_PING, MCommandParameterUInt(nTimeStamp), MCommandParameterInt(nType));
}
#endif
inline void ZPostGiveCoinsCash(char* pszTargetName, const int nCash)
{
	ZPOSTCMD2(MC_ADMIN_GIVE_COINSCASH, MCmdParamStr(pszTargetName), MCmdParamInt(nCash));
}
inline void ZPostGiveCoinsMedals(char* pszTargetName, const int nLC)
{
	ZPOSTCMD2(MC_ADMIN_GIVE_COINSMEDALS, MCmdParamStr(pszTargetName), MCmdParamInt(nLC));
}
inline void ZPostAdminStageJoin(MUID uidStage, char* trg)
{
	ZPOSTCMD2(MC_ADMIN_STAGE_JOIN, MCmdParamUID(uidStage), MCmdParamStr(trg));
}
inline void ZPostAdminSendItem(char* pszTargetName, int nItemId, int nDays)
{
	ZPOSTCMD3(MC_ADMIN_SENDITEM, MCmdParamStr(pszTargetName), MCommandParameterInt(nItemId), MCommandParameterInt(nDays));
}
#ifdef _STAFFCHAT
inline void ZPostStaffChat(char* szMsg)
{
	ZPOSTCMD3(MC_STAFF_CHAT, MCommandParameterString(szMsg), MCommandParameterString(""), MCommandParameterInt(0));
}
#endif
#ifdef _QUEST_ITEM
inline void ZPostRequestGetCharQuestItemInfo( const MUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_CHAR_QUEST_ITEM_LIST, MCmdParamUID(uid) );
}

inline void ZPostRequestBuyQuestItem( const MUID& uid, const unsigned long int nItemID, const int nCount = 1 )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_BUY_QUEST_ITEM, MCmdParamUID(uid), MCommandParameterInt(nItemID), MCommandParameterInt(nCount) );
}

inline void ZPostRequestSellQuestItem( const MUID& uid, const unsigned long int nItemID, const int nCount = 1 )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_SELL_QUEST_ITEM, MCmdParamUID(uid), MCommandParameterInt(nItemID), MCommandParameterInt(nCount) );
}

inline void ZPostRequestDropSacrificeItem( const MUID& uid, const int nSlotIndex, const unsigned long int nItemID )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_DROP_SACRIFICE_ITEM, MCmdParamUID(uid), MCommandParameterInt(nSlotIndex), MCommandParameterInt(nItemID) );
}

inline void ZPostRequestCallbackSacrificeItem( const MUID& uid, const int nSlotIndex, const unsigned long int nItemID )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_CALLBACK_SACRIFICE_ITEM, MCmdParamUID(uid), MCommandParameterInt(nSlotIndex), MCommandParameterInt(nItemID) );
}

inline void ZPostRequestQL( const MUID& uid )
{
	ZPOSTCMD1( MC_QUEST_REQUEST_QL, MCmdParamUID(uid) );
}

inline void ZPostRequestSacrificeSlotInfo( const MUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_SLOT_INFO, MCmdParamUID(uid) );
}

inline void ZPostQuestStageMapset(const MUID& uidStage, int nMapsetID)
{
	ZPOSTCMD2(MC_QUEST_STAGE_MAPSET, MCommandParameterUID(uidStage), MCommandParameterChar((char)nMapsetID));
}

inline void ZPostRequestMonsterBibleInfo( const MUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_MONSTER_BIBLE_INFO, MCmdParamUID(uid) );
}

inline void ZPostResponseXTrapSeedKey(unsigned char *szComBuf)			// Update sgk 0706
{
	void *pBlob = MMakeBlobArray(sizeof(unsigned char), 128);
	unsigned char *pCmdBlock = (unsigned char*)MGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, szComBuf, 128);

	ZPOSTCMD1(MC_RESPONSE_XTRAP_SEEDKEY, MCommandParameterBlob(pBlob, MGetBlobArraySize(pBlob)));

	MEraseBlobArray(pBlob);
}
inline void ZPostAdminFreezeAll(int State)
{
	ZPOSTCMD1(MC_ADMIN_FREEZE_ALL, MCmdParamInt(State));
}
inline void ZPsotResponseGameguardAuht( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	ZPOSTCMD4( MC_RESPONSE_GAMEGUARD_AUTH, MCmdParamUInt(dwIndex), MCmdParamUInt(dwValue1), MCmdParamUInt(dwValue2), MCmdParamUInt(dwValue3) );
}
#ifdef _ANTISPAM
inline void ZPostAdminWall(const MUID& uidChar, char* szMsg, const char* szChar)
{
	ZPOSTCMD3(MC_ADMIN_WALL, MCommandParameterUID(uidChar), MCommandParameterString(szMsg), MCommandParameterString(szChar));
}
#endif
inline void ZPostResponseFirstGameguardAuth( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	ZPOSTCMD4( MC_RESPONSE_FIRST_GAMEGUARD_AUTH, MCmdParamUInt(dwIndex), MCmdParamUInt(dwValue1), MCmdParamUInt(dwValue2), MCmdParamUInt(dwValue3) );
}

inline void ZPostRequestGamble( const MUID& uid)
{
	ZPOSTCMD1( MC_MATCH_REQUEST_GAMBLE, MCmdParamUID(uid) );
}

inline void ZPostResponseResourceCRC32( DWORD dwCRC, DWORD dwXOR )
{
	if( 0 == dwCRC )
	{
		_ASSERT( 0 != dwCRC );
		return;
	}

	ZPOSTCMD2( MC_RESPONSE_RESOURCE_CRC32, MCmdParamUInt(dwCRC), MCmdParamUInt(dwXOR) );
}

inline void ZPostRequestUseSpendableNormalItem( const MUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_USE_SPENDABLE_NORMAL_ITEM, MCmdParamUID(uid) );
}

inline void ZPostRequestUseSpendableBuffItem( const MUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_USE_SPENDABLE_BUFF_ITEM, MCmdParamUID(uid) );
}

#endif


inline void ZPostNewQuestRequestNPCDead(const MUID& uidKiller, const MUID& uidNPC, const rvector& vPos)
{
	ZPOSTCMD3(CLOAK_CMD_ID(MC_NEWQUEST_REQUEST_NPC_DEAD, 84781), MCmdParamUID(uidKiller), MCmdParamUID(uidNPC), MCmdParamShortVector(vPos.x, vPos.y, vPos.z));
}

inline void ZPostRequestUpdateSkillMapBestTime(const MUID& player, const int& mapID, unsigned int bestTime)
{
	ZPOSTCMD3(MC_MATCH_UPDATE_SKILLMAP_BESTTIME, MCmdParamUID(player), MCmdParamInt(mapID), MCmdParamInt(bestTime));
}

inline void ZPostRequestSkillMapBestTime(const MUID& player, const int& mapName)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_SKILLMAP_BESTTIME, MCmdParamUID(player), MCmdParamInt(mapName));
}

inline void ZPostNewQuestPeerNpcActionExecute(const MUID& uid, int nActionIndex)
{
	ZPOSTCMD2(CLOAK_CMD_ID(MC_NEWQUEST_PEER_NPC_ACTION_EXECUTE, 99975), MCmdParamUID(uid), MCmdParamInt(nActionIndex) );
}
inline void ZPostNewQuestRequestNpcSpawn(const MUID& uid, const MUID& uid2, const int num, const char* szStr, const rvector& pos, const rvector& dir, const int route)
{
	ZPOSTCMD7(MC_NEWQUEST_REQUEST_NPC_SPAWN, MCmdParamUID(uid), MCmdParamUID(uid2), MCmdParamInt(num), MCmdParamStr(szStr), MCmdParamShortVector(pos.x, pos.y, pos.z), MCmdParamShortVector(dir.x, dir.y, dir.z),
		MCmdParamInt(route));
}
////////////////////////////////////////////////////////////
inline void ZPostActivateSpyMap(int nMapID, bool bExclude)
{
	ZPOSTCMD2(MC_SPY_STAGE_ACTIVATE_MAP, MCmdParamInt(nMapID), MCmdParamBool(bExclude));
}
inline void ZPostFreeze(int Type, char* Name)
{
	ZPOSTCMD2(MC_MATCH_FREEZESPY, MCmdParamInt(Type), MCmdParamStr(Name));
}

inline void ZPostUnFreeze(int Type, char* Name)
{
	ZPOSTCMD2(MC_MATCH_UNFREEZESPY, MCmdParamInt(Type), MCmdParamStr(Name));
}
inline void ZPostRequestSpyBanMapList()
{
	ZPOSTCMD0(MC_SPY_STAGE_REQUEST_BAN_MAP_LIST);
}

inline void ZPostSpyStageStart(void* pMapListBlob)
{
	ZPOSTCMD1(MC_SPY_STAGE_REQUEST_START, MCmdParamBlob(pMapListBlob, MGetBlobArraySize(pMapListBlob)));
}
////////////////////////////////////////////////////////////
inline void ZPostAccountCreate(const char* szUserID, const char* szPassword, int nCert, const char* szName, int nAge, int nSex)
{
	ZPOSTCMD6(MC_MATCH_REQUEST_CREATE_ACCOUNT, MCmdParamStr(szUserID), MCmdParamStr(szPassword), MCmdParamInt(nCert),
		MCmdParamStr(szName), MCmdParamInt(nAge), MCmdParamInt(nSex));
}
//Custom: Blitz request upgrade
inline void ZPostRequestBlitzUpgrade(const int keyIndex, const unsigned int playerHonor, const int currUpgradeLevel)
{
	ZPOSTCMD2(MC_BLITZ_REQUEST_UPGRADE, MCmdParamInt(keyIndex), MCmdParamUInt(playerHonor), MCmdParamInt(currUpgradeLevel));
}

inline void ZPostRequestBlitzClass(const int classID)
{
	ZPOSTCMD1(MC_BLITZ_REQUEST_CLASS, MCmdParamInt(classID));
}
// Custom: admin kick all command.
inline void ZPostAdminKickAll(const MUID& uidMe)
{
	ZPOSTCMD1(MC_ADMIN_KICKALL, MCommandParameterUID(uidMe));
}
#ifdef _REPORT
inline void ZPostReport(char* pszTargetPlayerName, char* pszReason)
{
	ZPOSTCMD2(MC_REPORT_USER, MCmdParamStr(pszTargetPlayerName), MCmdParamStr(pszReason));
}
#endif
// Custom: in-game chat server sided.
inline void ZPostGameChat(const MUID& uidMe, const MUID& uidStage, char* Msg, int nTeam)
{
	char szSendMsg[CHAT_STRING_LEN] = {0,};
	const size_t nInputLen = strlen(Msg);

	if(CHAT_STRING_LEN > nInputLen ){
		ZPOSTCMD4(MC_MATCH_GAME_CHAT, MCommandParameterUID(uidMe), MCommandParameterUID(uidStage), MCommandParameterString(Msg), MCommandParameterInt(nTeam));
	}
	else{
		strncpy(szSendMsg, Msg, CHAT_STRING_LEN - 1);
		ZPOSTCMD4(MC_MATCH_GAME_CHAT, MCommandParameterUID(uidMe), MCommandParameterUID(uidStage), MCommandParameterString(szSendMsg), MCommandParameterInt(nTeam));
	}	
}
#ifdef _GRADECHANGE
inline void ZPostGetGrade(char* pszTargetName, const int nUGradeID)
{
	ZPOSTCMD2(MC_UGRADEID_SEND, MCmdParamStr(pszTargetName), MCmdParamInt(nUGradeID));
}
#endif
#endif