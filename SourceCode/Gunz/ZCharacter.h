#ifndef _ZCHARACTER_H
#define _ZCHARACTER_H

//#pragma	once

#include "MRTTI.h"
#include "ZCharacterObject.h"
//#include "ZActor.h"
#include "MUID.h"
#include "RTypes.h"
#include "RPathFinder.h"
#include "RVisualMeshMgr.h"

#include "MObjectTypes.h"
//#include "MObjectCharacter.h"
#include "ZItem.h"
#include "ZCharacterItem.h"
#include "ZCharacterBuff.h"

#include "MMatchObject.h"
#include "RCharCloth.h"
#include "ZFile.h"
#include "Mempool.h"

#include "ZModule_HPAP.h"
#include "MMatchSpyMode.h"
#include <list>
#include <string>


using namespace std;

_USING_NAMESPACE_REALSPACE2

#define MAX_SPEED			1000.f			// √÷¥ÅE”µµ..
#define RUN_SPEED			630.f			// ¥ﬁ∏Æ¥¬ º”µµ
#define BACK_SPEED			450.f			// µ⁄≥™ ø∑¿∏∑Œ ∞•∂ß º”µµ
#define ACCEL_SPEED			7000.f			// ∞°º”µµ
#define STOP_SPEED			3000.f			// æ∆π´≈∞µµ æ»¥≠∑∂¿ª∂ß ∞®º”µµ..
#define STOP_FORMAX_SPEED	7100.f			// ¥ﬁ∏Æ¥¬ º”µµ ¿ÃªÛ¿∏∑Œ ø√∂Û∞¨¿ª∂ß ª°∏Æ ∞®º”«œ¥¬ º”µµ

#define CHARACTER_RADIUS	35.f		// ƒ≥∏Ø≈Õ √Êµπ π›¡ˆ∏ß
#define CHARACTER_HEIGHT	180.0f		// ƒ≥∏Ø≈Õ √Êµπ ≥Ù¿Ã

#define ARRIVAL_TOLER		5.f

class ZShadow;

struct ZANIMATIONINFO {
	char *Name;
	bool bEnableCancel;		// ƒµΩΩ ∞°¥…«—¡ÅE
	bool bLoop;				// π›∫π µ«¥¬ µø¿€
	bool bMove;				// øÚ¡˜¿”¿Ã ∆˜«‘µ» æ÷¥œ∏ﬁ¿Ãº«
	bool bContinuos;		// ∆˜«‘µ» øÚ¡˜¿”¿Ã Ω√¿€∫Œ≈Õ ø¨∞·µ«æ˚‹÷¥¬¡ÅE
};

struct ZFACECOSTUME
{
	char* szMaleMeshName;
	char* szFemaleMeshName;
};


enum ZC_SKILL {

	ZC_SKILL_NONE = 0,

	ZC_SKILL_UPPERCUT,
	ZC_SKILL_SPLASHSHOT,
	ZC_SKILL_DASH,
	ZC_SKILL_CHARGEDSHOT,

	ZC_SKILL_END
};


enum ZC_DIE_ACTION
{
	ZC_DIE_ACTION_RIFLE = 0,
	ZC_DIE_ACTION_KNIFE,
	ZC_DIE_ACTION_SHOTGUN,
	ZC_DIE_ACTION_ROCKET,

	ZC_DIE_ACTION_END
};

enum ZC_SPMOTION_TYPE {

	ZC_SPMOTION_TAUNT = 0,
	ZC_SPMOTION_BOW ,
	ZC_SPMOTION_WAVE ,
	ZC_SPMOTION_LAUGH ,
	ZC_SPMOTION_CRY ,
	ZC_SPMOTION_DANCE ,

	ZC_SPMOTION_END
};

enum ZC_WEAPON_SLOT_TYPE {

	ZC_SLOT_MELEE_WEAPON = 0,
	ZC_SLOT_PRIMARY_WEAPON,
	ZC_SLOT_SECONDARY_WEAPON,
	ZC_SLOT_ITEM1,
	ZC_SLOT_ITEM2,

	ZC_SLOT_END,
};

enum ZC_SHOT_SP_TYPE {
	ZC_WEAPON_SP_NONE = 0,

	// grenade type
	ZC_WEAPON_SP_GRENADE,
	ZC_WEAPON_SP_ROCKET,
	ZC_WEAPON_SP_FLASHBANG,
	ZC_WEAPON_SP_SMOKE,
	ZC_WEAPON_SP_TEAR_GAS,

	ZC_WEAPON_SP_ITEMKIT,	// medikit, repairkit, bulletkit µ˚—ÅE

	ZC_WEAPON_SP_POTION,
	ZC_WEAPON_SP_TRAP,
	ZC_WEAPON_SP_DYNAMITE,

	ZC_WEAPON_SPY_STUNGRENADE,
	ZC_WEAPON_SP_FLASHBANG_SPY,
	ZC_WEAPON_SP_SMOKE_SPY,
	ZC_WEAPON_SP_TRAP_SPY,
	ZC_WEAPON_SP_END,
};

enum ZSTUNTYPE {
	ZST_NONE	=	-1,
	ZST_DAMAGE1	=	0,
	ZST_DAMAGE2,
	ZST_SLASH,			// ∞≠∫£±ÅEΩ∫≈œ
	ZST_BLOCKED,		// ƒÆ ∏∑«˚¿ª∂ß Ω∫≈œ
	ZST_LIGHTNING,		// ¿Œ√¶∆Æ¡ﬂ Lightning
	ZST_LOOP,			// Ω∫≈≥¡ﬂ root º”º∫
};


class ZSlot {
public:
	ZSlot() {
		m_WeaponID = 0;
	}

	int m_WeaponID;
};

/// ƒ≥∏Ø≈Õ º”º∫ - ¿Ã ∞™¿∫ ∫Ø«œ¡ÅEæ ¥¬¥Ÿ.
struct ZCharacterProperty_Old
{
	char		szName[MATCHOBJECT_NAME_LENGTH];
	char		szClanName[CLAN_NAME_LENGTH];
	MMatchSex	nSex;
	int			nHair;
	int			nFace;
	int			nLevel;
	float		fMaxHP;
	float		fMaxAP;
	int			nMoveSpeed;
	int			nWeight;
	int			nMaxWeight;
	int			nSafeFall;
	ZCharacterProperty_Old() :	nSex(MMS_MALE),
							nHair(0),
							nFace(0),
							nLevel(1),
							fMaxHP(1000.f), 
							fMaxAP(1000.f), 
							nMoveSpeed(100), 
							nWeight(0), 
							nMaxWeight(100), 
							nSafeFall(100)
							{ 
								szName[0] = 0;
								szClanName[0] = 0;
							}
	void SetName(const char* name) { strcpy(szName, name); }
	void SetClanName(const char* name) { strcpy(szClanName, name); }
};

