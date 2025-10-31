#ifndef _ZGAME_H
#define _ZGAME_H

//#pragma once
#include "IGame.h"
#include "ZPrerequisites.h"

#include "MMatchClient.h"
#include "MDataChecker.h"

#include "RTypes.h"
#include "RBspObject.h"
#include "ZMatch.h"
#include "ZGameTimer.h"
#include "ZWater.h"
#include "ZClothEmblem.h"
#include "ZEffectManager.h"
#include "ZWeaponMgr.h"
#include "ZHelpScreen.h"
#include "ZCharacterManager.h"
#include "ZObjectManager.h"
#include "ZWorld.h"
#include "ZNavigationMesh.h"
_USING_NAMESPACE_REALSPACE2

class MZFileSystem;

class ZLoading;
class ZGameAction;
class ZSkyBox;
class ZFile;
class ZObject;
class ZCharacter;
class ZMyCharacter;
class ZMiniMap;
class ZMsgBox;
class ZInterfaceBackground;
class ZCharacterSelectView;
class ZScreenEffectManager;
class ZPlayerMenu;
class ZGameClient;
class ZMapDesc;
class ZReplayLoader;
//typedef map<MUID, vector<MTD_ShotInfo*>> ShotArray;
//typedef map<int, vector<MTD_ShotInfo*>> RetryArray;
//typedef map<int, unsigned long int> RecvArray;
void CheckMsgAboutChat(char * msg);

// Game Loop Ω√¿€«œ±ÅE¿ÅE« √ ±‚»≠π◊ ΩÃ≈©øœ∑ÅE∞ÀªÁø° æ≤¿”
enum ZGAME_READYSTATE {
	ZGAME_READYSTATE_INIT,
	ZGAME_READYSTATE_WAITSYNC,
	ZGAME_READYSTATE_RUN
};

struct ZObserverCommandItem {
	float fTime;
	MCommand *pCommand;
};

class ZObserverCommandList : public list<ZObserverCommandItem*> {
public:
	~ZObserverCommandList() { Destroy(); }
	void Destroy() {
		while(!empty())
		{
			ZObserverCommandItem *pItem=*begin();
			delete pItem->pCommand;
			delete pItem;
			erase(begin());
		}
	}
};
struct UseItemCount
{
	int Itemid;
	int ItemUseCount;
};
struct ReplayInfo_UseSpendItem
{
	MUID uid;
	UseItemCount Item[5]; // 5∞≥ ¿ÃªÛ¿« ¡æ∑˘¿Ã∏ÅEπˆ∏∞¥Ÿ(»Æ∑ÅE¿˚¿Ω)
};

// Custom: Forwarded declaration for GetUserGradeIDColor()
bool GetUserGradeIDColor(MMatchUserGradeID gid,MCOLOR& UserNameColor,char* sp_name);
float CalcQuestDamageForClothing(ZCharacter* pAttacker);

// Custom: AntiLead structs
//struct ZAntiLeadTemporaryInfo
//{
//    unsigned long    m_nVictimLowID;
//    char            m_nPartsType;
//};

struct ZShotInfo
{
	unsigned long m_nVictimLowID;
	char   m_nSelType;
	char   m_nPartsType;
};

struct ZAntiLead_UIDListNode
{
    unsigned long                    LowID;
    vector<ZShotInfo*>    Info;
};

#define TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME 3.0f
typedef map<MUID, float> DamageCounterMap;

#define MAX_COMBO 99

#define PEERMOVE_TICK			100		// 0.1√  ∏∂¥Ÿ ¿Ãµø∏ﬁºº¡ˆ∏¶ ∫∏≥Ω¥Ÿ (√ ¥ÅE10»∏)
#define PEERMOVE_AGENT_TICK		100		// Agent∏¶ ≈ÅEœ∏ÅE√ ¥ÅE10πÅE∏ﬁΩ√¡ˆ∏¶ ∫∏≥Ω¥Ÿ.

#include "IGame.h"
class ZGame : public IGame {
protected:
	enum ZGAME_LASTTIME
	{
		ZLASTTIME_HPINFO = 0,
		ZLASTTIME_BASICINFO,
		ZLASTTIME_PEERPINGINFO,
		ZLASTTIME_SYNC_REPORT,
		ZLASTTIME_MAX
	};