/// ∏ﬁ∏∏Æ«Ÿ πÊæ˚€ÅE∏∑Œ ªı∑Œ ∏∏µÅEƒ≥∏Ø≈Õ º”º∫ - ±‚¡∏ ±∏¡∂√º¥¬ ∏Æ«√∑π¿Ã »£»Ø∂ßπÆø° ≥≤∞‹µŒæ˙¿Ω
// ±‚¡∏ º”º∫ ±∏¡∂√ºø°º≠ æ»æ≤¥¬ ∏‚πˆ∫Øºˆ¥¬ ª©πˆ∑»¥Ÿ
struct ZCharacterProperty_CharClanName
{
	char		szName[MATCHOBJECT_NAME_LENGTH];
	char		szClanName[CLAN_NAME_LENGTH];
};
struct ZCharacterProperty
{
	MProtectValue<ZCharacterProperty_CharClanName> nameCharClan;
	MMatchSex	nSex;
	int			nHair;
	int			nFace;
	int			nLevel;
	MProtectValue<float>		fMaxHP;		// ¿Ã ∞™¿ª πŸ≤Ÿ∞ÅE¿⁄ªÅE>∏ÆΩ∫∆˘«ÿº≠ HP∏¶ ªΩ∆¢±‚«œ¥¬ πÊΩƒ¿« «ÿ≈∑
	MProtectValue<float>		fMaxAP;
	ZCharacterProperty() :	nSex(MMS_MALE),
		nHair(0),
		nFace(0),
		nLevel(1)
	{ 
		nameCharClan.Ref().szName[0] = 0;
		nameCharClan.Ref().szClanName[0] = 0;
		nameCharClan.MakeCrc();

		//fMaxHP.Set_MakeCrc(1000.f);
		//fMaxAP.Set_MakeCrc(1000.f);
		fMaxHP.Set_MakeCrc(DEFAULT_CHAR_HP);
		fMaxAP.Set_MakeCrc(DEFAULT_CHAR_AP);
	}
	void SetName(const char* name)	   { nameCharClan.CheckCrc(); strcpy(nameCharClan.Ref().szName, name);	   nameCharClan.MakeCrc(); }
	void SetClanName(const char* name) { nameCharClan.CheckCrc(); strcpy(nameCharClan.Ref().szClanName, name); nameCharClan.MakeCrc(); }
	const char* GetName() { return nameCharClan.Ref().szName; }
	const char* GetClanName() { return nameCharClan.Ref().szClanName; }

	void CopyToOldStruct(ZCharacterProperty_Old& old)	// ∏Æ«√∑π¿Ã ¿˙¿ÂøÅE
	{
		memcpy(old.szName, nameCharClan.Ref().szName, MATCHOBJECT_NAME_LENGTH);
		memcpy(old.szClanName, nameCharClan.Ref().szClanName, CLAN_NAME_LENGTH);
		
		old.nSex = nSex;
		old.nHair = nHair;
		old.nFace = nFace;
		old.nLevel = nLevel;
		old.fMaxHP = fMaxHP.Ref();		// TodoH(ªÅE - ∏Æ«√∑π¿Ã ∞ÅE√..
		old.fMaxAP = fMaxAP.Ref();		// TodoH(ªÅE - ∏Æ«√∑π¿Ã ∞ÅE√..
		old.nMoveSpeed = 100;
		old.nWeight = 0;
		old.nMaxWeight = 100;
		old.nSafeFall = 100;
	}

	void CopyFromOldStruct(ZCharacterProperty_Old& old)	// ∏Æ«√∑π¿Ã ¿Áª˝øÅE
	{
		memcpy(nameCharClan.Ref().szName, old.szName, MATCHOBJECT_NAME_LENGTH);
		memcpy(nameCharClan.Ref().szClanName, old.szClanName, CLAN_NAME_LENGTH);
		nameCharClan.MakeCrc();

		nSex = old.nSex;
		nHair = old.nHair;
		nFace = old.nFace;
		nLevel = old.nLevel;
		fMaxHP.Set_MakeCrc(old.fMaxHP);		// TodoH(ªÅE - ∏Æ«√∑π¿Ã ∞ÅE√..
		fMaxAP.Set_MakeCrc(old.fMaxAP);		// TodoH(ªÅE - ∏Æ«√∑π¿Ã ∞ÅE√..
	}
};

/// ƒ≥∏Ø≈Õ ªÛ≈¬∞™
struct ZCharacterStatus
{	// ºˆ¡§Ω√ ±‚¡∏ ∏Æ«√∑π¿Ã ∑ŒµÅE∞˙”¡«ÿæﬂ«’¥œ¥Ÿ
	int			nLife;
	int			nKills;
	int			nDeaths;
	int			nLoadingPercent;	// √≥¿Ω πÊø° µÈæ˚€√∂ß ∑Œµ˘¿Ã ¥Ÿ µ«æ˙¥¬¡ˆ¿« ∆€ºæ∆Æ 100¿Ã µ«∏ÅE∑Œµ˘¿Ã ¥Ÿµ»∞Õ
	int			nCombo;
	int			nMaxCombo;
	int			nAllKill;
	int			nExcellent;
	int			nFantastic;
	int			nHeadShot;
	int			nUnbelievable;
	int			nExp;
	int			nScore;
	int			nWinCount;
	int			nLossCount;
	int         isSpeed;
	int         isSuper;
	int         isJjang;
	int         isAim;
	int			isESP;
	int			nGivenDamage;
	int			nTakenDamage;
	int			nRoundTakenDamage;
	int			nRoundGivenDamage;
	int			nRoundLastGivenDamage;
	int			nRoundLastTakenDamage;
#ifdef _ANTILEADNEW
#endif
#ifdef _BARNPC
	MUID		nNPCUID;
#endif
	bool		nIsShoot;
#ifdef _BULLETBAR
	int         nMaxCurrBullet;
#endif

	bool			bIsTalking;
#ifdef _KILLSTREAK
	int         nKillStreakCount;
#endif
	ZCharacterStatus() :	
							nLife(10),
							nKills(0),
							nDeaths(0),
							nLoadingPercent(0),
							nCombo(0),
							nMaxCombo(0),
							nAllKill(0),
							nExcellent(0),
							nFantastic(0),
							nHeadShot(0),
							nUnbelievable(0),
							nExp(0),
							nScore(0),
							nWinCount(0),
		                    isSpeed(0),
		                    isSuper(0),
		                    isJjang(0),
		                    isAim(0), 
		                    isESP(0),
	                    	nGivenDamage(0),
		                    nTakenDamage(0),
	                    	nRoundLastGivenDamage(0),
	                    	nRoundLastTakenDamage(0),
	                    	nRoundGivenDamage(0),
	                     	nRoundTakenDamage(0),
#ifdef _ANTILEADNEW
#endif
#ifdef _BARNPC
							nNPCUID(0,0),
#endif
							nIsShoot(false),
#ifdef _BULLETBAR
		                    nMaxCurrBullet(0),
#endif
		                   bIsTalking(false),
#ifdef _KILLSTREAK
		nKillStreakCount(0),
#endif
							nLossCount(0)
							{  }

	void AddKills(int nAddedKills = 1) { nKills += nAddedKills; }
	void AddDeaths(int nAddedDeaths = 1) { nDeaths += nAddedDeaths;  }
	void AddExp(int _nExp=1) { nExp += _nExp; }

};

struct ZCharacterStatus_Old
{	// ºˆ¡§Ω√ ±‚¡∏ ∏Æ«√∑π¿Ã ∑ŒµÅE∞˙”¡«ÿæﬂ«’¥œ¥Ÿ
	int			nLife;
	int			nKills;
	int			nDeaths;
	int			nLoadingPercent;	// √≥¿Ω πÊø° µÈæ˚€√∂ß ∑Œµ˘¿Ã ¥Ÿ µ«æ˙¥¬¡ˆ¿« ∆€ºæ∆Æ 100¿Ã µ«∏ÅE∑Œµ˘¿Ã ¥Ÿµ»∞Õ
	int			nCombo;
	int			nMaxCombo;
	int			nAllKill;
	int			nExcellent;
	int			nFantastic;
	int			nHeadShot;
	int			nUnbelievable;
	int			nExp;

	ZCharacterStatus_Old() :
		nLife(10),
		nKills(0),
		nDeaths(0),
		nLoadingPercent(0),
		nCombo(0),
		nMaxCombo(0),
		nAllKill(0),
		nExcellent(0),
		nFantastic(0),
		nHeadShot(0),
		nUnbelievable(0),
		nExp(0)
	{  }

	void AddKills(int nAddedKills = 1) { nKills += nAddedKills; }
	void AddDeaths(int nAddedDeaths = 1) { nDeaths += nAddedDeaths; }
	void AddExp(int _nExp = 1) { nExp += _nExp; }
};

// ¿Ã∞Õ¿∫ ƒ≥∏Ø≈Õ≥¢∏Æ ¡÷∞˙’ﬁ¥¬ µ•¿Ã≈Õ∑Œ ≥™¡ﬂø° ≈ı«• ∆«¡§¿« ±Ÿ∞≈∞° µ»¥Ÿ.
/*
struct ZHPItem {
	MUID muid;
	float fHP;
};
*/

//struct ZHPInfoItem : public CMemPoolSm<ZHPInfoItem>{
//	ZHPInfoItem()	{ pHPTable=NULL; }
//	~ZHPInfoItem()	{ if(pHPTable) delete pHPTable; }
//	
//	int		nCount;
//	ZHPItem *pHPTable;
//};


//class ZHPInfoHistory : public list<ZHPInfoItem*> {
//};

#define CHARACTER_ICON_DELAY		2.f
#define CHARACTER_ICON_FADE_DELAY	.2f
#define CHARACTER_ICON_SIZE			32.f		// æ∆¿Ãƒ‹ ≈©±ÅE(640x480±‚¡ÿ)

class ZModule_HPAP;
class ZModule_QuestStatus;

// 1bit ∆–≈∑
struct ZCharaterStatusBitPacking { 
	union {
		struct {
			bool	m_bLand:1;				// ¡ˆ±› πﬂ¿ª ∂•ø° ¥ÅE˙‹÷¥¬¡ÅE.
			bool	m_bWallJump:1;			// ∫Æ¡°«¡ ¡ﬂ¿Œ¡ÅE
			bool	m_bJumpUp:1;			// ¡°«¡ø√∂Û∞°¥¬¡ﬂ
			bool	m_bJumpDown:1;			// ≥ª∑¡∞°¥¬¡ﬂ
			bool	m_bWallJump2:1;			// ¿Ã∞« walljump »ƒø° ¬¯¡ˆΩ√ µŒπ¯¬∞ ∆®∞‹¡Æ ≥™ø¿¥¬ ¡°«¡..
			bool	m_bTumble:1;			// ¥˝∫˙‘µ ¡ﬂ
			bool	m_bBlast:1;				// ∂Áøˆ¡ÅEÁ«“∂ß ( ø√∂Û∞•∂ß )
			bool	m_bBlastFall:1;			// ∂Áøˆ¡Æº≠ ∂≥æ˚›˙∂ß
			bool	m_bBlastDrop:1;			// ∂≥æ˚›ˆ¥Ÿ ∂•ø° ∆®±Ê∂ß
			bool	m_bBlastStand:1;		// ¿œæ˚œØ∂ß
			bool	m_bBlastAirmove:1;		// ∞¯¡ﬂ»∏¿ÅEƒ ¬¯¡ÅE
			bool	m_bSpMotion:1;
			bool	m_bCommander:1;			///< ¥ÅEÅE
		//	bool	m_bCharging:1;			// »˚∏¿∏∞ÅE¿÷¥¬¡ﬂ
		//	bool	m_bCharged:1;			// »˚∏¿ŒªÛ≈¬
			bool	m_bLostConEffect:1;		// ≥◊∆ÆøÅE¿¿¥‰¿Ã æ¯¿ª∂ß ∏”∏Æø° ∂ﬂ¥¬ ¿Ã∆Â∆Æ∞° ≥™øÕæﬂ «œ¥¬¡ÅE
			bool	m_bChatEffect:1;		// √§∆√Ω√ ∏”∏Æø° ∂ﬂ¥¬ ¿Ã∆Â∆Æ∞° ≥™øÕæﬂ «œ¥¬¡ÅE
			bool	m_bBackMoving:1;		// µ⁄∑Œ ¿Ãµø«“∂ß