	//	ZWorld				*m_pWorld;
	ZGameAction* m_pGameAction;
	MDataChecker		m_DataChecker;
	ZGameTimer			m_GameTimer;
	MProtectValue<float> m_fTime;

	DWORD				m_nLastTime[ZLASTTIME_MAX];

	bool				m_bIsCreate;

	ZGAME_READYSTATE	m_nReadyState;

	set<const ZCharacter*>	m_setCharacterExceptFromNpcTarget;		// NPC∞° ≈∏∞Ÿ¿∏∑Œ ªÅEÅEæ æ∆æﬂ «œ¥¬ ƒ≥∏Ø≈Õ

	void OnPreDraw();
	bool OnRuleCommand(MCommand* pCommand);
	bool InRanged(ZObject* pAttacker, ZObject* pVictim, int& nDebugRegister/*µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ¡ˆ∏¶ ¿ß«— ∫ØºÅE*/);
public:

	DamageCounterMap* DmgCounter;
	//todok ¥‹¿ß≈◊Ω∫∆Æ∏¶ ¿ß«ÿº≠¥¬ √÷¡æ¿˚¿∏∑Œ¥¬ private¿∏∑Œ πŸ≤„æﬂ «—¥Ÿ.

		// «Ÿ ¡¶¿€¿ª πÊ«ÿ«œ±ÅE¿ß«ÿ ∏‚πˆ∫Øºˆ¿« ¿ßƒ°∏¶ ∫ÙµÂ∂ß∏∂¥Ÿ µ⁄ºØ±ÅE¿ß«— ¡÷ºÆ∏≈≈©∑Œ(runtime/ShuffleCode.bat Ω««ÅE
		// m_pMyCharacter∞° ¡ﬂø‰«œ¥Ÿ. ZGame ∆˜¿Œ≈Õ∏¶ ≈ÅEÿ ≥ª ƒ≥∏Ø≈Õ∏¶ √£æ∆ π´¿˚«Ÿ¿ª ¡¶¿€«—¥Ÿ.
	/* [[SHUFFLE_LINE]] ZGame */	ZMyCharacter* m_pMyCharacter;

	/* [[SHUFFLE_LINE]] ZGame */	ZEffectManager* m_pEffectManager;

	/* [[SHUFFLE_LINE]] ZGame */	ZHelpScreen	m_HelpScreen;
	/* [[SHUFFLE_LINE]] ZGame */	ZCharacterManager	m_CharacterManager;

	/* [[SHUFFLE_LINE]] ZGame */	bool m_bShowWireframe;

	/* [[SHUFFLE_LINE]] ZGame */	RParticles* m_pParticles;

	/* [[SHUFFLE_LINE]] ZGame */	RVisualMeshMgr		m_VisualMeshMgr;

	/* [[SHUFFLE_LINE]] ZGame */	ZWeaponMgr			m_WeaponManager;

	/* [[SHUFFLE_LINE]] ZGame */	ZObjectManager		m_ObjectManager;
	/* [[SHUFFLE_LINE]] ZGame */	int					m_render_poly_cnt;

	MUID m_FreezerUID;
	bool m_bPause;
	rvector rStuckPosition;

public:
	virtual MUID GetMyUid();
	virtual ZMyCharacter* GetMyCharacter() { return m_pMyCharacter; }

	virtual ZCharacterManagerBase* GetCharacterMgr() { return &m_CharacterManager; }


	void BerserkerKI();
	void GetJjang();

#ifdef _SRVRPNG
	unsigned long int st_nLastTime[3];
#endif

public:

	ZGame();
	virtual ~ZGame();
	bool Create(MZFileSystem *pfs, ZLoadingProgress *pLoading);
	unsigned long int LastShotTime, LastWarningTime, LastSwitchTime;

	bool IsCreated() { return m_bIsCreate; }
	void Draw();
	void Draw(MDrawContextR2 &dc);
	void Update(float fElapsed);
	void Destroy();

	void OnCameraUpdate(float fElapsed);
	void OnInvalidate();
	void OnRestore();