			bool	m_bAdminHide:1;					///< admin hide µ«æ˚‹÷¥¬¡ÅE.
			bool	m_bDie:1;						///< ¡◊æ˙¥¬¡ÅE√º≈©
			bool	m_bStylishShoted:1;				///< ∏∂¡ˆ∏∑¿∏∑Œ Ω∞‘ Ω∫≈∏¿œ∏ÆΩ¨ «ﬂ¥¬¡ÅE√º≈©
			bool	m_bFallingToNarak:1;			///< ≥™∂Ù¿∏∑Œ ∂≥æ˚›ˆ∞ÅE¿÷¥¬¡ÅEø©∫Œ
			bool	m_bStun:1;						///< stun ..øÚ¡˜¿œºˆæ¯∞‘µ»ªÛ≈¬.
			bool	m_bDamaged:1;					///< µ•πÃ¡ÅEø©∫Œ
				
			bool	m_bPlayDone:1;				// æ÷¥œ∏ﬁ¿Ãº« «√∑π¿Ã∞° ¥Ÿ µ«æ˙¥¬¡ÅE ¥Ÿ¿Ωµø¿€¿∏∑Œ ≥—æ˚Ã°¥¬ ±‚¡ÿ
			bool	m_bPlayDone_upper:1;		// ªÛ√º æ÷¥œ∏ﬁ¿Ãº« «√∑π¿Ã∞° ¥Ÿ µ«æ˙¥¬¡ÅE ¥Ÿ¿Ωµø¿€¿∏∑Œ ≥—æ˚Ã°¥¬ ±‚¡ÿ
			bool	m_bIsLowModel:1;
			bool	m_bTagger:1;					///< º˙∑°
			bool	m_bSniping:1;				// Custom: Snipers
			bool	m_bFrozen:1;				// Custom: Frozen (refuse any sort of input + movement)
		};

		DWORD dwFlagsPublic;
	}; // ∆–≈∑ ≥°
};

struct ZUserAndClanName
{
	char m_szUserName[MATCHOBJECT_NAME_LENGTH];			///< ¿Ø¡Æ¿Ã∏ß(ø˚€µ¿⁄¥¬ 'ø˚€µ¿⁄')
	// Custom: Char name + Clan name fix
	char m_szUserAndClanName[MATCHOBJECT_NAME_LENGTH + CLAN_NAME_LENGTH];  ///< ¿Ø¿˙¿Ã∏ß(≈¨∑£¿Ã∏ß) ø‰∞… Ω∫ƒµ«ÿº≠ æ÷µÈ¿Ã ∏ﬁ∏∏Æ «Ÿ¿ª æ≤¥œ±ÅEø‰∞… º˚∞‹πˆ∏Æ¿⁄..
};
struct ZShotInfo;


/// ƒ≥∏Ø≈Õ ≈¨∑°Ω∫
class ZCharacter : public ZCharacterObject
{
	MDeclareRTTI;
	//friend class ZCharacterManager;
private:
	int  m_Honor;
	MMatchObjectClass  m_blitzClass;
	unsigned int	m_skillMapBestTime;

public:
	std::vector<std::vector<int>> m_blitzUpgradeLevel;
protected:

	// ∏µ‚µÅE «—πÅEª˝º∫µ«∞ÅEº“∏ÅE…∂ß ∞∞¿Ã ¡ˆø˚–Ÿ
	ZModule_HPAP			*m_pModule_HPAP;
	ZModule_QuestStatus		*m_pModule_QuestStatus;
	ZModule_Resistance		*m_pModule_Resistance;
	ZModule_FireDamage		*m_pModule_FireDamage;
	ZModule_ColdDamage		*m_pModule_ColdDamage;
	ZModule_PoisonDamage	*m_pModule_PoisonDamage;
	ZModule_LightningDamage	*m_pModule_LightningDamage;
	ZModule_HealOverTime	*m_pModule_HealOverTime;

	MProtectValue<MTD_CharInfo>			m_MInitialInfo;
	MUID								m_uidLastTarget;
	

	ZCharacterProperty					m_Property;		///< HP µ˚‹« ƒ≥∏Ø≈Õ º”º∫
	MProtectValue<ZCharacterStatus>		m_Status;		///< «√∑π¿ÃæÅEªÛ≈¬∞™
	int						m_nCountryFlag;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ∞‘¿” æ»ø°º≠¿« ƒ…∏Ø≈Õ πˆ«¡ ∞ÅE√ ¡§∫∏ π◊ «‘ºÅE
protected:	
	//πˆ«¡¡§∫∏¿”Ω√¡÷ºÆ ZCharacterBuff m_CharacterBuff;				///< ¿˚øÅE«∞ÅE¿÷¥¬ πˆ«¡

	float m_fPreMaxHP;
	float m_fPreMaxAP;

#ifdef _ANTILEADNEW
#endif
public:
#ifdef _BARNPC
	int         nBossDamage;
#endif

#ifdef _HITSCOUNT
	int m_nHits;
#endif

	void ApplyBuffEffect();

	float GetMaxHP();
	float GetMaxAP();
	float GetHP();
	float GetAP();
	void InitAccumulationDamage();
	float GetAccumulationDamage();
	void EnableAccumulationDamage(bool bAccumulationDamage);

	__forceinline void SetMaxHP(float nMaxHP) { m_pModule_HPAP->SetMaxHP(nMaxHP); }
	__forceinline void SetMaxAP(float nMaxAP) { m_pModule_HPAP->SetMaxAP(nMaxAP); }

	__forceinline void SetHP(float nHP)	{ m_pModule_HPAP->SetHP(nHP); }
	__forceinline void SetAP(float nAP) { m_pModule_HPAP->SetAP(nAP); }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	MProtectValue<ZUserAndClanName>*  m_pMUserAndClanName;  ///< ƒ≥∏Ø∏ÅE≈¨∑£∏ÅE

	struct KillInfo {
		int			m_nKillsThisRound;				///< ¿Ãπ¯∂Ûø˚—Âø°º≠¿« kills ( unbelievable ∆«¡§)
		float		m_fLastKillTime;				///< ∏∂¡ˆ∏∑ø° ¡◊¿Œ Ω√∞£ (excellent)∏¶ «•Ω√«œ±ÅE¿ß«‘
	};
	MProtectValue<KillInfo> m_killInfo;