	void ParseReservedWord(char* pszDest, const char* pszSrc);

	void ShowReplayInfo( bool bShow);

	void OnExplosionGrenade(MUID uidOwner,rvector pos,float fDamage,float fRange,float fMinDamage,float fKnockBack,MMatchTeam nTeamID);
	void OnExplosionMagic(ZWeaponMagic *pWeapon, MUID uidOwner,rvector pos,float fMinDamage,float fKnockBack,MMatchTeam nTeamID,bool bSkipNpc);
	void OnExplosionMagicThrow(ZWeaponMagic *pWeapon, MUID uidOwner,rvector pos,float fMinDamage,float fKnockBack,MMatchTeam nTeamID,bool bSkipNpc, rvector from, rvector to);
	void OnExplosionMagicNonSplash(ZWeaponMagic *pWeapon, MUID uidOwner, MUID uidTarget, rvector pos, float fKnockBack);
	void OnReloadComplete(ZCharacter *pCharacter);
	
	void OnPeerShotSp(MUID& uid, float fShotTime, rvector& pos, rvector& dir, int type, MMatchCharItemParts sel_type);

	void OnChangeWeapon(MUID& uid, MMatchCharItemParts parts);

	rvector GetMyCharacterFirePosition(void);

	void CheckMyCharDead(float fElapsed);
	//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «Ÿ πÊæÅE¿ß«ÿ checkMyChatDead∏¶ µŒ∞≥∑Œ ¬…∞∑..
	void CheckMyCharDeadByCriticalLine();
	void CheckMyCharDeadUnchecked();

	bool SimpleOcclusionCullTest(const rvector &Top, const rvector &Bottom);

	void CheckStylishAction(ZCharacter* pCharacter);
	void CheckCombo( ZCharacter *pOwnerCharacter , ZObject *pHitObject ,bool bPlaySound);
	void UpdateCombo(bool bShot = false );
	//void AssignCommander(const MUID& uidRedCommander, const MUID& uidBlueCommander);
	//void SetGameRuleInfo(const MUID& uidRedCommander, const MUID& uidBlueCommander);

	void PostBasicInfo();
	void PostHPAPInfo();
	void PostDuelTournamentHPAPInfo();
	void PostPeerPingInfo();
	void PostSyncReport();
	void PostMyBuffInfo();
	
	int  SelectSlashEffectMotion(ZCharacter* pCharacter);
	//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ¡ˆ~!!!
	bool CheckWall(ZObject* pObj1,ZObject* pObj2, bool bCoherentToPeer=false);
	bool CheckWall(ZObject* pObj1,ZObject* pObj2, int & nDebugRegister/*¥‹ºÅE∫Ò±≥øÅE*/, bool bCoherentToPeer=false);
	bool InRanged(ZObject* pAttacker, ZObject* pVictim);

	virtual bool IsWallBlocked(ZObject* pObj1, ZObject* pObj2, bool bCoherentToPeer = false);
	virtual bool IsWallBlocked(ZObject* pObj1, ZObject* pObj2, int& nDebugRegister/*¥‹º¯ ∫Ò±≥øÎ*/, bool bCoherentToPeer = false);

	void InitRound();
	void AddEffectRoundState(MMATCH_ROUNDSTATE nRoundState, int nArg);
	void ZGame::RoundEndDamage();

	bool CreateMyCharacter(MTD_CharInfo* pCharInfo);//πˆ«¡¡§∫∏¿”Ω√¡÷ºÆ , MTD_CharBuffInfo* pCharBuffInfo);
	void DeleteCharacter(const MUID& uid);
	void RefreshCharacters();
	void ConfigureCharacter(const MUID& uidChar, MMatchTeam nTeam, unsigned char nPlayerFlags);

	bool OnCommand(MCommand* pCommand);
	bool OnCommand_Immidiate(MCommand* pCommand);

	// ≥ÅE≠ & ¿Áª˝
	void ToggleRecording();
	void StartRecording();
	void StopRecording();

	bool OnLoadReplay(ZReplayLoader* pLoader);
	bool IsReplay() { return m_bReplaying.Ref(); }
	bool IsShowReplayInfo() { return m_bShowReplayInfo; }
	void EndReplay();

	void OnReplayRun();


	// ø…¿˙πÅE
	void OnObserverRun();
	void OnCommand_Observer(MCommand* pCommand);
	void FlushObserverCommands();
	int	GetObserverCommandListCount() { return (int)m_ObserverCommandList.size(); }

	void ReserveObserver();
	void ReleaseObserver();

	// ø‹∫Œø°º≠ ¬ÅE∂«“∏∏«— ∞ÕµÅE
	ZMatch* GetMatch()				{ return &m_Match; }
	ZMapDesc* GetMapDesc()			{ return GetWorld() ? GetWorld()->GetDesc() : NULL; }
	ZWorld* GetWorld()				{ return ZGetWorldManager()->GetCurrent(); }

	ZGameTimer* GetGameTimer()		{ return &m_GameTimer; }
	unsigned long GetTickTime()		{ return m_GameTimer.GetGlobalTick(); }
	float GetTime()			{ /*mlog("«ˆ¿ÁΩ√∞£: %f \n", m_fTime.GetData());*/ return m_fTime.Ref(); }

	int GetPing(MUID& uid);

	MDataChecker* GetDataChecker()	{ return &m_DataChecker; }

	virtual bool Pick(ZObject* pOwnerObject, rvector& origin, rvector& dir, ZPICKINFO* pickinfo, DWORD dwPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE, bool bMyChar = false);
	bool PickTo(ZObject* pOwnerObject, rvector& origin, rvector& to, ZPICKINFO* pickinfo, DWORD dwPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE, bool bMyChar = false);
	bool PickHistory(ZObject* pOwnerObject, float fTime, const rvector& origin, const rvector& to, ZPICKINFO* pickinfo, DWORD dwPassFlag, bool bMyChar = false);
	bool ObjectColTest(ZObject* pOwner, rvector& origin, rvector& to, float fRadius, ZObject** poutTarget);

	virtual bool PickWorld(const rvector& pos, const rvector& dir, RBSPPICKINFO* pOut, DWORD dwPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_USEOPACITY | RM_FLAG_HIDE);
	virtual bool CheckWall(rvector& origin, rvector& targetpos, float fRadius, float fHeight = 0.f, RCOLLISIONMETHOD method = RCW_CYLINDER, int nDepth = 0, rplane* pimpactplane = NULL);
	virtual rvector GetFloor(rvector pos, rplane* pimpactplane = NULL, MUID uID = MUID(0, 0));
	// æÓ∂≤ ø¿∫Í¡ß∆Æ∞° diff∏∏≈≠ ¿Ãµø«œ∑¡ «“∂ß ¥Ÿ∏• ƒ≥∏Ø≈ÕøÕ¿« √Êµπ¿ª ∞Ì∑¡«ÿ diff∏¶ ºˆ¡§«ÿ¡÷¥¬ «‘ºˆ
	virtual void AdjustMoveDiff(ZObject* pObject, rvector& diff);
	bool CharacterOverlapCollision(ZObject* pFloorObject, float WorldFloorHeight, float ObjectFloorHeight);

	virtual ZNavigationMesh GetNavigationMesh();	// ¥‹ºÅE∆˜¿Œ≈Õ ∑°∆€¿Ãπ«∑Œ ±◊≥… return by value

	char* GetSndNameFromBsp(const char* szSrcSndName, RMATERIAL* pMaterial);

	bool CheckGameReady();
	ZGAME_READYSTATE GetReadyState()			{ return m_nReadyState; }
	void SetReadyState(ZGAME_READYSTATE nState)	{ m_nReadyState = nState; }

	bool GetSpawnRequested()			{ return m_bSpawnRequested; }
	void SetSpawnRequested(bool bVal)	{ m_bSpawnRequested = bVal; }


	bool GetUserNameColor(MUID uid,MCOLOR& color,char* sp_name);
	//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ¡ˆ∏¶ ¿ß«ÿ CanAttack()«‘ºˆ∏¶ ¿Ã∏ß∏∏ πŸ≤„º≠ ªÁøÅE
	bool CanAttack(ZObject *pAttacker, ZObject *pTarget);
	//bool CalcActualDamage(ZObject* pAttacker, ZObject* pVictim, float fDamage);
	bool CanAttack_DebugRegister(ZObject *pAttacker, ZObject *pTarget);