	struct DamageInfo {
		DWORD			m_dwLastDamagedTime;		// ∏∂¡ˆ∏∑¿∏∑Œ ∞¯∞› πﬁ¿∫ Ω√∞£
		ZSTUNTYPE		m_nStunType;				///< ∏¬¥¬ æ÷¥œ∏ﬁ¿Ãº« ¡æ∑ÅE. 2:∏∂¡ˆ∏∑≈∏∞›,4:lightning,5:∑Á«¡
		ZDAMAGETYPE		m_LastDamageType;			///< ∏∂¡ˆ∏∑ µ•πÃ¡ÅE≈∏¿‘		
		MMatchWeaponType m_LastDamageWeapon;		///< ∏∂¡ˆ∏∑ µ•πÃ¡ÅEπ´±ÅE.
		rvector			m_LastDamageDir;			///< ∏∂¡ˆ∏∑ µ•πÃ¡ÅEπÊ«ÅE( ¡◊¥¬ ∏º«¿ª ∞·¡§ )
		float			m_LastDamageDot;
		float			m_LastDamageDistance;

		MUID			m_LastThrower;				///< ∏∂¡ˆ∏∑ ∂ÁøÅEªÁ∂ÅE
		float			m_tmLastThrowClear;			///< ∏∂¡ˆ∏∑ ∂ÁøÅEªÁ∂ÅE¿ÿæ˚—µ µ«¥¬Ω√∞£
	};
	MProtectValue<DamageInfo> m_damageInfo;

	int	m_nWhichFootSound;	///< πﬂº“∏Æ∏¶ π¯∞•æ∆ ≥ª±‚¿ß«ÿ æ˚–¿ πﬂ¿Œ¡ÅE¿˙¿Â«— ∫ØºÅE

	MProtectValue<DWORD>* m_pMdwInvincibleStartTime;		// π´¿˚¿« Ω√¿€ Ω√∞£
	MProtectValue<DWORD>* m_pMdwInvincibleDuration;		// π´¿˚¿« ¡ˆº”Ω√∞£

	virtual void UpdateSound();
public:
	void InitMesh();	///< ƒ≥∏Ø≈Õ ∆ƒ√ÅEµ˚‹« ∏ﬁΩ¨¡§∫∏ ºº∆√. InitCharInfoø°º≠ »£√ÅE
	void InitProperties();
	int GetHonor() { return m_Honor; }
	void AddHonor(int n) { m_Honor += n; }
	void ReduceHonor(int n) { m_Honor -= n; }
	void SetBlitzClass(MMatchObjectClass n) { m_blitzClass = n; }
	const MMatchObjectClass GetBlitzClass() const { return m_blitzClass; }

protected:

//	float m_fIconStartTime[ZCI_END];	///< ∏”∏Æ¿ßø° ∂ﬂ¥¬ æ∆¿Ãƒ‹µÅE

	void CheckLostConn();
	virtual void OnLevelDown();
	virtual void OnLevelUp();
	virtual void OnDraw();
//	virtual void RegisterModules();
	virtual void	OnDie();

	void ShiftFugitiveValues();
	void CalcBlitzDelay(DWORD& nWeaponDelay, MMatchWeaponType& weaponType);
public:
	float	m_fLastValidTime;		// Dead Reckoningø° « ø‰«— ∫ØºÅE-> ¡ˆ±› ƒ⁄µÂø°º≠ « ø‰æ¯æ˚÷∏¿”
	DWORD		m_dwIsValidTime;	//µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊæ˚‘¶ ¿ß«— ≈∏¿” √º≈©..∞≠∫£±‚¬ ..

//	float	m_fDistToFloor;			// πŸ¥⁄±˚›ˆ¿« ∞≈∏Æ
//	rplane	m_FloorPlane;			// πŸ¥⁄ ∆Ú∏È¿« πÊ¡§Ωƒ
//	float	m_fFallHeight;			// ≥´«œ∞° Ω√¿€µ» Ω√¡°

	MProtectValue<ZCharaterStatusBitPacking> m_dwStatusBitPackingValue;	// æÅE¬ ¿Œ∞£¿˚¿∏∑Œ crc√º≈©±˚›ÅE∏¯«œ∞⁄¥Ÿ...;;

	//mmemory proxy
	MProtectValue<bool>* m_bCharged;
	MProtectValue<bool>* m_bCharging;

	

	MProtectValue<float>	m_fChargedFreeTime;		// »˚∏¿Œ∞‘ «Æ∏Æ¥¬ Ω√∞£
	MProtectValue<int>		m_nWallJumpDir;			// ∫Æ¡°«¡«œ¥¬ πÊ«ÅE
	MProtectValue<int>		m_nBlastType;			// ¥‹µµ∞Ëø≠√ﬂ∞°~


	ZC_STATE_LOWER	m_SpMotion;

	
	/*
	bool	m_bClimb;				// ≈Œµ˚€° ø√∂Û∞°∞˙‹÷¥¬ ∞ÊøÅE
	rvector	m_ClimbDir;				// ø√∂Û∞°¥¬ πÊ«ÅE
	float	m_fClimbZ;				// ø√∂Û∞°±ÅEΩ√¿€«— ≥Ù¿Ã
	rplane	m_ClimbPlane;
	*/

//	bool	m_bRendered;				///< ¿Ã¿ÅE¡∑π¿”ø°º≠ »≠∏Èø° ≥™ø‘¥¬¡ÅE

	/////// ≥◊∆Æø˜ø°º≠ ¿”Ω√∑Œ ø  ∞•æ∆ ¿‘±ÅE¿ß«— «ˆ¿ÅEº±≈√µ» ∆ƒ√˜¡§∫∏
	//int m_t_parts[6];	//≥≤¿⁄
	//int m_t_parts2[6];	//ø©¿⁄
	
	
	// rvector		m_vProxyPosition, m_vProxyDirection;	///< ¿Ãµø∞™¿Ã ¿÷¥¬ æ÷¥œ∏ﬁ¿Ãº«¿« ∑ª¥ı∏µ ¿ßƒ°∏¶ ¿ß«— ∫ØºÅE
	
//	ZHPInfoHistory		m_HPHistory;		///< ≈ı«•∏¶ ¿ß«ÿ ∏˚ﬂ ∞£¿« µ•¿Ã≈Õ∏¶ ∞°¡ˆ∞˙‹÷¥¬¥Ÿ

	ZCharacter(IGame* pGame);
	virtual ~ZCharacter();

	virtual bool Create(MTD_CharInfo* pCharInfo/*, MTD_CharBuffInfo* pCharBuffInfo*/);//πˆ«¡¡§∫∏¿”Ω√¡÷ºÆ 
	virtual void Destroy();
	
	void InitMeshParts();
	
	void EmptyHistory();

	rvector m_TargetDir;
	rvector m_DirectionLower, m_DirectionUpper;


private:
	//Custom: Character Posture
	v3 m_vProxyPosition{ 0, 0, 0 };
public:

	// ¿Ã ∫ØºˆµÈ¿∫ ¿Ãµøº”µµ «ÿ≈∑¥ÅEÛ¿Ã µ 
	MProtectValue<rvector> m_RealPositionBefore;			// æ÷¥œ∏ﬁ¿Ãº«¿« øÚ¡˜¿”¿ª √ﬂ¿˚«œ±ÅE¿ß«— ∫ØºÅE
	MProtectValue<rvector> m_AnimationPositionDiff;
	MProtectValue<rvector> m_Accel;


	MProtectValue<ZC_STATE_UPPER>	m_AniState_Upper;		// ªÛ√º æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬
	MProtectValue<ZC_STATE_LOWER>	m_AniState_Lower;		// «œ√º æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬ (±‚∫ª)
	ZANIMATIONINFO *m_pAnimationInfo_Upper,*m_pAnimationInfo_Lower;

	void AddIcon(int nIcon);
//	float GetIconStartTime(int nIcon);

	MProtectValue<int>				m_nVMID;	// VisualMesh ID
	//MUID	m_UID;		// º≠πˆø°º≠ ∫Œø©«— ƒ≥∏Ø≈Õ¿« UID
	MProtectValue<MMatchTeam>		m_nTeamID;	// Team ID

	MProtectValue<MCharacterMoveMode>		m_nMoveMode;
	MProtectValue<MCharacterMode>			m_nMode;
	MProtectValue<MCharacterState>			m_nState;

//	RVisualMesh*			m_pVMesh;

//	float	m_fLastAdjustedTime;
	MProtectValue<float>	m_fAttack1Ratio;//ƒÆ¡˙µ˚‹« ∞ÊøÅE√ππ¯¬∞∫Ò¿≤¿ª ≥™¡ﬂ≈∏ø°µµ ¿˚øÅE—¥Ÿ..
//	rvector m_AdjustedNormal;

	/*
	bool	m_bAutoDir;

	bool	m_bLeftMoving,m_bRightMoving;
	bool	m_bForwardMoving;
	*/

//	float	m_fLastUpdateTime;
	float	m_fLastReceivedTime;	// basicinfo µ•¿Ã≈Õ∏¶ ∏∂¡ˆ∏∑ πﬁ¿∫ Ω√∞£
	MProtectValue<float> m_fHitBoxRealVal;
	MProtectValue<float> m_fHitBoxRealVal2;
	float	m_fTimeOffset;				// ≥™øÕ ¿Ã ƒ≥∏Ø≈Õ¿« Ω√∞£¬˜¿Ã
	float	m_fAccumulatedTimeError;	// «ˆ¿ÁΩ√∞£¿« ¥©¿˚µ» ø¿¬ÅE
	int		m_nTimeErrorCount;			// «ˆ¿ÁΩ√∞£¿« ø¿¬˜∞° ¥©¿˚µ» »∏ºÅE

	float	m_fGlobalHP;			// ¥Ÿ∏•ªÁ∂˜µÈ¿Ã ∫º∂ß ¿Ãƒ≥∏Ø≈Õ¿« HP¿« ∆Ú±’.. ≈ı«•∏¶ ¿ß«‘.
	int		m_nReceiveHPCount;		// ∆Ú±’≥ª±‚¿ß«—...

	
	//float	m_fAveragePingTime;				// ¿œ¡§Ω√∞£ ∆Ú±’ ≥◊∆ÆøÅE¡ˆø¨ Ω√∞£

	// ±€∑ŒπÅEΩ√∞£∞˙¿« ¬˜¿Ã∏¶ ¥©¿˚«—¥Ÿ. ¿Ã º˝¿⁄∞° ¡°¡° ƒø¡ˆ¥¬ ¿Ø¿˙¥¬ Ω∫««µÂ«Ÿ¿Ã ¿«Ω…µ»¥Ÿ.
	//#define TIME_ERROR_CORRECTION_PERIOD	20

	//int		m_nTimeErrorCount;
	//float	m_TimeErrors[TIME_ERROR_CORRECTION_PERIOD];
	//float	m_fAccumulatedTimeError;

	//list<float> m_PingData;			// ∏˚Ã≥¿« «Œ ≈∏¿”¿ª ∞°¡ˆ∞ÅE∆Ú±’¿ª ≥Ω¥Ÿ.

//	DWORD m_dwBackUpTime;

	// π´±ÅEπﬂªÁº”µµ¿« ¿ÃªÛ¿Øπ´∏¶ ∞À√‚«—¥Ÿ.
	int		m_nLastShotItemID;
	float	m_fLastShotTime;
	int		m_nDamageThisRound;

	// Custom: Infected
	bool	m_bInfected;

	MProtectValue<float> m_fHitBox;
	MProtectValue<float> m_fHitBox2;

	// Custom: GunGame
	// Description: Properties derived from MatchServer for updating purposes
	int		m_nGunGameWeaponID[3];
	int		m_nGunGameWeaponLevel;

	float	m_fDamageCaused;

	bool	m_bSBTest;
	DWORD	m_dwSBTestStart;

	// Custom: NOLEAD basic info check
	DWORD	m_dwLastBasicInfoTime;

	void SetInvincibleTime(int nDuration);
	inline bool	isInvincible();

	bool IsMan();

	virtual void  OnUpdate(float fDelta);

	//πˆ«¡¡§∫∏¿”Ω√¡÷ºÆ virtual void  UpdateBuff();
	virtual void  UpdateSpeed();
	virtual float GetMoveSpeedRatio();

	virtual void UpdateVelocity(float fDelta);
	virtual void UpdateHeight(float fDelta);

	//Custom: Character Posture
	virtual void UpdateMotion(float fDelta = 0.f);
	virtual void UpdateDirection(float fDelta, const v3& Direction);