	bool CanISeeAttacker( ZCharacter* pAtk, const rvector& vRequestPos );

	// npc AI∞° ≈∏∞Ÿ¿∏∑Œ ªÅEˆæ æ∆æﬂ «œ¥¬ ƒ≥∏Ø≈Õ ∏Ò∑œ ∞ÅE√«‘ºÅE
	void ExceptCharacterFromNpcTargetting(const ZCharacter* pChar) { m_setCharacterExceptFromNpcTarget.insert(pChar); }
	void ClearListExceptionFromNpcTargetting() { m_setCharacterExceptFromNpcTarget.clear(); }
	bool IsExceptedFromNpcTargetting(const ZCharacter* pChar) { return m_setCharacterExceptFromNpcTarget.find(pChar) != m_setCharacterExceptFromNpcTarget.end(); }

protected:
	char m_szReplayFileName[_MAX_PATH];	// ∏Æ«√∑π¿Ã ¿˙¿ÅEøœ∑ÅE∏ﬁΩ√¡ÅE√‚∑¬¿ª ¿ß«ÿ
	ZFile *m_pReplayFile;
//	FILE *m_pRecordingFile;
	MProtectValue<bool> m_bReplaying;
	bool m_bShowReplayInfo;

	bool m_bRecording;
	bool m_bReserveObserver;

	bool m_bSuicide;
	DWORD m_dwReservedSuicideTime;
	int m_DiceRoll;

	// because for readability
public:
	DWORD m_dwLastInputTime;

protected:
	unsigned long int m_nReservedObserverTime;
	int m_t;
	ZMatch				m_Match;
	unsigned long int	m_nSpawnTime;
	bool				m_bSpawnRequested;

	ZObserverCommandList m_ObserverCommandList;		// observer ªÛ≈¬¿œ∂ß command ∏¶ ¿˙¿Â«ÿµŒ¥¬ ∞ÅE
	ZObserverCommandList m_ReplayCommandList;		// replay ªÛ≈¬¿œ∂ß command ∏¶ ¿˙¿Â«ÿµŒ¥¬ ∞ÅE ∂« ≥ÅE≠«“∂ß ¿˙¿Â«œ¥¬ ∞ÅE

	ZObserverCommandList m_DelayedCommandList;		// ¡ˆø¨Ω√∞£¿ª ∞°¡ˆ¥¬ command µÈ¿Ã¥Ÿ. ex) ∂ÁøÅEÅEƒÆ¡ÅE

	float m_ReplayLogTime;
	ReplayInfo_UseSpendItem m_Replay_UseItem[16];	// 16∏ÅE¿ÃªÅEªÁøÅE√ π´Ω√(»Æ∑ÅE˚¿Ω) mapµ˚‹ª ªÁøÅE“ « ø‰º∫....

	void CheckKillSound(ZCharacter* pAttacker);
	
	void OnReserveObserver();
	void DrawDebugInfo();

	void OnStageEnterBattle(MCmdEnterBattleParam nParam, MTD_PeerListNode* pPeerNode);
	void OnStageLeaveBattle(const MUID& uidChar, const bool bIsRelayMap);//, const MUID& uidStage);
	void OnPeerList(const MUID& uidStage, void* pBlob, int nCount);
	void OnAddPeer(const MUID& uidChar, DWORD dwIP, const int nPort =	
		MATCHCLIENT_DEFAULT_UDP_PORT, MTD_PeerListNode* pNode = NULL);
	void OnGameRoundState(const MUID& uidStage, int nRound, int nRoundState, int nArg);

	void OnGameResponseTimeSync(unsigned int nLocalTimeStamp, unsigned int nGlobalTimeSync);
	void OnEventUpdateJjang(const MUID& uidChar, bool bJjang);

	// ªÁ∂Û¡¯µÅE
//	void OnPeerShot_Item(ZCharacter* pOwnerCharacter,float fShotTime, rvector& pos, rvector& dir,int type);