	virtual void UpdateAnimation();

	int  GetSelectWeaponDelay(MMatchItemDesc* pSelectItemDesc);

	void UpdateLoadAnimation();

	void Stop();
	//void DrawForce(bool bDrawShadow);	

	void CheckDrawWeaponTrack();
	void UpdateSpWeapon();

	void SetAnimation(char *AnimationName,bool bEnableCancel,int tick);
	void SetAnimation(RAniMode mode,char *AnimationName,bool bEnableCancel,int tick);

	void SetAnimationLower(ZC_STATE_LOWER nAni);
	void SetAnimationUpper(ZC_STATE_UPPER nAni);
	
	ZC_STATE_LOWER GetStateLower() { return m_AniState_Lower.Ref(); }
	ZC_STATE_UPPER GetStateUpper() { return m_AniState_Upper.Ref(); }

	bool IsUpperPlayDone()	{ return m_dwStatusBitPackingValue.Ref().m_bPlayDone_upper; }

	bool IsMoveAnimation();		// øÚ¡˜¿”¿Ã ∆˜«‘µ» æ÷¥œ∏ﬁ¿Ãº«¿Œ∞° ?

//	bool IsRendered()		{ return m_bRendered; }

	bool IsTeam(ZCharacter* pChar);

	bool IsRunWall();
	bool IsMeleeWeapon();
	virtual bool IsCollideable();

//	void SetAnimationForce(ZC_STATE nState, ZC_STATE_SUB nStateSub) {}

	void SetTargetDir(rvector vDir); 

//	bool Pick(int x,int y,RPickInfo* pInfo);
//	bool Pick(int x,int y,rvector* v,float* f);
	virtual bool Pick(rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);

//	bool Move(rvector &diff);

	void OnSetSlot(int nSlot,int WeaponID);
	void OnChangeSlot(int nSlot);

	virtual void OnChangeWeapon(char* WeaponModelName);
	void OnChangeParts(RMeshPartsType type,int PartsID);
	void OnAttack(int type,rvector& pos);
//	void OnHeal(ZCharacter* pAttacter,int type,int heal);
	void OnShot();

	void ChangeWeapon(MMatchCharItemParts nParts, bool bReSelect = false);

	int GetLastShotItemID()	{ return m_nLastShotItemID; }
	float GetLastShotTime()						{ return m_fLastShotTime; }
	bool CheckValidShotTime(int nItemID, float fTime, ZItem* pItem);
	void UpdateValidShotTime(int nItemID, float fTime)	
	{ 
		m_nLastShotItemID = nItemID;
		m_fLastShotTime = fTime;
	}

	bool IsDie() { return m_dwStatusBitPackingValue.Ref().m_bDie; }
	void ForceDie() 
	{
		SetHP(0); 
		m_dwStatusBitPackingValue.Ref().m_bDie = true; 
	}		// ¿Ã∞Õ¿∫ ±◊≥… ¡◊¿∫ ªÛ≈¬∑Œ ∏∏µÈ±ÅE¿ß«“∂ß ªÁøÅE

	void SetAccel(rvector& accel) { m_Accel.Set_CheckCrc(accel); }
	virtual void SetDirection(rvector& dir);

	struct SlotInfo
	{
		int		m_nSelectSlot;
		ZSlot	m_Slot[ZC_SLOT_END];
	};
	MProtectValue<SlotInfo>	m_slotInfo;

	MProtectValue<ZCharacterStatus>& GetStatus()	{ return m_Status; }

	// Character Property
	ZCharacterProperty* GetProperty() { return &m_Property; }

	MMatchUserGradeID GetUserGrade()	{ return m_MInitialInfo.Ref().nUGradeID; }
	unsigned int GetClanID()	{ return m_MInitialInfo.Ref().nClanCLID; }

	void SetName(const char* szName) { m_Property.SetName(szName); }

	const char *GetUserName()			{ return m_pMUserAndClanName->Ref().m_szUserName;	}		// ø˚€µ¿⁄¥¬ √≥∏Æµ 
	const char* GetCharName()           { return m_pMUserAndClanName->Ref().m_szUserName; }
	const char *GetUserAndClanName()	{ return m_pMUserAndClanName->Ref().m_szUserAndClanName; }	// ø˚€µ¿⁄¥¬ ≈¨∑£«•Ω√ æ»«‘
	bool IsAdminName();

#ifdef _VIPGRADES
	bool IsVIP1Name();
	bool IsVIP2Name();
	bool IsVIP3Name();
	bool IsVIP4Name();
	bool IsVIP5Name();
	bool IsVIP6Name();
	bool IsVIP7Name();
#endif

#ifdef _EVENTGRD
	bool IsEvent1Name();
	bool IsEvent2Name();
	bool IsEvent3Name();
	bool IsEvent4Name();
	bool IsEvent5Name();
	bool IsEvent6Name();
	bool IsStaffName();
	bool Event();
#endif

	bool IsAdminHide()			{ return m_dwStatusBitPackingValue.Ref().m_bAdminHide;	}
	void SetAdminHide(bool bHide) { m_dwStatusBitPackingValue.Ref().m_bAdminHide = bHide; }
	
//	void SetMoveSpeed(int nMoveSpeed) { m_Property.nMoveSpeed = nMoveSpeed; }
//	void SetWeight(int nWeight) { m_Property.nWeight = nWeight; }
//	void SetMaxWeight(int nMaxWeight) { m_Property.nMaxWeight = nMaxWeight; }
//	void SetSafeFall(int nSafeFall) { m_Property.nSafeFall = nSafeFall; }

	int GetKils() { return GetStatus().Ref().nKills; }
#ifdef _KILLSTREAK
	int GetKillStreaks() { return GetStatus().Ref().nKillStreakCount; }
#endif
	bool CheckDrawGrenade();

//	int GetWeaponEffectType();
//	float GetCurrentWeaponRange();
//	float GetMeleeWeaponRange();

	bool GetStylishShoted() { return m_dwStatusBitPackingValue.Ref().m_bStylishShoted; }
	void UpdateStylishShoted();
	
	MUID GetLastAttacker() { return m_pModule_HPAP->GetLastAttacker(); }
	void SetLastAttacker(MUID uid) { m_pModule_HPAP->SetLastAttacker(uid); }
	ZDAMAGETYPE GetLastDamageType() { return m_damageInfo.Ref().m_LastDamageType; }
	void SetLastDamageType(ZDAMAGETYPE type) { MEMBER_SET_CHECKCRC(m_damageInfo, m_LastDamageType, type); }

	bool DoingStylishMotion();
	
	bool IsObserverTarget();

	MMatchTeam GetTeamID() { return m_nTeamID.Ref(); }
	void SetTeamID(MMatchTeam nTeamID) { m_nTeamID.Set_CheckCrc(nTeamID); }
	bool IsSameTeam(ZCharacter* pCharacter) 
	{ 
		if (pCharacter->GetTeamID() == -1) return false;
		if (pCharacter->GetTeamID() == GetTeamID()) return true; return false; 
	}

	bool IsTagger() { return m_dwStatusBitPackingValue.Ref().m_bTagger; }
	void SetTagger(bool bTagger) { m_dwStatusBitPackingValue.Ref().m_bTagger = bTagger; }

	void SetLastThrower(MUID uid, float fTime) { MEMBER_SET_CHECKCRC(m_damageInfo, m_LastThrower, uid); MEMBER_SET_CHECKCRC(m_damageInfo, m_tmLastThrowClear, fTime); }
	const MUID& GetLastThrower() { return m_damageInfo.Ref().m_LastThrower; }
	float GetLastThrowClearTime() { return m_damageInfo.Ref().m_tmLastThrowClear; }

	// µø¿€¿Ã≥™ ¿Ã∫•∆Æø° ∞ÅE— ∞ÕµÅE
	//void Damaged(ZCharacter* pAttacker, rvector& dir, RMeshPartsType partstype,MMatchCharItemParts wtype,int nCount=-1);
	//void DamagedGrenade(MUID uidOwner, rvector& dir, float fDamage,int nTeamID);
	//void DamagedFalling(float fDamage);
	//void DamagedKatanaSplash(ZCharacter* pAttacker,float fDamageRange);

	void Revival();
	void Die();
	void ActDead();
	__forceinline void InitHPAP();	
	virtual void InitStatus();
	virtual void InitRound();
	virtual void InitItemBullet();

	void TestChangePartsAll();
	void TestToggleCharacter();
	void InfectCharacter(bool bFirst);

	virtual void OutputDebugString_CharacterState();

	void ToggleClothSimulation();
	void ChangeLowPolyModel();
	bool IsFallingToNarak() { return m_dwStatusBitPackingValue.Ref().m_bFallingToNarak; }

	MMatchItemDesc* GetSelectItemDesc() {
		if(GetItems())
			if(GetItems()->GetSelectedWeapon())
				return GetItems()->GetSelectedWeapon()->GetDesc();
		return NULL;
	}

	void LevelUp();
	void LevelDown();

	bool Save(ZFile *file);
	bool Load(ZFile *file,int nVersion);	// ≥™¡ﬂø° MZFile * ∑Œ ∆˜∆√

	RMesh *GetWeaponMesh(MMatchCharItemParts parts);

	virtual float ColTest(const rvector& pos, const rvector& vec, float radius, rplane* out=0);
	virtual bool IsAttackable();

	virtual bool IsGuard();
	virtual void OnMeleeGuardSuccess();
#ifdef _MACOLOR
	void AddMassiveEffect(const rvector& pos, const rvector& dir);
#endif
	virtual void OnDamagedAnimation(ZObject *pAttacker,int type);

	unsigned long int nHunter_LastWeapons;
	// ZObjectø° ∏¬∞‘ ∏∏µÅEµø¿€¿Ã≥™ ¿Ã∫•∆Æø° ∞ÅE— ∞ÕµÅE
	virtual ZOBJECTHITTEST HitTest( const rvector& origin, const rvector& to, float fTime ,rvector *pOutPos=NULL );

	virtual void OnKnockback(rvector& dir, float fForce);
//	virtual void OnDamage(int damage, float fRatio = 1.0f);
	
#ifdef _ANTILEADNEW
	virtual void OnDamagedAPlayer(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio = 1.f, int nMeleeTpye = -1);
	virtual void OnDamagedAPlayer(vector<MTD_AntiLeadJork*> vShots);
#endif

	virtual void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio=1.f, int nMeleeType=-1);
	virtual void OnScream();
	void SetCountry(int nCountry) { m_nCountryFlag = nCountry; }

	int GetDTLastWeekGrade() { return m_MInitialInfo.Ref().nDTLastWeekGrade; }
	int	GetCountry()         { return m_MInitialInfo.Ref().nCountryFlag; }
	const MTD_CharInfo* GetCharInfo() const { return &m_MInitialInfo.Ref(); }

	MUID GetLastTarget() { return m_uidLastTarget; }
	void SetLastTarget(MUID uidTarget) { m_uidLastTarget = uidTarget; }

	unsigned int GetSkillMapBestTime() { return m_skillMapBestTime; }
	void SetSkillMapBestTime(unsigned int n) { m_skillMapBestTime = n; }

public:
	void SpyHealthBonus(int nHPAP);

	void InitSpyWeaponBullet();

	virtual void DistributeSpyItem(vector<MMatchSpyItem>& vt);
	void TakeoutSpyItem();

};

void ZChangeCharParts(ZObjectVMesh* pVMesh, MMatchSex nSex, int nHair, int nFace, const unsigned long int* pItemID);
void ZChangeCharPartsNoAvatar(ZObjectVMesh* pVMesh, MMatchSex nSex, int nHair, int nFace, const unsigned long int* pItemID);
void ZChangeCharWeaponMesh(ZObjectVMesh* pVMesh, unsigned long int nWeaponID);
bool CheckTeenVersionMesh(RMesh** ppMesh);

//dll-injection¿∏∑Œ »£√‚«œ¡ÅE∏¯«œµµ∑œ ∞≠¡¶ ¿Œ∂Û¿Ã¥◊
//__forceinline void ZCharacter::InitHPAP(); // Custom: Moved this to ZCharacter.cpp

//////////////////////////////////////////////////////////////////////////
//	ENUM
//////////////////////////////////////////////////////////////////////////
enum CHARACTER_LIGHT_TYPE
{
	GUN,
	SHOTGUN,
	CANNON,
	NUM_LIGHT_TYPE,
};

//////////////////////////////////////////////////////////////////////////
//	STRUCT
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int			iType;
	float		fLife;
	rvector		vLightColor;
	float		fRange;
}	sCharacterLight;




int gaepanEncode(int a, int depth);
int gaepanDecode(int a, int depth);

#endif