	void OnPeerDead(const MUID& uidAttacker, const unsigned long int nAttackerArg, 
					const MUID& uidVictim, const unsigned long int nVictimArg);
	void OnReceiveTeamBonus(const MUID& uidChar, const unsigned long int nExpArg);
	void OnPeerDie(MUID& uidVictim, MUID& uidAttacker);
	void OnPeerDieMessage(ZCharacter* pVictim, ZCharacter* pAttacker);
	void OnChangeParts(MUID& uid,int partstype,int PartsID);
	//void OnAssignCommander(const MUID& uidRedCommander, const MUID& uidBlueCommander);
	void OnAttack(MUID& uid,int type,rvector& pos);
	void OnDamage(MUID& uid,MUID& tuid,int damage);
	void OnPeerReload(MUID& uid);
	void OnPeerSpMotion(MUID& uid,int nMotionType);
	void OnPeerChangeCharacter(MUID& uid);
	void OnPeerSpawn(MUID& uid, rvector& pos, rvector& dir);
	void OnAdminSBTest(const MUID& uidAdmin, const char* szTargetName);
	void OnSetObserver(MUID& uid);
	

//	void OnPeerAdd();
	void OnPeerBasicInfo(MCommand *pCommand,bool bAddHistory=true,bool bUpdate=true);
	void OnPeerHPInfo(MCommand *pCommand);
	void OnPeerHPAPInfo(MCommand *pCommand);
	void OnPeerDuelTournamentHPAPInfo(MCommand *pCommand);
	void OnPeerPing(MCommand *pCommand);
	void OnPeerPong(MCommand *pCommand);
	void OnPeerOpened(MCommand *pCommand);
	void OnPeerDash(MCommand* pCommand);
	void OnPeerBuffInfo(const MUID& uidSender, void* pBlobBuffInfo);

		
	bool FilterDelayedCommand(MCommand *pCommand);
	void ProcessDelayedCommand();

	// ≈ı«•¥¬ ∫¿¿Œ
	//	void AdjustGlobalTime();
	//	void AdjustMyData();

	void OnLocalOptainSpecialWorldItem(MCommand* pCommand);
	void OnResetTeamMembers(MCommand* pCommand);

public:
#ifdef _ASSITEN
	void AutoAiming();
#endif
	void OnPeerShot( const MUID& uid, float fShotTime, const rvector& pos, const rvector& to, const MMatchCharItemParts sel_type);

	void PostSpMotion(ZC_SPMOTION_TYPE type);

	// peershot¿Ã ≥ π´ ±Êæ˚›Æº≠ ∫–∏Æ
	void OnPeerShot_Melee(const MUID& uidOwner, float fShotTime);
	void OnPeerShot_Range(const MMatchCharItemParts sel_type, const MUID& uidOwner, float fShotTime, const rvector& pos, const rvector& to);
	void OnPeerShot_Range_Damaged(ZObject* pOwner, float fShotTime, const rvector& pos, const rvector& to, ZPICKINFO pickinfo, DWORD dwPickPassFlag, rvector& v1, rvector& v2, ZItem *pItem, rvector& BulletMarkNormal, bool& bBulletMark, ZTargetType& nTargetType);
	void OnPeerShot_Shotgun(ZItem *pItem, ZCharacter* pOwnerCharacter, float fShotTime, const rvector& pos, const rvector& to);

#ifdef _ANTILEADNEW
	MTD_AntiLeadJork*OnPeerShotgun_Damaged(ZObject* pOwner, float fShotTime, const rvector& pos, rvector &dir, ZPICKINFO pickinfo, DWORD dwPickPassFlag, rvector& v1, rvector& v2, ZItem *pItem, rvector& BulletMarkNormal, bool& bBulletMark, ZTargetType& nTargetType, bool& bHitEnemy);
#endif

#ifdef _NOLEAD
	void OnNoLead(const MUID& uidAttacker, const MUID& uidVictim, const rvector& pos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType);
	void OnNoLeadBlob(const MUID& uidSender, void* pBlobArray);
#endif
	void OnVampire( const MUID& uidVictim, const MUID& uidAttacker, float fDamage );
	void OnPeerScope( const MUID& uidPeer, bool bScope );
	void OnAdminSummon( const MUID& uidAdmin, const char* szTargetName );
	void OnAdminGoTo( const MUID& uidAdmin, const char* szTargetName );
	void OnAdminSlap( const MUID& uidAdmin, const char* szTargetName );
	void OnAdminSpawn( const MUID& uidAdmin, const MUID& uidVictim );
	void OnAdminFreeze(const MUID& uidAdmin, const char* szTargetName);

    void ReserveSuicide( void);
	bool IsReservedSuicide( void)		{ return m_bSuicide; }
	void CancelSuicide( void)			{ m_bSuicide = false; }

	int GetRolledDice( void)			{ return m_DiceRoll; }
	void SetRolledDice( int DiceRoll)	{ m_DiceRoll = DiceRoll; }
	void DestroyRoll( void)				{ m_DiceRoll = 0; }
	bool IsRecording( void)				{ return m_bRecording; }

	ZObserverCommandList* GetReplayCommandList()  { return &m_ReplayCommandList;} 

	void MakeResourceCRC32( const DWORD dwKey, DWORD& out_crc32, DWORD& out_xor );

	void OnResponseUseSpendableBuffItem(MUID& uidItem, int nResult);
	//πˆ«¡¡§∫∏¿”Ω√¡÷ºÆ 
	//void OnGetSpendableBuffItemStatus(MUID& uidChar, MTD_CharBuffInfo* pCharBuffInfo);

	void ApplyPotion(int nItemID, ZCharacter* pCharObj, float fRemainedTime);
	void OnUseTrap(int nItemID, ZCharacter* pCharObj, rvector& pos);
	void OnUseSpyTrap(int nItemID, ZCharacter* pCharObj, rvector& pos);
	void OnUseDynamite(int nItemID, ZCharacter* pCharObj, rvector& pos);
	void OnUseStunGrenade(int nItemID, ZCharacter* pCharObj, rvector& pos);

	void CheckZoneTrap(MUID uidOwner,rvector pos,MMatchItemDesc* pItemDesc, MMatchTeam nTeamID);
	void OnExplosionDynamite(MUID uidOwner, rvector pos, float fDamage, float fRange, float fKnockBack, MMatchTeam nTeamID);
	void OnExplosionStunGrenade(MUID uidOwner, rvector pos, float fDamage, float fRange, float fKnockBack, MMatchTeam nTeamID);
	void CheckZoneItemKit(MUID uidOwner, rvector pos, float fDamage, MMatchTeam nTeamID);
	int GetCharacterBasicInfoTick();
};


extern MUID g_MyChrUID;
extern float g_fFOV;

// dll-injection «Ÿ ∂ßπÆø° ∏≈≈©∑Œ ¿Œ∂Û¿Ã¥◊
#define ZGetCharacterManager()	(ZGetGame() ? &(ZGetGame()->m_CharacterManager) : NULL)
#define ZGetObjectManager()		(ZGetGame() ? &(ZGetGame()->m_ObjectManager) : NULL)

/*__forceinline bool IsMyCharacter(ZObject* pObject)
{
	return ((ZGetGame()) && ((ZObject*)ZGetGame()->m_pMyCharacter == pObject));
}*/

#define IsMyCharacter(pObject) \
( \
	ZGetGame() && \
	( \
		((ZObject*)(ZGetGame()->m_pMyCharacter))==((ZObject*)(pObject)) \
	) \
)


/*
// Damage ∞ËªÅE° « ø‰«— ªÁ«◊
#define DAMAGE_MELEE_HEAD	0.6f
#define DAMAGE_MELEE_CHEST	0.6f
#define DAMAGE_MELEE_HANDS	0.6f
#define DAMAGE_MELEE_LEGS	0.6f
#define DAMAGE_MELEE_FEET	0.6f

#define DAMAGE_RANGE_HEAD	0.8f
#define DAMAGE_RANGE_CHEST	0.5f
#define DAMAGE_RANGE_HANDS	0.5f
#define DAMAGE_RANGE_LEGS	0.5f
#define DAMAGE_RANGE_FEET	0.5f
*/


#endif