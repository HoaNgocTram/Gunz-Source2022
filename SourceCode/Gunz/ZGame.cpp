#include "stdafx.h"

#include "ZGame.h"
#include <windows.h>
//#include <zmouse.h>

#include "MZFileSystem.h"
#include "RealSpace2.h"
#include "FileInfo.h"
#include "MDebug.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "ZConsole.h"
#include "MCommandLogFrame.h"
#include "ZInterface.h"
#include "ZGameInterface.h"
#include "ZApplication.h"
#include "ZCommandTable.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "MStrEx.h"
#include "MMatchItem.h"
#include "ZEffectManager.h"
#include "ZEffectBillboard.h"
#include "Config.h"
#include "MProfiler.h"
#include "MMatchItem.h"
#include "ZScreenEffectManager.h"
#include "RParticleSystem.h"
#include "RDynamicLight.h"
#include "ZConfiguration.h"
#include "ZLoading.h"
#include "Physics.h"
#include "zeffectflashbang.h"
#include "ZInitialLoading.h"
#include "RealSoundEffect.h"
#include "RLenzFlare.h"
#include "ZWorldItem.h"
#include "ZMyInfo.h"
#include "ZNetCharacter.h"
#include "ZSecurity.h"
#include "ZStencilLight.h"
#include "ZMap.h"
#include "ZEffectStaticMesh.h"
#include "ZEffectAniMesh.h"
#include "ZGameAction.h"
#include "ZSkyBox.h"
#include "ZFile.h"
#include "ZObject.h"
#include "ZCharacter.h"
#include "ZActor.h"
#include "ZActorActionManager.h"
#include "ZMapDesc.h"
#include "MXml.h"
#include "ZGameClient.h"
#include "MUtil.h"
#include "RMeshMgr.h"
#include "RVisualMeshMgr.h"
#include "RMaterialList.h"
#include "RAnimationMgr.h"
#include "ZCamera.h"
#include "Mint4R2.h"
#include "../CSCommon/BlitzKriegs/Blitz.h"
//#include "RParticleSystem.h"
#include "ZItemDesc.h"

//#include "MObjectCharacter.h"
#include "MMath.h"
#include "ZQuest.h"
#include "MMatchUtil.h"
#include "ZReplay.h"
#include "ZRuleBerserker.h"
#include "ZRuleDuelTournament.h"
#include "ZRuleQuestChallenge.h"
#include "IRuleNewQuest.h"
#include "ZApplication.h"
#include "ZGameConst.h"

#include "ZRuleDuel.h"
#include "ZMyCharacter.h"
#include "MMatchCRC32XORCache.h"
#include "MMatchObjCache.h"

#include "ZModule_HealOverTime.h"
#include "ZRuleDeathMatch.h"
#include "ZActorWithFSM.h"
#include "ZNavigationMesh.h"

#include "ZRuleSpy.h"
#include "RGMain.h"
#include "VoiceChat.h"
#include <random>
#include "RBspObject.h"
#include "CodePageConversion.h"
#include "ZRuleBlitzKrieg.h"
#include "ZCompanion.h"
#include "ZRuleSkillMap.h"

// Custom: Disable NHN auth
//#ifdef LOCALE_NHNUSA
//#include "ZNHN_USA_Report.h"
//#endif

extern DWORD g_ShouldBanPlayer;

_USING_NAMESPACE_REALSPACE2

#define ENABLE_CHARACTER_COLLISION		// ƒ≥∏Ø≈Õ≥¢∏Æ √Êµπ√º≈©
#define ENABLE_ADJUST_MY_DATA			// ≈ı«•Ω√Ω∫≈€ µø¿€

/////////////////////////////////////////////////////////////////////
//√§∆√ πÆ¿⁄ø≠ ∞À?Eƒ⁄?E...¡ŸπŸ≤ﬁ πÆ¿⁄∏∏ ∞À?E....
void CheckMsgAboutChat(char* msg)
{
	//ø©±‚º≠ √§∆√?EÅE∞≈∏£¥¬∫Œ∫–.....
	int lenMsg = (int)strlen(msg);
	for (int i = 0; i < lenMsg; i++)
	{
		if (msg[i] == '\n' || msg[i] == '\r')
		{
			msg[i] = NULL;
			break;
		}
	}
}

struct RSnowParticle : public RParticle, CMemPoolSm<RSnowParticle>
{
	virtual bool Update(float fTimeElapsed);
};

bool RSnowParticle::Update(float fTimeElapsed)
{
	RParticle::Update(fTimeElapsed);

	if (position.z <= -1000.0f) return false;
	return true;
}

class ZSnowTownParticleSystem
{
private:
	RParticles* m_pParticles[3];

	bool IsSnowTownMap()
	{
	if (!strnicmp(ZGetGameClient()->GetMatchStageSetting()->GetMapName(), "snow", 4)) return true;
	  return false;
	}

public:
	void Update(float fDeltaTime)
	{
		if (!IsSnowTownMap()) return;

#define SNOW_PARTICLE_COUNT_PER_SECOND		400		// ?E?√ ?E300∞≥?Eø¯∑° «¡∑π¿”?E14∞≥ø¥¿Ω.

		int nSnowParticleCountPerSec = SNOW_PARTICLE_COUNT_PER_SECOND;
		switch (ZGetConfiguration()->GetVideo()->nEffectLevel)
		{
		case Z_VIDEO_EFFECT_HIGH:	break;
		case Z_VIDEO_EFFECT_NORMAL:	nSnowParticleCountPerSec = nSnowParticleCountPerSec / 4; break;
		case Z_VIDEO_EFFECT_LOW:	nSnowParticleCountPerSec = nSnowParticleCountPerSec / 8; break;
		default: nSnowParticleCountPerSec = 0; break;
		}

		int nCount = min(nSnowParticleCountPerSec * fDeltaTime, 20);	// «—π¯ø° 20∞≥ ¿ÃªÛ¿∫ æ»≥™ø¿µµ∑œ«—¥Ÿ
		for (int i = 0; i < nCount; i++)
		{
			RParticle* pp = new RSnowParticle();
			pp->ftime = 0;
			pp->position = rvector(RandomNumber(-8000.0f, 8000.0f), RandomNumber(-8000.0f, 8000.0f), 1500.0f);
			pp->velocity = rvector(RandomNumber(-40.0f, 40.0f), RandomNumber(-40.0f, 40.0f), RandomNumber(-150.0f, -250.0f));
			pp->accel = rvector(0, 0, -5.f);

			int particle_index = RandomNumber(0, 2);
			if (m_pParticles[particle_index]) m_pParticles[particle_index]->push_back(pp);
		}
	}
	void Create()
	{
		if (!IsSnowTownMap()) return;

		for (int i = 0; i < 3; i++)
		{
			m_pParticles[i] = NULL;
		}

		// ∆ƒ∆º≈¨ ª˝º∫
		m_pParticles[0] = RGetParticleSystem()->AddParticles("sfx/water_splash.bmp", 25.0f);
		m_pParticles[1] = RGetParticleSystem()->AddParticles("sfx/water_splash.bmp", 10.0f);
		m_pParticles[2] = RGetParticleSystem()->AddParticles("sfx/water_splash.bmp", 5.0f);
	}
	void Destroy()
	{
		if (!IsSnowTownMap()) return;

		// Custom: Checked for NULL pointer in m_pParticles
		if (m_pParticles[0])
		{
			m_pParticles[0]->Clear();
			m_pParticles[0] = NULL;
		}
		if (m_pParticles[1])
		{
			m_pParticles[1]->Clear();
			m_pParticles[1] = NULL;
		}
		if (m_pParticles[2])
		{
			m_pParticles[2]->Clear();
			m_pParticles[2] = NULL;
		}
	}
};

static ZSnowTownParticleSystem g_SnowTownParticleSystem;

ZGame* g_pGame = NULL;

float	g_fFOV = DEFAULT_FOV;
float	g_fFarZ = DEFAULT_FAR_Z;

//RParticleSystem* g_ParticleSystem = 0;
extern sCharacterLight g_CharLightList[NUM_LIGHT_TYPE];

MUID tempUID(0, 0);		// ∑Œƒ√ ≈◊Ω∫∆Æ?E
MUID g_MyChrUID(0, 0);

#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

void TestCreateEffect(int nEffIndex)
{
	float fDist = RandomNumber(0.0f, 100.0f);
	rvector vTar = rvector(RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f));
	rvector vPos = ZGetGame()->m_pMyCharacter->GetPosition();
	vPos.x += RandomNumber(0.0f, 100.0f);
	vPos.y += RandomNumber(0.0f, 100.0f);
	vPos.z += RandomNumber(0.0f, 100.0f);

	rvector vTarNormal = -RealSpace2::RCameraDirection;

	vTarNormal = rvector(RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f));

	ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;
	ZEffectManager* pEM = ZGetEffectManager();

	switch (nEffIndex)
	{
	case 0:
		pEM->AddReBirthEffect(vPos);
		break;
	case 1:
		pEM->AddLandingEffect(vPos, vTarNormal);
		break;
	case 2:
		pEM->AddGrenadeEffect(vPos, vTarNormal);
		break;
	case 3:
		pEM->AddRocketEffect(vPos, vTarNormal);
		break;
	case 4:
		pEM->AddRocketSmokeEffect(vPos);
		break;
	case 5:
		pEM->AddSwordDefenceEffect(vPos, -vTarNormal);
		break;
	case 6:
#ifdef _MACOLOR
		pEM->AddSwordWaveEffect(pCharacter->GetUID(), vPos, pCharacter->GetDirection());
#else
		pEM->AddSwordWaveEffect(vPos, 200, pCharacter);
#endif
		break;
	case 7:
		pEM->AddSwordUppercutDamageEffect(vPos, pCharacter->GetUID());
		break;
	case 8:
		pEM->AddBulletMark(vPos, vTarNormal);
		break;
	case 9:
		pEM->AddShotgunEffect(vPos, vPos, vTar, pCharacter);
		break;
	case 10:
		pEM->AddBloodEffect(vPos, vTarNormal);
		break;
	case 11:
		for (int i = 0; i < SEM_End; i++)
			pEM->AddSlashEffect(vPos, vTarNormal, i);
		break;
	case 12:
		pEM->AddSlashEffectWall(vPos, vTarNormal, 0);
		break;
	case 13:
		pEM->AddLightFragment(vPos, vTarNormal);
		break;
	case 14:
		//pEM->AddDashEffect(vPos, vTarNormal, pCharacter);
		pEM->AddDashEffect(vPos, vTarNormal, pCharacter, 0);
		break;
	case 15:
		pEM->AddSmokeEffect(vPos);
		break;
	case 16:
		pEM->AddSmokeGrenadeEffect(vPos);
		break;
	case 17:
		pEM->AddGrenadeSmokeEffect(vPos, 1.0f, 0.5f, 123);
		break;
	case 18:
		pEM->AddWaterSplashEffect(vPos, vPos);
		break;
	case 19:
		pEM->AddWorldItemEatenEffect(vPos);
		break;
	case 20:
		pEM->AddCharacterIcon(pCharacter, 0);
		break;
	case 21:
		pEM->AddCommanderIcon(pCharacter, 0);
		break;
	case 22:
		pEM->AddChatIcon(pCharacter);
		break;
	case 23:
		pEM->AddLostConIcon(pCharacter);
		break;
	case 24:
		pEM->AddChargingEffect(pCharacter);
		break;
	case 25:
		pEM->AddChargedEffect(pCharacter);
		break;
	case 26:
		pEM->AddTrackFire(vPos);
		pEM->AddTrackFire(vPos);
		pEM->AddTrackFire(vPos);
		pEM->AddTrackFire(vPos);
		break;
	case 27:

		ZEffectWeaponEnchant * pEWE = pEM->GetWeaponEnchant(ZC_ENCHANT_FIRE);

		if (pEWE) {
			//«•¡ÿ ªÁ¿Ã¡˚–¬ ƒ´≈∏≥™... 100 ¡§µµ..
			float fSIze = 105.f / 100.f;
			rvector vScale = rvector(0.6f * fSIze, 0.6f * fSIze, 0.9f * fSIze);// π´±‚¿« ≈©±‚ø° µ˚∂Ûº≠..
			pEWE->SetUid(pCharacter->GetUID());
			pEWE->SetAlignType(1);
			pEWE->SetScale(vScale);
			pEWE->Draw(timeGetTime());
		}

		break;
	}
}
//Custom: BlitzKRieg
float CalcBlitzDamage(MMatchWeaponType& weaponType)
{
	float damage = 0;
	switch (weaponType)
	{
	case MWT_SMG:
	case MWT_SMGx2:
		damage = MGetBlitz()->GetWeaponDPS().smg; break;
	case MWT_RIFLE:
		damage = MGetBlitz()->GetWeaponDPS().rifle; break;
	case MWT_MACHINEGUN:
		damage = MGetBlitz()->GetWeaponDPS().machineGun; break;
	case MWT_PISTOL:
	case MWT_PISTOLx2:
		damage = MGetBlitz()->GetWeaponDPS().pistol; break;
	case MWT_REVOLVER:
	case MWT_REVOLVERx2:
		damage = MGetBlitz()->GetWeaponDPS().revolver; break;
	case MWT_SHOTGUN:
	case MWT_SAWED_SHOTGUN:
		damage = MGetBlitz()->GetWeaponDPS().shotGun; break;
	case MWT_ROCKET:
		damage = MGetBlitz()->GetWeaponDPS().roket; break;
	case MWT_DAGGER:
	case MWT_DUAL_DAGGER:
		damage = MGetBlitz()->GetWeaponDPS().dagger; break;
	case MWT_KATANA:
		damage = MGetBlitz()->GetWeaponDPS().katana; break;
	case MWT_DOUBLE_KATANA:
		damage = MGetBlitz()->GetWeaponDPS().doubleKatana; break;
	}
	return damage;
}
float CalcActualDamage(ZObject* pAttacker, ZObject* pVictim, float fDamage, MMatchWeaponType weaponType, ZItem* pItem)
{
	if (!pItem)
		return 0.f;
	if (!pItem->GetDesc())
		return 0.f;

	float fOutDamage = pItem->GetDesc()->m_nDamage.Ref();
	if (!fOutDamage)
		return 0.f;

	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_DMG) &&
		!ZGetGame()->GetMatch()->IsQuestDrived() &&
		!ZGetGame()->GetMatch()->IsQuestChallengue() &&
		!ZGetGame()->GetMatch()->IsRuleSpy() &&
		!ZGetGame()->GetMatch()->IsBlitz() &&
		!ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED)
	{
		int nMultiplier = ZGetGameClient()->GetMatchStageSetting()->GetModifierValue(MMOD_DMG);

		if (nMultiplier > 1 && nMultiplier < 4)
			fOutDamage *= (float)nMultiplier;
	}

	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BERSERKER)
	{
		ZRuleBerserker* pRule = (ZRuleBerserker*)ZGetGame()->GetMatch()->GetRule();
		if ((pAttacker) && (pAttacker != pVictim) && (pAttacker->GetUID() == pRule->GetBerserkerUID()))
		{
			return fDamage * BERSERKER_DAMAGE_RATIO;
		}
	}
	if (ZGetGame()->GetMatch()->IsTurboMode())
	{
		return fDamage * 3.0f;
	}
	float bonusDmg = fDamage;
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BLITZKRIEG)
	{
		return fDamage + (bonusDmg * CalcBlitzDamage(weaponType));
	}

	return fDamage;
}

void TestCreateEffects()
{
	int nCount = 100;

	for (int i = 0; i < nCount; i++)
	{
		int nEffIndex = RandomNumber(25, 28);
		TestCreateEffect(nEffIndex);
	}
}

bool CompareZCharFloat(pair<ZCharacter*, float> i, pair<ZCharacter*, float> j) { return (i.second < j.second); }

ZGame::ZGame()
{
	m_bShowWireframe = false;
	m_pMyCharacter = NULL;

	m_bReserveObserver = false;

	//	memset(m_nLastTime, 0, sizeof(DWORD) * ZLASTTIME_MAX);
	for (int i = 0; i < ZLASTTIME_MAX; i++)
	{
		m_nLastTime[i] = timeGetTime();
	}

	//	m_fTime = 0.f;
	m_fTime.Set_MakeCrc(0.0f);

	m_nReadyState = ZGAME_READYSTATE_INIT;
	m_pParticles = NULL;
	m_render_poly_cnt = 0;
	m_nReservedObserverTime = 0;
	m_nSpawnTime = 0;
	m_t = 0;
	m_DiceRoll = 0;
	m_dwLastInputTime = timeGetTime();

	m_bRecording = false;
	//	m_pReplayFile = NULL;
	m_pReplayFile = NULL;

	m_bReplaying.Set_MakeCrc(false);
	m_bShowReplayInfo = true;
	m_bSpawnRequested = false;
	m_ReplayLogTime = 0;
	ZeroMemory(m_Replay_UseItem, sizeof(m_Replay_UseItem));

	m_pGameAction = new ZGameAction;
	DmgCounter = new DamageCounterMap();

	CancelSuicide();
}

ZGame::~ZGame()
{
	m_pMyCharacter = NULL;
	delete m_pGameAction;

	g_SnowTownParticleSystem.Destroy();
	RSnowParticle::Release();
}

bool ZGame::Create(MZFileSystem* pfs, ZLoadingProgress* pLoading)
{
	// ≈¨∑£?E°º≠¥¬ stagestate ∞° æ¯æ˚ÿ≠ CastStageBridgePeer ∏¶ æ»«ÿº≠ ø©±‚º≠»£?E
	// Custom: Updated string
	mlog("CastStageBridgePeer call in Zgame::Create\n");
	//mlog("CastStageBridgePeer »£?Ein Zgame::Create\n");
	ZGetGameClient()->CastStageBridgePeer(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());

	mlog("game create begin , type = %d\n", ZGetGameClient()->GetMatchStageSetting()->GetGameType());

	SetReadyState(ZGAME_READYSTATE_INIT);	// Sync ∏¬¿ª∂ß±˚?EGame Loop ¡¯¿‘æ µµ∑œ

#ifdef _QUEST
	{
		if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_QUEST_CHALLENGE)
			ZGetQuest()->OnGameCreate();
	}
#endif

	//	m_ItemDescManager.Create(FILENAME_ZITEM_DESC);	// ≥™¡ﬂø° ≥÷æ˚⁄ﬂ?E

		// world∏¶ ºº∆√
	if (ZGetApplication()->GetLaunchMode() != ZApplication::ZLAUNCH_MODE_STANDALONE_AI &&
		ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) && ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_QUEST_CHALLENGE) {
		for (int i = 0; i < ZGetQuest()->GetGameInfo()->GetMapSectorCount(); i++)
		{
			MQuestMapSectorInfo* pSecInfo = ZGetQuest()->GetSectorInfo(ZGetQuest()->GetGameInfo()->GetMapSectorID(i));
			if (pSecInfo == NULL)
			{
				char strBuf[256];
				sprintf(strBuf, "[MQuestMapSectorInfo] m_MapSectorVector[index]:%d, GetMapSectorID:%d\n", i, ZGetQuest()->GetGameInfo()->GetMapSectorID(i));
				ASSERT(0 && strBuf);
			}
			ZGetWorldManager()->AddWorld(pSecInfo->szTitle);
#ifdef _DEBUG
			mlog("map(%s)\n", pSecInfo ? pSecInfo->szTitle : "null");
#endif
		}
	}
	else if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_QUEST_CHALLENGE) {
		//toodk quest ¿Ã∞« ¿”Ω√ ∏ ∑Œµ˘¿Ã¥Ÿ.. ∏  ∏Æº“Ω∫∏¶ æ˚–¿ ∆˙¥ıø° µ— ∞Õ¿Œ?E ∑Œ?EπÊΩƒ¿∫ ¿Ã∑∏∞‘ µ˚∑Œ ifπÆ¿ª ≥÷¿ª ∞«?E
		// æ∆¥œ?E±‚¡∏ ±∏¡∂ø° ∏¬∞‘ ?E?≥÷¿ª ∞Õ¿Œ?E∞·¡§«ÿæﬂ∏∏ «—¥Ÿ
		//ZGetWorldManager()->AddWorld("Mansion_room2");

		if (!ZRuleQuestChallenge::LoadScenarioMap(ZGetGameClient()->GetMatchStageSetting()->GetMapName())) // Ω√≥™∏Æø¿ ¿Ã∏ß¿ª ≥—∞‹º≠ ∆˜«‘µ» ∏ ¿ª ∑Œµ˘Ω√≈¥
			return false;
	}
	else {
		ZGetWorldManager()->AddWorld(ZGetGameClient()->GetMatchStageSetting()->GetMapName());
	}

	if (!ZGetWorldManager()->LoadAll(pLoading))
		return false;

	ZGetWorldManager()->SetCurrent(0);

	if (ZGetApplication()->GetLaunchMode() != ZApplication::ZLAUNCH_MODE_STANDALONE_AI &&
		ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) && ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_QUEST_CHALLENGE)
	{
		if (ZGetQuest() && ZGetQuest()->GetGameInfo())
			ZGetWorldManager()->SetCurrent((ZGetQuest()->GetGameInfo()->GetCurrSectorIndex()));
	}

	//RSetCamera(rvector(-10.f,0.f,150.f),rvector(0.f,0.f,0.f),rvector(0.f,1.f,0.f));
	//RSetProjection(DEFAULT_FOV,DEFAULT_NEAR_Z,DEFAULT_FAR_Z);

	//char szMapFileName[256];
	//char szMapPath[64] = "";
	//ZGetCurrMapPath(szMapPath);

	//sprintf(szMapFileName, "%s%s/%s.rs",
	//	szMapPath,
	//	ZGetGameClient()->GetMatchStageSetting()->GetMapName(),
	//	ZGetGameClient()->GetMatchStageSetting()->GetMapName());

	//if(!strlen(szMapFileName))
	//	return false;

	//mlog("ZGame::Create() :: ReloadAllAnimation Begin \n");
	ZGetMeshMgr()->ReloadAllAnimation();// ¿–?Eæ ¿∫ ø°¥œ∏ﬁ¿Ãº«¿Ã ¿÷¥Ÿ?E∑Œ?E
	mlog("Reload all animation end \n");

	//ZGetInitialLoading()->SetPercentage( 90.f );
//	ZGetInitialLoading()->SetPercentage( 70.f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );

	// ≥≠¿‘¿œ∂ßø°¥¬ PeerList∏¶ ø‰√ª«—¥Ÿ
	if (ZGetGameClient()->IsForcedEntry())
	{
		ZPostRequestPeerList(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());
		//		ZPostRequestGameInfo(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());
	}

	//if(!GetWorld()->GetBsp()->Open(szMapFileName))
	//{
	//	MLog("error while loading %s \n",szMapFileName);
	//	return false;
	//}

	//mlog("ZGame::Create() GetWorld()->GetBsp()->Open %s \n",szMapFileName);

	//GetWorld()->GetBsp()->OptimizeBoundingBox();

	//ZGetInitialLoading()->SetPercentage( 100.f );

	//GetMapDesc()->Open(&m_bsp);

	g_fFOV = DEFAULT_FOV;
	rvector dir = GetMapDesc()->GetWaitCamDir();
	rvector pos = GetMapDesc()->GetWaitCamPos();
	rvector up(0, 0, 1);
	RSetCamera(pos, pos + dir, up);

	int nModelID = -1;

	m_Match.Create();

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));

	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 1.0f;
	mtrl.Diffuse.a = 1.0f;

	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 1.0f;
	mtrl.Ambient.a = 1.0f;

	RGetDevice()->SetMaterial(&mtrl);

	//	m_fTime=0.f;
	m_fTime.Set_CheckCrc(0.0f);
	m_bReserveObserver = false;

#ifdef _BIRDSOUND
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_BATTLE);
	ZApplication::GetSoundEngine()->PlayMusic();
#else
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_BATTLE, pfs);
	ZApplication::GetSoundEngine()->PlayMusic();
#endif

	m_CharacterManager.Clear();
	m_ObjectManager.Clear();

	// mlog("ZGame::Create() m_CharacterManager.Clear done \n");

	m_pMyCharacter = (ZMyCharacter*)m_CharacterManager.Add(ZGetGameClient()->GetPlayerUID(), rvector(0.0f, 0.0f, 0.0f), true);

	{
		g_CharLightList[GUN].fLife = 300;
		g_CharLightList[GUN].fRange = 100;
		g_CharLightList[GUN].iType = GUN;
		g_CharLightList[GUN].vLightColor.x = 5.0f;
		g_CharLightList[GUN].vLightColor.y = 1.0f;
		g_CharLightList[GUN].vLightColor.z = 1.0f;

		g_CharLightList[SHOTGUN].fLife = 1000;
		g_CharLightList[SHOTGUN].fRange = 150;
		g_CharLightList[SHOTGUN].iType = SHOTGUN;
		g_CharLightList[SHOTGUN].vLightColor.x = 6.0f;
		g_CharLightList[SHOTGUN].vLightColor.y = 1.3f;
		g_CharLightList[SHOTGUN].vLightColor.z = 1.3f;

		g_CharLightList[CANNON].fLife = 1300;
		g_CharLightList[CANNON].fRange = 200;
		g_CharLightList[CANNON].iType = CANNON;
		g_CharLightList[CANNON].vLightColor.x = 7.0f;
		g_CharLightList[CANNON].vLightColor.y = 1.3f;
		g_CharLightList[CANNON].vLightColor.z = 1.3f;
	}

	ZGetFlashBangEffect()->SetBuffer();
	ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();

#ifdef _BIRDSOUND

#else
	ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
	ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME);
#endif

	// Net init
	ZApplication::ResetTimer();
	m_GameTimer.Reset();
	ZSetupDataChecker_Game(&m_DataChecker);

	ZGetInitialLoading()->SetPercentage(100.f);
	ZGetInitialLoading()->Draw(MODE_DEFAULT, 0, true);

#ifdef _BIRDSOUND

#else
	list<AmbSndInfo*> aslist = GetWorld()->GetBsp()->GetAmbSndList();
	for (list<AmbSndInfo*>::iterator iter = aslist.begin(); iter != aslist.end(); ++iter)
	{
		AmbSndInfo* pAS = *iter;
		if (pAS->itype & AS_AABB)
			ZGetSoundEngine()->SetAmbientSoundBox(pAS->szSoundName, pAS->min, pAS->max, (pAS->itype & AS_2D) ? true : false);
		else if (pAS->itype & AS_SPHERE)
			ZGetSoundEngine()->SetAmbientSoundSphere(pAS->szSoundName, pAS->center, pAS->radius, (pAS->itype & AS_2D) ? true : false);
	}
#endif

	// ∑Œ?E¥Ÿ âÁæ˚€ÅE. ∂Û?E¥Ÿ∏• ªÁ∂˜µÈ«—≈◊ æÀ∏∞¥Ÿ.
	MEMBER_SET_CHECKCRC(m_pMyCharacter->GetStatus(), nLoadingPercent, 100);
	ZPostLoadingComplete(ZGetGameClient()->GetPlayerUID(), 100);

	// ∞‘¿”ø° µÈæ˚Ã¨¥Ÿ?Eº≠æÀ∏≤
	ZPostStageEnterBattle(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());

	char tmpbuf[128];
	_strtime(tmpbuf);

	// µµøÚ∏ª »≠∏Èª˝º∫..

	mlog("game created ( %s )\n", tmpbuf);

	ZGetGameInterface()->GetCamera()->SetLookMode(ZCAMERA_DEFAULT);


	g_SnowTownParticleSystem.Create();

	return true;
}

void ZGame::Destroy()
{
	//jintriple3 ø©?E≥ª∞° ∏÷ √ﬂ∞°«—∞≈?E?§—§Ã
	m_DataChecker.Clear();

	g_SnowTownParticleSystem.Destroy();

	SetClearColor(0);

	mlog("game destroy begin\n");

	ZGetGameClient()->AgentDisconnect();

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->CloseMusic();

	mlog("Destroy sound engine()\n");

#ifdef _QUEST
	{
		ZGetQuest()->OnGameDestroy();
	}
#endif

	m_Match.Destroy();

	mlog("game destroy match destroy \n");

	if (ZGetGameClient()->IsForcedEntry())
	{
		ZGetGameClient()->ReleaseForcedEntry();

		ZGetGameInterface()->SerializeStageInterface();
	}

	ZPostStageLeaveBattle(ZGetGameClient()->GetPlayerUID(), ZGetGameInterface()->GetIsGameFinishLeaveBattle());//, ZGetGameClient()->GetStageUID());

	//SAFE_DELETE( g_ParticleSystem );

	ReleaseFlashBangEffect();

	// mlog("ZGame::Destroy SAFE_DELETE(m_pSkyBox) \n");

	RGetLenzFlare()->Clear();

	// ∞‘¿”¿Ã ≥°≥Ø ∂ß∏∂¥Ÿ ∏ﬁ∏∏Æ ∏±∏Æ?E«ÿ¡‹...
	//ReleaseMemPool(RealSoundEffectFx);
	//ReleaseMemPool(RealSoundEffect);
	//ReleaseMemPool(RealSoundEffectPlay);

//	int e_size = m_EffectManager.m_Effects[0].size();
//	e_size += m_EffectManager.m_Effects[1].size();
//	e_size += m_EffectManager.m_Effects[2].size();
//	int w_size = m_WeaponManager.m_list.size();
//	mlog("ZGame e_size : w_size : %d %d\n",e_size,w_size);

//	ZGetEffectManager()->Clear();
	m_WeaponManager.Clear();
#ifdef _WORLD_ITEM_
	ZGetWorldItemManager()->Clear();
#endif

#ifdef _BIRDSOUND

#else
	ZGetSoundEngine()->ClearAmbientSound();
#endif

	m_ObserverCommandList.Destroy();
	m_ReplayCommandList.Destroy();
	m_DelayedCommandList.Destroy();

	ZGetEffectManager()->Clear();
	ZGetScreenEffectManager()->Clear();

	ZGetWorldManager()->Clear();

	// Custom: Delete character cache
	ZGetCharacterManager()->Clear();

	char tmpbuf[128];
	_strtime(tmpbuf);

	mlog("game destroyed ( %s )\n", tmpbuf);
}

bool ZGame::CreateMyCharacter(MTD_CharInfo* pCharInfo/*, MTD_CharBuffInfo* pCharBuffInfo*/)
{
	if (!m_pMyCharacter) return false;

	m_pMyCharacter->Create(pCharInfo/*, pCharBuffInfo*/);
	m_pMyCharacter->SetVisible(true);

	mlog("Create character : Name=%s Level=%d \n", pCharInfo->szName, pCharInfo->nLevel);
	return true;
}

bool ZGame::CheckGameReady()
{
	if (GetReadyState() == ZGAME_READYSTATE_RUN) {
		return true;
	}
	else if (GetReadyState() == ZGAME_READYSTATE_INIT) {
		SetReadyState(ZGAME_READYSTATE_WAITSYNC);
		// Ω√∞£ ΩÃ≈© ø‰√ª
		ZPostRequestTimeSync(GetTickTime());
		return false;
	}
	else if (GetReadyState() == ZGAME_READYSTATE_WAITSYNC) {
		return false;
	}
	return false;
}

void ZGame::OnGameResponseTimeSync(unsigned int nLocalTimeStamp, unsigned int nGlobalTimeSync)
{
	ZGameTimer* pTimer = GetGameTimer();
	int nCurrentTick = pTimer->GetGlobalTick();
	int nDelay = (nCurrentTick - nLocalTimeStamp) / 2;
	int nOffset = (int)nGlobalTimeSync - (int)nCurrentTick + nDelay;

	pTimer->SetGlobalOffset(nOffset);

	SetReadyState(ZGAME_READYSTATE_RUN);
}

void ZGame::Update(float fElapsed)
{
	if (CheckGameReady() == false) {
		OnCameraUpdate(fElapsed);
		return;
	}
#ifdef _ASSITEN
	if (ZGetCamera()->m_bAutoAiming)
		AutoAiming();
#endif

	/*if (ZGetGame())
		ZGetGame()->BerserkerKI();*/

		/*if (ZGetGame())
			ZGetGame()->GetJjang();*/

	GetWorld()->Update(fElapsed);

	ZGetEffectManager()->Update(fElapsed);
	ZGetScreenEffectManager()->UpdateEffects();

	m_GameTimer.UpdateTick(timeGetTime());
	m_fTime.Set_CheckCrc(m_fTime.Ref() + fElapsed);
	m_fTime.ShiftHeapPos();
	m_bReplaying.ShiftHeapPos_CheckCrc();
	m_ObjectManager.Update(fElapsed);

	if (m_pMyCharacter && !m_bReplaying.Ref())
	{
		if (ZGetGame() && ZGetGame()->m_pMyCharacter && !ZGetGame()->IsReplay())
		{
			//if (ZGetGame()->CheckGameReady() == false) continue;forworker
			ZGetGame()->PostBasicInfo();
			if (ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT)
				ZGetGame()->PostHPAPInfo();
			else
				ZGetGame()->PostDuelTournamentHPAPInfo(); // µ‡æÛ≈‰≥ ∏’∆Æ ø°º≠∏∏ √≥∏Æ«ÿ¡ÿ¥Ÿ.

			ZGetGame()->PostPeerPingInfo();
			ZGetGame()->PostSyncReport();
		}
	}
	CheckMyCharDead(fElapsed);
	if (m_bReserveObserver)
	{
		OnReserveObserver();
	}

	UpdateCombo();

	g_SnowTownParticleSystem.Update(fElapsed);

#ifdef _WORLD_ITEM_
	ZGetWorldItemManager()->update();
#endif
	m_Match.Update(fElapsed);

	if (m_bReplaying.Ref())
		OnReplayRun();
	// ∏Æ«√∑π¿Ã∂ßø°¥¬ π´¡∂∞« ø…¿˙?E√≥∏Æ∏¶ «ÿæﬂ «“?E?
	if (ZGetGameInterface()->GetCombatInterface()->GetObserverMode() || m_bReplaying.Ref())
		OnObserverRun();

	ProcessDelayedCommand();

	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_ROLLTHEDICE))
	{
		if (ZGetGame()->GetRolledDice() == 1)
			ZGetGameInterface()->GetCamera()->m_fDist = CAMERA_DIST_MAX;
		else
		{
			if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_FPS))
			{
				ZGetGameInterface()->GetCamera()->m_fDist = 30.f;
			}
			else
				ZGetGameInterface()->GetCamera()->m_fDist = CAMERA_DEFAULT_DISTANCE;
		}
	}
	if (ZGetGame() && ZGetConfiguration()->GetEtc()->bFirstPerson)
	{
		ZGetGameInterface()->GetCamera()->m_fDist = 30.f;
	}
	if ((ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_ROLLTHEDICE) && ZGetGame()->GetRolledDice() == 9))
	{
		if (m_pMyCharacter->GetDistToFloor() <= 3.0f) m_pMyCharacter->SetVelocity(m_pMyCharacter->GetVelocity().x, m_pMyCharacter->GetVelocity().y, 800.0f);
	}

	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_ROLLTHEDICE) && GetRolledDice() == 11)
		m_bShowWireframe = true;
	else
		m_bShowWireframe = false;

#ifdef _QUEST
	{
		ZGetQuest()->OnGameUpdate(fElapsed);
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	//	TODO : ªÛ»≤ø° µ˚?Eæ˜µ•¿Ã∆Æ æ»«ÿ¡‡µµ µ… ∂ß∏¶ ±∏∫∞«ÿ ≥æ?E¿÷¿ª?E
	//////////////////////////////////////////////////////////////////////////
	//g_ParticleSystem->Update(0.05);
	RGetParticleSystem()->Update(fElapsed);

	if (Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->Update();

	OnCameraUpdate(fElapsed);
	//	m_fTime->SetWarpingAdd(GetTickCount());

	m_WeaponManager.Update();
}

void ZGame::OnCameraUpdate(float Elapsed)
{
	if (m_Match.GetRoundState() == MMATCH_ROUNDSTATE_PREPARE)
	{
		rvector dir = GetMapDesc()->GetWaitCamDir();
		rvector pos = GetMapDesc()->GetWaitCamPos();
		rvector up(0, 0, 1);

		RSetCamera(pos, pos + dir, up);
	}
	else
	{
		ZGetGameInterface()->GetCamera()->Update(Elapsed);
	}
}
//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑
void ZGame::CheckMyCharDeadByCriticalLine()
{
	MUID uidAttacker = MUID(0, 0);
	bool bReturnValue = m_pMyCharacter->GetPosition().z >= DIE_CRITICAL_LINE;
	if (m_pMyCharacter->GetPosition().z >= DIE_CRITICAL_LINE)	//≥™?E¿ßø° ¿÷¿Ω ªÛ?Eœ¡ÅEæ ?E.
		PROTECT_DEBUG_REGISTER(bReturnValue)
		return;

	// ≥™∂Ù¿∏∑Œ ∂≥æ˚›ˆ?E≥°..-_-;
	//m_pMyCharacter->SetVelocity(rvector(0,0,0));
	uidAttacker = m_pMyCharacter->GetLastThrower();

	ZObject* pAttacker = ZGetObjectManager()->GetObject(uidAttacker);
	if (pAttacker == NULL || !CanAttack(pAttacker, m_pMyCharacter))
	{
		uidAttacker = ZGetMyUID();
		pAttacker = m_pMyCharacter;
	}

	m_pMyCharacter->OnDamaged(pAttacker, m_pMyCharacter->GetPosition(), ZD_FALLING, MWT_NONE, m_pMyCharacter->GetHP());
	// Custom: Made CMT_SYSTEM
	ZChatOutput(ZMsg(MSG_GAME_FALL_NARAK), ZChat::CMT_SYSTEM);

#ifdef _DEBUG
	if (ZIsLaunchDevelop() && ZGetQuest()->GetCheet(ZQUEST_CHEET_GOD) == true)
		ZGetGameInterface()->RespawnMyCharacter();
#endif
}
//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑
void ZGame::CheckMyCharDeadUnchecked()
{
	MUID uidAttacker = MUID(0, 0);
	//	if ((m_pMyCharacter->IsDie() == false) && (m_pMyCharacter->GetHP() <= 0))
	bool bCheck = (m_pMyCharacter->IsDie() == true) | (m_pMyCharacter->GetHP() > 0);
	if ((m_pMyCharacter->IsDie() == true) || (m_pMyCharacter->GetHP() > 0))
		PROTECT_DEBUG_REGISTER(bCheck)
		return;

	//hp <=0 && m_pMyCharacter->IsDie() == false
	if (uidAttacker == MUID(0, 0) && m_pMyCharacter->GetLastAttacker() != MUID(0, 0))
		uidAttacker = m_pMyCharacter->GetLastAttacker();

	// ¥Ÿ¿Ω∂Ûø˚—Â∑Œ ≥—æ˚Ã°?E¿ß«— finish ªÛ≈¬ø°º≠¥¬ ∏ﬁΩ√?E∂Û?E√¿?ª˝∑´«—¥Ÿ
	if (GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FINISH)
	{
		// ¡◊¥¬ √¥¿∫ «—¥Ÿ
		m_pMyCharacter->ActDead();
		m_pMyCharacter->Die();
		return;
	}

	ZPostDie(uidAttacker);		// ««æ˚—Èø°∞‘ ∫∏≥ª¥¬ ∏ﬁºº?E

	// ƒ˘Ω∫∆Æ ∏µÂ¥¬ ¡◊¿Ω ∏ﬁºº¡ˆ∞° ¥Ÿ∏£¥Ÿ.
	if (!ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) &&
		!ZGetGameTypeManager()->IsQuestChallengeOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) && !ZGetGameTypeManager()->IsBlitzQuest(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		ZPostGameKill(uidAttacker);	// º≠πˆø° ∫∏≥ª¥¬ ∏ﬁºº¡ˆ
	}
	else if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		ZPostQuestGameKill();
	}
	else if (ZGetGameTypeManager()->IsQuestChallengeOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) && ZGetGameTypeManager()->IsBlitzQuest(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) {
		ZCharacter* pCharacter = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(uidAttacker));
		if (pCharacter) {
			ZPostGameKill(uidAttacker);
		}
		else {
			ZActorWithFSM* pActor = dynamic_cast<ZActorWithFSM*>(ZGetObjectManager()->GetObject(uidAttacker));
			if (pActor) {
				ZPostQuestGameKill();
			}
		}
	}

	// ¡◊æ˙¿ª∂ß πË∆≤ø°º≠ ≥™∞°?Eøπæ‡¿Ã µ«æ˚‹÷¿∏?Eƒ´ø˚‚Æ∏¶ ¥ŸΩ√ Ω√¿€«ÿ¡ÿ¥Ÿ.
	if (ZApplication::GetGameInterface()->IsLeaveBattleReserved() == true)
		ZApplication::GetGameInterface()->ReserveLeaveBattle();
}

void ZGame::CheckMyCharDead(float fElapsed)
{
	//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «Ÿ
	bool bReturnValue = !m_pMyCharacter || m_pMyCharacter->IsDie();
	if (!m_pMyCharacter || m_pMyCharacter->IsDie())
		PROTECT_DEBUG_REGISTER(bReturnValue)
		return;

	CheckMyCharDeadByCriticalLine();
	CheckMyCharDeadUnchecked();
}

void ZGame::OnPreDraw()
{
	__BP(19, "ZGame::sub1");

	RSetProjection(g_fFOV, DEFAULT_NEAR_Z, g_fFarZ);

	bool bTrilinear = RIsTrilinear();
	RGetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE);
	RGetDevice()->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState(1, D3DSAMP_MIPFILTER, bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE);

	if (m_bShowWireframe) {
		RGetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		RGetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
		GetWorld()->SetFog(false);
	}
	else {
		RGetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		GetWorld()->SetFog(true);
	}

	GetWorld()->GetBsp()->SetWireframeMode(m_bShowWireframe);

	rmatrix initmat;
	D3DXMatrixIdentity(&initmat);
	RGetDevice()->SetTransform(D3DTS_WORLD, &initmat);
	RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	//not sure if right place
	RGetDevice()->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	LPDIRECT3DDEVICE9 pd3dDevice = RGetDevice();
	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->SetTexture(1, NULL);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	if (m_pMyCharacter)
	{
		if (ZGetConfiguration()->GetVideo()->bDynamicLight)
		{
			rvector pos = m_pMyCharacter->GetPosition();
			RGetDynamicLightManager()->SetPosition(pos);
		}
	}

	__EP(19);
}

int g_debug_render_mode = 0;

extern MDrawContextR2* g_pDC;

int IsNan(double x)
{
	union { unsigned __int64 u; double f; } ieee754;
	ieee754.f = x;

	return ((unsigned)(ieee754.u >> 32) & 0x7fffffff) + ((unsigned)ieee754.u != 0) > 0x7ff00000;
}

int IsInf(double x)
{
	union { unsigned __int64 u; double f; } ieee754;
	ieee754.f = x;

	return ((unsigned)(ieee754.u >> 32) & 0x7fffffff) == 0x7ff00000 && ((unsigned)ieee754.u == 0);
}

void ZGame::Draw()
{
	////////≈◊Ω∫∆Æ ?Eƒ⁄?E..≥™¡ﬂø° ¡ˆ?E?.//////////////////////////////
	////////////////////////////////////////////////////////////////////for test
		/*
	#ifdef _DEBUG
		if(GetAsyncKeyState( VK_UP ))	//ƒÆº¶ «Ÿ
		{
			fShotTime += 0.1f;
			rvector vPos = g_pGame->m_pMyCharacter->GetPosition();

			ZPostShotMelee( vPos, nShot);
			nShot++;
			if(nShot >4)
				nShot = 1;
		}
		if(GetAsyncKeyState( VK_LEFT ))		//∞≠∫£?E«Ÿ
		{
			fShotTime += 0.1f;
			ZPostSkill( ZC_SKILL_SPLASHSHOT, MMCIP_MELEE);
		}
		if(GetAsyncKeyState( VK_RIGHT ))	//ºˆ∑˘≈∫ «Ÿ
		{
			rvector vPos = g_pGame->m_pMyCharacter->GetPosition();
			rvector vDir = g_pGame->m_pMyCharacter->GetDirection();
			fShotTime += 0.1f;

			vPos.z += 120.0f;

			int type = ZC_WEAPON_SP_GRENADE;	//weapon
			int sel_type = MMCIP_MELEE; //MMCIP_PRIMARY; //MMCIP_MELEE;			//parts
			ZPostShotSp( vPos, vDir, type, sel_type);
		}
		if(GetAsyncKeyState( VK_DOWN ))	    //∑Œƒœ «Ÿ
		{
			rvector vPos = g_pGame->m_pMyCharacter->GetPosition();
			rvector vDir = g_pGame->m_pMyCharacter->GetDirection();

			vPos.z += 120.0f;
			fShotTime += 0.1f;

			int type = ZC_WEAPON_SP_ROCKET; //ZC_WEAPON_SP_GRENADE;	//weaponww
		»§Ω√ ≥™¡ﬂø° ∂« «Ÿ ?E√«ÿº≠ æµ¿œ¿Ã ¿÷¿ªºˆµµ ¿÷æ˚ÿ≠ ¡÷ºÆ√≥∏Æ«ÿº≠ ≥≤∞‹µ“..
		*/

		////////////////////////////////////////////////////////////////////
	__BP(20, "ZGame::Draw");

	RRESULT isOK = RIsReadyToRender();

	if (isOK == R_NOTREADY)
	{
		__EP(20);
		return;
	}

	OnPreDraw();		// Device ªÛ≈¬∞™ º≥¡§¿∫ ø©±‚º≠ «œ¿⁄

//	RRenderNodeMgr::m_bRenderBuffer = true;//------test code
	/*
	else if(isOK==R_RESTORED) {
	} // restore device dependent objects
	*/

	rmatrix _mat;
	RGetDevice()->GetTransform(D3DTS_WORLD, &_mat);

	__BP(21, "ZGame::Draw::DrawWorld");
	GetWorld()->Draw();
	__EP(21);

	// ∏ ø° ±‚º˙µ» ∆Ø?E¥ıπÃ ø¿?Eß∆Æ¡ﬂ ±◊∑¡æﬂ «“∞Õ¿Ã ¿÷¥¬ ∞Ê?E

	ZMapDesc* pMapDesc = GetMapDesc();

	if (pMapDesc) {
		pMapDesc->DrawMapDesc();
	}

	/*
	D3DLIGHT9 light;
	light.Type			= D3DLIGHT_POINT;
	light.Ambient.r		= 0.1f;
	light.Ambient.g		= 0.1f;
	light.Ambient.b		= 0.1f;
	light.Specular.r	= 1.0f;
	light.Specular.g	= 1.0f;
	light.Specular.b	= 1.0f;
	light.Attenuation0	= 0.05f;
	light.Attenuation1	= 0.002f;
	light.Attenuation2	= 0.0f;

	light.Range			= 200.f;
	light.Position		= m_pMyCharacter->GetPosition();

	light.Diffuse.r		= .9f;
	light.Diffuse.g		= .1f;
	light.Diffuse.b		= .1f;

	GetWorld()->GetBsp()->DrawLight(&light);
	//*/
	if (m_Match.GetRoundState() != MMATCH_ROUNDSTATE_PREPARE)
	{
		__BP(22, "ZGame::Draw::DrawCharacters");

		m_ObjectManager.Draw();

		__EP(22);

		m_render_poly_cnt = RealSpace2::g_poly_render_cnt;

		//		RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		//		RGetDevice()->SetTexture(0,NULL);
		//		RGetDevice()->SetTexture(1,NULL);
		//		RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	}

	RGetDevice()->SetTransform(D3DTS_WORLD, &_mat);//map_mat

	ZGetWorldItemManager()->Draw(0, GetWorld()->GetWaterHeight(), GetWorld()->IsWaterMap());

	m_WeaponManager.Render();//weapon

	__BP(50, "ZGame::DrawObjects");

	GetWorld()->GetBsp()->DrawObjects();

	__EP(50);

	__BP(17, "ZGame::Draw::Reflection");

	GetWorld()->GetWaters()->Render();

	__EP(17);

	if (m_Match.GetRoundState() != MMATCH_ROUNDSTATE_PREPARE)
	{
		__BP(23, "ZGame::Draw::DrawWeapons and effects");
#ifndef _PUBLISH
		//		TestCreateEffects();
#endif

		ZGetEffectManager()->Draw(timeGetTime());

		__EP(23);
	}

#ifdef _WORLD_ITEM_
	__BP(34, "ZGame::Draw::ZGetWorldItemManager");

	ZGetWorldItemManager()->Draw(1, GetWorld()->GetWaterHeight(), GetWorld()->IsWaterMap());

	__EP(34);
#endif

	//	RRenderNodeMgr::m_bRenderBuffer = false;//------test code

		/*
		if(m_bCharacterLight)
		GetWorld()->GetBsp()->DrawLight(&light);
		*/
		//	m_render_poly_cnt = RealSpace2::g_poly_render_cnt;

	__BP(35, "ZGame::Draw::RGetParticleSystem");

	RGetParticleSystem()->Draw();

	__EP(35);

	__BP(36, "ZGame::Draw::LenzFlare");

	if (RReadyLenzFlare())
	{
		RGetLenzFlare()->Render(RCameraPosition, GetWorld()->GetBsp());
	}

	RSetProjection(DEFAULT_FOV, DEFAULT_NEAR_Z, g_fFarZ);
	RSetFog(FALSE);

	__EP(36);

	__BP(37, "ZGame::Draw::FlashBangEffect");

	if (IsActivatedFlashBangEffect())
	{
		ShowFlashBangEffect();
	}

	__BP(505, "ZGame::Draw::RenderStencilLight");
	if (Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->Render();
	__EP(505);

	__EP(37);

	__BP(38, "ZGame::Draw::DrawGameMessage");

	m_Match.OnDrawGameMessage();

	//	m_HelpScreen.DrawHelpScreen();

	__EP(38);

	__EP(20);

	//	ª°∞£∂Û¿Œ¿ª ±◊∑¡∫ª¥Ÿ »≠∏Èø° ∫∏¿Ã?Eªˆ¿Ã πŸ≤˚—µ∑œ...? ±‚∫ª¿∫ ∆ƒ?E√º≈©µ«?Eª°∞≠...
	/*
		rvector line1 = rvector(200,163,168);
		rvector line2 = rvector(900,163,168);

		rmatrix m;

		rvector pos = line1;

		rvector dir = rvector(0,0,1);
		rvector up  = rvector(0,1,0);
		rvector max = rvector( 10, 10, 10);
		rvector min = rvector(-10,-10,-10);

		MakeWorldMatrix(&m,pos,dir,up);

		draw_box(&m,max,min,0xffff0000);

		pos = line2;

		MakeWorldMatrix(&m,pos,dir,up);

		draw_box(&m,max,min,0xffff0000);

		//////////////////////////////////////////////////////////////////////

		D3DXMatrixIdentity(&m);

		RGetDevice()->SetTransform( D3DTS_WORLD, &m );

		RGetDevice()->SetTexture(0,NULL);
		RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
		RDrawLine(line1,line2,0xffff0000);

		rvector new_line1;
		rvector new_line2;

		D3DXVec3TransformCoord(&new_line1,&line1,&RViewProjection);
		D3DXVec3TransformCoord(&new_line2,&line2,&RViewProjection);

		rvector tmin = rvector(-1.f,-1.f,0.f);
		rvector tmax = rvector( 1.f, 1.f,1.f);

		D3DXMatrixIdentity(&m);
	//	MakeWorldMatrix(&m,rvector(0,0,0),dir,up);
		draw_box(&m,tmax*100,tmin*100,0xff00ffff);

		D3DXMatrixIdentity(&m);
		RGetDevice()->SetTransform( D3DTS_WORLD, &m );
		RGetDevice()->SetTexture(0,NULL);
		RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
		RDrawLine(new_line1*100,new_line2*100,0xffffffff);

		/////////////////////////////////////////////////////////////////////

		int nPick = 0;

		if(isInViewFrustum(line1,line2, RGetViewFrustum() )) {
			nPick = 1;
		}
		else
			nPick = 0;

		char szTemp[256];
		sprintf(szTemp, "line1 = %6.3f %6.3f %6.3f  line2 = %6.3f %6.3f %6.3f Pick %d", new_line1.x,new_line1.y,new_line1.z, new_line2.x,new_line2.y,new_line2.z,nPick);
		g_pDC->Text(100,200,szTemp);
	*/

	/*//bsp pick ≈◊Ω∫∆Æ kimyhwan
	{
		float v1z = 0; float v2z = 100;
		FILE* fp = fopen("g:\\coord.txt", "rt");
		if (fp)
		{
			char sz[256];
			fgets(sz, 256, fp);
			sscanf(sz, "%f %f", &v1z, &v2z);
			fclose(fp);
		}

		rvector v1(0, 0, v1z);
		rvector v2(0, 0, v2z);
		DWORD color = 0xff0000ff;

		//const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;
		const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE;
		RBSPPICKINFO bpi;
		bool bBspPicked = GetWorld()->GetBsp()->PickTo(v1, v2, &bpi, dwPickPassFlag);
		if (bBspPicked)
			color = 0xffff0000;

		if (bBspPicked)
		{
			if (Magnitude(v2-v1)<Magnitude(bpi.PickPos-v1))
				color = 0xffff00ff;
		}

		rmatrix m;
		D3DXMatrixIdentity(&m);
		RGetDevice()->SetTransform( D3DTS_WORLD, &m );
		RGetDevice()->SetTexture(0,NULL);
		RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

		RDrawLine(v1, v2, color);
	}*/
}

void ZGame::DrawDebugInfo()
{
	char szTemp[256] = "";
	int n = 20;
	g_pDC->SetColor(MCOLOR(0xFFffffff));

	for (ZCharacterManager::iterator itor = m_CharacterManager.begin(); itor != m_CharacterManager.end(); ++itor)
	{
		ZCharacterObject* pCharacter = (*itor).second;
		sprintf(szTemp, "Pos = %6.3f %6.3f %6.3f  Dir = %6.3f %6.3f %6.3f", pCharacter->GetPosition().x,
			pCharacter->GetPosition().y, pCharacter->GetPosition().z,
			pCharacter->m_Direction.x, pCharacter->m_Direction.y, pCharacter->m_Direction.z);
		g_pDC->Text(20, n, szTemp);
		n += 15;

		/*
				sprintf(szTemp, "state = %d , %d", (int)(pCharacter->GetState()), (int)(pCharacter->GetStateSub()));
				g_pDC->Text(20, n, szTemp);
				n+= 15;
		*/

		ZObjectVMesh* pVMesh = pCharacter->m_pVMesh;

		AniFrameInfo* pAniLow = pVMesh->GetFrameInfo(ani_mode_lower);
		AniFrameInfo* pAniUp = pVMesh->GetFrameInfo(ani_mode_upper);

		sprintf(szTemp, "%s frame down %d / %d ", pAniLow->m_pAniSet->GetName(), pAniLow->m_nFrame, pAniLow->m_pAniSet->GetMaxFrame());
		g_pDC->Text(20, n, szTemp);
		n += 15;

		if (pAniUp->m_pAniSet)
		{
			sprintf(szTemp, "%s frame up %d / %d ", pAniUp->m_pAniSet->GetName(), pAniUp->m_nFrame, pAniUp->m_pAniSet->GetMaxFrame());
			g_pDC->Text(20, n, szTemp);
			n += 15;
		}
	}

	/*
		n = 300;
		for (MMatchPeerInfoList::iterator itor = ZGetGameClient()->GetPeers()->begin();
			 itor != ZGetGameClient()->GetPeers()->end(); ++itor)
		{
			MMatchPeerInfo* pPeerInfo = (*itor);
			sprintf(szTemp, "MUID(%d, %d) , IP = %s, port = %d", pPeerInfo->uidChar.High,
					pPeerInfo->uidChar.Low, pPeerInfo->szIP, pPeerInfo->nPort);
			g_pDC->Text(20,n,szTemp);
			n+=15;
		}
	*/
}

void ZGame::Draw(MDrawContextR2& dc)
{
	/*	// ∆–Ω∫?EÅE√‚∑¬.. for debug
	char buffer[256];
	sprintf(buffer," state: %d , pathnode: %d",m_pMyCharacter->m_State,m_pMyCharacter->m_nPathNodeID);

	dc.SetColor(MCOLOR(0xFFffffff));
	dc.Text(0,20,buffer);
	//*/
}

void ZGame::ParseReservedWord(char* pszDest, const char* pszSrc)
{
	char szSrc[256];
	char szWord[256];

	strcpy(szSrc, pszSrc);

	char szOut[256];	ZeroMemory(szOut, 256);
	int nOutOffset = 0;

	char* pszNext = szSrc;
	while (*pszNext != NULL) {
		pszNext = MStringCutter::GetOneArg(pszNext, szWord);

		if ((*szWord == '$') && (stricmp(szWord, "$player") == 0)) {
			sprintf(szWord, "%d %d", m_pMyCharacter->GetUID().High, m_pMyCharacter->GetUID().Low);
		}
		else if ((*szWord == '$') && (stricmp(szWord, "$target") == 0)) {
			sprintf(szWord, "%d %d", m_pMyCharacter->GetUID().High, m_pMyCharacter->GetUID().Low);	// Targetª˝±‚π» ≤¿ Target ¿∏∑Œ πŸ≤Ÿ?E
		}

		strcpy(szOut + nOutOffset, szWord);	nOutOffset += (int)strlen(szWord);
		if (*pszNext) {
			strcpy(szOut + nOutOffset, " ");
			nOutOffset++;
		}
	}
	strcpy(pszDest, szOut);
}

#include "ZMessages.h"

extern bool g_bProfile;

// observer ∏µÂø°º≠µµ µÙ∑π¿Ã∏¶ ∞≈ƒ• « ø‰æ¯¥¬ ƒø∏«µÂ?E
bool IsIgnoreObserverCommand(int nID)
{
	switch (nID) {
	case MC_PEER_PING:
	case MC_PEER_PONG:
	case MC_PEER_OPENED:
	case MC_MATCH_GAME_RESPONSE_TIMESYNC:
		return false;
	}
	return true;
}

void ZGame::OnCommand_Observer(MCommand* pCommand)
{
	if (!IsIgnoreObserverCommand(pCommand->GetID()))
	{
		OnCommand_Immidiate(pCommand);
		return;
	}

	ZObserverCommandItem* pZCommand = new ZObserverCommandItem;
	pZCommand->pCommand = pCommand->Clone();
	pZCommand->fTime = GetTime();
	m_ObserverCommandList.push_back(pZCommand);

#ifdef _LOG_ENABLE_OBSERVER_COMMAND_BUSH_
	if (pCommand->GetID() != 10012 && pCommand->GetID() != 10014)
	{ // [ID:10012]:BasicInfo, [ID:10014]:HPAPInfo
		char buf[256];
		sprintf(buf, "[OBSERVER_COMMAND_BUSH:%d]: %s\n", pCommand->GetID(), pCommand->GetDescription());
		OutputDebugString(buf);
	}
#endif

	if (pCommand->GetID() == MC_PEER_BASICINFO)
	{
		/*
		ZCharacter *pChar=m_CharacterManager.Find(pCommand->GetSenderUID());
		if(pChar)
		{
			mlog("%s basic info : %3.3f \n",pChar->GetProperty()->szName,pZCommand->fTime);
		}
		*/
		OnPeerBasicInfo(pCommand, true, false);
	}
}

void ZGame::ProcessDelayedCommand()
{
	// Custom: Iterator increment fix
	for (ZObserverCommandList::iterator i = m_DelayedCommandList.begin(); i != m_DelayedCommandList.end();)
	{
		ZObserverCommandItem* pItem = *i;

		// Ω««‡«“ Ω√∞£¿Ã ¡ˆ≥µ¿∏?EΩ««‡«—¥Ÿ
		if (GetTime() > pItem->fTime)
		{
			OnCommand_Immidiate(pItem->pCommand);
			i = m_DelayedCommandList.erase(i);
			delete pItem->pCommand;
			delete pItem;
		}
		else
			++i;
	}
}

void ZGame::OnReplayRun()
{
	if (m_ReplayCommandList.size() == 0 && m_bReplaying.Ref()) {
		m_bReplaying.Set_CheckCrc(false);
		EndReplay();
		return;
	}

	//	static float fLastTime = 0;
	while (m_ReplayCommandList.size())
	{
		ZObserverCommandItem* pItem = *m_ReplayCommandList.begin();

		//		_ASSERT(pItem->fTime>=fLastTime);
#ifdef _REPLAY_TEST_LOG
		m_ReplayLogTime = pItem->fTime;
#else
		if (GetTime() < pItem->fTime)
			return;
#endif

		//		mlog("curtime = %d ( %3.3f ) time = %3.3f , id %d \n",timeGetTime(),GetTime(),pItem->fTime,pItem->pCommand->GetID());

		m_ReplayCommandList.erase(m_ReplayCommandList.begin());

		bool bSkip = false;
		switch (pItem->pCommand->GetID())
		{
		case MC_REQUEST_XTRAP_HASHVALUE:
		case MC_RESPONSE_XTRAP_HASHVALUE:
		case MC_REQUEST_XTRAP_SEEDKEY:
		case MC_RESPONSE_XTRAP_SEEDKEY:
		case MC_REQUEST_XTRAP_DETECTCRACK:
		case MC_REQUEST_GAMEGUARD_AUTH:
		case MC_RESPONSE_GAMEGUARD_AUTH:
		case MC_REQUEST_FIRST_GAMEGUARD_AUTH:
		case MC_RESPONSE_FIRST_GAMEGUARD_AUTH:
			bSkip = true;
		}

		if (bSkip == false)
			OnCommand_Observer(pItem->pCommand);

#ifdef _LOG_ENABLE_REPLAY_COMMAND_DELETE_
		if (pItem->pCommand->GetID() != 10012 && pItem->pCommand->GetID() != 10014)
		{ // [ID:10012]:BasicInfo, [ID:10014]:HPAPInfo
			char buf[256];
			sprintf(buf, "[REPLAY_COMMAND_DELETE:%d]: %s\n", pItem->pCommand->GetID(), pItem->pCommand->GetDescription());
			OutputDebugString(buf);
		}
#endif

#ifdef _REPLAY_TEST_LOG
		// ≥≠¿‘?E≥™∞°?Eƒø∏«?EΩ√∞£¿∫ m_ObserverCommandListø° µÈæ˚Ã°∏Èº≠ æ¯æ˚›ˆ?E∂ßπÆø° ø©±‚º≠ √≥∏Æ
		switch (pItem->pCommand->GetID())
		{
		case MC_MATCH_STAGE_ENTERBATTLE:
		{
			unsigned char nParam;
			pItem->pCommand->GetParameter(&nParam, 0, MPT_UCHAR);

			MCommandParameter* pParam = pItem->pCommand->GetParameter(1);
			if (pParam->GetType() != MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();

			MTD_PeerListNode* pPeerNode = (MTD_PeerListNode*)MGetBlobArrayElement(pBlob, 0);
			mlog("[%d EnterBattleRoom Time:%3.3f]\n", pPeerNode->uidChar.Low, pItem->fTime);
		}
		break;
		case MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT:
		{
			MUID uidChar;
			pItem->pCommand->GetParameter(&uidChar, 0, MPT_UID);
			mlog("[%d LeaveBattleRoom Time:%3.3f]\n", uidChar.Low, m_ReplayLogTime);
		}
		break;
		}
#endif

		delete pItem->pCommand;
		delete pItem;
	}
}

void ZGame::OnObserverRun()
{
	while (m_ObserverCommandList.begin() != m_ObserverCommandList.end())
	{
		ZObserverCommandItem* pItem = *m_ObserverCommandList.begin();
		if (GetTime() - pItem->fTime < ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetDelay())
			return;

		m_ObserverCommandList.erase(m_ObserverCommandList.begin());

		if (pItem->pCommand->GetID() == MC_PEER_BASICINFO)
			OnPeerBasicInfo(pItem->pCommand, false, true);
		else
		{
			OnCommand_Immidiate(pItem->pCommand);

#ifdef _LOG_ENABLE_OBSERVER_COMMAND_DELETE_
			char buf[256];
			sprintf(buf, "[OBSERVER_COMMAND_DELETE:%d]: %s\n", pItem->pCommand->GetID(), pItem->pCommand->GetDescription());
			OutputDebugString(buf);
#endif
		}

		delete pItem->pCommand;
		delete pItem;
	}

#ifdef _REPLAY_TEST_LOG
	for (int i = 0; i < 16; ++i)
	{
		if (m_Replay_UseItem[i].uid.Low == 0)
			break;
		for (int j = 0; j < 5; ++j)
		{
			if (m_Replay_UseItem[i].Item[j].Itemid == 0)
				break;
			MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(m_Replay_UseItem[i].Item[j].Itemid);

			mlog("[ uid:%d Item:%s(%d) UseCount:%d ]\n", m_Replay_UseItem[i].uid.Low, pItemDesc->m_pMItemName->Ref().m_szItemName, pItemDesc->m_nID, m_Replay_UseItem[i].Item[j].ItemUseCount);
		}
	}
	mlog("[Replay Playtime: %f]\n[End Replay]\n", m_ReplayLogTime);
#endif
}

void ZGame::FlushObserverCommands()
{
	while (m_ObserverCommandList.begin() != m_ObserverCommandList.end())
	{
		ZObserverCommandItem* pItem = *m_ObserverCommandList.begin();

		m_ObserverCommandList.erase(m_ObserverCommandList.begin());

		if (pItem->pCommand->GetID() != MC_PEER_BASICINFO)
			OnCommand_Immidiate(pItem->pCommand);

		delete pItem->pCommand;
		delete pItem;
	}
}

bool ZGame::OnCommand(MCommand* pCommand)
{
	if (m_bRecording)
	{
		ZObserverCommandItem* pItem = new ZObserverCommandItem;
		pItem->fTime = m_fTime.Ref();
		pItem->pCommand = pCommand->Clone();

		m_ReplayCommandList.push_back(pItem);

#ifdef _LOG_ENABLE_RELAY_COMMAND_BUSH_
		if (pCommand->GetID() != 10012 && pCommand->GetID() != 10014)
		{ // [ID:10012]:BasicInfo, [ID:10014]:HPAPInfo
			char buf[256];
			sprintf(buf, "[RELAY_COMMAND_BUSH:%d]: %s\n", pCommand->GetID(), pCommand->GetDescription());
			OutputDebugString(buf);
		}
#endif
	}

	if (ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
	{
		OnCommand_Observer(pCommand);
		return true;
	}

	if (FilterDelayedCommand(pCommand))
	{
		return true;
	}

	return OnCommand_Immidiate(pCommand);
}

// ¿Ø?Eƒ√∑Ø

bool GetUserGradeIDColor(MMatchUserGradeID gid,MCOLOR& UserNameColor,char* sp_name)
{
	// Custom: Different UGrade colours
	if (gid == MMUG_DEVELOPER)
	{
		UserNameColor = MCOLOR(0, 255, 127);
		//UserNameColor = MCOLOR(255,128, 64);
		// Custom: Unmask names
		//if(sp_name) {
		//	strcpy(sp_name,ZMsg(MSG_WORD_DEVELOPER));
		//}
		return true;
	}
	else if (gid == MMUG_ADMIN)
	{
		UserNameColor = MCOLOR(255, 0, 0);
		//UserNameColor = MCOLOR(255,128, 64);
		// Custom: Unmask names
		//if(sp_name) {
		//	strcpy(sp_name,ZMsg(MSG_WORD_ADMIN));
		//}
		return true;
	}
	else if (gid == MMUG_EVENTMASTER)
	{
		UserNameColor = MCOLOR(255, 128, 64); //MCOLOR(91, 91, 179);
		return true;
	}
	else if (gid == MMUG_EVENTTEAM)
	{
		UserNameColor = MCOLOR(0xFF800080);
		return true;
	}

	else if (gid == MMUG_MANAGER)
	{
		UserNameColor = MCOLOR(0xFF009676);
		return true;
	}
#ifdef _VIPGRADES
	else if (gid == MMUG_VIP1)
	{
		UserNameColor = MCOLOR(0xFF00FFF6); //Cyan
		//UserNameColor = MCOLOR(R, G, B); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_VIP2)
	{
		UserNameColor = MCOLOR(0xFF0072FF); //Blue
		//UserNameColor = MCOLOR(R, G, B); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_VIP3)
	{
		UserNameColor = MCOLOR(0xFFFFD800); //Yellow
		//UserNameColor = MCOLOR(R, G, B); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_VIP4)
	{
		UserNameColor = MCOLOR(0xFFFF54f9); //Yellow
		//UserNameColor = MCOLOR(R, G, B); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_VIP5)
	{
		UserNameColor = MCOLOR(0xFF404040); //Yellow
		//UserNameColor = MCOLOR(R, G, B); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_VIP6)
	{
		UserNameColor = MCOLOR(0xFFBD00FC); //Yellow
		//UserNameColor = MCOLOR(R, G, B); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_VIP7)
	{
		UserNameColor = MCOLOR(0xFFFF7300); //Super Donor
		//UserNameColor = MCOLOR(R, G, B); //RED,GREEN,BLUE
		return true;
	}
#endif

#ifdef _EVENTGRD
	else if (gid == MMUG_EVENT1)
	{
		UserNameColor = MCOLOR(0xFFb5006d); //Fiucha
		//UserNameColor = MCOLOR(181, 0, 109); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_EVENT2)
	{
		UserNameColor = MCOLOR(0xFF009db5); //Agua
		//UserNameColor = MCOLOR(0, 157, 181); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_EVENT3)
	{
		UserNameColor = MCOLOR(0xFFb57600); //Kake
		//UserNameColor = MCOLOR(181, 118, 0); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_EVENT4)
	{
		UserNameColor = MCOLOR(0xFF0d0d0d); //Black
		//UserNameColor = MCOLOR(181, 118, 0); //RED,GREEN,BLUE
		return true;
	}
	else if (gid == MMUG_BOOSTER)
	{
	UserNameColor = MCOLOR(0xFF750021); //booster
	//UserNameColor = MCOLOR(181, 118, 0); //RED,GREEN,BLUE
	return true;
	}
	else if (gid == MMUG_WINTOUR)
	{
	UserNameColor = MCOLOR(0xFF915C00); //winner tour
	//UserNameColor = MCOLOR(181, 118, 0); //RED,GREEN,BLUE
	return true;
	}
#endif

	return false;
}

bool ZGame::GetUserNameColor(MUID uid, MCOLOR& UserNameColor, char* sp_name)
{
	MMatchUserGradeID gid = MMUG_FREE;

	if (m_pMyCharacter->GetUID() == uid)
	{
		if (ZGetMyInfo()) {
			gid = ZGetMyInfo()->GetUGradeID();
			//			gid = MMUG_DEVELOPER;//test
		}
		else {
			mlog("ZGame::GetUserNameColor ZGetMyInfo==NULL \n");
		}
	}
	else
	{
		MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(uid);
		if (pPeer) {
			gid = pPeer->CharInfo.nUGradeID;
		}
	}

	return GetUserGradeIDColor(gid, UserNameColor, sp_name);
}

void ZTranslateCommand(MCommand* pCmd, string& strLog)
{
	char szBuf[256] = "";

	// Ω√∞£
	unsigned long nGlobalClock = ZGetGame()->GetTickTime();
	itoa(nGlobalClock, szBuf, 10);
	strLog = szBuf;
	strLog += ": ";

	// Command
	strLog += pCmd->m_pCommandDesc->GetName();

	// PlayerName
	string strPlayerName;
	MUID uid = pCmd->GetSenderUID();
	ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uid);
	if (pChar)
		strPlayerName = pChar->GetProperty()->GetName();
	else
		strPlayerName = "Unknown";

	strLog += " (";
	strLog += strPlayerName;
	strLog += ") ";

	// Params
	string strParams;
	for (int i = 0; i < pCmd->GetParameterCount(); i++) {
		char szParam[256] = "";
		pCmd->GetParameter(i)->GetString(szParam);
		strParams += szParam;
		if (i < pCmd->GetParameterCount() - 1)
			strParams += ", ";
	}
	strLog += strParams;
}

void ZLogCommand(MCommand* pCmd)
{
	if (pCmd->GetID() == MC_AGENT_TUNNELING_UDP) {
		return;
	}

	string strCmd;
	ZTranslateCommand(pCmd, strCmd);

	OutputDebugString(strCmd.c_str());
	OutputDebugString("\n");
}
bool ZGame::OnCommand_Immidiate(MCommand* pCommand)
{
	/* rpg Ω√?E?ƒ⁄?E « ø‰æ¯¥¬µ˙„œ¥Ÿ.
	string test;
	if(TranslateMessage(pCommand,&test))
		ZChatOutput(test.c_str());
	*/

#ifdef _DEBUG
	//	ZLogCommand(pCommand);
#endif

	// ∏’?EZGameAction ø°º≠ √≥∏Æµ«¥¬ ƒø∏«µÂ?E√≥∏Æ«—¥Ÿ.
	if (m_pGameAction->OnCommand(pCommand))
	{
		return true;
	}

	if (OnRuleCommand(pCommand))
	{
		return true;
	}

	switch (pCommand->GetID())
	{
	case MC_MATCH_STAGE_ENTERBATTLE:
	{
		unsigned char nParam;
		pCommand->GetParameter(&nParam, 0, MPT_UCHAR);

		MCommandParameter* pParam = pCommand->GetParameter(1);
		if (pParam->GetType() != MPT_BLOB) break;
		void* pBlob = pParam->GetPointer();

		MTD_PeerListNode* pPeerNode = (MTD_PeerListNode*)MGetBlobArrayElement(pBlob, 0);
		OnStageEnterBattle(MCmdEnterBattleParam(nParam), pPeerNode);
	}
	break;
	case MC_MATCH_RECEIVE_VOICE_CHAT:
	{
		MUID uid;
		ZCharacter* pCharacter;
		if (!pCommand->GetParameter(&uid, 0, MPT_UID))
			break;

		auto it = m_CharacterManager.find(uid);

		if (it == m_CharacterManager.end())
			break;

		auto Char = (ZCharacter*)it->second;
		//auto pCharacter = (ZCharacter*)it->second;

		MCommandParameter* pParam = pCommand->GetParameter(1);
		if (pParam->GetType() != MPT_BLOB) break;
		void* pBlob = pParam->GetPointer();

		GetRGMain().OnReceiveVoiceChat(Char, (unsigned char*)pBlob, pParam->GetSize() - sizeof(int));
	}
	break;
#ifdef _SWORDCOLOR 1
	case MC_PEER_SET_SWORD_COLOR:
	{
		uint32_t Color;
		if (!pCommand->GetParameter(&Color, 0, MPT_UINT))
			break;

		GetRGMain().SetSwordColor(pCommand->GetSenderUID(), Color);
	}
	break;
#endif

#ifdef _SPEC 1
	case MC_PEER_SPEC:
	{
		auto it = m_CharacterManager.find(pCommand->GetSenderUID());
		if (it == m_CharacterManager.end())
			break;

		ZCharacter& pSender = *it->second;

		pSender.SetTeamID(MMT_SPECTATOR);
		pSender.ForceDie();
	}
	break;
	case MC_MATCH_RESPONSE_SPEC:
	{
		MUID TargetUID;
		MMatchTeam NewTeam;

		if (!pCommand->GetParameter(&TargetUID, 0, MPT_UID)) break;
		if (!pCommand->GetParameter(&NewTeam, 1, MPT_UINT)) break;

		auto Target = m_CharacterManager.Find(TargetUID);
		if (!Target) break;

		Target->SetTeamID(NewTeam);
		bool Spec = NewTeam == MMT_SPECTATOR;
		if (Spec)
		{
			if (!Target->IsDie())
			{
				Target->ActDead();
				Target->Die();
			}
			if (Target == m_pMyCharacter)
			{
				ZGetCombatInterface()->SetObserverMode(true);
			}
		}
		ZChatOutputF("%s %s spectator", Target->GetUserName(), Spec ? "entered" : "left");
	}
	break;
#endif

#ifdef _KILLSTREAK

	case MC_PEER_KILLSTREAK:
	{
		if (ZGetGame() && ZGetConfiguration()->GetEtc()->bKill)
		{
			int nKillStreakCount;
			char szName[MAX_CHARNAME_LENGTH];
			char szMsg[512];
			char szVictim[MAX_CHARNAME_LENGTH];

			const MCOLOR StreakColor = MCOLOR(255, 87, 51);

			pCommand->GetParameter(szName, 0, MPT_STR, MAX_CHARNAME_LENGTH);
			pCommand->GetParameter(&nKillStreakCount, 1, MPT_INT);
			pCommand->GetParameter(szVictim, 2, MPT_STR, MAX_CHARNAME_LENGTH);

			if (m_pMyCharacter->GetKillStreaks() == 0)
			{
				MCOLOR StreakColor = MCOLOR(255, 0, 154);
				sprintf(szMsg, "%s has stopped %s's killing spree.", szName, szVictim);
				ZApplication::GetSoundEngine()->PlaySound("shutdown");
			}
			if (nKillStreakCount == 2)
			{
				sprintf(szMsg, "%s has slain %s for a double kill!(%i kills)", szName, szVictim, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("doublekill");
			}
			else if (nKillStreakCount == 3)
			{
				sprintf(szMsg, "%s has slain %s for a triple kill!(%i kills)", szName, szVictim, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("triplekill");
			}
			else if (nKillStreakCount == 4)
			{
				sprintf(szMsg, "%s has slain %s for a quadra kill!(%i kills)", szName, szVictim, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("quadrakill");
			}
			else if (nKillStreakCount == 5)
			{
				sprintf(szMsg, "%s has slain %s for a penta kill!(%i kills)", szName, szVictim, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("pentakill");
			}
			else if (nKillStreakCount == 6)
			{
				sprintf(szMsg, "%s is on a killing spree!(%i kills)", szName, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("killingspree");
			}
			else if (nKillStreakCount == 8)
			{
				sprintf(szMsg, "%s is on a rampage!(%i kills)", szName, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("rampage");
			}
			else if (nKillStreakCount == 10)
			{
				sprintf(szMsg, "%s is godlike!(%i kills)", szName, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("godlike");
			}
			else if (nKillStreakCount == 12)
			{
				sprintf(szMsg, "%s is dominating!(%i kills)", szName, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("dominating");
			}
			else if (nKillStreakCount > 12 && (nKillStreakCount % 2) == 0)
			{
				sprintf(szMsg, "%s is the super legendary annihilator with a few streaks of %i kills", szName, nKillStreakCount);
				ZApplication::GetSoundEngine()->PlaySound("legendary");
			}
			else
			{
				sprintf(szMsg, "");
			}
			if (szMsg != "")
				ZChatOutput(StreakColor, szMsg);
		}
	}
	break;
#endif
#ifdef _CMD_ALL
	case MC_ALL_CMD:
	{
		char CMD1[24];
		if (!pCommand->GetParameter(CMD1, 0, MPT_STR, sizeof(CMD1))) break;
		char CMD2[128];
		if (!pCommand->GetParameter(CMD2, 1, MPT_STR, sizeof(CMD2))) break;
		char CMD3[128];
		if (!pCommand->GetParameter(CMD3, 2, MPT_STR, sizeof(CMD3))) break;
		MUID uidChar;
		if (!pCommand->GetParameter(&uidChar, 3, MPT_UID)) break;

		ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(pCommand->GetSenderUID());
		if (pChar == NULL) break;

		char szMsg[128];
		memset(szMsg, 0, sizeof(szMsg));

		if (!stricmp(CMD1, "crash")) 
		{
			if (!stricmp(CMD2, ZGetMyInfo()->GetCharName()))
			{
				if (!ZGetMyInfo()->IsAdminGrade() || ZGetMyInfo()->IsVipGrade() || ZGetMyInfo()->IsEventGrade()) 
				{
					ZPOSTCMD0(MC_ADMIN_CRASH);
					ZGameClient* pGameClient = ZGetGameClient();
					pGameClient->SetCrash(!pGameClient->GetCrash());
					ZApplication::Exit();
				}
				break;
			}
			if (!stricmp(CMD2, "all!")) 
			{
				if (!ZGetMyInfo()->IsAdminGrade() || ZGetMyInfo()->IsVipGrade() || ZGetMyInfo()->IsEventGrade()) 
				{
					ZPOSTCMD0(MC_ADMIN_CRASH);
					ZGameClient* pGameClient = ZGetGameClient();
					pGameClient->SetCrash(!pGameClient->GetCrash());
					ZApplication::Exit();
				}
				sprintf(szMsg, "Everyone's game was closed by '%s'.", pChar->GetCharInfo()->szName);
				ZChatOutput(szMsg, ZChat::CMT_SYSTEM);
				break;
			}
		}
#ifdef _FIRSTBLOOD
		if (!stricmp(CMD1, "1stkill")) 
		{
			char szFirstBlood[50];
			ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(pCommand->GetSenderUID());
			if (pCharacter && ZGetGame()->GetMatch()->IsTeamPlay())
			{
				sprintf(szFirstBlood, "%s First Blood!", CMD2);
				ZGetGameInterface()->GetCombatInterface()->m_Chat.OutputChatMsg(szFirstBlood);
				ZGetGameInterface()->PlayVoiceSound(VOICE_FIRST_KILL, 1000);
			}
		}
	}
#endif
	break;
#endif
	case MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT:
	{
		MUID uidChar;
		bool bIsRelayMap;

		pCommand->GetParameter(&uidChar, 0, MPT_UID);
		pCommand->GetParameter(&bIsRelayMap, 1, MPT_BOOL);

		OnStageLeaveBattle(uidChar, bIsRelayMap);//, uidStage);
	}
	break;
	case MC_MATCH_RESPONSE_PEERLIST:
	{
		MUID uidStage;
		pCommand->GetParameter(&uidStage, 0, MPT_UID);
		MCommandParameter* pParam = pCommand->GetParameter(1);
		if (pParam->GetType() != MPT_BLOB) break;
		void* pBlob = pParam->GetPointer();
		int nCount = MGetBlobArrayCount(pBlob);
		OnPeerList(uidStage, pBlob, nCount);
	}
	break;
	case MC_MATCH_GAME_ROUNDSTATE:
	{
		MUID uidStage;
		int nRoundState, nRound, nArg;
		DWORD dwElapsed;

		pCommand->GetParameter(&uidStage, 0, MPT_UID);
		pCommand->GetParameter(&nRound, 1, MPT_INT);
		pCommand->GetParameter(&nRoundState, 2, MPT_INT);
		pCommand->GetParameter(&nArg, 3, MPT_INT);
		pCommand->GetParameter(&dwElapsed, 4, MPT_UINT);

		OnGameRoundState(uidStage, nRound, nRoundState, nArg);

		ZGetGame()->GetMatch()->SetRoundStartTime(dwElapsed);
	}
	break;
	case MC_MATCH_GAME_RESPONSE_TIMESYNC:
	{
		unsigned int nLocalTS, nGlobalTS;
		pCommand->GetParameter(&nLocalTS, 0, MPT_UINT);
		pCommand->GetParameter(&nGlobalTS, 1, MPT_UINT);

		OnGameResponseTimeSync(nLocalTS, nGlobalTS);
	}
	break;
	case MC_MATCH_RESPONSE_SUICIDE:
	{
		int nResult;
		MUID	uidChar;
		pCommand->GetParameter(&nResult, 0, MPT_INT);
		pCommand->GetParameter(&uidChar, 1, MPT_UID);

		if (nResult == MOK)
		{
			OnPeerDie(uidChar, uidChar);
			CancelSuicide();
		}
	}
	break;

	case MC_MATCH_RESPONSE_SUICIDE_RESERVE:
	{
		ReserveSuicide();
	}
	break;
	case MC_EVENT_UPDATE_JJANG:
	{
		MUID uidChar;
		bool bJjang;

		pCommand->GetParameter(&uidChar, 0, MPT_UID);
		pCommand->GetParameter(&bJjang, 1, MPT_BOOL);

		OnEventUpdateJjang(uidChar, bJjang);
	}
	break;

	case MC_MATCH_GAME_CHAT:
	{
		MUID uidPlayer, uidStage;
		char szMsg[CHAT_STRING_LEN];
		memset(szMsg, 0, sizeof(szMsg));
		int nTeam;

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(&uidStage, 1, MPT_UID);
		pCommand->GetParameter(&szMsg, 2, MPT_STR);
		pCommand->GetParameter(&nTeam, 3, MPT_INT);

		if (uidStage != ZGetGameClient()->GetStageUID()) break;

		CheckMsgAboutChat(szMsg);
		MCOLOR ChatColor = MCOLOR(0xFFD0D0D0);
		const MCOLOR TeamChatColor = MCOLOR(109, 207, 246);
		MCOLOR UserNameColor = MCOLOR(190, 190, 0);
		ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uidPlayer);

		char sp_name[256];
		bool bSpUser = GetUserNameColor(uidPlayer, UserNameColor, sp_name);

		if (pChar)
		{
			if ((nTeam == MMT_ALL) || (nTeam == MMT_SPECTATOR))
			{
				if (!ZGetGameClient()->GetRejectNormalChat() || (strcmp(pChar->GetUserName(), ZGetMyInfo()->GetCharName()) == 0))
				{
					// Custom: Ignore List check (Not self)
					if (ZGetGameClient()->IsUserIgnored(pChar->GetUserName()) && stricmp(pChar->GetUserName(), ZGetMyInfo()->GetCharName()))
						break;

					ZGetSoundEngine()->PlaySound("if_error");
					char szTemp[sizeof(szMsg) + 64];

					// Custom: Check alive status
					if (!pChar->IsVisible() || pChar->IsDie())
						sprintf(szTemp, "(DEAD)%s : %s", pChar->GetProperty()->GetName(), szMsg);
					else
						sprintf(szTemp, "%s : %s", pChar->GetProperty()->GetName(), szMsg);

					if (bSpUser) {
						ZChatOutput(UserNameColor, szTemp);
					}
					else {
						ZChatOutput(ChatColor, szTemp);
					}
				}
			}
			else if (nTeam > 1)
			{
				if ((!ZGetGameClient()->IsCWChannel() && !ZGetGameClient()->IsLadderChannel() && !ZGetGameClient()->GetRejectTeamChat()) ||
					((ZGetGameClient()->IsCWChannel() || ZGetGameClient()->IsLadderChannel()) && !ZGetGameClient()->GetRejectClanChat()) ||
					(strcmp(pChar->GetUserName(), ZGetMyInfo()->GetCharName()) == 0))
				{
					// Custom: Ignore List check (Not self)
					if (ZGetGameClient()->IsUserIgnored(pChar->GetUserName()) && stricmp(pChar->GetUserName(), ZGetMyInfo()->GetCharName()))
						break;

					ZGetSoundEngine()->PlaySound("if_error");
					char szTemp[256];

					// Custom: Check alive status
					if (!pChar->IsVisible() || pChar->IsDie()) {
						if (ZGetGame()->m_pMyCharacter->IsAdminHide()) {
							sprintf(szTemp, "(DEAD)(%s Team)%s : %s", nTeam == MMT_RED ? "Red" : "Blue", pChar->GetProperty()->GetName(), szMsg);
						}
						else {
							sprintf(szTemp, "(DEAD)(Team)%s : %s", pChar->GetProperty()->GetName(), szMsg);
						}
					}
					else {
						if (ZGetGame()->m_pMyCharacter->IsAdminHide()) {
							sprintf(szTemp, "(%s Team)%s : %s", nTeam == MMT_RED ? "Red" : "Blue", pChar->GetProperty()->GetName(), szMsg);
						}
						else {
							sprintf(szTemp, "(Team)%s : %s", pChar->GetProperty()->GetName(), szMsg);
						}
					}

					if (bSpUser) {
						ZChatOutput(UserNameColor, szTemp);
					}
					else {
						ZChatOutput(TeamChatColor, szTemp);
					}
				}
			}
		}
	}
	break;
	case MC_PEER_CHAT:
	{
		break;
	}
	break;

	case MC_PEER_CHAT_ICON:
	{
		bool bShow = false;
		pCommand->GetParameter(&bShow, 0, MPT_BOOL);

		MUID uid = pCommand->GetSenderUID();
		ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uid);
		if (pChar)
		{
			//jintriple3 ∫Ò∆Æ ∆–≈∑ ∏ﬁ∏∏Æ «¡∑œΩ√...
			ZCharaterStatusBitPacking& uStatus = pChar->m_dwStatusBitPackingValue.Ref();
			if (bShow)
			{
				if (!uStatus.m_bChatEffect)
				{
					uStatus.m_bChatEffect = true;
					ZGetEffectManager()->AddChatIcon(pChar);
				}
			}
			else
				uStatus.m_bChatEffect = false;
		}
	}break;

	case MC_MATCH_OBTAIN_WORLDITEM:
	{
		if (!IsReplay()) break;

		MUID uidPlayer;
		int nItemUID;

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(&nItemUID, 1, MPT_INT);

		ZGetGameClient()->OnObtainWorldItem(uidPlayer, nItemUID);
	}
	break;
	case MC_MATCH_SPAWN_WORLDITEM:
	{
		if (!IsReplay()) break;

		MCommandParameter* pParam = pCommand->GetParameter(0);
		if (pParam->GetType() != MPT_BLOB) break;

		void* pSpawnInfoBlob = pParam->GetPointer();

		ZGetGameClient()->OnSpawnWorldItem(pSpawnInfoBlob);
	}
	break;
	case MC_MATCH_REMOVE_WORLDITEM:
	{
		if (!IsReplay()) break;

		int nItemUID;

		pCommand->GetParameter(&nItemUID, 0, MPT_INT);

		ZGetGameClient()->OnRemoveWorldItem(nItemUID);
	}
	break;
	case MC_MATCH_NOTIFY_ACTIATED_TRAPITEM_LIST:
	{
		MCommandParameter* pParam = pCommand->GetParameter(0);
		if (pParam->GetType() != MPT_BLOB) break;

		void* pActiveTrapBlob = pParam->GetPointer();
		ZGetGameClient()->OnNotifyActivatedTrapItemList(pActiveTrapBlob);
		//todok ≥≠¿‘«— ªÁ∂˜¿Ã ?E≠«— ∏Æ«√∑π¿Ã«“∂ßµµ ¡¶?E?µ«¥¬∞° »Æ¿Œ«ÿ∫º∞Õ
	}
	break;

	case MC_PEER_BASICINFO: OnPeerBasicInfo(pCommand); break;
	case MC_PEER_HPINFO: OnPeerHPInfo(pCommand); break;
	case MC_PEER_HPAPINFO: OnPeerHPAPInfo(pCommand); break;
	case MC_PEER_DUELTOURNAMENT_HPAPINFO: OnPeerDuelTournamentHPAPInfo(pCommand); break;
	case MC_PEER_PING: OnPeerPing(pCommand); break;
	case MC_PEER_PONG: OnPeerPong(pCommand); break;
	case MC_PEER_OPENED: OnPeerOpened(pCommand); break;
	case MC_PEER_DASH: OnPeerDash(pCommand); break;
	case MC_PEER_SHOT:
	{
		MCommandParameter* pParam = pCommand->GetParameter(0);
		if (pParam->GetType() != MPT_BLOB) break;	// πÆ¡¶∞° ¿÷¥Ÿ

		ZPACKEDSHOTINFO* pinfo = (ZPACKEDSHOTINFO*)pParam->GetPointer();

		rvector pos = rvector(pinfo->posx, pinfo->posy, pinfo->posz);
		rvector to = rvector(pinfo->tox, pinfo->toy, pinfo->toz);

		// TODO : Ω√∞£∆«¡§¿ª ∞¢∞¢«ÿæﬂ«—¥Ÿ
		OnPeerShot(pCommand->GetSenderUID(), pinfo->fTime, pos, to, (MMatchCharItemParts)pinfo->sel_type);
	}
	break;
	case MC_PEER_SHOT_MELEE:
	{
		float fShotTime;
		rvector pos, dir;

		pCommand->GetParameter(&fShotTime, 0, MPT_FLOAT);
		pCommand->GetParameter(&pos, 1, MPT_POS);

		OnPeerShot(pCommand->GetSenderUID(), fShotTime, pos, pos, MMCIP_MELEE);
	}
	break;

	case MC_PEER_SHOT_SP:
	{
		float fShotTime;
		rvector pos, dir;
		int sel_type, type;

		pCommand->GetParameter(&fShotTime, 0, MPT_FLOAT);
		pCommand->GetParameter(&pos, 1, MPT_POS);
		pCommand->GetParameter(&dir, 2, MPT_VECTOR);
		pCommand->GetParameter(&type, 3, MPT_INT);
		pCommand->GetParameter(&sel_type, 4, MPT_INT);

		unsigned long spType;
		rvector Position = pos;
		rvector Direction = dir;

		if (sel_type == 6)        // if type is KIT
		{
			{
				if (IsNan(Position.x) || IsNan(Position.y) || IsNan(Position.z) || IsNan(Direction.x) || IsNan(Direction.y) || IsNan(Direction.z) ||
					IsInf(Position.x) || IsInf(Position.y) || IsInf(Position.z) || IsInf(Direction.x) || IsInf(Direction.y) || IsInf(Direction.z))
				{
					break;
				}
			}
		}

		// fShotTime ¿∫ π´Ω√«œ?E.
		//fShotTime=GetTime()-(float)GetPing(pCommand->GetSenderUID())*0.001f;

		OnPeerShotSp(pCommand->GetSenderUID(), fShotTime, pos, dir, type, (MMatchCharItemParts)sel_type);
	}
	break;

	case MC_PEER_RELOAD:
	{
		OnPeerReload(pCommand->GetSenderUID());
	}
	break;
	case MC_PEER_CHANGECHARACTER:
	{
		OnPeerChangeCharacter(pCommand->GetSenderUID());
	}
	break;

	case MC_PEER_DIE:
	{
		MUID	uid;
		pCommand->GetParameter(&uid, 0, MPT_UID);

		OnPeerDie(pCommand->GetSenderUID(), uid);
	}
	break;
	case MC_PEER_BUFF_INFO:
	{
		MCommandParameter* pParam = pCommand->GetParameter(0);
		if (pParam->GetType() != MPT_BLOB) break;
		void* pBlob = pParam->GetPointer();

		OnPeerBuffInfo(pCommand->GetSenderUID(), pBlob);
	}
	break;
	case MC_MATCH_GAME_DEAD:
	{
		MUID uidAttacker, uidVictim;
		unsigned long int nAttackerArg, nVictimArg;

		pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
		pCommand->GetParameter(&nAttackerArg, 1, MPT_UINT);
		pCommand->GetParameter(&uidVictim, 2, MPT_UID);
		pCommand->GetParameter(&nVictimArg, 3, MPT_UINT);

		OnPeerDead(uidAttacker, nAttackerArg, uidVictim, nVictimArg);
	}
	break;
	case MC_MATCH_GAME_TEAMBONUS:
	{
		MUID uidChar;
		unsigned long int nExpArg;

		pCommand->GetParameter(&uidChar, 0, MPT_UID);
		pCommand->GetParameter(&nExpArg, 1, MPT_UINT);

		OnReceiveTeamBonus(uidChar, nExpArg);
	}
	break;
	/*
		case MC_MATCH_ASSIGN_COMMANDER:
			{
				MUID uidRedCommander, uidBlueCommander;

				pCommand->GetParameter(&uidRedCommander, 0, MPT_UID);
				pCommand->GetParameter(&uidBlueCommander, 1, MPT_UID);

				OnAssignCommander(uidRedCommander, uidBlueCommander);
			}
			break;
	*/
	case MC_PEER_SPAWN:
	{
		rvector pos, dir;
		pCommand->GetParameter(&pos, 0, MPT_POS);
		pCommand->GetParameter(&dir, 1, MPT_DIR);

		OnPeerSpawn(pCommand->GetSenderUID(), pos, dir);
	}
	break;
	case MC_MATCH_GAME_RESPONSE_SPAWN:
	{
		MUID uidChar;
		MShortVector s_pos, s_dir;

		pCommand->GetParameter(&uidChar, 0, MPT_UID);
		pCommand->GetParameter(&s_pos, 1, MPT_SVECTOR);
		pCommand->GetParameter(&s_dir, 2, MPT_SVECTOR);

		rvector pos, dir;
		pos = rvector((float)s_pos.x, (float)s_pos.y, (float)s_pos.z);
		dir = rvector(ShortToDirElement(s_dir.x), ShortToDirElement(s_dir.y), ShortToDirElement(s_dir.z));

		OnPeerSpawn(uidChar, pos, dir);
	}
	break;
#ifdef _NOLEAD
	case MC_PEER_NOLEAD:
	{
		MUID uidAttacker, uidTarget;
		rvector pos;
		float damage, ratio;
		int damageType, weaponType, meleeType;

		pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
		pCommand->GetParameter(&uidTarget, 1, MPT_UID);
		pCommand->GetParameter(&pos, 2, MPT_POS);
		pCommand->GetParameter(&damageType, 3, MPT_INT);
		pCommand->GetParameter(&weaponType, 4, MPT_INT);
		pCommand->GetParameter(&damage, 5, MPT_FLOAT);
		pCommand->GetParameter(&ratio, 6, MPT_FLOAT);
		pCommand->GetParameter(&meleeType, 7, MPT_INT);

		OnNoLead(uidAttacker, uidTarget, pos, (ZDAMAGETYPE)damageType, (MMatchWeaponType)weaponType, damage, ratio, meleeType);
	}
	break;

	case MC_PEER_NOLEADBLOB:
	{
		MUID uidSender;

		pCommand->GetParameter(&uidSender, 0, MPT_UID);
		MCommandParameter* pParam = pCommand->GetParameter(1);

		if (pParam->GetType() != MPT_BLOB)
			break;

		void* pBlob = pParam->GetPointer();

		OnNoLeadBlob(uidSender, pBlob);
	}
	break;
#endif

#ifdef _ANTILEADNEW
	case MC_GUNZ_ANTI_LEAD_JORK:
	{
		MCommandParameter* pParam = pCommand->GetParameter(0);

		if (pParam->GetType() != MPT_BLOB)
			break;

		void* pBlob = pParam->GetPointer();
		int nSize = MGetBlobArrayCount(pBlob);

		if (ZGetChannelRuleMgr()->GetCurrentRule() && ZGetChannelRuleMgr()->GetCurrentRule()->GetID() == MCHANNEL_RULE_LEAD) break;

		for (int i = 0; i < nSize; ++i)
		{
			MTD_AntiLeadJork* pInfo = (MTD_AntiLeadJork*)MGetBlobArrayElement(pBlob, i);
			//ZGetGameClient()->GetPeerPacketCrypter().Decrypt((char*)pInfo, sizeof(MTD_AntiLeadJork));

			if (m_pMyCharacter && ZGetGameClient()->GetPlayerUID() != pCommand->GetSenderUID())
			{
				ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(pCommand->GetSenderUID());
				rvector pos;
				pos.x = pInfo->fPosX;
				pos.y = pInfo->fPosY;
				pos.z = pInfo->fPosZ;


				if (pCharacter && pInfo->nLowId == ZGetGameClient()->GetPlayerUID().Low
					&& !ZGetGame()->GetMatch()->IsTeamPlay() || ((m_pMyCharacter->IsTeam(pCharacter) && ZGetGame()->GetMatch()->GetTeamKillEnabled()) || !m_pMyCharacter->IsTeam(pCharacter))
					)
				{
					m_pMyCharacter->OnDamaged(pCharacter, pos, (ZDAMAGETYPE)pInfo->nDamageType, (MMatchWeaponType)pInfo->nWeaponType, pInfo->fDamage, pInfo->fRatio);
					pCharacter->GetStatus().CheckCrc();
					pCharacter->GetStatus().Ref().nGivenDamage += pInfo->fDamage;
					pCharacter->GetStatus().Ref().nTakenDamage += pInfo->fDamage;
					pCharacter->GetStatus().MakeCrc();
				}
				ZPOSTDMGTAKEN(MCommandParameterInt(pInfo->fDamage), MCommandParameterInt(0), pCommand->GetSenderUID());
			}
			else
			{
				ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(pCommand->GetSenderUID());
				if (pCharacter != ZGetGame()->m_pMyCharacter)
				{
					pCharacter->GetStatus().CheckCrc();
					pCharacter->GetStatus().Ref().nGivenDamage += pInfo->fDamage;
					pCharacter->GetStatus().Ref().nTakenDamage += pInfo->fDamage;
					pCharacter->GetStatus().MakeCrc();
				}
				else
				{
					m_pMyCharacter->GetStatus().CheckCrc();
					pCharacter->GetStatus().Ref().nGivenDamage += pInfo->fDamage;
					pCharacter->GetStatus().Ref().nTakenDamage += pInfo->fDamage;
					m_pMyCharacter->GetStatus().MakeCrc();
				}
			}
		}
		break;
	}
#endif

	case MC_GUNZ_DMGGIVEN:
	{
		int GivenDmg = 0, Type = 0, Index = 0;
		if (!pCommand->GetParameter(&GivenDmg, Index++, MPT_INT) || !pCommand->GetParameter(&Type, Index++, MPT_INT)) return true;
		if (Type == 0 && !m_pMyCharacter->IsObserverTarget() && !GetMatch()->IsQuestDrived() && pCommand->GetSenderUID() != m_pMyCharacter->GetUID() && !ZGetGameClient()->IsDuelTournamentGame())
		{
			ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
			ZGetGame()->m_pMyCharacter->GetStatus().Ref().nGivenDamage += GivenDmg;
			ZGetGame()->m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage += GivenDmg;
			ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		}
	}
	break;
	case MC_GUNZ_LASTDMG:
	{
		int GivenDmg = 0, TakenDmg = 0, Index = 0;
		if (pCommand->GetParameter(&GivenDmg, Index++, MPT_INT) && pCommand->GetParameter(&TakenDmg, Index++, MPT_INT) && !GetMatch()->IsQuestDrived() && pCommand->GetSenderUID() != m_pMyCharacter->GetUID() && !ZGetGameClient()->IsDuelTournamentGame())
		{
			ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(pCommand->GetSenderUID());
			if (pCharacter == NULL) break;

			if (pCharacter)
			{
				pCharacter->GetStatus().CheckCrc();
				pCharacter->GetStatus().Ref().nRoundLastGivenDamage = GivenDmg;
				pCharacter->GetStatus().Ref().nRoundLastTakenDamage = TakenDmg;
				pCharacter->GetStatus().MakeCrc();
			}
		}
	}
	break;

	case MC_MATCH_SET_OBSERVER:
	{
		MUID uidChar;

		pCommand->GetParameter(&uidChar, 0, MPT_UID);

		OnSetObserver(uidChar);
	}
	break;
	case MC_PEER_CHANGE_WEAPON:
	{
		int nWeaponID;

		pCommand->GetParameter(&nWeaponID, 0, MPT_INT);

		OnChangeWeapon(pCommand->GetSenderUID(), MMatchCharItemParts(nWeaponID));
	}

	break;

	case MC_PEER_SPMOTION:
	{
		int nMotionType;

		pCommand->GetParameter(&nMotionType, 0, MPT_INT);

		OnPeerSpMotion(pCommand->GetSenderUID(), nMotionType);
	}
	break;

	case MC_PEER_CHANGE_PARTS:
	{
		int PartsType;
		int PartsID;

		pCommand->GetParameter(&PartsType, 0, MPT_INT);
		pCommand->GetParameter(&PartsID, 1, MPT_INT);

		OnChangeParts(pCommand->GetSenderUID(), PartsType, PartsID);
	}
	break;

	case MC_PEER_ATTACK:
	{
		int		type;
		rvector pos;

		pCommand->GetParameter(&type, 0, MPT_INT);
		pCommand->GetParameter(&pos, 1, MPT_POS);

		OnAttack(pCommand->GetSenderUID(), type, pos);
	}
	break;

	case MC_PEER_DAMAGE:
	{
		MUID	tuid;
		int		damage;

		pCommand->GetParameter(&tuid, 0, MPT_UID);
		pCommand->GetParameter(&damage, 1, MPT_INT);

		OnDamage(pCommand->GetSenderUID(), tuid, damage);
	}
	break;

	case MC_PEER_VAMPIRE:
	{
		MUID uidAttacker;
		float damage;

		pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
		pCommand->GetParameter(&damage, 1, MPT_FLOAT);

		OnVampire(pCommand->GetSenderUID(), uidAttacker, damage);
	}
	break;

	case MC_PEER_SNIFER:
	{
		bool bSniping = false;

		pCommand->GetParameter(&bSniping, 0, MPT_BOOL);

		OnPeerScope(pCommand->GetSenderUID(), bSniping);
	}
	break;

	case MC_ADMIN_SUMMON:
	{
		MUID uidPlayer;
		char szName[128];

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(szName, 1, MPT_STR, 128);

		OnAdminSummon(uidPlayer, szName);
	}
	break;

	case MC_ADMIN_FREEZE:
	{
		MUID uidPlayer;
		char szName[64];
		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(szName, 1, MPT_STR, sizeof(szName));
		OnAdminFreeze(uidPlayer, szName);
	}break;
	case MC_ADMIN_FREEZE_ALL:
	{
		MUID uidAdmin = MUID(0, 0);
		ZCharacter* pAdmin = NULL;

		if (ZGetGame() && ZGetGame()->m_pMyCharacter && (ZGetGame()->m_pMyCharacter->IsAdminName()))
			break;

		if (!ZGetCharacterManager())
			break;

		uidAdmin = pCommand->GetSenderUID();
		pAdmin = (ZCharacter*)ZGetCharacterManager()->Find(uidAdmin);

		if (pAdmin == NULL)
			break;

		if (!IsAdminGrade(pAdmin->GetCharInfo()->nUGradeID))
			break;

		int nstate = 0;
		pCommand->GetParameter(&nstate, 0, MPT_INT);

		m_bPause = (nstate != 0);

		if (m_pMyCharacter)
			rStuckPosition = m_pMyCharacter->GetPosition();

		char szMsg[256];
		if (nstate != 0)
			sprintf(szMsg, "The room has been freezed");
		else
			sprintf(szMsg, "The room has been unfreezed");

		ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szMsg);
	}
	break;
	case MC_ADMIN_GOTO:
	{
		MUID uidPlayer;
		char szName[128];

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(szName, 1, MPT_STR, 128);

		OnAdminGoTo(uidPlayer, szName);
	}
	break;

	case MC_ADMIN_SLAP:
	{
		MUID uidPlayer;
		char szName[128];

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(szName, 1, MPT_STR, 128);

		OnAdminSlap(uidPlayer, szName);
	}
	break;

	case MC_ADMIN_SPAWN_RESPONSE:
	{
		MUID uidPlayer, uidVictim;
		//char szName[128];

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(&uidVictim, 1, MPT_UID);

		OnAdminSpawn(uidPlayer, uidVictim);
	}
	break;

	case MC_ADMIN_SBTEST:
	{
		MUID uidPlayer;
		char szName[128];

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(szName, 1, MPT_STR, 128);

		OnAdminSBTest(uidPlayer, szName);
	}
	break;

	case MC_MATCH_RESPONSE_ROLL:
	{
		if (!ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_ROLLTHEDICE))
			break;

		char szMsg[256];
		MUID roller;
		int roll;

		pCommand->GetParameter(&roller, 0, MPT_UID);
		pCommand->GetParameter(&roll, 1, MPT_INT);
		pCommand->GetParameter(szMsg, 2, MPT_STR, sizeof(szMsg));

		if (roller == m_pMyCharacter->GetUID())
		{
			SetRolledDice(roll);
			ZGetGameInterface()->GetCombatInterface()->UpdateRTDMsg(szMsg);
			if (GetRolledDice() != roll) break;
			switch (GetRolledDice())
			{
			case 1:
				//Nothing Neccessary here.
				break;
			case 2:
				m_pMyCharacter->SetHP(20);
				m_pMyCharacter->SetAP(0);
				m_pMyCharacter->SetMaxAP(0); //Added 0 AP MAX to prevent users from fully healing back up.

				//ZGetScreenEffectManager()->ReSetHpPanel();

				break;
			case 3:
				//Nothing necessary here.
				break;
			case 4:
				m_pMyCharacter->SetMaxAP(300);
				m_pMyCharacter->SetMaxHP(300);
				m_pMyCharacter->SetHP(300);
				m_pMyCharacter->SetAP(300);

			//	ZGetScreenEffectManager()->ReSetHpPanel();

				break;
			case 5:
				//Nothing necessary here.
				break;
			case 6:
				//Nothing necessary here.
				break;
			case 7:
				//Nothing necessary here.
				break;
			case 8:
				//Nothing necessary here.
				break;
			case 9:
				//Nothing necessary here.
				break;
			case 10:
				//I left the cases here to confuse disassemblers. Once again nothing here. :>
				break;
			case 11:
				break;
			}
		}

		//if (strlen(szMsg) > 0) Bug fixed, no longer necessary.
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szMsg);
	}
	break;

	case MC_MATCH_RESET_TEAM_MEMBERS:
	{
		OnResetTeamMembers(pCommand);
	}
	break;

	case MC_REQUEST_XTRAP_HASHVALUE:				// Update sgk 0706 (ªÁ?E»«? ¿Ã?E˙¿?»£»Ø¿ª ¿ß«ÿ ƒø∏«µÂ∏∏ ¡∏?E
	{
	}
	break;

	case MC_MATCH_DISCONNMSG:
	{
		DWORD dwMsgID;
		pCommand->GetParameter(&dwMsgID, 0, MPT_UINT);

		ZApplication::GetGameInterface()->OnDisconnectMsg(dwMsgID);
	}
	break;

	/*
case MC_PEER_SKILL:
	{
		float fTime;
		int nSkill,sel_type;

		pCommand->GetParameter(&fTime, 0, MPT_FLOAT);
		pCommand->GetParameter(&nSkill, 1, MPT_INT);
		pCommand->GetParameter(&sel_type, 2, MPT_INT);

		OnPeerSkill( pCommand->GetSenderUID(), fTime, nSkill, sel_type );
	}
	break;
	*/

	case ZC_TEST_INFO:
	{
		OutputToConsole("Sync : %u", ZGetGameClient()->GetGlobalClockCount());

		rvector v;
		//				int iQueueCount;
		v = m_pMyCharacter->GetPosition();
		//				iQueueCount = (int)m_pMyCharacter->m_PathQueue.size();
		OutputToConsole("My Pos = %.2f %.2f %.2f", v.x, v.y, v.z);

		/*
		for (ZCharacterItor itor = m_OtherCharacters.begin(); itor != m_OtherCharacters.end(); )
		{
		ZCharacter* pCharacter = (*itor).second;
		v = pCharacter->m_Position;
		iQueueCount = (int)pCharacter->m_PathQueue.size();
		OutputToConsole("Other Pos(%d) = %.2f %.2f %.2f", iQueueCount, v.x, v.y, v.z);
		++itor;
		}
		*/
	}
	break;
	case ZC_BEGIN_PROFILE:
		g_bProfile = true;
		break;
	case ZC_END_PROFILE:
		g_bProfile = false;
		break;
	case ZC_EVENT_OPTAIN_SPECIAL_WORLDITEM:
	{
		OnLocalOptainSpecialWorldItem(pCommand);
	}
	break;

#ifdef _GAMEGUARD
	case MC_REQUEST_GAMEGUARD_AUTH:
	{
		DWORD dwIndex;
		DWORD dwValue1;
		DWORD dwValue2;
		DWORD dwValue3;

		pCommand->GetParameter(&dwIndex, 0, MPT_UINT);
		pCommand->GetParameter(&dwValue1, 1, MPT_UINT);
		pCommand->GetParameter(&dwValue2, 2, MPT_UINT);
		pCommand->GetParameter(&dwValue3, 3, MPT_UINT);

		ZApplication::GetGameInterface()->OnRequestGameguardAuth(dwIndex, dwValue1, dwValue2, dwValue3);

#ifdef _DEBUG
		mlog("zgame recevie request gameguard auth. CmdID(%u) : %u, %u, %u, %u\n", pCommand->GetID(), dwIndex, dwValue1, dwValue2, dwValue3);
#endif
	}
	break;
#endif

#ifdef _XTRAP
	case MC_REQUEST_XTRAP_SEEDKEY:									// add sgk 0411
	{
		MCommandParameter* pParam = pCommand->GetParameter(0);
		if (pParam->GetType() != MPT_BLOB)
		{
			break;
		}
		void* pComBuf = pParam->GetPointer();
		unsigned char* szComBuf = (unsigned char*)MGetBlobArrayElement(pComBuf, 0);
		ZApplication::GetGameInterface()->OnRequestXTrapSeedKey(szComBuf);
	}
	break;
#endif
	case MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM:
	{
		MUID uidItem;
		int nResult;

		pCommand->GetParameter(&uidItem, 0, MPT_UID);
		pCommand->GetParameter(&nResult, 0, MPT_INT);

		OnResponseUseSpendableBuffItem(uidItem, nResult);
	}
	break;

	case MC_MATCH_SPENDABLE_BUFF_ITEM_STATUS:
	{
		//πˆ«¡¡§∫∏¿”Ω√¡÷ºÆ
		_ASSERT(0);
		/*
		MUID uidChar;

		pCommand->GetParameter(&uidChar, 0, MPT_UID);

		MCommandParameter* pParam = pCommand->GetParameter(1);
		if (pParam->GetType() != MPT_BLOB) break;
		void* pCmdBuf = pParam->GetPointer();
		MTD_CharBuffInfo* pCharBuffInfo = (MTD_CharBuffInfo*)MGetBlobArrayElement(pCmdBuf, 0);

		OnGetSpendableBuffItemStatus(uidChar, pCharBuffInfo);
		*/
	}
	break;
	}

	// ∞‘¿”?E?æ˚“≤ ƒø∏«µÂ∞° √≥∏Æµ«æ˙¥¬?EæÀºˆ¿÷µµ∑œ ±‚»∏∏¶ ¡÷¿⁄
	// ∞‘¿”?E?µ˚?Eæ˚“≤ ƒø∏«µÂ∞° ZGameø°º≠ √≥∏Æµ» »ƒø° π∫∞° ?E«œ?EΩÕ¿ª ºˆµµ ¿÷¿ª ∂ß∏¶ ¿ß«—∞Õ
	ZRule* pRule = m_Match.GetRule();
	if (pRule) {
		pRule->AfterCommandProcessed(pCommand);
	}

	// return true;
	return false;
}

rvector ZGame::GetMyCharacterFirePosition(void)
{
	rvector p = ZGetGame()->m_pMyCharacter->GetPosition();
	p.z += 160.f;
	return p;
}

// ø…¿˙?E∂ßø°¥¬ ¿Ã ∆„º«¿« ø™«“¿Ã ∫–∏Æµ»¥Ÿ
// ?E πÃ∏Æ historyø° ¥ı«ÿ¡ˆ?E¿˚¿˝«— ≈∏¿Ãπ÷ø° Ω««‡µ»¥Ÿ.
void ZGame::OnPeerBasicInfo(MCommand* pCommand, bool bAddHistory, bool bUpdate)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if (pParam->GetType() != MPT_BLOB) return;

	ZPACKEDBASICINFO* ppbi = (ZPACKEDBASICINFO*)pParam->GetPointer();

	ZBasicInfo bi;
	bi.position = rvector(Roundf(ppbi->posx), Roundf(ppbi->posy), Roundf(ppbi->posz));
	bi.velocity = rvector(ppbi->velx, ppbi->vely, ppbi->velz);
	bi.direction = 1.f / 32000.f * rvector(ppbi->dirx, ppbi->diry, ppbi->dirz);

	MUID uid = pCommand->GetSenderUID();

	MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(uid);
	if (pPeer) {
		if (pPeer->IsOpened() == false) {
			MCommand* pCmd = ZGetGameClient()->CreateCommand(MC_PEER_OPENED, ZGetGameClient()->GetPlayerUID());
			pCmd->AddParameter(new MCmdParamUID(pPeer->uidChar));
			ZGetGameClient()->Post(pCmd);

			pPeer->SetOpened(true);
		}
	}

	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	// ƒ≥∏Ø≈Õ¿« «ˆ¿ÁΩ√∞£¿ª æ˜µ•¿Ã∆Æ«—¥Ÿ
	// ƒ≥∏Ø≈Õ¿« «ˆ¿ÁΩ√∞£ √ﬂ¡§ƒ°
	float fCurrentLocalTime = pCharacter->m_fTimeOffset + GetTime();

	// ƒ≥∏Ø≈Õ∞° ∫∏≥ªø¬ Ω√∞£¿Ã ≥ª∞° √ﬂ¡§«— Ω√∞£?E3√  ¿Ã?E¬˜¿Ã∞° ≥™?E≥ª∞° æÀ∞˙‹÷¥¬ Ω√∞£¿ª ∞˙‡£¥Ÿ.
	float fTimeError = ppbi->fTime - fCurrentLocalTime;
	if (fabs(fTimeError) > TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME) {
		pCharacter->m_fTimeOffset = ppbi->fTime - GetTime();
		pCharacter->m_fAccumulatedTimeError = 0;
		pCharacter->m_nTimeErrorCount = 0;
	}
	else
	{
		// ¬˜¿Ã∞° 3√  ¿Ã≥ª¿Ã?E¿œ¡§Ω√∞£ «’«ﬂ¥Ÿ∞° ¡∂±›(¬˜¿Ã¿« π›)æø ¡∂¿˝«—¥Ÿ
		pCharacter->m_fAccumulatedTimeError += fTimeError;
		pCharacter->m_nTimeErrorCount++;
		if (pCharacter->m_nTimeErrorCount > 10) {
			pCharacter->m_fTimeOffset += .5f * pCharacter->m_fAccumulatedTimeError / 10.f;
			pCharacter->m_fAccumulatedTimeError = 0;
			pCharacter->m_nTimeErrorCount = 0;
		}
	}

	// «ˆ¿ÁΩ√∞£¿ª ∏∂¡ˆ∏∑ µ•¿Ã≈Õ πﬁ¿∫Ω√∞£¿∏∑Œ ±‚∑œ.
	pCharacter->m_fLastReceivedTime = GetTime();

	pCharacter->m_dwLastBasicInfoTime = timeGetTime();

	// ≥™¡ﬂø° ∆«¡§¿ª ¿ß«ÿ histroy ø° ∫∏?E—¥?
	if (bAddHistory)
	{
		ZBasicInfoItem* pitem = new ZBasicInfoItem;
		CopyMemory(&pitem->info, &bi, sizeof(ZBasicInfo));

		pitem->fReceivedTime = GetTime();

		pitem->fSendTime = ppbi->fTime - pCharacter->m_fTimeOffset;	// ≥ª ±‚¡ÿ¿∏∑Œ ∫Ø»Ø

		pCharacter->m_BasicHistory.push_back(pitem);

		while (pCharacter->m_BasicHistory.size() > CHARACTER_HISTROY_COUNT)
		{
			delete* pCharacter->m_BasicHistory.begin();
			pCharacter->m_BasicHistory.erase(pCharacter->m_BasicHistory.begin());
		}
	}

	if (bUpdate)
	{
		if (!IsReplay() && pCharacter->IsHero() && !ZGetGame()->GetMatch()->IsTurboMode()) return;

		((ZNetCharacter*)(pCharacter))->SetNetPosition(bi.position, bi.velocity, bi.direction);

		pCharacter->SetAnimationLower((ZC_STATE_LOWER)ppbi->lowerstate);
		pCharacter->SetAnimationUpper((ZC_STATE_UPPER)ppbi->upperstate);

		// µÈ∞˙‹÷¥¬ π´±‚∞° ¥Ÿ∏£?EπŸ≤„¡ÿ¥Ÿ
		if (pCharacter->GetItems()->GetSelectedWeaponParts() != ppbi->selweapon) {
			pCharacter->ChangeWeapon((MMatchCharItemParts)ppbi->selweapon);
		}
	}
}

void ZGame::OnPeerHPInfo(MCommand* pCommand)
{
	MUID uid = pCommand->GetSenderUID();
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	float fHP = 0.0f;
	pCommand->GetParameter(&fHP, 0, MPT_FLOAT);

	// Custom: Changed logic here
	if (!IsReplay() && pCharacter->GetUID() != ZGetMyUID() && uid != ZGetMyUID())
		pCharacter->SetHP(fHP);
	else if (IsReplay() || ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
		pCharacter->SetHP(fHP);

	// ø…¿˙?E«œ?E¿÷¿ª∂ß¥¬ ∫∏ø©¡÷?E¿ß«ÿ hp ¡§∫∏∏¶ ∞ªΩ≈«—¥Ÿ.
	//if(ZGetGameInterface()->GetCombatInterface()->GetObserverMode()) {
	//	pCharacter->SetHP(fHP);
	//}
}

void ZGame::OnPeerHPAPInfo(MCommand* pCommand)
{
	MUID uid = pCommand->GetSenderUID();
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	float fHP = 0.0f;
	pCommand->GetParameter(&fHP, 0, MPT_FLOAT);
	float fAP = 0.0f;
	pCommand->GetParameter(&fAP, 1, MPT_FLOAT);

	// Custom: Changed logic here
	if (!IsReplay() && pCharacter->GetUID() != ZGetMyUID() && uid != ZGetMyUID())
	{
		pCharacter->SetHP(fHP);
		pCharacter->SetAP(fAP);
	}
	// Custom: Added a check for teambar (the teamid.ref() is a saftey check, although it's probably bypassed easily).
	else if (IsReplay() || ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
	{
		pCharacter->SetHP(fHP);
		pCharacter->SetAP(fAP);
	}

	// ø…¿˙?E«œ?E¿÷¿ª∂ß¥¬ ∫∏ø©¡÷?E¿ß«ÿ hp ¡§∫∏∏¶ ∞ªΩ≈«—¥Ÿ.
	//if(ZGetGameInterface()->GetCombatInterface()->GetObserverMode()) {
	//	pCharacter->SetHP(fHP);
	//	pCharacter->SetAP(fAP);
	//}
}

void ZGame::OnPeerDuelTournamentHPAPInfo(MCommand* pCommand)
{
	MUID uid = pCommand->GetSenderUID();
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	BYTE MaxHP = 0;
	BYTE MaxAP = 0;
	BYTE HP = 0;
	BYTE AP = 0;

	pCommand->GetParameter(&MaxHP, 0, MPT_UCHAR);
	pCommand->GetParameter(&MaxAP, 1, MPT_UCHAR);

	pCommand->GetParameter(&HP, 2, MPT_UCHAR);
	pCommand->GetParameter(&AP, 3, MPT_UCHAR);

	// ø¯∑° peer¿« HP/AP ¡§∫∏¥¬ ø¿?Eø…¿˙πˆø°∞‘ ∫∏ø©¡÷?E¿ß«ÿº≠∏∏ ?Eﬁµ»¥?
	// ±‚»π?Eµ‡æÛ≈‰≥ ∏’∆Æ¿œ ∂ßø°¥¬ Ω«¡¶ «√∑π¡ﬂø°µµ ?EÅEªÛ?E?HP,AP¿« UI∏¶ ±◊∑¡¡÷±‚∑Œ µ«?E¿÷¥Ÿ.
	// ø…¡Æπˆ∞° æ∆¥“∂ß(¡˜¡¢ «√∑°¿Ã∏¶ «“∂ß)ø° peer¿« ƒ≥∏Ø≈Õ HP, AP∏¶ ∞ªΩ≈«ÿ¡÷?E
	// '≥ª ƒ≥∏Ø≈Õ¿« ¡◊¿Ω¿∫ ≥ª∞° ¡˜¡¢ ∆«¥‹«—¥Ÿ'¥¬ ±‚¡∏ p2p¡§√•?EπÆ¡¶∞° πﬂª˝«“ ?E¿÷?Eƒ≥∏Ø≈Õø° ¡˜¡¢ HP/AP∏¶ set«œ?Eæ ?E
	// UI √‚∑¬?E∏∑?µ˚∑Œ HP/AP∞™¿ª ∫∏?E—¥?
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT) {
		((ZRuleDuelTournament*)m_Match.GetRule())->SetPlayerHpApForUI(uid, (float)MaxHP, (float)MaxAP, (float)HP, (float)AP);
	}

	// ø…¿˙?E«œ?E¿÷¿ª∂ß¥¬ ∫∏ø©¡÷?E¿ß«ÿ hp ¡§∫∏∏¶ ∞ªΩ≈«—¥Ÿ.
	// Custom: Changed logic here
	if (pCharacter->GetUID() != ZGetMyUID() && uid != ZGetMyUID())
	{
		pCharacter->SetMaxHP((float)MaxHP);
		pCharacter->SetMaxAP((float)MaxAP);
		pCharacter->SetHP((float)HP);
		pCharacter->SetAP((float)AP);
	}
	else if (ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
	{
		pCharacter->SetMaxHP((float)MaxHP);
		pCharacter->SetMaxAP((float)MaxAP);
		pCharacter->SetHP((float)HP);
		pCharacter->SetAP((float)AP);
	}
}

#ifdef _DEBUG
static int g_nPingCount = 0;
static int g_nPongCount = 0;
#endif

void ZGame::OnPeerPing(MCommand* pCommand)
{
	if (m_bReplaying.Ref()) return;

	unsigned int nTimeStamp;
	pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);

	// PONG ¿∏∑Œ ¿¿¥‰«—¥Ÿ
	MCommandManager* MCmdMgr = ZGetGameClient()->GetCommandManager();
	MCommand* pCmd = new MCommand(MCmdMgr->GetCommandDescByID(MC_PEER_PONG),
		pCommand->GetSenderUID(), ZGetGameClient()->GetUID());
	pCmd->AddParameter(new MCmdParamUInt(nTimeStamp));
	ZGetGameClient()->Post(pCmd);
}

void ZGame::OnPeerPong(MCommand* pCommand)
{
	MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(pCommand->GetSenderUID());
	if (pPeer == NULL)
		return;

	unsigned int nTimeStamp;
	pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);

	int nPing = (GetTickTime() - nTimeStamp) / 2;
	pPeer->UpdatePing(GetTickTime(), nPing);

	/*
		if (pPeer->IsOpened() == false) {
			MCommand* pCmd = ZGetGameClient()->CreateCommand(MC_PEER_OPENED, ZGetGameClient()->GetPlayerUID());
			pCmd->AddParameter(new MCmdParamUID(pPeer->uidChar));
			ZGetGameClient()->Post(pCmd);

			pPeer->SetOpened(true);
		}
	*/
#ifdef _DEBUG
	g_nPongCount++;

	// Custom: Added test messages to fix spike issue
	//OutputToConsole( "pong -> response (%s): %d", pPeer->CharInfo.szName, nPing );
#endif
}

void ZGame::OnPeerOpened(MCommand* pCommand)
{
	MUID uidChar;
	pCommand->GetParameter(&uidChar, 0, MPT_UID);

	//// Show Character ////////////////////////////////////////
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uidChar);
	if (pCharacter && pCharacter->IsDie() == false) {
		pCharacter->SetVisible(true);

		// Ω≈¿‘ ƒ≥∏Ø≈Õø°∞‘ ¿⁄Ω≈¿« π´±‚∏¶ æÀ∏∞¥Ÿ...
		ZCharacter* pMyCharacter = ZGetGame()->m_pMyCharacter;
		if (pMyCharacter)
		{
			int nParts = ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeaponParts();
			ZGetGame()->m_pMyCharacter->m_dwStatusBitPackingValue.Ref().m_bSpMotion = false;
			ZPostChangeWeapon(nParts);
		}
		// ¿⁄Ω≈¿« πˆ«¡ ªÛ≈¬∏¶ æÀ∏∞¥Ÿ
		PostMyBuffInfo();
	}

#ifdef _DEBUG
	//// PeerOpened Debug log //////////////////////////////////
	char* pszName = "Unknown";
	char* pszNAT = "NoNAT";
	MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(uidChar);
	if (pPeer) {
		pszName = pPeer->CharInfo.szName;
		if (pPeer->GetUDPTestResult() == false) pszNAT = "NAT";
	}

	char szBuf[64];
	sprintf(szBuf, "PEER_OPENED(%s) : %s(%d%d) \n", pszNAT, pszName, uidChar.High, uidChar.Low);
	OutputDebugString(szBuf);
#endif
}

void ZGame::OnChangeWeapon(MUID& uid, MMatchCharItemParts parts)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;
	//	_ASSERT(pCharacter != NULL);

	if (pCharacter && pCharacter != m_pMyCharacter)		// ≥ª ƒ≥∏Ø≈Õ¥¬ ¿ÃπÃ πŸ≤Â¥Ÿ.
	{
		pCharacter->ChangeWeapon(parts);
	}
	if (pCharacter)
	{
		if (pCharacter == m_pMyCharacter)
		{
			if (m_Match.GetMatchType() == MMATCH_GAMETYPE_SPY && m_Match.GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
			{
				if (!ZApplication::GetGameInterface()->GetCombatInterface()) return;

				ZApplication::GetGameInterface()->GetCombatInterface()->SetDefaultSpyTip(m_pMyCharacter->GetTeamID());

				if (!ZApplication::GetGameInterface()->GetCombatInterface()->m_bSpyLocationOpened && !ZApplication::GetGameInterface()->GetCombatInterface()->GetObserverMode())
				{
					ZItem* pItem = m_pMyCharacter->GetItems()->GetItem(parts);
					if (!pItem) return;

					const char* pszSpyTip = ZApplication::GetGameInterface()->GetCombatInterface()->GetSuitableSpyItemTip((int)pItem->GetDescID());
					if (pszSpyTip)
						ZApplication::GetGameInterface()->GetCombatInterface()->SetSpyTip(pszSpyTip);
				}
			}
		}
		else
		{
			pCharacter->ChangeWeapon(parts);
		}
	}
}

void ZGame::OnChangeParts(MUID& uid, int partstype, int PartsID)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;

	if (pCharacter) {
		pCharacter->OnChangeParts((RMeshPartsType)partstype, PartsID);
	}
}

void ZGame::OnAttack(MUID& uid, int type, rvector& pos)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;

		// ?E?∏º«∏∏..
	if (pCharacter) {
	}
}

void ZGame::OnDamage(MUID& uid, MUID& tuid, int damage)
{
	/*
		ZCharacter* pSender = NULL;
		ZCharacter* pTarget = NULL;

		pSender = m_CharacterManager.Find(uid);
		pTarget = m_CharacterManager.Find(tuid);

		pTarget->OnSimpleDamaged(NULL,damage,0.5f);
	*/
}
#ifdef _NOLEAD
void ZGame::OnNoLead(const MUID& uidAttacker, const MUID& uidVictim, const rvector& pos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType)
{
	ZCharacter* pAttacker = (ZCharacter*)m_CharacterManager.Find(uidAttacker);
	ZCharacter* pVictim = (ZCharacter*)m_CharacterManager.Find(uidVictim);

	if (!pAttacker || !pVictim)
		return;

	if (!pAttacker->GetInitialized() || !pVictim->GetInitialized() || !pAttacker->IsVisible() || !pVictim->IsVisible())
		return;

	// Game must start first.
	if (ZGetGame()->GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY)
		return;

	if (IsReplay())
	{
		// Packets are most likely checked already
		//if( CheckWall( pAttacker, pVictim, true ) )
		//	return;

		if (pAttacker->IsDie())
			return;

		pVictim->OnDamaged(pAttacker, pos, damageType, weaponType, fDamage, fPiercingRatio, nMeleeType);
		return;
	}

	// Custom: NOLEAD check
	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_LEAD) || ZGetGameClient()->GetMatchStageSetting()->IsQuestDrived())
		return;

	if (uidVictim == ZGetMyUID() && uidAttacker != ZGetMyUID())
	{
		ZMyCharaterStatusBitPacking& zStatus = m_pMyCharacter->m_statusFlags.Ref();

		// Don't use walls as lagshield
		//if( CheckWall( pAttacker, pVictim, true ) && !zStatus.m_bDrop )
		//	return;

		MMatchPeerInfo* pAttackerPeer = ZGetGameClient()->FindPeer(uidAttacker);

		if (!pAttackerPeer)
			return;

		if (!pAttackerPeer->IsOpened())
			return;

		if (pAttacker->IsDie())
			return;

		// Stop no-lead from working if user has 999 ping - Spiking / disconnected.
		if (pAttackerPeer->GetPing(ZGetGame()->GetTickTime()) == MAX_PING)
			return;

		// Useless, it's still a snippet taken from leading
		/*
		if ( damageType == ZD_MELEE && pVictim->IsGuard() && ( DotProduct( pVictim->m_Direction, pAttacker->m_Direction ) < 0))
		{
			rvector vMyPos = pVictim->GetPosition();
			vMyPos.z += 120.f;

			pVictim->SetPosition( vMyPos );

			ZGetEffectManager()->AddSwordDefenceEffect( pos + ( pVictim->m_Direction * 50.f), pVictim->m_Direction);
			pVictim->OnMeleeGuardSuccess();
			return;
		}
		*/

		m_pMyCharacter->OnDamaged(pAttacker, pos, damageType, weaponType, fDamage, fPiercingRatio, nMeleeType);
	}
}
void ZGame::OnNoLeadBlob(const MUID& uidSender, void* pBlobArray)
{
	// Custom: NOLEAD check
	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_LEAD) || ZGetGameClient()->GetMatchStageSetting()->IsQuestDrived())
		return;

	int nSize = MGetBlobArrayCount(pBlobArray);

	// glitch?
	if (nSize <= 0 || nSize > 255)
		return;

	for (int i = 0; i < nSize; ++i)
	{
		MTD_ShotInfo* pInfo = (MTD_ShotInfo*)MGetBlobArrayElement(pBlobArray, i);

		if (m_pMyCharacter && ZGetGameClient()->GetPlayerUID() != uidSender)
		{
			ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(uidSender);

			rvector pos;
			pos.x = pInfo->fPosX;
			pos.y = pInfo->fPosY;
			pos.z = pInfo->fPosZ;

			if (pCharacter != NULL && pInfo->uid == m_pMyCharacter->GetUID() /*ZGetGameClient()->GetPlayerUID()*/ && CanAttack(pCharacter, m_pMyCharacter))
			{
				if (!m_pMyCharacter->GetInitialized() || !pCharacter->GetInitialized() || !m_pMyCharacter->IsVisible() || !pCharacter->IsVisible())
					return;

				// Game must start first. We do not want random deaths in Finish roundstate
				if (ZGetGame()->GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY)
					return;

				// if it's not a replay..
				if (!IsReplay())
				{
					MMatchPeerInfo* pAttackerPeer = ZGetGameClient()->FindPeer(uidSender);

					if (!pAttackerPeer)
						return;

					if (!pAttackerPeer->IsOpened())
						return;

					// Stop no-lead from working if user has 999 ping - Spiking / disconnected.
					if (pAttackerPeer->GetPing(ZGetGame()->GetTickTime()) == MAX_PING)
						return;

					// sanity check
					if (timeGetTime() - pCharacter->m_dwLastBasicInfoTime >= 4000)
						return;
				}

				if (pCharacter->IsDie())
					return;

				bool bMelee = ((ZDAMAGETYPE)pInfo->nDamageType) == ZD_MELEE;

				m_pMyCharacter->OnDamaged(pCharacter, pos, (ZDAMAGETYPE)pInfo->nDamageType, (MMatchWeaponType)pInfo->nWeaponType, pInfo->fDamage, pInfo->fRatio, bMelee ? pInfo->nMeleeType : -1);
			}
		}
	}
}
#endif
void ZGame::OnVampire(const MUID& uidVictim, const MUID& uidAttacker, float fDamage)
{
	ZCharacter* pAttacker = (ZCharacter*)m_CharacterManager.Find(uidAttacker);
	ZCharacter* pVictim = (ZCharacter*)m_CharacterManager.Find(uidVictim);

	if (!pAttacker || !pVictim)
		return;

	if (!pAttacker->GetInitialized() || !pVictim->GetInitialized() || !pAttacker->IsVisible() || !pVictim->IsVisible())
		return;

	if (IsReplay())
	{
		int nAddHP = (int)(fDamage * 0.33f);
		int nCurrHP = pAttacker->GetHP();
		int nMaxHP = pAttacker->GetMaxHP();

		if (nCurrHP + nAddHP > nMaxHP)
		{
			pAttacker->SetHP(nMaxHP);

			int nAddAP = (nCurrHP + nAddHP) - nMaxHP;
			int nCurrAP = pAttacker->GetAP();
			int nMaxAP = pAttacker->GetMaxAP();

			if (nCurrAP + nAddAP > nMaxAP)
				pAttacker->SetAP(nMaxAP);
			else
				pAttacker->SetAP(nCurrAP + nAddAP);
		}
		else
			pAttacker->SetHP(nCurrHP + nAddHP);

		return;
	}

	if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_VAMPIRE) && !ZGetGameClient()->GetMatchStageSetting()->IsQuestDrived()
		&& ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_QUEST_CHALLENGE)
	{
		if (uidAttacker == ZGetMyUID() && uidVictim != ZGetMyUID())
		{
			MMatchPeerInfo* pVictimPeer = ZGetGameClient()->FindPeer(uidVictim);

			if (!pVictimPeer)
				return;

			if (!pVictimPeer->IsOpened())
				return;

			int nAddHP = (int)(fDamage * 0.33f);
			int nCurrHP = m_pMyCharacter->GetHP();
			int nMaxHP = m_pMyCharacter->GetMaxHP();

			if (nCurrHP + nAddHP > nMaxHP)
			{
				m_pMyCharacter->SetHP(nMaxHP);

				int nAddAP = (nCurrHP + nAddHP) - nMaxHP;
				int nCurrAP = m_pMyCharacter->GetAP();
				int nMaxAP = m_pMyCharacter->GetMaxAP();

				if (nCurrAP + nAddAP > nMaxAP)
					m_pMyCharacter->SetAP(nMaxAP);
				else
					m_pMyCharacter->SetAP(nCurrAP + nAddAP);
			}
			else
				m_pMyCharacter->SetHP(nCurrHP + nAddHP);
		}
	}
}
void ZGame::OnPeerScope(const MUID& uidPeer, bool bScope)
{
	ZCharacter* pOwner = (ZCharacter*)m_CharacterManager.Find(uidPeer);

	if (!pOwner)
		return;

	// visible? what if I scoped? I'll be invisible and this will return
	if (!pOwner->GetInitialized() /*|| !pOwner->IsVisible()*/)
		return;

	if (!IsReplay() && uidPeer == ZGetMyUID())
		return;

	ZCharaterStatusBitPacking& uStatus = pOwner->m_dwStatusBitPackingValue.Ref();

	if (ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
	{
		if (bScope)
		{
			if (!uStatus.m_bSniping)
			{
				uStatus.m_bSniping = true;

				if (ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter() == pOwner)
					ZGetGameInterface()->GetCombatInterface()->OnGadget(MWT_SNIFER);
#ifdef _HITSCOUNT
				if (pOwner == m_pMyCharacter)
					ZGetGame()->m_pMyCharacter->m_nHits += 1;
#endif
			}
		}
		else
		{
			if (uStatus.m_bSniping)
			{
				uStatus.m_bSniping = false;

				if (ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter() == pOwner)
					ZGetGameInterface()->GetCombatInterface()->OnGadgetOff();
			}
		}
	}
}

void ZGame::OnAdminFreeze(const MUID& uidAdmin, const char* szTargetName)
{
	ZCharacter* pCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidAdmin);
	if (!pCharacter) return;
	ZCharacter* pTarget = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(ZGetGame()->m_pMyCharacter->GetUID());

	char szTemp[128];
	if (!stricmp(szTargetName, pTarget->GetUserName()))
	{
		sprintf(szTemp, "%s has frozen you.", pCharacter->GetUserName());
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), szTemp, ZChat::CL_CURRENT);
		if (!pTarget->IsAdminName() && pTarget->GetUserGrade() != MMUG_EVENTTEAM)
		{
			if (pTarget->m_dwStatusBitPackingValue.Ref().m_bFrozen == true)
			{
				pTarget->m_dwStatusBitPackingValue.Ref().m_bFrozen = false;
			}
			else
			{
				pTarget->m_dwStatusBitPackingValue.Ref().m_bFrozen = true;
			}
		}
	}
	if (!stricmp(szTargetName, "all"))
	{
		sprintf(szTemp, "%s has frozen the room", pCharacter->GetUserName());
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), szTemp, ZChat::CL_CURRENT);
		if (!pTarget->IsAdminName() && pTarget->GetUserGrade() != MMUG_EVENTTEAM)
		{
			if (pTarget->m_dwStatusBitPackingValue.Ref().m_bFrozen == true) {
				pTarget->m_dwStatusBitPackingValue.Ref().m_bFrozen = false;
			}
			else
			{
				pTarget->m_dwStatusBitPackingValue.Ref().m_bFrozen = true;
			}
		}
	}
}

void ZGame::OnAdminSummon(const MUID& uidAdmin, const char* szTargetName)
{
	ZCharacter* pAdmin = (ZCharacter*)m_CharacterManager.Find(uidAdmin);

	if (!pAdmin)
		return;

	if (!pAdmin->GetInitialized() || !pAdmin->IsVisible())
		return;

	if (!stricmp(szTargetName, m_pMyCharacter->GetUserName()))
	{
		char szTemp[128];
		sprintf(szTemp, "%s has summoned you.", pAdmin->GetUserName());
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), szTemp, ZChat::CL_CURRENT);
		m_pMyCharacter->SetPosition(pAdmin->GetPosition());

		ZGetEffectManager()->AddReBirthEffect(pAdmin->GetPosition());
		ZGetSoundEngine()->PlaySound("fx_respawn");
	}
}

void ZGame::OnAdminGoTo(const MUID& uidAdmin, const char* szTargetName)
{
	ZCharacter* pTarget = NULL;

	// herp derp
	// could put it into a new variable for Muid, but meh.
	for (ZCharacterManager::iterator i = ZGetCharacterManager()->begin(); i != ZGetCharacterManager()->end(); ++i)
	{
		ZCharacter* pChar = (ZCharacter*)(*i).second;

		if (pChar && pChar->GetInitialized() && pChar->IsVisible() && !pChar->IsAdminHide() && !stricmp(pChar->GetUserName(), szTargetName))
		{
			pTarget = pChar;
			break;
		}
	}

	if (!pTarget)
		return;

	if (!pTarget->GetInitialized() || !pTarget->IsVisible())
		return;

	// do not output a message. it's done by the server
	m_pMyCharacter->SetPosition(pTarget->GetPosition());

	ZGetEffectManager()->AddReBirthEffect(m_pMyCharacter->GetPosition());
	ZGetSoundEngine()->PlaySound("fx_respawn");
}

void ZGame::OnAdminSlap(const MUID& uidAdmin, const char* szTargetName)
{
	ZCharacter* pAdmin = (ZCharacter*)m_CharacterManager.Find(uidAdmin);

	if (!pAdmin)
		return;

	if (!pAdmin->GetInitialized() || !pAdmin->IsVisible())
		return;

	if (!stricmp(szTargetName, m_pMyCharacter->GetUserName()))
	{
		char szTemp[128];
		sprintf(szTemp, "%s has slapped you!", pAdmin->GetUserName());
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), szTemp, ZChat::CL_CURRENT);
		// Custom:Improved /admin_slap. Less glitchier, properly awards the kill etc.
		//m_pMyCharacter->SetAP( 0 );
		//m_pMyCharacter->SetHP( 0 );
		ZPostGameKill(uidAdmin);
		ZPostDie(uidAdmin);
	}
}

void ZGame::OnAdminSpawn(const MUID& uidAdmin, const MUID& uidVictim)
{
	ZCharacter* pAdmin = (ZCharacter*)m_CharacterManager.Find(uidAdmin);
	ZCharacter* pVictim = (ZCharacter*)m_CharacterManager.Find(uidVictim);

	if (!pAdmin || !pVictim)
		return;

	if (!pAdmin->GetInitialized() || !pAdmin->IsVisible() || !pVictim->GetInitialized() || !pVictim->IsVisible())
		return;

	rvector pos = pVictim->GetPosition(), dir = pVictim->GetDirection();

	//I realise this is a dirty way of doing it, and relying on GunZ to update the position
	// but if you want to keep the ranomized spawn location, this is the only logical way
	// Short of posting ZPostSpawn, anyway.
	if (uidVictim == m_pMyCharacter->GetUID())
	{
		ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetSoloRandomData();
		if (pSpawnData != NULL)
		{
			pos = pSpawnData->m_Pos;
			dir = pSpawnData->m_Dir;
		}
	}

	OnPeerSpawn(pVictim->GetUID(), pos, dir);

	if (uidVictim == m_pMyCharacter->GetUID())
	{
		ReleaseObserver();
		//ZGetCombatInterface()->SetObserverMode(false);
	}
}
void ZGame::OnAdminSBTest(const MUID& uidAdmin, const char* szTargetName)
{
	ZCharacter* pAdmin = (ZCharacter*)m_CharacterManager.Find(uidAdmin);

	if (!pAdmin)
		return;

	// admin doesn't need to spawn
	//if( !pAdmin->GetInitialized() || !pAdmin->IsVisible() )
	//	return;

	m_pMyCharacter->m_dwSBTestStart = timeGetTime();
	m_pMyCharacter->m_bSBTest = true;
}
void ZGame::OnPeerShotSp(MUID& uid, float fShotTime, rvector& pos, rvector& dir, int type, MMatchCharItemParts sel_type)
{
	ZCharacter* pOwnerCharacter = NULL;		// √— ?EªÁ?E

	pOwnerCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	//	if (uid == ZGetGameClient()->GetUID()) pOwnerCharacter = m_pMyCharacter;

	//	_ASSERT(pOwnerCharacter != NULL);
	if (pOwnerCharacter == NULL) return;
	if (!pOwnerCharacter->GetInitialized()) return;
	if (!pOwnerCharacter->IsVisible()) return;

	ZItem* pItem = pOwnerCharacter->GetItems()->GetItem(sel_type);
	if (!pItem) return;

	MMatchItemDesc* pDesc = pItem->GetDesc();
	if (pDesc == NULL) return;

	// fShotTime ¿Ã ±◊ ƒ≥∏Ø≈Õ¿« ∑Œƒ√ Ω√∞£¿Ãπ«∑Œ ≥ª Ω√∞£¿∏∑Œ ∫Ø»Ø«ÿ¡ÿ¥Ÿ
	fShotTime -= pOwnerCharacter->m_fTimeOffset;

	/*
	float fCurrentTime = g_pGame->GetTime();
	if( abs(fCurrentTime - fShotTime) > TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME )
	{
#ifdef _DEBUG
		mlog("!!!!ºˆ∑˘≈∫ «Ÿ!!!! ƒ≥∏Ø≈Õ ≥◊¿”: %s      fShotTime : %f     fCurrentTime : %f \n",
			pOwnerCharacter->GetUserName(), (fShotTime - pOwnerCharacter->m_fTimeOffset) , fCurrentTime);
#endif
		return;
	}
	¿Ã ∫Œ∫–¿∫ «Ÿø°º≠ shot¿ª «— Ω√∞£¿ª ¡∂¿€«œø© ∫∏≥ª¥¬ ∞Õ¿ª ∞®¡ˆ«œø© «Ÿ¿ª ∏∑¥¬ ƒ⁄µÂø¥¥¬µ• πﬁ¥¬ ¬ ø°º≠ Ω√∞£ ∞ÀªÁ∏¶ «œ?E∏ª?E
	∫∏≥ª¥¬ ¬ ø°º≠ ∞ÀªÁ∏¶ «ÿº≠ shot¿ª «— Ω√∞£¿Ã «ÿ?Eƒ≥∏Ø≈Õ¿« lacal time?E∏¬?Eæ ¿∏?Eæ∆øπ ∆–≈∂¿ª ∫∏≥ª?Eæ µµ∑œ πŸ≤Â¥Ÿ.
	µ˚∂Ûº≠ «ÿ?Eƒ⁄µÂ∞° « ?Eæ¯∞‘ µ . √ﬂ»ƒ localtime¿ª ¡∂¿€«“ ∞Ê?E??EÒ«?¡÷ºÆ√≥∏Æ∑Œ ≥≤∞‹µ“..
	*/

	//ø©?E∆¯πﬂπ∞ ?EÅE«‘ºˆ¿Ã?E∂ßπÆø° π´±‚∑˘øÕ ∆¯≈∫∑˘∏¶ ¥„¿ª ?E¿÷¥¬ ∆ƒ√˜∞° æ∆¥œ?Eπ´Ω√«—¥Ÿ.
	if (sel_type != MMCIP_PRIMARY && sel_type != MMCIP_SECONDARY && sel_type != MMCIP_CUSTOM1 && sel_type != MMCIP_CUSTOM2 && sel_type != MMCIP_CUSTOM3)
		return;

	MMatchCharItemParts parts = (MMatchCharItemParts)sel_type;

	if (parts != pOwnerCharacter->GetItems()->GetSelectedWeaponParts()) { ///< ¡ˆ±› µÈ?E¿÷¥¬ π´±‚øÕ ∫∏≥ª?Eπ´±‚∞° ∆≤∏Æ¥Ÿ?E∫∏≥ª?Eπ´±‚∑Œ πŸ≤„¡ÿ¥Ÿ..
		OnChangeWeapon(uid, parts);
	}

	//«Ÿ πÊ¡ˆ∏¶ ¿ß«ÿ ø˛?E≈∏¿‘¿ª ∫Ò±≥..
	MMatchWeaponType nType = pDesc->m_nWeaponType.Ref();
	//µÈ?E¿÷¥¬ π´±‚∞° ∑Œƒœ ≈∏¿‘¿Œµ•
	if (nType == MWT_ROCKET) {
		if (type != ZC_WEAPON_SP_ROCKET) {	//type¿Ã ∑Œƒœ¿Ã æ∆¥œ?EπÃΩ∫ ∏≈ƒ°....π´Ω√«—¥Ÿ.
			return;
		}
	}
	else if (nType == MWT_MED_KIT || nType == MWT_REPAIR_KIT || nType == MWT_BULLET_KIT || nType == MWT_FOOD || nType == MWT_LANDMINE_SPY || nType == MWT_LANDMINE) {
		if (type != ZC_WEAPON_SP_ITEMKIT) {
			return;
		}
	}
	else if (nType == MWT_FLASH_BANG) {
		if (type != ZC_WEAPON_SP_FLASHBANG) {
			return;
		}
	}
	else if (nType == MWT_FRAGMENTATION) {
		if (type != ZC_WEAPON_SP_GRENADE) {
			return;
		}
	}
	else if (nType == MWT_SMOKE_GRENADE) {
		if (type != ZC_WEAPON_SP_SMOKE) {
			return;
		}
	}
	else if (nType == MWT_POTION) {
		if (type != ZC_WEAPON_SP_POTION) {
			return;
		}
	}
	else if (nType == MWT_TRAP) {
		if (type != ZC_WEAPON_SP_TRAP) {
			return;
		}
	}
	else if (nType == MWT_DYNAMITYE) {
		if (type != ZC_WEAPON_SP_DYNAMITE) {
			return;
		}
	}
	///// SPY MODE /////
	else if (nType == MWT_FLASH_BANG_SPY) {
		if (type != ZC_WEAPON_SP_FLASHBANG_SPY) {
			return;
		}
	}
	else if (nType == MWT_SMOKE_GRENADE_SPY) {
		if (type != ZC_WEAPON_SP_SMOKE_SPY) {
			return;
		}
	}
	else if (nType == MWT_TRAP_SPY) {
		if (type != ZC_WEAPON_SP_TRAP_SPY) {
			return;
		}
	}
	else if (nType == MWT_STUN_GRENADE_SPY) {
		if (type != ZC_WEAPON_SPY_STUNGRENADE) {
			return;
		}
	}
	else {
		return;
	}

	// ∫Ò¡§ªÛ¿˚¿Œ πﬂªÁº”µµ∏¶ π´Ω√«—¥Ÿ.
	if (pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem)) {
		pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
	}
	else {
		return;
	}

	if (uid == ZGetMyUID()) {
		int nCurrMagazine = pItem->GetBulletCurrMagazine();
		if (!pItem->Shot()) return;

		if (!(pItem->GetBulletCurrMagazine() < nCurrMagazine)) {
			if (sel_type != MMCIP_MELEE) ZGetApplication()->Exit();
		}
	}
	else {
		if (!pItem->Shot()) return;
	}

#ifdef _RADAR
	if (strstr(ZGetGameClient()->GetChannelName(), "Ladder"))
	{
		if (type == ZC_WEAPON_SP_ROCKET || ZC_WEAPON_SP_GRENADE)
			ZGetCombatInterface()->GetRadar()->OnAttack(pos, uid);
	}
#endif

	rvector velocity;
	rvector _pos;

	bool dLight = true;
	bool bSpend = false;

	switch (type)
	{
	case ZC_WEAPON_SP_GRENADE:
	{
		//static RealSoundEffectSource* pSES	= ZGetSoundEngine()->GetSES("we_grenade_fire");
		//if( pSES != NULL )
		//{
		//	ZGetSoundEngine()->PlaySE( pSES, pos.x, pos.y, pos.z, pOwnerCharacter == m_pMyCharacter );
		//}
		bSpend = true;

		velocity = pOwnerCharacter->GetVelocity() + pOwnerCharacter->m_TargetDir * 1200.f;
		velocity.z += 300.f;
		m_WeaponManager.AddGrenade(pos, velocity, pOwnerCharacter);
		//m_WeaponManager.AddFlashBang( pos - rvector(10,10,10), velocity, pOwnerCharacter );
		//m_WeaponManager.AddSmokeGrenade( pos + rvector(10,10,10), velocity, pOwnerCharacter );
	}
	break;

	case ZC_WEAPON_SP_ROCKET:
	{
		m_WeaponManager.AddRocket(pos, dir, pOwnerCharacter);
		if (Z_VIDEO_DYNAMICLIGHT) {
			ZGetStencilLight()->AddLightSource(pos, 2.0f, 100);
		}
	}
	break;

	case ZC_WEAPON_SP_FLASHBANG:
	{
		bSpend = true;

		velocity = pOwnerCharacter->GetVelocity() + pOwnerCharacter->m_TargetDir * 1200.f;
		velocity.z += 300.0f;
		m_WeaponManager.AddFlashBang(pos, velocity, pOwnerCharacter);
		dLight = false;
	}
	break;

	case ZC_WEAPON_SP_SMOKE:
	{
		bSpend = true;

		velocity = pOwnerCharacter->GetVelocity() + pOwnerCharacter->m_TargetDir * 1200.f;
		velocity.z += 300.0f;
		m_WeaponManager.AddSmokeGrenade(pos, velocity, pOwnerCharacter);
		dLight = false;
	}
	break;

	case ZC_WEAPON_SP_TEAR_GAS:
	{
		bSpend = true;
		dLight = false;
	}
	break;

	case ZC_WEAPON_SP_ITEMKIT:
	{
		int nLinkedWorldItem = ZGetWorldItemManager()->GetLinkedWorldItemID(pItem->GetDesc());

		velocity = dir;
		_pos = pos;

		m_WeaponManager.AddKit(_pos, velocity, pOwnerCharacter, 0.2f, pItem->GetDesc()->m_pMItemName->Ref().m_szMeshName, nLinkedWorldItem);
		dLight = false;
	}
	break;

	case ZC_WEAPON_SP_POTION:
	{
		ApplyPotion(pItem->GetDescID(), pOwnerCharacter, 0);
	}
	break;

	case ZC_WEAPON_SP_TRAP:
	{
		OnUseTrap(pItem->GetDescID(), pOwnerCharacter, pos);
		dLight = true;
	}
	break;

	case ZC_WEAPON_SP_DYNAMITE:
	{
		OnUseDynamite(pItem->GetDescID(), pOwnerCharacter, pos);
		dLight = true;
	}
	break;

	case ZC_WEAPON_SP_FLASHBANG_SPY:
	{
		bSpend = true;

		velocity = pOwnerCharacter->GetVelocity() + pOwnerCharacter->m_TargetDir * 1200.f;
		velocity.z += 300.0f;
		m_WeaponManager.AddSpyFlashBang(pos, velocity, pOwnerCharacter);
		dLight = false;
	}
	break;

	//////////// SPY ////////////
	case ZC_WEAPON_SP_SMOKE_SPY:
	{
		bSpend = true;

		velocity = pOwnerCharacter->GetVelocity() + pOwnerCharacter->m_TargetDir * 1200.f;
		velocity.z += 300.0f;
		m_WeaponManager.AddSpySmokeGrenade(pos, velocity, pOwnerCharacter);
		dLight = false;
	}
	break;

	case ZC_WEAPON_SP_TRAP_SPY:
	{
		OnUseSpyTrap(pItem->GetDescID(), pOwnerCharacter, pos);
		dLight = true;
	}
	break;

	case ZC_WEAPON_SPY_STUNGRENADE:
	{
		OnUseStunGrenade(pItem->GetDescID(), pOwnerCharacter, pos);
		dLight = true;
	}
	break;

	default:
		_ASSERT(0);
		break;
	}

#ifdef _REPLAY_TEST_LOG
	if (type == ZC_WEAPON_SP_POTION || type == ZC_WEAPON_SP_ITEMKIT)
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_Replay_UseItem[i].uid.Low == 0)
			{	// uid ≥ª?E?æ¯¥Ÿ?E
				m_Replay_UseItem[i].uid = uid;
				m_Replay_UseItem[i].Item[0].Itemid = pDesc->m_nID;
				m_Replay_UseItem[i].Item[0].ItemUseCount++;
				break;
			}
			if (m_Replay_UseItem[i].uid == uid)
			{	// uid∞° ∞∞¥Ÿ?E
				for (int j = 0; j < 5; ++j)
				{
					if (m_Replay_UseItem[i].Item[j].Itemid == 0)
					{	// itemid ≥ª?E?æ¯¥Ÿ?E
						m_Replay_UseItem[i].uid = uid;
						m_Replay_UseItem[i].Item[j].Itemid = pDesc->m_nID;
						m_Replay_UseItem[i].Item[j].ItemUseCount++;
						break;
					}
					if (m_Replay_UseItem[i].Item[j].Itemid == pDesc->m_nID)
					{
						m_Replay_UseItem[i].Item[j].ItemUseCount++;
						break;
					}
				}
				break;
			}
		}
		//mlog("[%s(%d) : %s(%d)]\n", pOwnerCharacter->GetCharInfo()->szName, uid.Low, pDesc->m_pMItemName->Ref().m_szItemName, pDesc->m_nID);
	}
#endif

	// ∆˜º«∑˘¥¬ ±◊≥… «œµÂƒ⁄µ˘¿∏∑Œ æ∆¿Ã≈€ ∏‘¥¬ ªÁø˚—Â∏¶ ≥ª∞‘∏∏ µÈ∑¡¡ÿ¥Ÿ
	// ±◊≥… ¿œπ› π´±‚√≥∑≥ √≥∏Æ«œ?EπﬂªÁ¿Ω¿∏∑Œ ∞£¡÷µ«æ˚ÿ≠ ¡÷∫ØªÁ∂˜µÈø°∞‘ µÈ∏Æ∞‘ µ«¥¬µ•,
	// æ∆¿Ã≈€ ∏‘¥¬ ªÁø˚—Â∞° 2dªÁø˚—Â∂Ûº≠ ?E?Eºø?µÈ∏Æ∞‘ µ«?E?Eı«?
	if (type == ZC_WEAPON_SP_POTION)
	{
		if (pOwnerCharacter == ZGetGame()->m_pMyCharacter) {
			ZGetSoundEngine()->PlaySound("fx_itemget");
		}
	}
	else
	{
		ZApplication::GetSoundEngine()->PlaySEFire(pItem->GetDesc(), pos.x, pos.y, pos.z, (pOwnerCharacter == m_pMyCharacter));
	}

	if (dLight)
	{
		// √— ΩÚ∂ß ∂Û¿Ã∆Æ √ﬂ∞°
		ZCharacter* pChar;

		if (ZGetConfiguration()->GetVideo()->bDynamicLight && pOwnerCharacter != NULL) {
			pChar = pOwnerCharacter;

			if (pChar->m_bDynamicLight) {
				pChar->m_vLightColor = g_CharLightList[CANNON].vLightColor;
				pChar->m_fLightLife = g_CharLightList[CANNON].fLife;
			}
			else {
				pChar->m_bDynamicLight = true;
				pChar->m_vLightColor = g_CharLightList[CANNON].vLightColor;
				pChar->m_vLightColor.x = 1.0f;
				pChar->m_iDLightType = CANNON;
				pChar->m_fLightLife = g_CharLightList[CANNON].fLife;
			}

			if (pOwnerCharacter->IsHero())
			{
				RGetDynamicLightManager()->AddLight(GUNFIRE, pos);
			}
		}
	}

	if (ZGetMyUID() == pOwnerCharacter->GetUID())
	{
		ZItem* pSelItem = pOwnerCharacter->GetItems()->GetSelectedWeapon();
		if (pSelItem && pSelItem->GetDesc() &&
			pSelItem->GetDesc()->IsSpendableItem())
		{
			ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetEquipedItem((MMatchCharItemParts)sel_type);
			if (pItemNode)
			{
				pItemNode->SetItemCount(pItemNode->GetItemCount() - 1);
				ZPostRequestUseSpendableNormalItem(pItemNode->GetUID());
			}
		}
	}
}

bool ZGame::CheckWall(ZObject* pObj1, ZObject* pObj2, bool bCoherentToPeer)
{
	//### ¿Ã «‘ºˆ∏¶ ºˆ¡§«œ?E∂»∞∞¿Ã IsWallBlocked()ø°µµ ¿˚?E?¡÷æ˚⁄ﬂ «’¥œ¥Ÿ. ###

	// ««æ˚œ¢∏Æ ¡¬«•∏¶ ∫∏≥æ∂ß «ˆ¿Á¥¬ float->short ƒ≥Ω∫∆√¿Ã ¿œæ˚œ≠¥Ÿ (?Eº“ºˆ¡°¿Ã«œ πˆ∏≤)
	// µ˚∂Ûº≠ ≈¨∂Û¿Ãæ∆ÆµÈ¿Ã ∞¢¿⁄ ∆«¡§«— ∞™¿Ã πÃπ¶«œ∞‘ ¥Ÿ∏¶ ºˆ∞° ¿÷¥Ÿ. ¿Ã∞Õ¿Ã ±‚¡∏ø° πÆ¡¶∏¶ ¿œ¿∏≈∞?Eæ æ“¿∏≥™
	// º≠πŸ¿Ãπ˙ø°º≠ ≥∑¿∫ »Æ?E?πÆ¡¶∞° πﬂª˝: npc∞° «√∑π¿Ãæ˚‘¶ ±Ÿ¡¢∞¯∞›«œ∑¡?E«“∂ß, npc ƒ¡∆Æ∑—∑Ø¥¬ ∞¯∞› ∞°¥…«œ¥Ÿ?E∆«¡§.
	// ««∞›¥Á«œ¥¬ ≈¨∂Û¿Ãæ∆Æ¥¬ ∞¯∞› ∞°¥…«œ?Eæ ¥Ÿ?E∆«¡§. ¿Ã∑Œ?E««∞›µ«¥¬ ¿Ø¿˙∞° ¿ßƒ°∏¶ πŸ≤Ÿ?Eæ ¥¬«— ∏ÛΩ∫≈Õ¥¬ ¡¶¿⁄∏Æø°º≠ π´«— ?EÊ¿?ƒ°∞‘µ  (º÷±˚‘ª æ«?E∫“∞°∂Û?Eª˝∞¢«œ¡ˆ∏∏ ∆€∫˙‘Æº≈¿« ±Ÿº∫ø° ¡≥¿Ω)
	// bCoherentToPeer==true ¿œ∂ß ««æ˚€°∞‘ ∫∏≥Ω ∞Õ?E∞∞¿∫ ∞™¿ª ªÁ?E?.

	if ((pObj1 == NULL) || (pObj2 == NULL))
		return false;

	if ((pObj1->GetVisualMesh() == NULL) || (pObj2->GetVisualMesh() == NULL))
		return false;

	// ø°¥œ∏ﬁ¿Ãº« ∂ßπÆø° ∫Æ¿ª ∂’?EµÈæ˚Ã°¥¬ ∞Ê?E?¿÷æ˚ÿ≠..
	rvector p1 = pObj1->GetPosition() + rvector(0.f, 0.f, 100.f);
	rvector p2 = pObj2->GetPosition() + rvector(0.f, 0.f, 100.f);

	if (bCoherentToPeer)
	{
		p1.x = short(p1.x);
		p1.y = short(p1.y);
		p1.z = short(p1.z);
		p2.x = short(p2.x);
		p2.y = short(p2.y);
		p2.z = short(p2.z);
		// ø¿¬˜∑Œ ¿Œ«— πˆ±◊ ¿Á?E≈◊Ω∫∆Æ∏¶ Ω±∞‘ «œ?E¿ß«ÿ 1¿« ¿⁄∏Æ±˚?E¿˝ªÁ«— πˆ?E
		/*p1.x = (short(p1.x * 0.1f)) * 10.f;
		p1.y = (short(p1.y * 0.1f)) * 10.f;
		p1.z = (short(p1.z * 0.1f)) * 10.f;
		p2.x = (short(p2.x * 0.1f)) * 10.f;
		p2.y = (short(p2.y * 0.1f)) * 10.f;
		p2.z = (short(p2.z * 0.1f)) * 10.f;*/
	}

	rvector dir = p2 - p1;

	Normalize(dir);

	ZPICKINFO pickinfo;

	if (Pick(pObj1, p1, dir, &pickinfo)) {//∫Æ¿Ã∂Û?E
		if (pickinfo.bBspPicked)//∏ ¿Ã ∞…∏∞∞Ê?E
			return true;
	}

	return false;
}
//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑.....∫Ò±≥∏¶ ¿ß«ÿº≠
bool ZGame::CheckWall(ZObject* pObj1, ZObject* pObj2, int& nDebugRegister/*¥‹?E∫Ò±≥?E*/, bool bCoherentToPeer)
{	//bCoherentToPeerø° ?E—∞Õ¿?ø¯∫ª CheckWall ¡÷ºÆ ?EÅE
	if ((pObj1 == NULL) || (pObj2 == NULL))
	{
		nDebugRegister = -10;	//ø™Ω√≥™ º˝¿⁄¥¬ ¿«πÃ∞° æ¯¥Ÿ..
		return false;
	}

	if ((pObj1->GetVisualMesh() == NULL) || (pObj2->GetVisualMesh() == NULL))
	{
		nDebugRegister = -10;
		return false;
	}

	// ø°¥œ∏ﬁ¿Ãº« ∂ßπÆø° ∫Æ¿ª ∂’?EµÈæ˚Ã°¥¬ ∞Ê?E?¿÷æ˚ÿ≠..
	rvector p1 = pObj1->GetPosition() + rvector(0.f, 0.f, 100.f);
	rvector p2 = pObj2->GetPosition() + rvector(0.f, 0.f, 100.f);

	if (bCoherentToPeer)
	{
		p1.x = short(p1.x);
		p1.y = short(p1.y);
		p1.z = short(p1.z);
		p2.x = short(p2.x);
		p2.y = short(p2.y);
		p2.z = short(p2.z);
		// ø¿¬˜∑Œ ¿Œ«— πˆ±◊ ¿Á?E≈◊Ω∫∆Æ∏¶ Ω±∞‘ «œ?E¿ß«ÿ 1¿« ¿⁄∏Æ±˚?E¿˝ªÁ«— πˆ?E
		/*p1.x = (short(p1.x * 0.1f)) * 10.f;
		p1.y = (short(p1.y * 0.1f)) * 10.f;
		p1.z = (short(p1.z * 0.1f)) * 10.f;
		p2.x = (short(p2.x * 0.1f)) * 10.f;
		p2.y = (short(p2.y * 0.1f)) * 10.f;
		p2.z = (short(p2.z * 0.1f)) * 10.f;*/
	}

	rvector dir = p2 - p1;

	Normalize(dir);

	ZPICKINFO pickinfo;

	if (Pick(pObj1, p1, dir, &pickinfo)) {//∫Æ¿Ã∂Û?E
		if (pickinfo.bBspPicked)//∏ ¿Ã ∞…∏∞∞Ê?E
		{
			nDebugRegister = FOR_DEBUG_REGISTER;
			return true;
		}
	}
	nDebugRegister = -10;
	return false;
}
bool ZGame::IsWallBlocked(ZObject* pObj1, ZObject* pObj2, bool bCoherentToPeer)
{
	//### ¿Ã «‘ºˆ∏¶ ºˆ¡§«œ∏È ∂»∞∞¿Ã IsWallBlocked()ø°µµ ¿˚øÎ«ÿ ¡÷æÓæﬂ «’¥œ¥Ÿ. ###

	// ««æÓ≥¢∏Æ ¡¬«•∏¶ ∫∏≥æ∂ß «ˆ¿Á¥¬ float->short ƒ≥Ω∫∆√¿Ã ¿œæÓ≥≠¥Ÿ (¡Ô º“ºˆ¡°¿Ã«œ πˆ∏≤)
	// µ˚∂Ûº≠ ≈¨∂Û¿Ãæ∆ÆµÈ¿Ã ∞¢¿⁄ ∆«¡§«— ∞™¿Ã πÃπ¶«œ∞‘ ¥Ÿ∏¶ ºˆ∞° ¿÷¥Ÿ. ¿Ã∞Õ¿Ã ±‚¡∏ø° πÆ¡¶∏¶ ¿œ¿∏≈∞¡¯ æ æ“¿∏≥™
	// º≠πŸ¿Ãπ˙ø°º≠ ≥∑¿∫ »Æ∑¸∑Œ πÆ¡¶∞° πﬂª˝: npc∞° «√∑π¿ÃæÓ∏¶ ±Ÿ¡¢∞¯∞›«œ∑¡∞Ì «“∂ß, npc ƒ¡∆Æ∑—∑Ø¥¬ ∞¯∞› ∞°¥…«œ¥Ÿ∞Ì ∆«¡§.
	// ««∞›¥Á«œ¥¬ ≈¨∂Û¿Ãæ∆Æ¥¬ ∞¯∞› ∞°¥…«œ¡ˆ æ ¥Ÿ∞Ì ∆«¡§. ¿Ã∑ŒΩ· ««∞›µ«¥¬ ¿Ø¿˙∞° ¿ßƒ°∏¶ πŸ≤Ÿ¡ˆ æ ¥¬«— ∏ÛΩ∫≈Õ¥¬ ¡¶¿⁄∏Æø°º≠ π´«— «ÍπÊ¿ª ƒ°∞‘µ  (º÷±Ó∏ª æ«øÎ ∫“∞°∂Û∞Ì ª˝∞¢«œ¡ˆ∏∏ ∆€∫Ì∏Æº≈¿« ±Ÿº∫ø° ¡≥¿Ω)
	// bCoherentToPeer==true ¿œ∂ß ««æÓø°∞‘ ∫∏≥Ω ∞Õ∞˙ ∞∞¿∫ ∞™¿ª ªÁøÎ«‘..

	if ((pObj1 == NULL) || (pObj2 == NULL))
		return false;

	if ((pObj1->GetVisualMesh() == NULL) || (pObj2->GetVisualMesh() == NULL))
		return false;

	// ø°¥œ∏ﬁ¿Ãº« ∂ßπÆø° ∫Æ¿ª ∂’∞Ì µÈæÓ∞°¥¬ ∞ÊøÏµµ ¿÷æÓº≠..
	rvector p1 = pObj1->GetPosition() + rvector(0.f, 0.f, 100.f);
	rvector p2 = pObj2->GetPosition() + rvector(0.f, 0.f, 100.f);

	if (bCoherentToPeer)
	{
		p1.x = short(p1.x);
		p1.y = short(p1.y);
		p1.z = short(p1.z);
		p2.x = short(p2.x);
		p2.y = short(p2.y);
		p2.z = short(p2.z);
		// ø¿¬˜∑Œ ¿Œ«— πˆ±◊ ¿Á«ˆ ≈◊Ω∫∆Æ∏¶ Ω±∞‘ «œ±‚ ¿ß«ÿ 1¿« ¿⁄∏Æ±Ó¡ˆ ¿˝ªÁ«— πˆ¿¸
		/*p1.x = (short(p1.x * 0.1f)) * 10.f;
		p1.y = (short(p1.y * 0.1f)) * 10.f;
		p1.z = (short(p1.z * 0.1f)) * 10.f;
		p2.x = (short(p2.x * 0.1f)) * 10.f;
		p2.y = (short(p2.y * 0.1f)) * 10.f;
		p2.z = (short(p2.z * 0.1f)) * 10.f;*/
	}

	rvector dir = p2 - p1;

	Normalize(dir);

	ZPICKINFO pickinfo;

	if (Pick(pObj1, p1, dir, &pickinfo)) {//∫Æ¿Ã∂Û∏È
		if (pickinfo.bBspPicked)//∏ ¿Ã ∞…∏∞∞ÊøÏ
			return true;
	}

	return false;
}
//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑.....∫Ò±≥∏¶ ¿ß«ÿº≠
bool ZGame::IsWallBlocked(ZObject* pObj1, ZObject* pObj2, int& nDebugRegister/*¥‹º¯ ∫Ò±≥øÎ*/, bool bCoherentToPeer)
{	//bCoherentToPeerø° ¥Î«—∞Õ¿∫ ø¯∫ª CheckWall ¡÷ºÆ ¬¸∞Ì
	if ((pObj1 == NULL) || (pObj2 == NULL))
	{
		nDebugRegister = -10;	//ø™Ω√≥™ º˝¿⁄¥¬ ¿«πÃ∞° æ¯¥Ÿ..
		return false;
	}

	if ((pObj1->GetVisualMesh() == NULL) || (pObj2->GetVisualMesh() == NULL))
	{
		nDebugRegister = -10;
		return false;
	}

	// ø°¥œ∏ﬁ¿Ãº« ∂ßπÆø° ∫Æ¿ª ∂’∞Ì µÈæÓ∞°¥¬ ∞ÊøÏµµ ¿÷æÓº≠..
	rvector p1 = pObj1->GetPosition() + rvector(0.f, 0.f, 100.f);
	rvector p2 = pObj2->GetPosition() + rvector(0.f, 0.f, 100.f);

	if (bCoherentToPeer)
	{
		p1.x = short(p1.x);
		p1.y = short(p1.y);
		p1.z = short(p1.z);
		p2.x = short(p2.x);
		p2.y = short(p2.y);
		p2.z = short(p2.z);
		// ø¿¬˜∑Œ ¿Œ«— πˆ±◊ ¿Á«ˆ ≈◊Ω∫∆Æ∏¶ Ω±∞‘ «œ±‚ ¿ß«ÿ 1¿« ¿⁄∏Æ±Ó¡ˆ ¿˝ªÁ«— πˆ¿¸
		/*p1.x = (short(p1.x * 0.1f)) * 10.f;
		p1.y = (short(p1.y * 0.1f)) * 10.f;
		p1.z = (short(p1.z * 0.1f)) * 10.f;
		p2.x = (short(p2.x * 0.1f)) * 10.f;
		p2.y = (short(p2.y * 0.1f)) * 10.f;
		p2.z = (short(p2.z * 0.1f)) * 10.f;*/
	}

	rvector dir = p2 - p1;

	Normalize(dir);

	ZPICKINFO pickinfo;

	if (Pick(pObj1, p1, dir, &pickinfo)) {//∫Æ¿Ã∂Û∏È
		if (pickinfo.bBspPicked)//∏ ¿Ã ∞…∏∞∞ÊøÏ
		{
			nDebugRegister = FOR_DEBUG_REGISTER;
			return true;
		}
	}
	nDebugRegister = -10;
	return false;
}
void ZGame::OnExplosionGrenade(MUID uidOwner, rvector pos, float fDamage, float fRange, float fMinDamage, float fKnockBack, MMatchTeam nTeamID)
{
	ZObject* pTarget = NULL;

	float fDist, fDamageRange;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		pTarget = (*itor).second;
		//jintriple3 ????? ???????? ??y ???? ????.....
		bool bReturnValue = !pTarget || pTarget->IsDie();
		if (!pTarget || pTarget->IsDie())
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		fDist = Magnitude(pos - (pTarget->GetPosition() + rvector(0, 0, 80)));
		//jintriple3 ????? ???????? ?? ???? ????.....
		bReturnValue = fDist >= fRange;
		if (fDist >= fRange)
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		rvector dir = pos - (pTarget->GetPosition() + rvector(0, 0, 80));
		Normalize(dir);

		// ???? ???? ?æ??.
		if (GetDistance(pos, pTarget->GetPosition() + rvector(0, 0, 50), pTarget->GetPosition() + rvector(0, 0, 130)) < 50)
		{
			fDamageRange = 1.f;
		}
		else
		{
#define MAX_DMG_RANGE	50.f	// ??????u ?????? ??? ???????? ?? ??¥?
			//#define MIN_DMG			0.4f	// ??? ?? ???????? ??????.
			fDamageRange = 1.f - (1.f - fMinDamage) * (max(fDist - MAX_DMG_RANGE, 0) / (fRange - MAX_DMG_RANGE));
		}

		// ????z?? ?????? ??????? ???????.
		ZActor* pATarget = MDynamicCast(ZActor, pTarget);
		ZActorWithFSM* pFSMActor = MDynamicCast(ZActorWithFSM, pTarget);

		bool bPushSkip = false;

		if (pATarget)
		{
			bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
		}
		if (pFSMActor)
		{
			bPushSkip = pFSMActor->GetActorDef()->IsNeverBlasted();
		}

		if (!bPushSkip)
		{
			pTarget->AddVelocity(fKnockBack * 7.f * (fRange - fDist) * -dir);
		}
		else
		{
			ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
		}

		ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
		if (pOwnerCharacter)
		{
			CheckCombo(pOwnerCharacter, pTarget, !bPushSkip);
			CheckStylishAction(pOwnerCharacter);
		}

		float fActualDamage = fDamage * fDamageRange;
		float fRatio = ZItem::GetPiercingRatio(MWT_FRAGMENTATION, eq_parts_chest);//????z?? ???? ???????..
		pTarget->OnDamaged(pOwnerCharacter, pos, ZD_EXPLOSION, MWT_FRAGMENTATION, fActualDamage, fRatio);
		//					pTarget->OnDamagedGrenade( uidOwner, dir, fDamage * fDamageRange, nTeamID);

				/*if(pTarget && !pTarget->IsDie())
				{
					fDist = Magnitude(pos-(pTarget->GetPosition()+rvector(0,0,80)));
					if(fDist < fRange)
					{
						// ?? ????????? ?????? ????????~
		//				if( CheckWall(pos,pTarget) == false )
						{
							rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
							Normalize(dir);

							// ???? ???? ?æ??.
							if(GetDistance(pos,pTarget->GetPosition()+rvector(0,0,50),pTarget->GetPosition()+rvector(0,0,130))<50)
							{
								fDamageRange = 1.f;
							}
							else
							{
		#define MAX_DMG_RANGE	50.f	// ??????u ?????? ??? ???????? ?? ??¥?
		//#define MIN_DMG			0.4f	// ??? ?? ???????? ??????.
								fDamageRange = 1.f - (1.f-fMinDamage)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
							}

							// ????z?? ?????? ??????? ???????.
							ZActor* pATarget = MDynamicCast(ZActor,pTarget);

							bool bPushSkip = false;

							if(pATarget)
							{
								bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
							}

							if(bPushSkip==false)
							{
								pTarget->AddVelocity(fKnockBack*7.f*(fRange-fDist)*-dir);
							}
							else
							{
								ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
							}

							ZCharacterClass* pOwnerCharacter = g_pGame->m_CharacterManager.Find( uidOwner );
							if(pOwnerCharacter)
							{
								CheckCombo(pOwnerCharacter, pTarget,!bPushSkip);
								CheckStylishAction(pOwnerCharacter);
							}

							float fActualDamage = fDamage * fDamageRange;
							float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//????z?? ???? ???????..
							pTarget->OnDamaged(pOwnerCharacter,pos,ZD_EXPLOSION,MWT_FRAGMENTATION,fActualDamage,fRatio);
		//					pTarget->OnDamagedGrenade( uidOwner, dir, fDamage * fDamageRange, nTeamID);
						}
					}
				}*/
	}

#define SHOCK_RANGE		1500.f			// 10??????? ?????

	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	float fPower = (SHOCK_RANGE - Magnitude(pTargetCharacter->GetPosition() + rvector(0, 0, 50) - pos)) / SHOCK_RANGE;

	if (fPower > 0)
		ZGetGameInterface()->GetCamera()->Shock(fPower * 500.f, .5f, rvector(0.0f, 0.0f, -1.0f));

	GetWorld()->GetWaters()->CheckSpearing(pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f);

	//	static RealSoundEffectSource* pSES= ZApplication::GetSoundEngine()->GetSES("explosion");
	//	ZApplication::GetSoundEngine()->PlaySE(pSES,pos.x,pos.y,pos.z);
}
void ZGame::OnExplosionMagic(ZWeaponMagic* pWeapon, MUID uidOwner, rvector pos, float fMinDamage, float fKnockBack, MMatchTeam nTeamID, bool bSkipNpc)
{
	ZObject* pTarget = NULL;

	float fRange = 100.f * pWeapon->GetDesc()->fEffectArea;
	float fDist, fDamageRange;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		pTarget = (*itor).second;
		// ?Eß∞¯∞›¿?æ∆¥œ∂Û?E≈∏∞Ÿ∏∏ ∞ÀªÁ«œ∏Èµ»¥Ÿ.
		// µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?E
		bool bForDebugRegister = !pTarget || pTarget->IsDie() || pTarget->IsNPC();
		if (!pTarget || pTarget->IsDie() || pTarget->IsNPC())
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			continue;
		bForDebugRegister = !pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget() != pTarget->GetUID();
		if (!pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget() != pTarget->GetUID())
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			continue;

		ZObject* pOwner = ZGetObjectManager()->GetObject(uidOwner);

		bForDebugRegister = !pOwner || pOwner->IsDie();
		if (!pOwner || pOwner->IsDie())
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			return;

		// µŒ ƒ≥∏Ø≈ÕªÁ¿Ãø° ¿Âæ÷π∞¿Ã æ¯æ˚⁄ﬂ«—¥Ÿ~
		//				if( CheckWall(pos,pTarget) == false )
		{
			fDist = Magnitude(pos - (pTarget->GetPosition() + rvector(0, 0, 80)));
			if (pWeapon->GetDesc()->IsAreaTarget())	// ?Eß∞¯∞›¿Ã∏ÅE∞≈∏Æø° µ˚∏• µ•πÃ¡ˆ∏¶ ∞Ë?E—¥?
			{
				PROTECT_DEBUG_REGISTER(fDist > fRange) continue;	// ?Eß∏?π˛æ˚œµ¥Ÿ

				// ∏ˆø° ¡˜¡¢ ∏¬æ“¥Ÿ.
				if (GetDistance(pos, pTarget->GetPosition() + rvector(0, 0, 50), pTarget->GetPosition() + rvector(0, 0, 130)) < 50)
				{
					fDamageRange = 1.f;
				}
				else
				{
#define MAX_DMG_RANGE	50.f	// π›∞Ê¿Ã∏∏≈≠ ±˚›ˆ¥¬ √÷?Eµ•πÃ¡ˆ∏¶ ¥Ÿ ∏‘¥¬¥Ÿ

					fDamageRange = 1.f - (1.f - fMinDamage) * (max(fDist - MAX_DMG_RANGE, 0) / (fRange - MAX_DMG_RANGE));
				}
			}
			else
			{
				fDamageRange = 1.f;
			}

			// resist ∏¶ √º≈©«—¥Ÿ
			//µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?Eª¿‘.
			float fDamage = pWeapon->GetDesc()->nModDamage;
			bForDebugRegister = pWeapon && pWeapon->GetDesc()->CheckResist(pTarget, &fDamage);
			if (!(pWeapon->GetDesc()->CheckResist(pTarget, &fDamage)))
				PROTECT_DEBUG_REGISTER(bForDebugRegister)
				continue;

			ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
			if (pOwnerCharacter)
			{
				CheckCombo(pOwnerCharacter, pTarget, true);
				CheckStylishAction(pOwnerCharacter);
			}

			// ºˆ∑˘≈∫¿ª ∏¬¿∏?Eπ›µø¿∏∑Œ ∆¢æ˚œ™∞£¥Ÿ.
			rvector dir = pos - (pTarget->GetPosition() + rvector(0, 0, 80));
			Normalize(dir);
			pTarget->AddVelocity(fKnockBack * 7.f * (fRange - fDist) * -dir);

			float fActualDamage = fDamage * fDamageRange;
			float fRatio = ZItem::GetPiercingRatio(MWT_FRAGMENTATION, eq_parts_chest);//ºˆ∑˘≈∫?E∑Œƒœ ±∏∫–æ¯¥Ÿ..

			if (m_pMyCharacter && pTarget->GetUID() == m_pMyCharacter->GetUID())
				m_pMyCharacter->OnDamaged(pOwner, pos, ZD_MAGIC, MWT_FRAGMENTATION, fActualDamage, fRatio);
			else
				pTarget->OnDamaged(pOwner, pos, ZD_MAGIC, MWT_FRAGMENTATION, fActualDamage, fRatio);

			// resist ∏¶ √º≈©«—¥Ÿ
			/*			float fDamage = pWeapon->GetDesc()->nModDamage;
			if(pWeapon->GetDesc()->CheckResist(pTarget,&fDamage))
			{
			ZCharacter* pOwnerCharacter = g_pGame->m_CharacterManager.Find( uidOwner );
			if(pOwnerCharacter)
			{
			CheckCombo(pOwnerCharacter, pTarget,true);
			CheckStylishAction(pOwnerCharacter);
			}

			// ºˆ∑˘≈∫¿ª ∏¬¿∏?Eπ›µø¿∏∑Œ ∆¢æ˚œ™∞£¥Ÿ.
			rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
			Normalize(dir);
			pTarget->AddVelocity(fKnockBack*7.f*(fRange-fDist)*-dir);

			float fActualDamage = fDamage * fDamageRange;
			float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//ºˆ∑˘≈∫?E∑Œƒœ ±∏∫–æ¯¥Ÿ..
			pTarget->OnDamaged(pOwnerCharacter,pos,ZD_MAGIC,MWT_FRAGMENTATION,fActualDamage,fRatio);
			}
			else
			{
			// TODO: ¿˙«◊ø° º∫∞¯«ﬂ¿∏¥œ ¿Ã∆Â∆Æ∏¶ ∫∏ø©¡÷¿⁄.
			}*/
		}
	}

	/*
	#define SHOCK_RANGE		1500.f			// 10πÃ≈Õ±˚?E»ÁµÈ∏∞¥Ÿ

	ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	float fPower= (SHOCK_RANGE-Magnitude(pTargetCharacter->GetPosition()+rvector(0,0,50) - pos))/SHOCK_RANGE;

	if ((fPower>0) && (pWeapon->GetDesc()->bCameraShock))
	ZGetGameInterface()->GetCamera()->Shock(fPower*500.f, .5f, rvector(0.0f, 0.0f, -1.0f));
	*/

	if (pWeapon->GetDesc()->bCameraShock)
	{
		ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
		const float fDefaultPower = 500.0f;
		float fShockRange = pWeapon->GetDesc()->fCameraRange;
		float fDuration = pWeapon->GetDesc()->fCameraDuration;
		float fPower = (fShockRange - Magnitude(pTargetCharacter->GetPosition() + rvector(0, 0, 50) - pos)) / fShockRange;
		fPower *= pWeapon->GetDesc()->fCameraPower;

		if (fPower > 0)
		{
			ZGetGameInterface()->GetCamera()->Shock(fPower * fDefaultPower, fDuration, rvector(0.0f, 0.0f, -1.0f));
		}
	}

	GetWorld()->GetWaters()->CheckSpearing(pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f);
}

// ∏≈¡˜∑˘¿« π´±‚¿« µ•πÃ¡ˆ∏¶ ¡ÿ¥Ÿ
//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?Eª¿‘
void ZGame::OnExplosionMagicThrow(ZWeaponMagic* pWeapon, MUID uidOwner, rvector pos, float fMinDamage, float fKnockBack, MMatchTeam nTeamID, bool bSkipNpc, rvector from, rvector to)
{
	ZObject* pTarget = NULL;

	float fDist, fDamageRange;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		pTarget = (*itor).second;
		// ?Eß∞¯∞›¿?æ∆¥œ∂Û?E≈∏∞Ÿ∏∏ ∞ÀªÁ«œ∏Èµ»¥Ÿ.
		// µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?E
		bool bForDebugRegister = !pTarget || pTarget->IsDie() || pTarget->IsNPC();
		if (!pTarget || pTarget->IsDie() || pTarget->IsNPC())
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			continue;
		bForDebugRegister = !pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget() != pTarget->GetUID();
		if (!pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget() != pTarget->GetUID())
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			continue;

		ZObject* pOwner = ZGetObjectManager()->GetObject(uidOwner);

		bForDebugRegister = !pOwner || pOwner->IsDie();
		if (!pOwner || pOwner->IsDie())
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			return;

		fDist = GetDistance(pTarget->GetPosition() + rvector(0, 0, 80), from, to);

		if (fDist > pWeapon->GetDesc()->fColRadius + 100)
			continue;

		if (pWeapon->GetDesc()->IsAreaTarget())	// ?Eß∞¯∞›¿Ã∏ÅE∞≈∏Æø° µ˚∏• µ•πÃ¡ˆ∏¶ ∞Ë?E—¥?
		{
			if (fDist > pWeapon->GetDesc()->fColRadius + 80) // ƒ≥∏Ø≈Õ≈©±‚∏¶ 160 ¡§µµ∑Œ ¿‚?E±∏∑Œ ƒ°?Eπ›¡ˆ∏ß¿∫ 80 ¡§µµ∑Œ ∞Ë?E
			{
				fDamageRange = 0.1f;
			}
			else
			{
				fDamageRange = 1.f - 0.9f * fDist / (pWeapon->GetDesc()->fColRadius + 80);
			}
		}
		else
		{
			fDamageRange = 1.f;
		}

		// resist ∏¶ √º≈©«—¥Ÿ
		//µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?Eª¿‘.
		float fDamage = pWeapon->GetDesc()->nModDamage;
		bForDebugRegister = pWeapon && pWeapon->GetDesc()->CheckResist(pTarget, &fDamage);
		if (!(pWeapon->GetDesc()->CheckResist(pTarget, &fDamage)))
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			continue;

		ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
		if (pOwnerCharacter)
		{
			CheckCombo(pOwnerCharacter, pTarget, true);
			CheckStylishAction(pOwnerCharacter);
		}

		float fActualDamage = fDamage * fDamageRange;
		float fRatio = ZItem::GetPiercingRatio(MWT_FRAGMENTATION, eq_parts_chest);//ºˆ∑˘≈∫?E∑Œƒœ ±∏∫–æ¯¥Ÿ..

		if (m_pMyCharacter && pTarget->GetUID() == m_pMyCharacter->GetUID())
			m_pMyCharacter->OnDamaged(pOwner, pos, ZD_MAGIC, MWT_FRAGMENTATION, fActualDamage, fRatio);
		else
			pTarget->OnDamaged(pOwner, pos, ZD_MAGIC, MWT_FRAGMENTATION, fActualDamage, fRatio);
	}

	if (pWeapon->GetDesc()->bCameraShock)
	{
		ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
		const float fDefaultPower = 500.0f;
		float fShockRange = pWeapon->GetDesc()->fCameraRange;
		float fDuration = pWeapon->GetDesc()->fCameraDuration;
		float fPower = (fShockRange - Magnitude(pTargetCharacter->GetPosition() + rvector(0, 0, 50) - pos)) / fShockRange;
		fPower *= pWeapon->GetDesc()->fCameraPower;

		if (fPower > 0)
		{
			ZGetGameInterface()->GetCamera()->Shock(fPower * fDefaultPower, fDuration, rvector(0.0f, 0.0f, -1.0f));
		}
	}

	GetWorld()->GetWaters()->CheckSpearing(pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f);
}

//µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?Eª¿‘
void ZGame::OnExplosionMagicNonSplash(ZWeaponMagic* pWeapon, MUID uidOwner, MUID uidTarget, rvector pos, float fKnockBack)
{
	ZObject* pTarget = m_CharacterManager.Find(uidTarget);
	bool bForDebugRegister = pTarget == NULL || pTarget->IsNPC();
	if (pTarget == NULL || pTarget->IsNPC()) PROTECT_DEBUG_REGISTER(bForDebugRegister) return;

	bForDebugRegister = !pTarget || pTarget->IsDie();
	if (!pTarget || pTarget->IsDie())
		PROTECT_DEBUG_REGISTER(bForDebugRegister)
		return;

	ZObject* pOwner = ZGetObjectManager()->GetObject(uidOwner);

	bForDebugRegister = !pOwner || pOwner->IsDie();
	if (!pOwner || pOwner->IsDie())
		PROTECT_DEBUG_REGISTER(bForDebugRegister)
		return;

	// resist ∏¶ √º≈©«—¥Ÿ
	float fDamage = pWeapon->GetDesc()->nModDamage;
	bForDebugRegister = pWeapon && pWeapon->GetDesc()->CheckResist(pTarget, &fDamage);
	if (!pWeapon->GetDesc()->CheckResist(pTarget, &fDamage))
		PROTECT_DEBUG_REGISTER(bForDebugRegister)
		return;

	ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
	if (pOwnerCharacter)
	{
		CheckCombo(pOwnerCharacter, pTarget, true);
		CheckStylishAction(pOwnerCharacter);
	}
	// π›µø¿∏∑Œ ∆¢æ˚œ™∞£¥Ÿ.
	rvector dir = pos - (pTarget->GetPosition() + rvector(0, 0, 80));
	Normalize(dir);
	pTarget->AddVelocity(fKnockBack * 7.f * -dir);

	float fRatio = ZItem::GetPiercingRatio(MWT_FRAGMENTATION, eq_parts_chest);//ºˆ∑˘≈∫?E∑Œƒœ ±∏∫–æ¯¥Ÿ..
	if (m_pMyCharacter && pTarget->GetUID() == m_pMyCharacter->GetUID())
		m_pMyCharacter->OnDamaged(pOwner, pos, ZD_MAGIC, MWT_FRAGMENTATION, fDamage, fRatio);
	else
		pTarget->OnDamaged(pOwner, pos, ZD_MAGIC, MWT_FRAGMENTATION, fDamage, fRatio);

	/*	if(pTarget && !pTarget->IsDie()) {
			// resist ∏¶ √º≈©«—¥Ÿ
			float fDamage = pWeapon->GetDesc()->nModDamage;
			if(pWeapon->GetDesc()->CheckResist(pTarget,&fDamage))
			{
				ZCharacter* pOwnerCharacter = g_pGame->m_CharacterManager.Find( uidOwner );
				if(pOwnerCharacter) {
					CheckCombo(pOwnerCharacter, pTarget,true);
					CheckStylishAction(pOwnerCharacter);
				}

				// π›µø¿∏∑Œ ∆¢æ˚œ™∞£¥Ÿ.
				rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
				Normalize(dir);
				pTarget->AddVelocity(fKnockBack*7.f*-dir);

				float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//ºˆ∑˘≈∫?E∑Œƒœ ±∏∫–æ¯¥Ÿ..
				pTarget->OnDamaged(pOwnerCharacter,pos,ZD_MAGIC,MWT_FRAGMENTATION,fDamage,fRatio);
			} else {
				// TODO: ¿˙«◊ø° º∫∞¯«ﬂ¿∏¥œ ¿Ã∆Â∆Æ∏¶ ∫∏ø©¡÷¿⁄.
			}
		}*/
}

int ZGame::SelectSlashEffectMotion(ZCharacter* pCharacter)
{
	// Custom: Exploit fix (pCharacter->GetSelectItemDesc() == NULL crash)
	if (pCharacter == NULL || pCharacter->GetSelectItemDesc() == NULL) return SEM_None;

	// ≥≤≥‡∞° ∞∞æ∆¡≥¡ˆ∏∏ »§Ω√ ∂« πŸ≤?E∏∏£¥œ ≥ˆµ–¥Ÿ~~

//	MWT_DAGGER
//	MWT_DUAL_DAGGER
//	MWT_KATANA
//	MWT_GREAT_SWORD
//	MWT_DOUBLE_KATANA

	ZC_STATE_LOWER lower = pCharacter->m_AniState_Lower.Ref();

	int nAdd = 0;
	int ret = 0;

	MMatchWeaponType nType = pCharacter->GetSelectItemDesc()->m_nWeaponType.Ref();

	if (pCharacter->IsMan()) {
		if (lower == ZC_STATE_LOWER_ATTACK1) { nAdd = 0; }
		else if (lower == ZC_STATE_LOWER_ATTACK2) { nAdd = 1; }
		else if (lower == ZC_STATE_LOWER_ATTACK3) { nAdd = 2; }
		else if (lower == ZC_STATE_LOWER_ATTACK4) { nAdd = 3; }
		else if (lower == ZC_STATE_LOWER_ATTACK5) { nAdd = 4; }
		else if (lower == ZC_STATE_LOWER_UPPERCUT) { return SEM_ManUppercut; }

		if (nType == MWT_KATANA)		return SEM_ManSlash1 + nAdd;
		else if (nType == MWT_DOUBLE_KATANA)	return SEM_ManDoubleSlash1 + nAdd;
		else if (nType == MWT_GREAT_SWORD)	return SEM_ManGreatSwordSlash1 + nAdd;
		else if (nType == MWT_SPYCASE)		return SEM_ManSlash1 + nAdd;
	}
	else {
		if (lower == ZC_STATE_LOWER_ATTACK1) { nAdd = 0; }
		else if (lower == ZC_STATE_LOWER_ATTACK2) { nAdd = 1; }
		else if (lower == ZC_STATE_LOWER_ATTACK3) { nAdd = 2; }
		else if (lower == ZC_STATE_LOWER_ATTACK4) { nAdd = 3; }
		else if (lower == ZC_STATE_LOWER_ATTACK5) { nAdd = 4; }
		else if (lower == ZC_STATE_LOWER_UPPERCUT) { return SEM_WomanUppercut; }

		if (nType == MWT_KATANA)		return SEM_WomanSlash1 + nAdd;
		else if (nType == MWT_DOUBLE_KATANA)	return SEM_WomanDoubleSlash1 + nAdd;
		else if (nType == MWT_GREAT_SWORD)	return SEM_WomanGreatSwordSlash1 + nAdd;
		else if (nType == MWT_SPYCASE)		return SEM_WomanSlash1 + nAdd;
	}

	return SEM_None;
}

// shot ¿Ã ≥ π´ ƒøº≠ ∫–∏Æ..
void ZGame::OnPeerShot_Melee(const MUID& uidOwner, float fShotTime)
{
	// ??? ??? ??
	ZObject* pAttacker = m_ObjectManager.GetObject(uidOwner);
	float time = fShotTime;
	//jintriple3 ??? ???? ? ??? ??
	bool bReturnValue = !pAttacker;
	if (!pAttacker)
		PROTECT_DEBUG_REGISTER(bReturnValue)
		return;

	// Melee ??? ?E???? ??
	ZItem* pItem = pAttacker->GetItems()->GetItem(MMCIP_MELEE);
	//jintriple3    ??? ????? ? ??E..
	bReturnValue = !pItem;
	if (!pItem)
		PROTECT_DEBUG_REGISTER(bReturnValue)
		return;

	MMatchItemDesc* pItemDesc = pItem->GetDesc();
	//jintriple3    ??Eassert? ??? ?...?? ??E.
	bReturnValue = !pItemDesc;
	if (!pItemDesc)
		PROTECT_DEBUG_REGISTER(bReturnValue)
	{
		_ASSERT(FALSE);
		return;
	}

	// ?? ??? ?E?????.
	float fRange = pItemDesc->m_nRange.Ref();
	if (fRange == 0)
		fRange = 150.f;

	// ?? ??? ??? ???.
	float fAngle = cosf(ToRadian(pItemDesc->m_nAngle.Ref() * 0.5f));

	// NPC? ??E??? ?? ?E?????.
	if (pAttacker->IsNPC())
	{
		fRange += fRange * 0.2f;            // ??? 20% ??
		fAngle -= fAngle * 0.1f;            // ??? 10% ??
	}

	// ?? ??? ?E???? ??
	fShotTime = GetTime();

	// ???E??
	//ZGetSoundEngine()->PlaySound( "blade_swing", pAttacker->GetPosition(), uidOwner==ZGetGameInterface()->GetCombatInterface()->GetTargetUID());

	// ???? ??? ??? ?E????
	rvector AttackerPos = pAttacker->GetPosition();
	rvector AttackerNorPos = AttackerPos;
	AttackerNorPos.z = 0;

	rvector AttackerDir = pAttacker->m_Direction;
	rvector AttackerNorDir = AttackerDir;
	AttackerNorDir.z = 0;
	Normalize(AttackerNorDir);

	// ??E? ???? ??? ?????
	int cm = 0;
	ZCharacter* pOwnerCharacter = (ZCharacter*)m_CharacterManager.Find(uidOwner);
	if (pOwnerCharacter)
		cm = SelectSlashEffectMotion(pOwnerCharacter);

	// ???E??
	//if ( !pAttacker->IsNPC())
	//{
	//  ZGetSoundEngine()->PlaySound( "blade_swing", pAttacker->m_Position, uidOwner==ZGetGameInterface()->GetCombatInterface()->GetTargetUID());
	//}

	// ???E??
	bool bPlayer = false;
	rvector Pos = pAttacker->GetPosition();
	if (pAttacker == m_pMyCharacter)
	{
		Pos = RCameraPosition;
		bPlayer = true;
	}

	// default
	ZApplication::GetSoundEngine()->PlaySoundElseDefault("blade_swing", "blade_swing", rvector(Pos.x, Pos.y, Pos.z), bPlayer);

	// ??? ??? ?? ???E????.
	bool bHit = false;

	// ??? ?E???? ????.
	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		// ??? ?E???? ???.
		ZObject* pVictim = (*itor).second;

		// ??? ?? ???E?? ???? ????.
		//jintriple3    ??? ???? ? ??E...
		ZModule_HPAP* pModule = (ZModule_HPAP*)pVictim->GetModule(ZMID_HPAP);
		if (pVictim->IsDie())
			PROTECT_DEBUG_REGISTER(pModule->GetHP() == 0)
			continue;

		// ??? ??? ????E?? ???? ????.
		//jintriple3    ??? ???? ? ??E...
		bReturnValue = pAttacker == pVictim;
		if (pAttacker == pVictim)
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		// ?? ??? ??E????E?? ???? ????.
		//jintriple3    ??? ???? ? ??E...
		bool bRetVal = CanAttack(pAttacker, pVictim);
		if (!bRetVal)
			PROTECT_DEBUG_REGISTER(!CanAttack_DebugRegister(pAttacker, pVictim))
			continue;

		// ??? ??? ???.
		rvector VictimPos, VictimDir, VictimNorDir;
		rvector ZeroVector = rvector(0.0f, 0.0f, 0.0f);
		VictimPos = ZeroVector;
		bRetVal = pVictim->GetHistory(&VictimPos, &VictimDir, fShotTime);
		//jintriple3    ??? ???? ? ??E...
		if (!bRetVal)
			PROTECT_DEBUG_REGISTER(VictimPos == ZeroVector)
			continue;

		// NPC? ?? ??? ??? ?...
		if (!pAttacker->IsNPC())
		{
			// ???? ???? ??? ???.
			rvector swordPos = AttackerPos + (AttackerNorDir * 100.f);
			swordPos.z += pAttacker->GetCollHeight() * .5f;
			float fDist = GetDistanceLineSegment(swordPos, VictimPos, VictimPos + rvector(0, 0, pVictim->GetCollHeight()));

			// ???? ???? ??? ?? ?? ???? ??E?? ???? ????.
			//jintriple3    ??? ???? ?? ??E...
			bReturnValue = fDist > fRange;
			if (fDist > fRange)
				PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

			// ???? ?E???? ??? ??? ???
			rvector VictimNorDir = VictimPos - (AttackerPos - (AttackerNorDir * 50.f));
			Normalize(VictimNorDir);

			// ??? ?? ??? ?? ???E?? ???? ????.
			//jintriple3    ??? ???? ? ??E...
			float fDot = D3DXVec3Dot(&AttackerNorDir, &VictimNorDir);
			bReturnValue = fDot < 0.5f;
			if (fDot < 0.5f)
				PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;
		}

		// ??? ?? NPC? ??? ?...  (??? ???? ??E?? ??? ??? ??? ???? ????)
		// (??????? ??? ?? ZActorAction?? ????? ??)
		else
		{
			// ???? ?E???? ??? ??E???
			rvector VictimNorPos = VictimPos;
			VictimNorPos.z = 0;

			rvector VictimNorDir = VictimPos - (AttackerPos - (AttackerNorDir * 50.f));
			VictimNorDir.z = 0;
			Normalize(VictimNorDir);

			// ???? ???? x,y ??E?? ??? ?? ??, ??? ?? ?? ???? ??E?? ???? ????.
			//jintriple3    ??? ???? ? ??E...
			float fDist = Magnitude(AttackerNorPos - VictimNorPos);
			bReturnValue = fDist > fRange;
			if (fDist > fRange)
				PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

			// ??? ?? ??? ?? ???E?? ???? ????.
			float fDot = D3DXVec3Dot(&AttackerNorDir, &VictimNorDir);
			//jintriple3    ??? ???? ? ??E...
			bReturnValue = fDot < fAngle;
			if (fDot < fAngle)
				PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

			// ??? ??E?? ?E???? ?? ?? ??? ???E????.
			//jintriple3    ??? ???? ? ??E...
			int nDebugRegister = 0;
			if (!InRanged(pAttacker, pVictim, nDebugRegister))
				PROTECT_DEBUG_REGISTER(nDebugRegister != FOR_DEBUG_REGISTER)
				continue;
		}

		// ???? ?? ??? ?? ??E???E?? ???? ????.
		int nDebugRegister = 0;
		bRetVal = CheckWall(pAttacker, pVictim, nDebugRegister, true);
		//jintriple3 ??? ???? ? ??E...
		if (bRetVal)
			PROTECT_DEBUG_REGISTER(nDebugRegister == FOR_DEBUG_REGISTER)
			continue;

		// ??E????? ??E??? ???E??E??E
		//jintriple3 ??? ???? ? ??E..
		bRetVal = pVictim->IsGuard() && (DotProduct(pVictim->m_Direction, AttackerNorDir) < 0);
		if (pVictim->IsGuard() && (DotProduct(pVictim->m_Direction, AttackerNorDir) < 0))
		{
			PROTECT_DEBUG_REGISTER(bRetVal)
			{
				rvector pos = pVictim->GetPosition();
				pos.z += 120.f;

				// ??E??? ??
				ZGetEffectManager()->AddSwordDefenceEffect(pos + (pVictim->m_Direction * 50.f), pVictim->m_Direction);
				pVictim->OnMeleeGuardSuccess();
				return;
			}
		}

		// ?E?ÅE???? ??? ????E..
		rvector pos = pVictim->GetPosition();
		pos.z += 130.f;
		pos -= AttackerDir * 50.f;

		// ???? ????.
		ZGetEffectManager()->AddBloodEffect(pos, -VictimNorDir);
		ZGetEffectManager()->AddSlashEffect(pos, -VictimNorDir, cm);

#ifdef _CALCDMG
		float fActualDamage = CalcActualDamage(pAttacker, pVictim, (float)pItemDesc->m_nDamage.Ref(), pItem->GetDesc()->m_nWeaponType.Ref(), pItem);
#else
		float fActualDamage = CalcActualDamage(pAttacker, pVictim, (float)pItemDesc->m_nDamage.Ref(), pItem->GetDesc()->m_nWeaponType.Ref());
#endif
		float fRatio = pItem->GetPiercingRatio(pItemDesc->m_nWeaponType.Ref(), eq_parts_chest);
		pVictim->OnDamaged(pAttacker, pAttacker->GetPosition(), ZD_MELEE, pItemDesc->m_nWeaponType.Ref(),
			fActualDamage, fRatio, cm);

		ZActor* pATarget = MDynamicCast(ZActor, pVictim);
		ZActorWithFSM* pFSMActor = MDynamicCast(ZActorWithFSM, pVictim);

		ZPOSTDMGTAKEN(MCommandParameterInt(fActualDamage), MCommandParameterInt(0), pAttacker->GetUID()); // Custom: Fix Damage Count Melee
		// ?? ??? ?? ??
		bool bPushSkip = false;
		if (pATarget)
		{
			bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
		}
		if (pFSMActor)
		{
			bPushSkip = pFSMActor->GetActorDef()->IsNeverBlasted();
		}

		float fKnockbackForce = pItem->GetKnockbackForce();
		if (bPushSkip)
		{
			ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
			fKnockbackForce = 1.0f;
		}

		pVictim->OnKnockback(pAttacker->m_Direction, fKnockbackForce);

		// ??? ???E??
		ZGetSoundEngine()->PlaySoundBladeDamage(pItemDesc, pos);

		// ?? ????E?? ??? ???? ??.
		if (pAttacker == m_pMyCharacter)
		{
			CheckCombo(m_pMyCharacter, pVictim, !bPushSkip);
			CheckStylishAction(m_pMyCharacter);
		}

		// ??? ???.
		bHit = true;
	}

	// ???? ???E????E..
	// test ???? ??E?????E???..
	if (!bHit)
	{
		rvector vPos = pAttacker->GetPosition();
		rvector vDir = AttackerNorDir;

		vPos.z += 130.f;

		RBSPPICKINFO bpi;

		if (GetWorld()->GetBsp()->Pick(vPos, vDir, &bpi)) {
			float fDist = Magnitude(vPos - bpi.PickPos);

			if (fDist < fRange) {
				rplane r = bpi.pInfo->plane;
				rvector vWallDir = rvector(r.a, r.b, r.c);
				Normalize(vWallDir);

				ZGetEffectManager()->AddSlashEffectWall(bpi.PickPos - (vDir * 5.f), vWallDir, cm);

				rvector pos = bpi.PickPos;

				ZGetSoundEngine()->PlaySoundBladeConcrete(pItemDesc, pos);
			}
		}
	}

	return;
}

bool ZGame::InRanged(ZObject* pAttacker, ZObject* pVictim)
{
	//### ¿Ã «‘ºˆ∏¶ ºˆ¡§«œ?E∂»∞∞¿Ã æ∆∑°¿« InRanged()ø°µµ ¿˚?E?¡÷æ˚⁄ﬂ «’¥œ¥Ÿ. ###

	// ∞¯∞›¿⁄øÕ ≈∏∞Ÿ Ω«∏∞¥ı¿« πŸ¥⁄?E¿≠∏È¿« ¿ßƒ°∏¶ ±∏«—¥Ÿ.
	float fBotAtk = pAttacker->GetPosition().z;
	//	float fTopAtk	= fBotAtk + pAttacker->GetCollHeight();
	float fTopAtk = fBotAtk + pAttacker->GetCollHeight() + (pAttacker->GetCollHeight() * 0.5f);

	float fBotVct = pVictim->GetPosition().z;
	float fTopVct = fBotVct + pVictim->GetCollHeight();

	// ≈∏ƒœ¿« ∏« æ∆∑°∞° ∞¯∞›¿⁄∫∏¥Ÿ ¿ßø° ¿÷¿∏?Eøµø™ π€¿Ã¥Ÿ.
	if (fBotVct > fTopAtk)
		return false;

	// ≈∏ƒœ¿« ∏« ¿ß∞° ∞¯∞›¿⁄∫∏¥Ÿ æ∆∑°ø° ¿÷¿∏?Eøµø™ π€¿Ã¥Ÿ.
	else if (fTopVct < fBotAtk)
		return false;

	// ±◊ ø‹ø°¥¬ ?E?øµø™ æ»¿Ã¥Ÿ.
	return true;
}

bool ZGame::InRanged(ZObject* pAttacker, ZObject* pVictim, int& nDebugRegister/*µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ¡ˆ∏¶ ¿ß«— ∫Ø?E*/)
{
	// ∞¯∞›¿⁄øÕ ≈∏∞Ÿ Ω«∏∞¥ı¿« πŸ¥⁄?E¿≠∏È¿« ¿ßƒ°∏¶ ±∏«—¥Ÿ.
	float fBotAtk = pAttacker->GetPosition().z;
	//	float fTopAtk	= fBotAtk + pAttacker->GetCollHeight();
	float fTopAtk = fBotAtk + pAttacker->GetCollHeight() + (pAttacker->GetCollHeight() * 0.5f);

	float fBotVct = pVictim->GetPosition().z;
	float fTopVct = fBotVct + pVictim->GetCollHeight();

	// ≈∏ƒœ¿« ∏« æ∆∑°∞° ∞¯∞›¿⁄∫∏¥Ÿ ¿ßø° ¿÷¿∏?Eøµø™ π€¿Ã¥Ÿ.
	if (fBotVct > fTopAtk)
	{
		nDebugRegister = -10;//º˝¿⁄¥¬ ¿«πÃ∞° æ¯¥Ÿ..¥‹?E∫Ò±≥∏¶ ¿ß«— ?E..
		return false;
	}

	// ≈∏ƒœ¿« ∏« ¿ß∞° ∞¯∞›¿⁄∫∏¥Ÿ æ∆∑°ø° ¿÷¿∏?Eøµø™ π€¿Ã¥Ÿ.
	else if (fTopVct < fBotAtk)
	{
		nDebugRegister = -10;
		return false;
	}

	// ±◊ ø‹ø°¥¬ ?E?øµø™ æ»¿Ã¥Ÿ.
	nDebugRegister = FOR_DEBUG_REGISTER;
	return true;
}

//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?Eª¿‘
void ZGame::OnPeerShot_Range_Damaged(ZObject* pOwner, float fShotTime, const rvector& pos, const rvector& to, ZPICKINFO pickinfo, DWORD dwPickPassFlag, rvector& v1, rvector& v2, ZItem* pItem, rvector& BulletMarkNormal, bool& bBulletMark, ZTargetType& nTargetType)
{
	MMatchItemDesc* pDesc = pItem->GetDesc();
	bool bReturnValue = !pDesc;
	if (!pDesc) PROTECT_DEBUG_REGISTER(bReturnValue) { _ASSERT(FALSE); return; }

	rvector dir = to - pos;

	bReturnValue = !(ZGetGame()->PickHistory(pOwner, fShotTime, pos, to, &pickinfo, dwPickPassFlag));
	if (!(ZGetGame()->PickHistory(pOwner, fShotTime, pos, to, &pickinfo, dwPickPassFlag)))
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			v1 = pos;
			v2 = pos + dir * 10000.f;
			nTargetType = ZTT_NOTHING;
			return;
		}
	}

	// ∂´ªß -bird
	//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ?Eƒ⁄?E
	bReturnValue = (!pickinfo.pObject) && (!pickinfo.bBspPicked);
	if (pickinfo.bBspPicked)
	{
		PROTECT_DEBUG_REGISTER(pickinfo.nBspPicked_DebugRegister == FOR_DEBUG_REGISTER)
		{
			nTargetType = ZTT_OBJECT;

			v1 = pos;
			v2 = pickinfo.bpi.PickPos;

			// √—≈∫ »Á?E
			BulletMarkNormal.x = pickinfo.bpi.pInfo->plane.a;
			BulletMarkNormal.y = pickinfo.bpi.pInfo->plane.b;
			BulletMarkNormal.z = pickinfo.bpi.pInfo->plane.c;
			Normalize(BulletMarkNormal);
			bBulletMark = true;
			return;
		}
	}
	else if ((!pickinfo.pObject) && (!pickinfo.bBspPicked))
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			_ASSERT(false);
			return;
		}
	}
	//¿ßø°±˚›ˆ¥¬ ∞À?E¥‹?E..

	ZObject* pObject = pickinfo.pObject;
	bool bGuard = pObject->IsGuard() && (pickinfo.info.parts != eq_parts_legs) &&     // ¥Ÿ∏Æ¥¬ ∏∑¿ªºˆæ¯¥Ÿ
		DotProduct(dir, pObject->GetDirection()) < 0;

	if (pObject->IsGuard() && (pickinfo.info.parts != eq_parts_legs) &&        // ¥Ÿ∏Æ¥¬ ∏∑¿ªºˆæ¯¥Ÿ
		DotProduct(dir, pObject->GetDirection()) < 0) //ø©±‚µµ..
	{
		PROTECT_DEBUG_REGISTER(bGuard)
		{
			nTargetType = ZTT_CHARACTER_GUARD;
			// ∏∑æ“¥Ÿ
			rvector t_pos = pObject->GetPosition();
			t_pos.z += 100.f;
			ZGetEffectManager()->AddSwordDefenceEffect(t_pos + (-dir * 50.f), -dir);
			pObject->OnGuardSuccess();
			v1 = pos;
			v2 = pickinfo.info.vOut;
			return;
		}
	}

	nTargetType = ZTT_CHARACTER;

	ZActor* pATarget = MDynamicCast(ZActor, pickinfo.pObject);
	ZActorWithFSM* pFSMActor = MDynamicCast(ZActorWithFSM, pickinfo.pObject);
	bool bPushSkip = false;

	if (pATarget)
	{
		bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
	}
	if (pFSMActor)
	{
		bPushSkip = pFSMActor->GetActorDef()->IsNeverBlasted();
	}
	
	float fKnockbackForce = pItem->GetKnockbackForce();

	if (bPushSkip)
	{
		rvector vPos = pOwner->GetPosition() + (pickinfo.pObject->GetPosition() - pOwner->GetPosition()) * 0.1f;
		ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met", vPos);
		fKnockbackForce = 1.0f;
	}

	pickinfo.pObject->OnKnockback(pOwner->m_Direction, fKnockbackForce);
#ifdef _CALCDMG
	float fActualDamage = CalcActualDamage(pOwner, pickinfo.pObject, (float)pDesc->m_nDamage.Ref(), pItem->GetDesc()->m_nWeaponType.Ref(), pItem);
#else
	float fActualDamage = CalcActualDamage(pOwner, pickinfo.pObject, (float)pDesc->m_nDamage.Ref(), pItem->GetDesc()->m_nWeaponType.Ref());
#endif
	float fRatio = pItem->GetPiercingRatio(pDesc->m_nWeaponType.Ref(), pickinfo.info.parts);
	ZDAMAGETYPE dt = (pickinfo.info.parts == eq_parts_head) ? ZD_BULLET_HEADSHOT : ZD_BULLET;
#ifdef _BOXLEAD
	if (pickinfo.pObject->IsNPC() == true || ZGetGameClient()->GetMatchStageSetting()->IsLead() == true || (ZGetChannelRuleMgr()->GetCurrentRule() && ZGetChannelRuleMgr()->GetCurrentRule()->GetID() == MCHANNEL_RULE_LEAD))
	{
		pickinfo.pObject->OnDamaged(pOwner, pOwner->GetPosition(), dt, pDesc->m_nWeaponType.Ref(), fActualDamage, fRatio);
	}
	else
	{
#ifdef _ANTILEADNEW
		((ZCharacter*)(pickinfo.pObject))->OnDamagedAPlayer(pOwner, pOwner->GetPosition(), dt, pDesc->m_nWeaponType.Ref(), fActualDamage, fRatio);
#endif
	}
#endif

	if (pOwner == m_pMyCharacter)
	{
		CheckCombo(m_pMyCharacter, pickinfo.pObject, !bPushSkip);
		CheckStylishAction(m_pMyCharacter);
#ifdef _HITSCOUNT
		if (pOwner == m_pMyCharacter)
			ZGetGame()->m_pMyCharacter->m_nHits += 1;
#endif
	}

	v1 = pos;
	v2 = pickinfo.info.vOut;
}
void ZGame::OnPeerShot_Range(const MMatchCharItemParts sel_type, const MUID& uidOwner, float fShotTime, const rvector& pos, const rvector& to)
{
	ZObject* pOwner = m_ObjectManager.GetObject(uidOwner);
	if (!pOwner) return;

	ZItem* pItem = pOwner->GetItems()->GetItem(sel_type);
	if (!pItem) return;
	MMatchItemDesc* pDesc = pItem->GetDesc();
	if (!pDesc) { _ASSERT(FALSE); return; }

	rvector dir = to - pos;

	Normalize(dir);

	rvector v1, v2;
	rvector BulletMarkNormal;
	bool bBulletMark = false;
	ZTargetType nTargetType = ZTT_OBJECT;

	// ZCharacter* pCharacter = NULL;

	ZPICKINFO pickinfo;

	memset(&pickinfo, 0, sizeof(ZPICKINFO));

	// √—æÀ¿∫ ∑Œƒœ¿Ã ?E˙«œ¥¬∞˜µ??E˙«—¥?
	const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;

	// Ω˚–¬ ƒ≥∏Ø≈Õ »ÁµÈ?E¡÷?E.
	pOwner->Tremble(8.f, 50, 30);

	/*
		if(pOwner->m_pVMesh)
		{
			float fMaxValue = 8.f;// »Á?E∞≠µµ +- ∞°¥…

			RFrameTime* ft = &pOwner->m_pVMesh->m_FrameTime;
			if(ft && !ft->m_bActive)
				ft->Start(fMaxValue,50,30);// ∞≠µµ , √÷?E√∞?, ∫π±ÕΩ√∞£...
		}
	*/
	//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «Ÿ πÊ¡ˆ∏¶ ¿ß«ÿ «‘ºˆ∑Œ ª∞¥Ÿ..±Ÿµ• ¿Œ¿⁄∞° ≥—?E∏πæ∆..§Ã§Ã
	OnPeerShot_Range_Damaged(pOwner, fShotTime, pos, to, pickinfo, dwPickPassFlag, v1, v2, pItem, BulletMarkNormal, bBulletMark, nTargetType);
	/*
	if(g_pGame->PickHistory(pOwner,fShotTime,pos,to,&pickinfo,dwPickPassFlag))
	{
		// ∂´ªß -bird

		if(pickinfo.pObject)
		{
			ZObject *pObject = pickinfo.pObject;
			bool bGuard = pObject->IsGuard() && (pickinfo.info.parts!=eq_parts_legs) &&		// ¥Ÿ∏Æ¥¬ ∏∑¿ªºˆæ¯¥Ÿ
							DotProduct(dir,pObject->GetDirection())<0;

			if(bGuard)
			{
				nTargetType = ZTT_CHARACTER_GUARD;
				// ∏∑æ“¥Ÿ
				rvector t_pos = pObject->GetPosition();
				t_pos.z += 100.f;
				ZGetEffectManager()->AddSwordDefenceEffect(t_pos+(-dir*50.f),-dir);
				pObject->OnGuardSuccess();
			}
			else
			{
				nTargetType = ZTT_CHARACTER;

				ZActor* pATarget = MDynamicCast(ZActor,pickinfo.pObject);

				bool bPushSkip = false;

				if(pATarget)
				{
					bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
				}

				float fKnockbackForce = pItem->GetKnockbackForce();

				if(bPushSkip)
				{
//					ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
					rvector vPos = pOwner->GetPosition() + (pickinfo.pObject->GetPosition() - pOwner->GetPosition()) * 0.1f;
					ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met", vPos );
					fKnockbackForce = 1.0f;
				}

				pickinfo.pObject->OnKnockback( pOwner->m_Direction, fKnockbackForce );

				float fActualDamage = CalcActualDamage(pOwner, pickinfo.pObject, (float)pDesc->m_nDamage);
				float fRatio = pItem->GetPiercingRatio( pDesc->m_nWeaponType, pickinfo.info.parts );
				ZDAMAGETYPE dt = (pickinfo.info.parts==eq_parts_head) ? ZD_BULLET_HEADSHOT : ZD_BULLET;
				//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «Ÿ¿∏∑Œ∫Œ≈Õ æ»?Eœ∞?∫∏»£«ÿæﬂ µ… ∫Œ∫–..
				pickinfo.pObject->OnDamaged(pOwner, pOwner->GetPosition(), dt, pDesc->m_nWeaponType, fActualDamage, fRatio );

				if(pOwner == m_pMyCharacter)
				{
					CheckCombo(m_pMyCharacter,pickinfo.pObject,!bPushSkip);
					CheckStylishAction(m_pMyCharacter);
				}
			}

			v1 = pos;
			v2 = pickinfo.info.vOut;
		}
		else if(pickinfo.bBspPicked)
		{
			nTargetType = ZTT_OBJECT;

			v1 = pos;
			v2 = pickinfo.bpi.PickPos;

			// √—≈∫ »Á?E
			BulletMarkNormal.x = pickinfo.bpi.pInfo->plane.a;
			BulletMarkNormal.y = pickinfo.bpi.pInfo->plane.b;
			BulletMarkNormal.z = pickinfo.bpi.pInfo->plane.c;
			Normalize(BulletMarkNormal);
			bBulletMark = true;
		}
		else
		{
			_ASSERT(false);
			return;
		}
	}

	else
	{
		v1 = pos;
		v2 = pos+dir*10000.f;
		nTargetType	= ZTT_NOTHING;
	}*/

	bool bPlayer = false;
	//bool b3D = (pOwnerCharacter!=m_pMyCharacter);	// ¿⁄±‚∞° ≥ª¥¬ ªÁø˚—Â¥¬ 2D∑Œ √‚∑¬«—¥Ÿ.
	//rvector Pos = pOwnerCharacter->GetPosition();
	rvector Pos = v1;
	if (pOwner == m_pMyCharacter)
	{
		Pos = RCameraPosition;
		bPlayer = true;
	}

	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	ZApplication::GetSoundEngine()->PlaySEFire(pDesc, Pos.x, Pos.y, Pos.z, bPlayer);
	//if(nTargetType == ZTT_OBJECT) { ZApplication::GetSoundEngine()->PlaySERicochet(v2.x, v2.y, v2.z); }
#define SOUND_CULL_DISTANCE 1500.0F
	if (D3DXVec3LengthSq(&(v2 - pTargetCharacter->GetPosition())) < (SOUND_CULL_DISTANCE * SOUND_CULL_DISTANCE))
	{
		if (nTargetType == ZTT_OBJECT) {
			ZGetSoundEngine()->PlaySEHitObject(v2.x, v2.y, v2.z, pickinfo.bpi);
		}

		if (nTargetType == ZTT_CHARACTER) {
			ZGetSoundEngine()->PlaySEHitBody(v2.x, v2.y, v2.z);
		}
	}

	//// ∫∏¿Ã?Eæ ¿∏?E¿Ã∆Â∆Æ∏¶ ±◊∏±« ø‰¥¬ æ¯¥Ÿ - ¡§»Æ«— ƒ√∏µ¿ª ø‰∏¡.. by bird
	//if(!pOwner->IsRendered()) return;

	// Ω˚–¬?Eπ›?E100cm ∞° »≠∏Èø° µÈæ˚Ã°¥¬?E√º≈©«—¥Ÿ
	bool bDrawFireEffects = isInViewFrustum(v1, 100.f, RGetViewFrustum());

	if (!isInViewFrustum(v1, v2, RGetViewFrustum()) // Ω˚–¬∞˜ø°º≠ ∏¬¥¬∞˜¿« ∂Û¿Œ¿Ã ∫∏¿Ã¥¬?E.
		&& !bDrawFireEffects) return;					// Ω˚–¬∞˜ø°º≠µµ ±◊∏±∞‘ æ¯¥¬?E.

	bool bDrawTargetEffects = isInViewFrustum(v2, 100.f, RGetViewFrustum());

	/////////////////////// ¿Ã»ƒ¥¬ ¿Ã∆Â∆Æ √ﬂ∞°

	// π∞∆¢¥¬ ¿Ã∆Â∆Æ √º≈©
	GetWorld()->GetWaters()->CheckSpearing(v1, v2, 250, 0.3);

	// TODO: NPC ¿« √—±∏¿ßƒ° ¿Œ≈Õ∆‰¿ÃΩ∫∞° »Æ¡§µ«?E∏∂?E√ﬂ∞°«œ¿⁄.
//	ZCharacter *pOwnerCharacter = m_CharacterManager.Find(uidOwner);

	ZCharacterObject* pCOwnerObject = MDynamicCast(ZCharacterObject, pOwner);

	if (pCOwnerObject)
	{
		// √—±∏ »≠ø∞¿Ã∆Â∆Æ
		rvector pdir = v2 - v1;
		Normalize(pdir);

		int size = 3;

		rvector v[6];

		//		size = GetWeapondummyPos(pOwnerCharacter,v);
		if (pCOwnerObject->IsRendered())
			size = pCOwnerObject->GetWeapondummyPos(v);
		else
		{
			size = 6;
			v[0] = v[1] = v[2] = v1;
			v[3] = v[4] = v[5] = v[0];
		}

		MMatchWeaponType wtype = pDesc->m_nWeaponType.Ref();
		bool bSlugOutput = pDesc->m_bSlugOutput; // ≈∫««¿˚?Etrue, false)

		// Effect
		if (bBulletMark == false) BulletMarkNormal = -pdir;

		ZGetEffectManager()->AddShotEffect(v, size, v2, BulletMarkNormal, nTargetType, wtype, bSlugOutput, pCOwnerObject, bDrawFireEffects, bDrawTargetEffects);

		// √— ΩÚ∂ß ∂Û¿Ã∆Æ √ﬂ∞°
		ZCharacterObject* pChar;

		if (ZGetConfiguration()->GetVideo()->bDynamicLight && pCOwnerObject != NULL)
		{
			pChar = pCOwnerObject;

			if (pChar->m_bDynamicLight)
			{
				pChar->m_vLightColor = g_CharLightList[GUN].vLightColor;
				pChar->m_fLightLife = g_CharLightList[GUN].fLife;
			}
			else
			{
				pChar->m_bDynamicLight = true;
				pChar->m_vLightColor = g_CharLightList[GUN].vLightColor;
				pChar->m_vLightColor.x = 1.0f;
				pChar->m_iDLightType = GUN;
				pChar->m_fLightLife = g_CharLightList[GUN].fLife;
			}
		}
	}

	// ?Eﬂø??E¿˚?E		   p
	GetWorld()->GetFlags()->CheckSpearing(v1, v2, BULLET_SPEAR_EMBLEM_POWER);
	if (Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->AddLightSource(v1, 2.0f, 75);
}

void ZGame::OnPeerShot_Shotgun(ZItem* pItem, ZCharacter* pOwnerCharacter, float fShotTime, const rvector& pos, const rvector& to)
{
	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if (!pTargetCharacter) return;

	MMatchItemDesc* pDesc = pItem->GetDesc();
	if (!pDesc) { _ASSERT(false); return; }

#define SHOTGUN_BULLET_COUNT    11
#define SHOTGUN_DIFFUSE_RANGE	0.1f 

	if (pOwnerCharacter == NULL) return;

	int* seed = (int*)&fShotTime;
	srand(*seed);

	bool bHitGuard = false, bHitBody = false, bHitGround = false, bHitEnemy = false;
	rvector GuardPos, BodyPos, GroundPos;
	bool waterSound = false;

	rvector v1, v2;
	rvector dir;

	rvector origdir = to - pos;
	Normalize(origdir);

	int nHitCount = 0;

#ifdef _ANTILEADNEW
	vector<MTD_AntiLeadJork*> vShots;
#endif

	for (int i = 0; i < SHOTGUN_BULLET_COUNT; i++)
	{
		dir = origdir;
		{
			rvector r, up(0, 0, 1), right;
			D3DXQUATERNION q;
			D3DXMATRIX mat;

			float fAngle = (rand() % (31415 * 2)) / 1000.0f;
			float fForce = RANDOMFLOAT * SHOTGUN_DIFFUSE_RANGE;

			D3DXVec3Cross(&right, &dir, &up);
			D3DXVec3Normalize(&right, &right);
			D3DXMatrixRotationAxis(&mat, &right, fForce);
			D3DXVec3TransformCoord(&r, &dir, &mat);

			D3DXQuaternionRotationAxis(&q, &dir, fAngle);
			D3DXMatrixRotationQuaternion(&mat, &q);
			D3DXVec3TransformCoord(&r, &r, &mat);

			dir = r;
		}
		rvector BulletMarkNormal;
		bool bBulletMark = false;
		ZTargetType nTargetType = ZTT_OBJECT;

		ZPICKINFO pickinfo;

		memset(&pickinfo, 0, sizeof(ZPICKINFO));

		const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;

#ifdef _ANTILEADNEW
		MTD_AntiLeadJork* pShotInfo = OnPeerShotgun_Damaged(pOwnerCharacter, fShotTime, pos, dir, pickinfo, dwPickPassFlag, v1, v2, pItem, BulletMarkNormal, bBulletMark, nTargetType, bHitEnemy);
		if (pShotInfo)
			vShots.push_back(pShotInfo);
	}
#endif

	if (bHitEnemy) {
		CheckStylishAction(pOwnerCharacter);
		CheckCombo(pOwnerCharacter, NULL, true);
#ifdef _HITSCOUNT
		if (pOwnerCharacter == m_pMyCharacter)
			ZGetGame()->m_pMyCharacter->m_nHits += 1;
#endif

	}

#ifdef _ANTILEADNEW
	if (vShots.size() > 0 && !ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
	{
		ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(MUID(0, vShots[0]->nLowId));

		if (pOwnerCharacter->GetUID() == ZGetGameClient()->GetPlayerUID())
		{
			if (!GetMatch()->IsTeamPlay())
			{
				pCharacter->OnDamagedAPlayer(vShots);
			}
			else if (GetMatch()->IsTeamPlay() && ZGetGame()->GetMatch()->GetTeamKillEnabled() && pCharacter->IsTeam(pOwnerCharacter))
			{
				pCharacter->OnDamagedAPlayer(vShots);
			}
			else if (GetMatch()->IsTeamPlay() && !pCharacter->IsTeam(pOwnerCharacter))
			{
				pCharacter->OnDamagedAPlayer(vShots);
			}
		}

		for (auto i = vShots.begin(); i != vShots.end(); i++)
		{
			auto p = *i;
			delete p;
		}
	}
#endif

	ZApplication::GetSoundEngine()->PlaySEFire(pItem->GetDesc(), pos.x, pos.y, pos.z, (pOwnerCharacter == m_pMyCharacter));

	if (!pOwnerCharacter->IsRendered()) return;

	rvector v[6];

	int _size = pOwnerCharacter->GetWeapondummyPos(v);

	dir = to - pos;
	Normalize(dir);

	ZGetEffectManager()->AddShotgunEffect(const_cast<rvector&>(pos), v[1], dir, pOwnerCharacter);

	ZCharacter* pChar;
	if (ZGetConfiguration()->GetVideo()->bDynamicLight && pOwnerCharacter != NULL)
	{
		pChar = pOwnerCharacter;

		if (pChar->m_bDynamicLight)
		{
			pChar->m_vLightColor = g_CharLightList[SHOTGUN].vLightColor;
			pChar->m_fLightLife = g_CharLightList[SHOTGUN].fLife;
		}
		else
		{
			pChar->m_bDynamicLight = true;
			pChar->m_vLightColor = g_CharLightList[SHOTGUN].vLightColor;
			pChar->m_vLightColor.x = 1.0f;
			pChar->m_iDLightType = SHOTGUN;
			pChar->m_fLightLife = g_CharLightList[SHOTGUN].fLife;
		}
	}
	if (Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->AddLightSource(v1, 2.0f, 200);

}

MTD_AntiLeadJork* ZGame::OnPeerShotgun_Damaged(ZObject* pOwner, float fShotTime, const rvector& pos, rvector& dir, ZPICKINFO pickinfo, DWORD dwPickPassFlag, rvector& v1, rvector& v2, ZItem* pItem,rvector& BulletMarkNormal, bool& bBulletMark, ZTargetType& nTargetType, bool& bHitEnemy)
{
	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	bool bReturnValue = !pTargetCharacter;
	if (!pTargetCharacter)PROTECT_DEBUG_REGISTER(bReturnValue) return NULL;

	MMatchItemDesc* pDesc = pItem->GetDesc();
	bReturnValue = !pDesc;
	if (!pDesc)PROTECT_DEBUG_REGISTER(bReturnValue) { _ASSERT(FALSE); return NULL; }

	bool waterSound = false;
	bReturnValue = !(ZGetGame()->PickHistory(pOwner, fShotTime, pos, pos + 10000.f * dir, &pickinfo, dwPickPassFlag));
	if (!(ZGetGame()->PickHistory(pOwner, fShotTime, pos, pos + 10000.f * dir, &pickinfo, dwPickPassFlag)))
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			v1 = pos;
			v2 = pos + dir * 10000.f;
			nTargetType = ZTT_NOTHING;
			waterSound = GetWorld()->GetWaters()->CheckSpearing(v1, v2, 250, 0.3, !waterSound);
			return NULL;
		}
	}
	bReturnValue = (!pickinfo.pObject) && (!pickinfo.bBspPicked);
	if (pickinfo.bBspPicked)
	{
		PROTECT_DEBUG_REGISTER(pickinfo.nBspPicked_DebugRegister == FOR_DEBUG_REGISTER)
		{
			nTargetType = ZTT_OBJECT;

			v1 = pos;
			v2 = pickinfo.bpi.PickPos;
			BulletMarkNormal.x = pickinfo.bpi.pInfo->plane.a;
			BulletMarkNormal.y = pickinfo.bpi.pInfo->plane.b;
			BulletMarkNormal.z = pickinfo.bpi.pInfo->plane.c;
			Normalize(BulletMarkNormal);
			bBulletMark = true;
			bool bDrawTargetEffects = isInViewFrustum(v2, 20.f, RGetViewFrustum());
			if (bDrawTargetEffects)
				ZGetEffectManager()->AddBulletMark(v2, BulletMarkNormal);
			waterSound = GetWorld()->GetWaters()->CheckSpearing(v1, v2, 250, 0.3, !waterSound);
			return NULL;
		}
	}
	else if ((!pickinfo.pObject) && (!pickinfo.bBspPicked))
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			_ASSERT(false);
			return NULL;
		}
	}

	ZObject* pObject = pickinfo.pObject;
	bool bGuard = pObject->IsGuard() && (pickinfo.info.parts != eq_parts_legs) &&   
		DotProduct(dir, pObject->GetDirection()) < 0;

	if (pObject->IsGuard() && (pickinfo.info.parts != eq_parts_legs) &&
		DotProduct(dir, pObject->GetDirection()) < 0)
	{
		PROTECT_DEBUG_REGISTER(bGuard)
		{
			nTargetType = ZTT_CHARACTER_GUARD;
			// ∏∑æ“¥Ÿ
			rvector t_pos = pObject->GetPosition();
			t_pos.z += 100.f;
			ZGetEffectManager()->AddSwordDefenceEffect(t_pos + (-dir * 50.f), -dir);
			pObject->OnGuardSuccess();
			v1 = pos;
			v2 = pickinfo.info.vOut;
			return NULL;
		}
	}

	ZActor* pATarget = MDynamicCast(ZActor, pObject);
	ZActorWithFSM* pFSMActor = MDynamicCast(ZActorWithFSM, pObject);
	nTargetType = ZTT_CHARACTER;

	bool bPushSkip = false;

	if (pATarget)
	{
		bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
	}
	if (pFSMActor)
	{
		bPushSkip = pFSMActor->GetActorDef()->IsNeverBlasted();
	}

	float fKnockbackForce = pItem->GetKnockbackForce() / (.5f * float(SHOTGUN_BULLET_COUNT));

	if (bPushSkip)
	{
		rvector vPos = pOwner->GetPosition() + (pickinfo.pObject->GetPosition() - pOwner->GetPosition()) * 0.1f;
		ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met", vPos);
		fKnockbackForce = 1.0f;
	}

	pObject->OnKnockback(dir, fKnockbackForce);
#ifdef _CALCDMG
	float fActualDamage = CalcActualDamage(pOwner, pObject, (float)pDesc->m_nDamage.Ref(), pItem->GetDesc()->m_nWeaponType.Ref(), pItem);
#else
	float fActualDamage = CalcActualDamage(pOwner, pObject, (float)pDesc->m_nDamage.Ref(), pItem->GetDesc()->m_nWeaponType.Ref());
#endif
	float fRatio = pItem->GetPiercingRatio(pDesc->m_nWeaponType.Ref(), pickinfo.info.parts);
	ZDAMAGETYPE dt = (pickinfo.info.parts == eq_parts_head) ? ZD_BULLET_HEADSHOT : ZD_BULLET;
#ifdef _ANTILEADNEW
	MTD_AntiLeadJork* pShotInfo = NULL;
#endif
#ifdef _BOXLEAD
	if (pickinfo.pObject->IsNPC() == true || ZGetGameClient()->GetMatchStageSetting()->IsLead() == true || (ZGetChannelRuleMgr()->GetCurrentRule() && ZGetChannelRuleMgr()->GetCurrentRule()->GetID() == MCHANNEL_RULE_LEAD))
	{
		pickinfo.pObject->OnDamaged(pOwner, pOwner->GetPosition(), dt, pDesc->m_nWeaponType.Ref(), fActualDamage, fRatio);
	}
	else
	{
#ifdef _ANTILEADNEW
		pShotInfo = new MTD_AntiLeadJork;
		pShotInfo->nLowId = pickinfo.pObject->GetUID().Low;
		pShotInfo->fDamage = fActualDamage;
		pShotInfo->fPosX = pOwner->GetPosition().x;
		pShotInfo->fPosY = pOwner->GetPosition().y;
		pShotInfo->fPosZ = pOwner->GetPosition().z;
		pShotInfo->fRatio = fRatio;
		pShotInfo->nDamageType = dt;
		pShotInfo->nWeaponType = pDesc->m_nWeaponType.Ref();
#endif
	}
#endif
	if (!m_Match.IsTeamPlay() || (pTargetCharacter->GetTeamID() != pObject->GetTeamID()))
	{
		bHitEnemy = true;
	}

	v1 = pos;
	v2 = pickinfo.info.vOut;

	waterSound = GetWorld()->GetWaters()->CheckSpearing(v1, v2, 250, 0.3, !waterSound);

	return pShotInfo;
}

bool ZGame::CanISeeAttacker(ZCharacter* pAtk, const rvector& vRequestPos)
{
	const rvector& vAtkPos = pAtk->GetPosition();

	long double x = pow(vAtkPos.x - vRequestPos.x, 2);
	long double y = pow(vAtkPos.y - vRequestPos.y, 2);
	long double z = pow(vAtkPos.z - vRequestPos.z, 2);

	long double Len = x + y + z;

	// base infoø° ¿˙¿Âµ«?E¿÷¥¬ ¿ßƒ°øÕ «ˆ¡¶ πﬂªÁ«— π´±‚¿ß ¿ßƒ°¬˜∞° æ∆∑° ºˆƒ°∫∏¥Ÿ ¿€æ˚⁄ﬂ «—¥Ÿ.
	// ?E?√÷?E1√ ¡§µµ µÙ∑π¿Ã∞° µ…?E¿÷¥Ÿ?E∞°¡§«‘.
	// ∏∏?E?E±Ê¥Ÿ?E?E¡§?E¿ßƒ°ø°º≠ ∞¯∞›«— ∞…∑Œ ∆«¥‹«—¥Ÿ. - by SungE 2007-04-17
#define MAX_VIEW_LENGTH 800000 // ?E?¿Â∞À?E?2π¯«— ∞≈∏Æ.

	if (MAX_VIEW_LENGTH < Len)
	{
#ifdef _DEBUG
		static rvector rv(0.0f, 0.0f, 0.0f);

		long double l = pow(vRequestPos.x - rv.x, 2) + pow(vRequestPos.y - rv.y, 2) + pow(vRequestPos.z - rv.z, 2);

		rv = vRequestPos;

		mlog("len : %f(%f), res(%d)\n", Len, sqrt(Len), MAX_VIEW_LENGTH < Len);
#endif
		return false;
	}

	return true;
}

// shot ¿ª shot_range, shot_melee, shot_shotgun ¿∏∑Œ command ∏¶ ∞¢∞¢ ∫–∏Æ«œ¥¬∞Õµµ πÊπ˝¿Ã ¡¡¿ª?E
void ZGame::OnPeerShot(const MUID& uid, float fShotTime, const rvector& pos, const rvector& to, const MMatchCharItemParts sel_type)
{
	ZCharacter* pOwnerCharacter = NULL;

	pOwnerCharacter = (ZCharacter*)m_CharacterManager.Find(uid);

	if (pOwnerCharacter == NULL) return;
	if (!pOwnerCharacter->IsVisible()) return;

#ifdef LOCALE_NHNUSA
	if (!CanISeeAttacker(pOwnerCharacter, pos)) return;
#endif

	pOwnerCharacter->OnShot();

	// fShotTime ¿Ã ±◊ ƒ≥∏Ø≈Õ¿« ∑Œƒ√ Ω√∞£¿Ãπ«∑Œ ≥ª Ω√∞£¿∏∑Œ ∫Ø»Ø«ÿ¡ÿ¥Ÿ
	fShotTime -= pOwnerCharacter->m_fTimeOffset;

	ZItem* pItem = pOwnerCharacter->GetItems()->GetItem(sel_type);
	if (!pItem || !pItem->GetDesc()) return;

	// ∫Ò¡§ªÛ¿˚¿Œ πﬂªÁº”µµ∏¶ π´Ω√«—¥Ÿ.
	if (pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem))
	{
		pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
	}
	else
	{
		return;
	}

	//// ∑Á«¡¡ﬂ MEMORYHACK¿÷æ˙≥™ ∞À?E
	if (uid == ZGetMyUID()) {
		int nCurrMagazine = pItem->GetBulletCurrMagazine();

		// Ω«¡¶∑Œ π´±‚∏¶ º“?E
		if (!pItem->Shot()) return;

		if (!(pItem->GetBulletCurrMagazine() < nCurrMagazine))	// Shotø°º≠ √—æÀ ¡Ÿæ˚⁄ﬂ∏∏ ¡§ªÛ¿Ã¥Ÿ
			if (sel_type != MMCIP_MELEE)
				ZGetApplication()->Exit();
	}
	else {
		// Ω«¡¶∑Œ π´±‚∏¶ º“?E
		if (!pItem->Shot()) {
			//			_ASSERT(FALSE);	// πÆ¡¶∞°¿÷¥Ÿ, ƒ°∆√ ?
			if (!ZGetGame()->IsReplay())	// ∏Æ«√∑π¿Ã∂Û?E√—æÀ¿« ¿Øπ´ø° ªÛ?E¯¿?πﬂªÁ√≥∏Æ∏¶ «ÿ¡ÿ¥Ÿ.
				return;	// SHOT
		}
	}

	// MELEE¿œ ∞Ê?E
	if (sel_type == MMCIP_MELEE)
	{
		OnPeerShot_Melee(uid, fShotTime);

		return;
	}

	if ((sel_type != MMCIP_PRIMARY) && (sel_type != MMCIP_SECONDARY) && (sel_type != MMCIP_CUSTOM1)) return;

	if (!pItem->GetDesc()) return;
	MMatchWeaponType wtype = pItem->GetDesc()->m_nWeaponType.Ref();

#ifdef _RADAR
	if (strstr(ZGetGameClient()->GetChannelName(), "Ladder"))
	{
		rvector _pos = pos;
		MUID _uid = uid;
		if ((wtype >= MWT_SHOTGUN) && (wtype <= MWT_RIFLE))
			ZGetCombatInterface()->GetRadar()->OnAttack(_pos, _uid);
	}
#endif

	if (wtype == MWT_SHOTGUN)
	{
		OnPeerShot_Shotgun(pItem, pOwnerCharacter, fShotTime, pos, to);
		return;
	}
	else {
		OnPeerShot_Range(sel_type, uid, fShotTime, pos, to);

		rvector position;
		pOwnerCharacter->GetWeaponTypePos(weapon_dummy_muzzle_flash, &position);
		if (ZGetConfiguration()->GetVideo()->bDynamicLight)
		{
			RGetDynamicLightManager()->AddLight(GUNFIRE, position);
		}
	}
}

void ZGame::OnPeerDie(MUID& uidVictim, MUID& uidAttacker)
{
	ZCharacter* pVictim = (ZCharacter*)m_CharacterManager.Find(uidVictim);
	if (pVictim == NULL) return;

	pVictim->ActDead();

	if (pVictim == m_pMyCharacter)
	{
		pVictim->Die();		// ø©±‚º≠ Ω«¡¶∑Œ ¡◊¥¬¥Ÿ. ≥™ ¿⁄Ω≈¿∫ Ω«¡¶∑Œµµ ø©±‚º≠ ¡◊¥¬∞Õ √≥∏Æ

		// ∆¿«√Ω√ ∂«¥¬ ƒ˘Ω∫∆Æ∏µÂΩ√ ¡◊¿∏?Eø…¡Æ?E∏?E
		if (m_Match.IsWaitForRoundEnd())
		{
			if (m_CharacterManager.GetCount() > 2)
			{
				if (GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUEL)
					ReserveObserver();
			}
		}
#ifdef _QUEST
		else if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) ||
			ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_QUEST_CHALLENGE ||
			ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_BLITZKRIEG)
		{
			if (m_CharacterManager.GetCount() >= 2)
			{
				ReserveObserver();
			}
		}
#endif

		CancelSuicide();
	}

	ZCharacter* pAttacker = (ZCharacter*)m_CharacterManager.Find(uidAttacker);
	if (pAttacker == NULL) return;	// ¡◊¿Œ ªÁ∂˜¿Ã ¥©±∏¿Œ?E∏∏¶ ∞Ê?E?±◊≥… ∏Æ≈œ
	if (pAttacker != pVictim)	// ¿⁄?E?æ∆¥œ?E¿Ã∆Â∆Æ ∫∏ø©¡Ÿ∞≈ ∫∏ø©¡÷¿⁄
	{
		//Only use in team games, no duels or anything else
		if (ZGetGame()->GetMatch()->IsTeamPlay() && ZGetGame()->GetMatch()->GetRoundKills() == 1)
		{
			pAttacker->AddIcon(ZCI_FIRSTBLOOD);
			return;
		}

		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
		{
			if (pAttacker->GetKils() + 1 == 5)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nFantastic, pAttacker->GetStatus().Ref().nFantastic + 1);
				pAttacker->AddIcon(ZCI_FANTASTIC);
			}
			else if (pAttacker->GetKils() + 1 == 15)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nExcellent, pAttacker->GetStatus().Ref().nExcellent + 1);
				pAttacker->AddIcon(ZCI_EXCELLENT);
			}
			else if (pAttacker->GetKils() + 1 == 30)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nUnbelievable, pAttacker->GetStatus().Ref().nUnbelievable + 1);
				pAttacker->AddIcon(ZCI_UNBELIEVABLE);
			}
		}
		else
		{
			if (pAttacker->GetKils() >= 3)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nFantastic, pAttacker->GetStatus().Ref().nFantastic + 1);
				pAttacker->AddIcon(ZCI_FANTASTIC);
			}
		}

		if (pVictim->GetLastDamageType() == ZD_BULLET_HEADSHOT)
		{
			MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nHeadShot, pAttacker->GetStatus().Ref().nHeadShot + 1);
			pAttacker->AddIcon(ZCI_HEADSHOT);
		}
	}
}

// º≠πˆ∑Œ∫Œ≈Õ ¡˜¡¢ ≥Ø∂Ûø¿¥¬ Dead∏ﬁºº?E
void ZGame::OnPeerDead(const MUID& uidAttacker, const unsigned long int nAttackerArg,
	const MUID& uidVictim, const unsigned long int nVictimArg)
{
	ZCharacter* pVictim = (ZCharacter*)m_CharacterManager.Find(uidVictim);
	ZCharacter* pAttacker = (ZCharacter*)m_CharacterManager.Find(uidAttacker);

	bool bSuicide = false;
	if (uidAttacker == uidVictim) bSuicide = true;

	int nAttackerExp = 0, nVictimExp = 0;

	nAttackerExp = GetExpFromTransData(nAttackerArg);
	nVictimExp = -GetExpFromTransData(nVictimArg);

	if (pAttacker)
	{
		pAttacker->GetStatus().CheckCrc();

		pAttacker->GetStatus().Ref().AddExp(nAttackerExp);
		if (!bSuicide)
			pAttacker->GetStatus().Ref().AddKills();

		pAttacker->GetStatus().MakeCrc();
	}

	if (pVictim)
	{
		if (pVictim != m_pMyCharacter)
		{
			pVictim->Die();		// ø©±‚º≠ Ω«¡¶∑Œ ¡◊¥¬¥Ÿ
		}

		pVictim->GetStatus().CheckCrc();

		pVictim->GetStatus().Ref().AddExp(nVictimExp);
		pVictim->GetStatus().Ref().AddDeaths();
		if (pVictim->GetStatus().Ref().nLife > 0)
			pVictim->GetStatus().Ref().nLife--;

		pVictim->GetStatus().MakeCrc();
	}

	// »≠?E∞Ê«Ëƒ° ¿Ã∆Â∆Æ «•Ω√
	if (bSuicide && (ZGetCharacterManager()->Find(uidAttacker) == ZGetGame()->m_pMyCharacter))
	{
		// ¿⁄?E
		ZGetScreenEffectManager()->AddExpEffect(nVictimExp);
		int nExpPercent = GetExpPercentFromTransData(nVictimArg);
		ZGetMyInfo()->SetLevelPercent(nExpPercent);

		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}
	else if (ZGetCharacterManager()->Find(uidAttacker) == m_pMyCharacter)
	{
		// ≥ª∞° attacker ¿œ∂ß
		int nExpPercent;
		ZGetScreenEffectManager()->AddExpEffect(nAttackerExp);
		nExpPercent = GetExpPercentFromTransData(nAttackerArg);
		ZGetMyInfo()->SetLevelPercent(nExpPercent);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}
	else if (ZGetCharacterManager()->Find(uidVictim) == m_pMyCharacter)
	{
		// ≥ª∞° victim ¿œ∂ß
		ZGetScreenEffectManager()->AddExpEffect(nVictimExp);

		int nExpPercent = GetExpPercentFromTransData(nVictimArg);
		ZGetMyInfo()->SetLevelPercent(nExpPercent);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}

	m_Match.AddRoundKills();

	CheckKillSound(pAttacker);
	OnPeerDieMessage(pVictim, pAttacker);
}

void ZGame::CheckKillSound(ZCharacter* pAttacker)
{
	if ((!pAttacker) || (pAttacker != m_pMyCharacter)) return;
#ifdef _FIRSTBLOOD
	if (m_Match.GetRoundKills() == 1 && (ZGetGame()->GetMatch()->IsTeamPlay())) 
	{
		char szFirstBlood[50];
		sprintf(szFirstBlood, "%s", pAttacker->GetCharInfo()->szName);
		ZPostAllCmd("1stkill", szFirstBlood, "");
		ZApplication::GetSoundEngine()->PlayVoiceSound(VOICE_FIRST_KILL);
	}
#else
	if (m_Match.GetRoundKills() == 1)
	{
		ZApplication::GetSoundEngine()->PlayVoiceSound(VOICE_FIRST_KILL);
	}
#endif
}

void ZGame::OnReceiveTeamBonus(const MUID& uidChar, const unsigned long int nExpArg)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uidChar);
	if (pCharacter == NULL) return;

	int nExp = 0;

	nExp = GetExpFromTransData(nExpArg);

	if (pCharacter)
	{
		pCharacter->GetStatus().CheckCrc();
		pCharacter->GetStatus().Ref().AddExp(nExp);
		pCharacter->GetStatus().MakeCrc();
	}

	// »≠?E∞Ê«Ëƒ° ¿Ã∆Â∆Æ «•Ω√
	if (pCharacter == m_pMyCharacter)
	{
#ifdef _DEBUG
		char szTemp[128];
		sprintf(szTemp, "TeamBonus = %d\n", nExp);
		OutputDebugString(szTemp);
#endif

		// ≥ª∞° attacker ¿œ∂ß
		ZGetScreenEffectManager()->AddExpEffect(nExp);

		int nExpPercent = GetExpPercentFromTransData(nExpArg);
		ZGetMyInfo()->SetLevelPercent(nExpPercent);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}
}

void ZGame::OnPeerDieMessage(ZCharacter* pVictim, ZCharacter* pAttacker)
{
	const char* testdeathnametable[ZD_END + 1] = { "ø°∑Ø", "√—", "ƒÆ", "√ﬂ∂Ù", "∆¯πﬂ", "HEADSHOT", "∏∂¡ˆ∏∑ƒÆ¡˙" };
	char szMsg[256] = "";

	const char* szAnonymous = "?Unknown?";

	char szVictim[256];
	strcpy(szVictim, pVictim ? pVictim->GetUserAndClanName() : szAnonymous);

	char szAttacker[256];
	strcpy(szAttacker, pAttacker ? pAttacker->GetUserAndClanName() : szAnonymous);

	if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED)
	{
		char szInfectedMsg[128];
		memset(szInfectedMsg, 0, sizeof(szInfectedMsg));

		if (pVictim == pAttacker && pVictim->GetTeamID() == MMT_BLUE)
			sprintf_s(szInfectedMsg, "Human '%s' has died!", pVictim ? pVictim->GetUserName() : "Unknown");
		else if (pVictim->GetTeamID() == MMT_RED)
		{
			sprintf_s(szInfectedMsg, "Zombie '%s' has been slain!", pVictim ? pVictim->GetUserName() : "Unknown");
			ZGetSoundEngine()->PlaySound("zombie_die");
		}
		else if (pVictim->GetTeamID() == MMT_BLUE)
			sprintf_s(szInfectedMsg, "Human '%s' has been infected!", pVictim ? pVictim->GetUserName() : "Unknown");
		ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg(szInfectedMsg);
	}

	if (pAttacker == pVictim)
	{
		if (pVictim == m_pMyCharacter)
		{
			m_pMyCharacter->GetStatus().CheckCrc();
#ifdef _KILLSTREAK
			if (ZGetGame() && ZGetConfiguration()->GetEtc()->bKill)
			{
			m_pMyCharacter->GetStatus().Ref().nKillStreakCount = 0;
			}
#endif
			m_pMyCharacter->GetStatus().MakeCrc();

			if (m_pMyCharacter->GetLastDamageType() == ZD_EXPLOSION) {
				sprintf(szMsg, ZMsg(MSG_GAME_LOSE_BY_MY_BOMB));
			}
			else {
				sprintf(szMsg, ZMsg(MSG_GAME_LOSE_MYSELF));
			}

			ZChatOutput(MCOLOR(0xFFCF2020), szMsg);
		}
		else
		{
			ZTransMsg(szMsg, MSG_GAME_WHO_LOSE_SELF, 1, szAttacker);
			ZChatOutput(MCOLOR(0xFF707070), szMsg);

			// Admin Grade
			if (ZGetMyInfo()->IsAdminGrade()) {
				MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
				if (pCache && pCache->CheckFlag(MTD_PlayerFlags_AdminHide))
				{
					sprintf(szMsg, "",
						(pAttacker->GetTeamID() == MMT_BLUE) ? 3 : 1,
						pAttacker->GetProperty()->GetName());
					ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(szMsg);
				}
			}
		}
	}

	else if (pAttacker == m_pMyCharacter)
	{
#ifdef _KILLSTREAK
		if (ZGetGame() && ZGetConfiguration()->GetEtc()->bKill)
		{
			int killz = m_pMyCharacter->GetKillStreaks() + 1;
			m_pMyCharacter->GetStatus().CheckCrc();
			m_pMyCharacter->GetStatus().Ref().nKillStreakCount = killz;
			m_pMyCharacter->GetStatus().MakeCrc();
			ZPostKillStreak(szAttacker, m_pMyCharacter->GetKillStreaks(), szVictim);
		}
#endif
#ifdef _FIRSTBLOOD
		if (m_Match.GetRoundKills() == 1 && (ZGetGame()->GetMatch()->IsTeamPlay())) {
			char szFirstBlood[50];
			sprintf(szFirstBlood, "%s", pAttacker->GetCharInfo()->szName);
			ZPostAllCmd("1stkill", szFirstBlood, "");
		}
#endif
	}

	else if (pVictim == m_pMyCharacter)
	{
		// Custom: Fix
		m_pMyCharacter->GetStatus().CheckCrc();
#ifdef _KILLSTREAK
		if (ZGetGame() && ZGetConfiguration()->GetEtc()->bKill)
		{
			m_pMyCharacter->GetStatus().Ref().nKillStreakCount = 0;
		}
#endif
		m_pMyCharacter->GetStatus().MakeCrc();

		//MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
		//if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_CLASSIC_TEAM && !ZGetGameClient()->IsCWChannel() && !m_Match.IsQuestDrived() && pObjCache && pObjCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false)
		//{
		//	ZGetCombatInterface()->SetDeathObserverMode(pAttacker->GetUID());
		//}

		if (pAttacker && !ZGetGameClient()->IsCWChannel())
		{
			ZTransMsg(szMsg, MSG_GAME_LOSE_FROM_WHO, 1, szAttacker);
			ZChatOutput(MCOLOR(0xFF00E5FF), szMsg);
		}
		else if (pAttacker && !ZGetGameClient()->IsLadderChannel())
		{
			ZTransMsg(szMsg, MSG_GAME_LOSE_FROM_WHO, 1, szAttacker);
			ZChatOutput(MCOLOR(0xFF00E5FF), szMsg);
		}
		else
		{
			char szTemp[128];
			sprintf(szTemp, " (HP: %.0f/%.0f, AP: %.0f/%.0f)", pAttacker->GetHP(), pAttacker->GetMaxHP(), pAttacker->GetAP(), pAttacker->GetMaxAP());
			ZTransMsg(szMsg, MSG_GAME_LOSE_FROM_WHO, 1, szAttacker);
			ZChatOutput(MCOLOR(0xFF00E5FF), szMsg);
			strcat(szMsg, szTemp);
		}
		if (ZGetGame() && ZGetConfiguration()->GetEtc()->bKillCam)
		{
			MMATCH_GAMETYPE gametype = ZGetGameClient()->GetMatchStageSetting()->GetGameType();
			if ((ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_DEATHMATCH_SOLO) ||
				(ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_TRAINING ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_GLADIATOR_SOLO ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_BERSERKER ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_GUNGAME ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_TURBO))
			{
				ZGetCombatInterface()->SetObserverMode(true);
				ZGetCombatInterface()->GetObserver()->SetTarget(pAttacker->GetUID());
			}
		}
	}
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BLITZKRIEG)
	{
		ZGetCombatInterface()->SetObserverMode(true);
		ZGetCombatInterface()->GetObserver()->SetTarget(pAttacker->GetUID());
	}
	else
	{
		// 		sprintf(szMsg, "%s¥‘¿Ã %s¥‘¿∏∑Œ∫Œ≈Õ Ω¬∏Æ«œø¥Ω¿¥œ¥Ÿ.", szAttacker, szVictim );
		ZTransMsg(szMsg, MSG_GAME_WHO_WIN_FROM_OTHER, 2, szAttacker, szVictim);
		ZChatOutput(MCOLOR(0xFFFFb300), szMsg);

		// Admin Grade
		if (ZGetMyInfo()->IsAdminGrade()) {
			MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
			if (pCache && pCache->CheckFlag(MTD_PlayerFlags_AdminHide))
			{
				// Custom: Changed string
				if (pAttacker)
				{
					sprintf(szMsg, "^%d%s^9 wins,  ^%d%s^9 loses", //sprintf( szMsg, "^%d%s^9 Ω¬∏Æ,  ^%d%s^9 ∆–?E,
						(pAttacker->GetTeamID() == MMT_BLUE) ? 3 : 1, pAttacker->GetProperty()->GetName(),
						(pVictim->GetTeamID() == MMT_BLUE) ? 3 : 1, pVictim->GetProperty()->GetName());
					ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(szMsg);
				}
			}
		}
	}
}

void ZGame::OnReloadComplete(ZCharacter* pCharacter)
{
	ZItem* pItem = pCharacter->GetItems()->GetSelectedWeapon();

	pCharacter->GetItems()->Reload();

	if (pCharacter == m_pMyCharacter)
		ZApplication::GetSoundEngine()->PlaySound("we_weapon_rdy");

	return;

}

void ZGame::OnPeerSpMotion(MUID& uid, int nMotionType)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);

	if (pCharacter == NULL) return;

	// Custom: Ignore List check
	if (ZGetGameClient()->IsUserIgnored(pCharacter->GetUserName()) && (nMotionType == ZC_SPMOTION_TAUNT || nMotionType == ZC_SPMOTION_CRY || nMotionType == ZC_SPMOTION_LAUGH))
		return;

	pCharacter->m_dwStatusBitPackingValue.Ref().m_bSpMotion = true;

	ZC_STATE_LOWER zsl = ZC_STATE_TAUNT;

	if (nMotionType == ZC_SPMOTION_TAUNT)
	{
		zsl = ZC_STATE_TAUNT;

		char szSoundName[50];
		if (pCharacter->GetProperty()->nSex == MMS_MALE)
			sprintf(szSoundName, "fx2/MAL1%d", (RandomNumber(0, 300) % 3) + 1);
		else
			sprintf(szSoundName, "fx2/FEM1%d", (RandomNumber(0, 300) % 3) + 1);

		ZGetSoundEngine()->PlaySound(szSoundName, pCharacter->GetPosition());
	}
	else if (nMotionType == ZC_SPMOTION_BOW)
		zsl = ZC_STATE_BOW;
	else if (nMotionType == ZC_SPMOTION_WAVE)
		zsl = ZC_STATE_WAVE;
	else if (nMotionType == ZC_SPMOTION_LAUGH)
	{
		zsl = ZC_STATE_LAUGH;

		if (pCharacter->GetProperty()->nSex == MMS_MALE)
			ZGetSoundEngine()->PlaySound("fx2/MAL01", pCharacter->GetPosition());
		else
			ZGetSoundEngine()->PlaySound("fx2/FEM01", pCharacter->GetPosition());
	}
	else if (nMotionType == ZC_SPMOTION_CRY)
	{
		zsl = ZC_STATE_CRY;

		if (pCharacter->GetProperty()->nSex == MMS_MALE)
			ZGetSoundEngine()->PlaySound("fx2/MAL02", pCharacter->GetPosition());
		else
			ZGetSoundEngine()->PlaySound("fx2/FEM02", pCharacter->GetPosition());
	}
	else if (nMotionType == ZC_SPMOTION_DANCE)
		zsl = ZC_STATE_DANCE;

	pCharacter->m_SpMotion = zsl;

	pCharacter->SetAnimationLower(zsl);
}

void ZGame::OnPeerReload(MUID& uid)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	if (pCharacter == NULL || pCharacter->IsDie()) return;
	if (pCharacter == m_pMyCharacter)
	{
		// Custom: Fast Reload Room Modifier
		if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_FASTRELOAD) && !ZGetGameClient()->GetMatchStageSetting()->IsQuestDrived() && !ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST_CHALLENGE)
		{
			m_pMyCharacter->Animation_Reload();
			OnReloadComplete(pCharacter);
		}
		if (ZGetGame()->GetMatch()->IsTurboMode())
		{
			m_pMyCharacter->Animation_Reload();
			OnReloadComplete(pCharacter);
		}
		else
			m_pMyCharacter->Animation_Reload();
	}
	else
		OnReloadComplete(pCharacter);

	// Sound Effect
	if (pCharacter->GetItems()->GetSelectedWeapon() != NULL) {
		rvector p = pCharacter->GetPosition() + rvector(0, 0, 160.f);
		ZApplication::GetSoundEngine()->PlaySEReload(pCharacter->GetItems()->GetSelectedWeapon()->GetDesc(), p.x, p.y, p.z, (pCharacter == m_pMyCharacter));
	}
}

void ZGame::OnPeerChangeCharacter(MUID& uid)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);

	//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;

	if (pCharacter == NULL) return;

	pCharacter->TestToggleCharacter();
}

/*
void ZGame::OnAssignCommander(const MUID& uidRedCommander, const MUID& uidBlueCommander)
{
	AssignCommander(uidRedCommander, uidBlueCommander);
}

void ZGame::AssignCommander(const MUID& uidRedCommander, const MUID& uidBlueCommander)
{
	ZCharacter* pRedChar = m_CharacterManager.Find(uidRedCommander);
	ZCharacter* pBlueChar = m_CharacterManager.Find(uidBlueCommander);

	if(pRedChar) {
		ZGetEffectManager()->AddCommanderIcon(pRedChar,0);
		pRedChar->m_bCommander = true;
	}
	if(pBlueChar) {
		ZGetEffectManager()->AddCommanderIcon(pBlueChar,1);
		pBlueChar->m_bCommander = true;
	}

#ifdef _DEBUG
	//// DEBUG LOG ////
	const char *szUnknown = "unknown";
	char szBuf[128];
	sprintf(szBuf, "RedCMDER=%s , BlueCMDER=%s \n",
		pRedChar ? pRedChar->GetProperty()->szName : szUnknown ,
		pBlueChar ? pBlueChar->GetProperty()->szName : szUnknown );
	OutputDebugString(szBuf);
	///////////////////
#endif
}
*/
void ZGame::OnSetObserver(MUID& uid)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	if (pCharacter == NULL) return;

	if (pCharacter == m_pMyCharacter)
	{
		ZGetCombatInterface()->SetObserverMode(true);
	}
	pCharacter->SetVisible(false);
	pCharacter->ForceDie();
}

void ZGame::OnPeerSpawn(MUID& uid, rvector& pos, rvector& dir)
{
	m_nSpawnTime = timeGetTime();
	SetSpawnRequested(false);

	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);
	if (pCharacter == NULL) return;
	bool isRespawn = (pCharacter->IsDie() == true) ? true : false;

	//	dir = rvector(-1.f,0,0);
	pCharacter->SetVisible(true);
	pCharacter->Revival();
	pCharacter->SetPosition(pos);
	pCharacter->SetDirection(dir);
	pCharacter->SetSpawnTime(GetTime());

	ZGetEffectManager()->AddReBirthEffect(pos);

	if (pCharacter == m_pMyCharacter)
	{
		m_pMyCharacter->InitSpawn();
		if (ZGetGame() && ZGetConfiguration()->GetEtc()->bKillCam)
		{
			MMATCH_GAMETYPE gametype = ZGetGameClient()->GetMatchStageSetting()->GetGameType();
			if ((ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_DEATHMATCH_SOLO) ||
				(ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_TRAINING ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_GLADIATOR_SOLO ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_BERSERKER ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_GUNGAME ||
					ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_TURBO))
				ZGetCombatInterface()->SetObserverMode(false);
		}
		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BLITZKRIEG)
		{
			ZGetCombatInterface()->SetObserverMode(false);
		}
		if (isRespawn)
		{
			ZGetSoundEngine()->PlaySound("fx_respawn");
		}
		else
		{
			ZGetSoundEngine()->PlaySound("fx_whoosh02");
		}
		//ZGetScreenEffectManager()->ReSetHpPanel();
#ifdef _SPEC 1
		ReleaseObserver();
#endif
	}
#ifdef _ANTILEADNEW
	pCharacter->GetStatus().CheckCrc();
	pCharacter->GetStatus().Ref().nGivenDamage = 0;
	int LastGiven = m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage, LastTaken = m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage;
	m_pMyCharacter->GetStatus().Ref().nRoundLastGivenDamage = LastGiven;
	m_pMyCharacter->GetStatus().Ref().nRoundLastTakenDamage = LastTaken;
	ZPOSTLASTDMG(MCommandParameterInt(LastGiven), MCommandParameterInt(LastTaken));
	m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage = 0;
	m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage = 0;
	pCharacter->GetStatus().MakeCrc();
#endif

	// Custom: CTF
	//if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2)
	if (ZGetGameTypeManager()->IsTeamExtremeGame(GetMatch()->GetMatchType()))
		pCharacter->SetInvincibleTime(5000);
	// Custom: Spawn protection for non-team modes
	else if (!ZGetGameTypeManager()->IsTeamGame(GetMatch()->GetMatchType()) && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUEL)
		pCharacter->SetInvincibleTime(2000);

	if (ZGetGame() && ZGetConfiguration()->GetEtc()->bRespawnTrasparent)
	{
		pCharacter->SetInvincibleTime(2000);
	}
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SKILLMAP)
	{
		pCharacter->SetInvincibleTime(15000);
	}
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BLITZKRIEG)
	{
		pCharacter->SetInvincibleTime(5000);
	}
}

void ZGame::OnPeerDash(MCommand* pCommand)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if (pParam->GetType() != MPT_BLOB) return;

	MUID uid = pCommand->GetSenderUID();
	ZPACKEDDASHINFO* ppdi = (ZPACKEDDASHINFO*)pParam->GetPointer();

	rvector pos, dir;
	int sel_type;

	pos = rvector(Roundf(ppdi->posx), Roundf(ppdi->posy), Roundf(ppdi->posz));
	dir = 1.f / 32000.f * rvector(ppdi->dirx, ppdi->diry, ppdi->dirz);
	sel_type = (int)ppdi->seltype;

	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uid);

	if (pCharacter == NULL) return;

	MMatchCharItemParts parts = (MMatchCharItemParts)sel_type;

	if (parts != pCharacter->GetItems()->GetSelectedWeaponParts()) {
		// ¡ˆ±› µÈ?E¿÷¥¬ π´±‚øÕ ∫∏≥ª?Eπ´±‚∞° ∆≤∏Æ¥Ÿ?E∫∏≥ª?Eπ´±‚∑Œ πŸ≤„¡ÿ¥Ÿ..
		OnChangeWeapon(uid, parts);
	}
	// Custom: this time we're using zpackeddashinfo to add the dash effect(allows players to see other colors when packet is sent)
	ZGetEffectManager()->AddDashEffect(pos, dir, pCharacter, ppdi->nDashColor);

}

//#define CHAR_COLLISION_HEIGHT	170.f

rvector ZGame::GetFloor(rvector pos, rplane* pimpactplane, MUID myUID)
{
	rvector floor = ZGetGame()->GetWorld()->GetBsp()->GetFloor(pos + rvector(0, 0, 120), CHARACTER_RADIUS - 1.1f, 58.f, pimpactplane);

#ifdef ENABLE_CHARACTER_COLLISION
	ZObjectManager::iterator itor = m_ObjectManager.begin();
	for (; itor != m_ObjectManager.end(); ++itor)
	{
		ZObject* pObject = (*itor).second;
		if (pObject->IsCollideable())
			//		if(!pCharacter->IsDie() && !pCharacter->m_bBlastDrop)
		{
			rvector diff = pObject->GetPosition() - pos;
			diff.z = 0;

			// ≥™¡ﬂø° radiusªÛºˆ∞™¿∏∑Œ µ»∞Õ Object¿« ∏‚πˆ∫Øºˆ∑Œ ∞˙‡°¿⁄
			if (Magnitude(diff) < CHARACTER_RADIUS && pos.z > pObject->GetPosition().z)
			{
				rvector newfloor = pObject->GetPosition() + rvector(0, 0, pObject->GetCollHeight());
				if (floor.z < newfloor.z)
				{
					if (m_pMyCharacter->GetUID() == myUID)
					{// ≥ª ƒ≥∏Ø≈Õ πŸ¥⁄ ¿ßƒ°∏∏ √≥∏Æ«ÿ¡ÿ¥Ÿ.
						///< ¡°«¡πˆ±◊ ∂ßπÆø° ¿€?E«—ƒ≥∏Ø≈Õ¿ßø° ¥Ÿ∏• ƒ≥∏Ø≈Õ∞° ø√?E∞¨¿ª∂ß πÿø° ƒ≥∏Ø≈Õ∞° ¡°«¡Ω√
						///< ¿ßƒ≥∏Ø≈Õ¿« ≥Ù¿Ã∞° ∞ªΩ≈¿Ã æ»µ«?EµŒƒ≥∏Ø≈Õ∞° π´«—¡§ ¿ß∑Œ ø√∂Û∞°¥¬ πˆ±◊)
						if (CharacterOverlapCollision(pObject, floor.z, newfloor.z) == false)
							continue;
					}

					floor = newfloor;
					if (pimpactplane)
					{
						rvector up = rvector(0, 0, 1);
						D3DXPlaneFromPointNormal(pimpactplane, &floor, &up);
					}
				}
			}
		}
	}
#endif

	return floor;
}

bool ZGame::CharacterOverlapCollision(ZObject* pFloorObject, float WorldFloorHeight, float ObjectFloorHeight)
{
	OVERLAP_FLOOR* pOverlapObject = m_pMyCharacter->GetOverlapFloor();

	if (pOverlapObject->FloorUID != pFloorObject->GetUID())
	{ // π‚∞˙‹÷?Eƒ≥∏Ø≈Õ∞° ∫Ø∞Êµ≈?E¥ŸΩ√ ºº∆√«ÿ¡ÿ¥Ÿ.
		pOverlapObject->FloorUID = pFloorObject->GetUID();
		pOverlapObject->vecPosition.z = ObjectFloorHeight;
		pOverlapObject->nFloorCnt = 0;
		pOverlapObject->bJumpActivity = false;
	}
	else
	{ // ∞Ëº” π‚∞˙‹÷¥¬ ƒ≥∏Ø≈Õ √≥∏Æ
		if (pOverlapObject->bJumpActivity)
		{ // ¡°«¡πˆ±◊ πﬂµø
			if (m_pMyCharacter->GetPosition().z - WorldFloorHeight > 20.f)
			{ // ≥´«œ≥Ù¿Ã∞° πŸ¥⁄ø° ∞°±˚?E¡≥¿ª∂ß OVERLAP_FLOOR √ ±‚»≠
				pOverlapObject->FloorUID = MUID(0, 0);
				pOverlapObject->nFloorCnt = 0;
				pOverlapObject->vecPosition.x = 0;
				pOverlapObject->vecPosition.y = 0;
				pOverlapObject->vecPosition.z = 0;
				pOverlapObject->bJumpActivity = false;
			}
			return false;	// ¡°«¡πˆ±◊∞° πﬂµøµ∆¿∏?Eπ‚∞˙‹÷?Eƒ≥∏Ø≈Õ∏¶ π´Ω√«‘¿∏∑Œ ≥´«œ«‘
		}

		if (ObjectFloorHeight - pOverlapObject->vecPosition.z > 150.f)
		{
			pOverlapObject->vecPosition.z = ObjectFloorHeight;
			pOverlapObject->nFloorCnt++;
			if (pOverlapObject->nFloorCnt >= 3)
			{
				pOverlapObject->bJumpActivity = true;
				mlog("Jump bug Activity \n");
				return false;
			}
		}
	}

	return true;
}

/*
rvector ZGame::GetCeiling(rvector pos)
{
rvector ceiling=g_pGame->GetWorld()->GetBsp()->GetCeiling(pos+rvector(0,0,130),CHARACTER_RADIUS-0.1f);

#ifdef ENABLE_CHARACTER_COLLISION
for (ZCharacterManager::iterator itor = m_CharacterManager.begin();
itor != m_CharacterManager.end(); ++itor)
{
ZCharacter* pCharacter = (*itor).second;
if(pCharacter!=m_pMyCharacter && !pCharacter->IsDie() && !pCharacter->m_bBlastDrop)
{
rvector diff=pCharacter->m_Position-m_pMyCharacter->m_Position;
diff.z=0;

if(Magnitude(diff)<CHARACTER_RADIUS && pos.z+CHAR_COLLISION_HEIGHT<pCharacter->m_Position.z)
{
rvector newceiling=pCharacter->m_Position;
if(ceiling.z<newceiling.z)
ceiling=newceiling;
}
}
}
#endif

return ceiling;
}
*/

bool ZGame::Pick(ZObject* pOwnerObject, rvector& origin, rvector& dir, ZPICKINFO* pickinfo, DWORD dwPassFlag, bool bMyChar)
{
	return PickHistory(pOwnerObject, GetTime(), origin, origin + 10000.f * dir, pickinfo, dwPassFlag, bMyChar);
}

bool ZGame::PickTo(ZObject* pOwnerObject, rvector& origin, rvector& to, ZPICKINFO* pickinfo, DWORD dwPassFlag, bool bMyChar)
{
	return PickHistory(pOwnerObject, GetTime(), origin, to, pickinfo, dwPassFlag, bMyChar);
}

// fTime Ω√∞£¿« ƒ≥∏Ø≈Õ ¿ßƒ°∑Œ pick «—¥Ÿ.. ƒ≥∏Ø≈Õ¥¬ Ω«∏∞?E∆«¡§.
bool ZGame::PickHistory(ZObject* pOwnerObject, float fTime, const rvector& origin, const rvector& to, ZPICKINFO* pickinfo, DWORD dwPassFlag, bool bMyChar)
{
	pickinfo->pObject = NULL;
	pickinfo->bBspPicked = false;
	pickinfo->nBspPicked_DebugRegister = -10;

	RPickInfo info;
	memset(&info, 0, sizeof(RPickInfo));

	ZObject* pObject = NULL;

	bool bCheck = false;

	float fCharacterDist = FLT_MAX;			// ƒ≥∏Ø≈ÕµÈ ªÁ¿Ã¿« √÷º“∞≈∏Æ¡ˆ¡°¿ª √£¥¬¥Ÿ
	for (ZObjectManager::iterator i = m_ObjectManager.begin(); i != m_ObjectManager.end(); i++)
	{
		ZObject* pc = i->second;

		bCheck = false;

		if (bMyChar) {
			if (pc == pOwnerObject && pc->IsVisible()) {
				bCheck = true;
			}
		}
		else {
			if (pc != pOwnerObject && pc->IsVisible()) {
				bCheck = true;
			}
		}

		if (pc->IsDie())//¡◊¿∫≥—¿Ã ∏ˆªß«—¥Ÿ∞Ì «ÿº≠~
			bCheck = false;

		if (bCheck)
		{
			rvector hitPos;
			ZOBJECTHITTEST ht = pc->HitTest(origin, to, fTime, &hitPos);
			if (ht != ZOH_NONE) {
				float fDistToChar = Magnitude(hitPos - origin);
				if (fDistToChar < fCharacterDist) {
					pObject = pc;
					fCharacterDist = fDistToChar;
					info.vOut = hitPos;
					switch (ht) {
					case ZOH_HEAD: info.parts = eq_parts_head; break;
					case ZOH_BODY: info.parts = eq_parts_chest; break;
					case ZOH_LEGS:	info.parts = eq_parts_legs; break;
					}
				}
			}
		}
	}

	RBSPPICKINFO bpi;
	bool bBspPicked = GetWorld()->GetBsp()->PickTo(origin, to, &bpi, dwPassFlag);

	int nCase = 0;

	if (pObject && bBspPicked)		// µ—¥Ÿ ∏¬æ“¿ª∂ß¥¬ ∞≈∏Æ∞° ∞°±ÓøÓ¬ ¿ª ≈√«—¥Ÿ.
	{
		if (Magnitude(info.vOut - origin) > Magnitude(bpi.PickPos - origin))
			nCase = 1;
		else
			nCase = 2;
	}
	else
		if (bBspPicked)				// µ—¡ﬂ «œ≥™∏∏ ∏¬æ“¿∏∏È ∏¬¿∫∞… ≈√«œ∏È µ»¥Ÿ.
			nCase = 1;
		else
			if (pObject)
				nCase = 2;

	if (nCase == 0) return false;

	switch (nCase)
	{
	case 1:						// ∏ ø° ∏¬¿∫∞ÊøÏ
		pickinfo->bBspPicked = true;
		pickinfo->nBspPicked_DebugRegister = FOR_DEBUG_REGISTER;
		pickinfo->bpi = bpi;
		break;
	case 2:						// ªÁ∂˜ø° ∏¬¿∫∞ÊøÏ.
		pickinfo->pObject = pObject;
		pickinfo->info = info;
		break;
	}
	return true;
}

bool ZGame::ObjectColTest(ZObject* pOwner, rvector& origin, rvector& to, float fRadius, ZObject** poutTarget)
{
	// ∏ ø° ∏¬¥¬∞Õ¿∫ √º≈©«œ?Eæ ¥¬¥Ÿ.

	for (ZObjectManager::iterator i = m_ObjectManager.begin(); i != m_ObjectManager.end(); i++)
	{
		ZObject* pc = i->second;

		if (pc == pOwner)
			continue;

		if (!pc->IsVisible())
			continue;

		if (pc->IsDie())
			continue;

		if (pc->ColTest(origin, to, fRadius, GetTime()))
		{
			*poutTarget = pc;
			return true;
		}
	}

	return false;
}

char* ZGame::GetSndNameFromBsp(const char* szSrcSndName, RMATERIAL* pMaterial)
{
	char szMaterial[256] = "";
	static char szRealSndName[256] = "";
	szRealSndName[0] = 0;

	if (pMaterial == NULL) return "";

	strcpy(szMaterial, pMaterial->Name.c_str());

	size_t nLen = strlen(szMaterial);

#define ZMETERIAL_SNDNAME_LEN 7

	if ((nLen > ZMETERIAL_SNDNAME_LEN) &&
		(!strnicmp(&szMaterial[nLen - ZMETERIAL_SNDNAME_LEN + 1], "mt", 2)))
	{
		strcpy(szRealSndName, szSrcSndName);
		strcat(szRealSndName, "_");
		strcat(szRealSndName, &szMaterial[nLen - ZMETERIAL_SNDNAME_LEN + 1]);
	}
	else
	{
		strcpy(szRealSndName, szSrcSndName);
	}

	return szRealSndName;
}

/*
void ZGame::AdjustGlobalTime()
{
// ƒ´ø˚‚Æ ¥Ÿø˚„“∂ß∏∏ Ω√∞£¿ª ΩÃ≈©«—¥Ÿ
//	if(GetMatch()->GetRoundState()!=MMATCH_ROUNDSTATE_COUNTDOWN) return;
static DWORD nLastTime = GetTickTime();
DWORD nNowTime = GetTickTime();
if((nNowTime - nLastTime) < 100) return;	// 100π–∏Æººƒ¡µÂ∏∂¥Ÿ √º≈©

nLastTime = nNowTime;

float fAverageTime=0.f;

int nValidCount=0;

ZCharacterManager::iterator i;
for(i=m_CharacterManager.begin();i!=m_CharacterManager.end();i++)
{
ZCharacter *pCharacter=i->second;
if(pCharacter->m_BasicHistory.size()==0) continue;		// ¿Ø»ø«œ?Eæ ¥Ÿ

// ƒ≥∏Ø≈Õ∞° ∏∂¡ˆ∏∑¿∏∑Œ ∫∏≥ªø¬ ¡§∫∏∏¶ ?E?
ZBasicInfoHistory::iterator infoi=pCharacter->m_BasicHistory.end();
infoi--;
ZBasicInfoItem *pInfo=*infoi;

// ∏∂¡ˆ∏∑ µ•¿Ã≈Õ πﬁ¿∫?E3√  ¿ÃªÛ¿Ã?EπÆ¡¶∞° ¿÷¥Ÿ?E∆«¡§. ¿Ø»ø«œ?Eæ ¥Ÿ
if(GetTime()-pInfo->fReceivedTime > 3.f) continue;

float fCharacterTime=pInfo->info.fSendTime+(GetTime()-pInfo->fReceivedTime);

nValidCount++;
fAverageTime+=fCharacterTime;
}

fAverageTime/=(float)nValidCount;
fAverageTime=max(fAverageTime,0);	// 0∫∏¥Ÿ ¿€¿ªºˆ¥¬ æ¯¥Ÿ.

// ±€∑Œ?EΩ√∞£∞˙¿« ¬˜¿Ã∏¶ ¥©¿˚«—¥Ÿ.
for(i=m_CharacterManager.begin();i!=m_CharacterManager.end();i++)
{
ZCharacter *pCharacter=i->second;
if(pCharacter->m_BasicHistory.size()==0) continue;		// ¿Ø»ø«œ?Eæ ¥Ÿ

ZBasicInfoHistory::iterator infoi=pCharacter->m_BasicHistory.end();
infoi--;
ZBasicInfoItem *pInfo=*infoi;
float fCharacterTime=pInfo->info.fSendTime+(GetTime()-pInfo->fReceivedTime);

pCharacter->m_TimeErrors[pCharacter->m_nTimeErrorCount++]=fAverageTime-fCharacterTime;
if( TIME_ERROR_CORRECTION_PERIOD == pCharacter->m_nTimeErrorCount )
{
pCharacter->m_nTimeErrorCount=0;
float fAvrTimeError=0;
for(int j=0;j<TIME_ERROR_CORRECTION_PERIOD;j++)
fAvrTimeError+=pCharacter->m_TimeErrors[j];
fAvrTimeError/=(float)TIME_ERROR_CORRECTION_PERIOD;

pCharacter->m_fAccumulatedTimeError+=fAvrTimeError*.5f;
if(fabs(pCharacter->m_fAccumulatedTimeError)>10.f)
{
#ifndef _PUBLISH
char szTemp[256];
sprintf(szTemp, "%s¥‘¿Ã Ω∫««µÂ«Ÿ ? %3.1f", pCharacter->GetProperty()->szName,pCharacter->m_fAccumulatedTimeError);
ZGetGameInterface()->OutputChatMsg(MCOLOR(0xFFFF0000), szTemp);
#endif

pCharacter->m_fAccumulatedTimeError=0;
}

if(pCharacter==m_pMyCharacter)
{
m_fTime+=fAvrTimeError*.5f;
}
}
}
}
*/

// ?E?
#ifdef _ASSITEN
void ZGame::AutoAiming()
{

	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PREPARE)
		return;
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PRE_COUNTDOWN)
		return;
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FREE)
		return;
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_EXIT)
		return;
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FINISH)
		return;
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_END)
		return;

	if (ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
		return;

	if (ZGetGame()->m_pMyCharacter->IsDie())
		return;

	if (ZApplication::GetGameInterface()->IsLeaveBattleReserved() == true)
		return;

	ZCamera* pCamera = ZGetGameInterface()->GetCamera();
	rvector vMyPos = m_pMyCharacter->GetPosition();


	ZCharacter* pCharacter;
	rvector pos;
	rvector dir;

	for (ZCharacterManager::iterator i = m_CharacterManager.begin(); i != m_CharacterManager.end(); i++)
	{
		pCharacter = (ZCharacter*)i->second;
		if (pCharacter != m_pMyCharacter)
		{
			if (pCharacter->IsDie() == false)
			{
				pos = pCharacter->GetPosition();
				pos.z += 140.f;
				dir = pos - RCameraPosition;
				Normalize(dir);
				pCamera->SetDirection(dir);
			}
		}
	}
}
#endif

#define MAX_PLAYERS		64

// ≈ı«•¥¬ ¡¶∞≈ µ«æ˙¿∏π«∑Œ ≥ª ««¡§∫∏∏∏ ∫∏≥Ω¥Ÿ
void ZGame::PostHPAPInfo()
{
	DWORD nNowTime = GetTickTime();

	if (m_pMyCharacter->GetInitialized() == false) return;

	if ((nNowTime - m_nLastTime[ZLASTTIME_HPINFO]) >= PEER_HP_TICK)
	{
		m_nLastTime[ZLASTTIME_HPINFO] = nNowTime;

		ZPostHPAPInfo(m_pMyCharacter->GetHP(), m_pMyCharacter->GetAP());
	}

#ifdef ENABLE_ADJUST_MY_DATA
	//	AdjustMyData();
#endif
}

// µ‡æÛ≈‰≥ ∏’∆Æ UI¡ﬂø°º≠ ªÛ?EÅE∞‘¿Ã¡ˆµµ ∫∏ø©¡÷±‚∂ßπÆø°
// 0.1√ ∏∂¥Ÿ HP, AP∏¶ ¿Ø¿˙µÈø°∞‘ ∫∏≥ª¡‡æﬂ «—¥Ÿ. (PostHPAPInfo()ø°º≠¥¬ 1√ ø° «—π¯æø √≥∏Æ«ÿ¡‹)
void ZGame::PostDuelTournamentHPAPInfo()
{
	DWORD nNowTime = GetTickTime();

	if (m_pMyCharacter->GetInitialized() == false) return;

	if ((nNowTime - m_nLastTime[ZLASTTIME_HPINFO]) >= PEER_DUELTOURNAMENT_HPAP_TICK)
	{
		m_nLastTime[ZLASTTIME_HPINFO] = nNowTime;

		BYTE MaxHP = (BYTE)m_pMyCharacter->GetMaxHP();
		BYTE MaxAP = (BYTE)m_pMyCharacter->GetMaxAP();
		BYTE HP = (BYTE)m_pMyCharacter->GetHP();
		BYTE AP = (BYTE)m_pMyCharacter->GetAP();

		ZPostDuelTournamentHPAPInfo(MaxHP, MaxAP, HP, AP);
	}
}

void ZGame::PostBasicInfo()
{
	// ¿Œ≈Õ≥›¿Ã ≤˜∞Â¿∏?E≈∞¿Œ«≤ √≥∏Æ∏¶ «œ?Eæ ¥¬¥Ÿ.(∑£º± ªÃæ∆ æ«?EπÊ?E
	if (!ZGetGameInterface()->GetCombatInterface()->IsNetworkalive())
		return;

	DWORD nNowTime = timeGetTime();

	if (m_pMyCharacter->GetInitialized() == false) return;

	// ¡◊∞˙œ™º≠ 5√ ∞° ¡ˆ≥™?Ebasicinfo∏¶ ∫∏≥ª?Eæ ¥¬¥Ÿ.
	if (m_pMyCharacter->IsDie() && GetTime() - m_pMyCharacter->m_timeInfo.Ref().m_fDeadTime > 5.f) return;

	// ≥≠¿‘«— ¡˜»ƒø°µµ ∫∏≥ª?Eæ ¥¬¥Ÿ ( global time ¿Ã ºº∆√µ«?Eæ æ“?E∂ßπÆø° )
	//	if(m_bForceEntry) return;

	int nMoveTick = GetCharacterBasicInfoTick();

	if ((int)(nNowTime - m_nLastTime[ZLASTTIME_BASICINFO]) >= nMoveTick)
	{
		m_nLastTime[ZLASTTIME_BASICINFO] = nNowTime;

		ZPACKEDBASICINFO pbi;
		pbi.fTime = GetTime();

		pbi.posx = m_pMyCharacter->GetPosition().x;
		pbi.posy = m_pMyCharacter->GetPosition().y;
		pbi.posz = m_pMyCharacter->GetPosition().z;
		/*pbi.posx = (int(m_pMyCharacter->GetPosition().x * 0.1f)) * 10.f;
		pbi.posy = (int(m_pMyCharacter->GetPosition().y * 0.1f)) * 10.f;
		pbi.posz = (int(m_pMyCharacter->GetPosition().z * 0.1f)) * 10.f;*/	// ø¿¬˜∑Œ ¿Œ«— πˆ±◊¿Á«ˆ¿ª Ω±∞‘ «œ?E¿ß«ÿ 1¿« ¿⁄∏Æ±˚?E¿˝ªÁ«— πˆ?E

		pbi.velx = m_pMyCharacter->GetVelocity().x;
		pbi.vely = m_pMyCharacter->GetVelocity().y;
		pbi.velz = m_pMyCharacter->GetVelocity().z;

		pbi.dirx = m_pMyCharacter->m_TargetDir.x * 32000;
		pbi.diry = m_pMyCharacter->m_TargetDir.y * 32000;
		pbi.dirz = m_pMyCharacter->m_TargetDir.z * 32000;

		pbi.upperstate = m_pMyCharacter->GetStateUpper();
		pbi.lowerstate = m_pMyCharacter->GetStateLower();
		pbi.selweapon = m_pMyCharacter->GetItems()->GetSelectedWeaponParts();

		ZPOSTCMD1(MC_PEER_BASICINFO, MCommandParameterBlob(&pbi, sizeof(ZPACKEDBASICINFO)));
	}
}

void ZGame::PostPeerPingInfo()
{
	if (!ZGetGameInterface()->GetCombatInterface()->IsShowScoreBoard()) return;

	DWORD nNowTime = GetTickTime();

	if ((nNowTime - m_nLastTime[ZLASTTIME_PEERPINGINFO]) >= PEER_PING_TICK) {
		m_nLastTime[ZLASTTIME_PEERPINGINFO] = nNowTime;

		unsigned long nTimeStamp = GetTickTime();
		MMatchPeerInfoList* pPeers = ZGetGameClient()->GetPeers();
		for (MMatchPeerInfoList::iterator itor = pPeers->begin(); itor != pPeers->end(); ++itor) {
			MMatchPeerInfo* pPeerInfo = (*itor).second;
			if (pPeerInfo->uidChar != ZGetGameClient()->GetPlayerUID()) {
				_ASSERT(pPeerInfo->uidChar != MUID(0, 0));

				MCommandManager* MCmdMgr = ZGetGameClient()->GetCommandManager();
				MCommand* pCmd = new MCommand(MCmdMgr->GetCommandDescByID(MC_PEER_PING),
					pPeerInfo->uidChar, ZGetGameClient()->GetUID());
				pCmd->AddParameter(new MCmdParamUInt(nTimeStamp));
				ZGetGameClient()->Post(pCmd);

#ifdef _DEBUG
				g_nPingCount++;
#endif
				pPeerInfo->SetLastPingTime(nTimeStamp);
			}
		}
	}
}

void ZGame::PostSyncReport()
{
	DWORD nNowTime = GetTickTime();

#ifdef _PUBLISH
	if ((nNowTime - m_nLastTime[ZLASTTIME_SYNC_REPORT]) >= MATCH_CYCLE_CHECK_SPEEDHACK) {
#else
	if ((nNowTime - m_nLastTime[ZLASTTIME_SYNC_REPORT]) >= 1000/*MATCH_CYCLE_CHECK_SPEEDHACK*/) {
#endif
		m_nLastTime[ZLASTTIME_SYNC_REPORT] = nNowTime;
		int nDataChecksum = 0;
		if (m_DataChecker.UpdateChecksum() == false) {
			nDataChecksum = m_DataChecker.GetChecksum();
			ZGetApplication()->Exit();
		}

		if (ZCheckHackProcess() == true) {
			ZPostDisconnect();
		}
		ZPOSTCMD2(MC_MATCH_GAME_REPORT_TIMESYNC, MCmdParamUInt(nNowTime), MCmdParamUInt(nDataChecksum));
	}
	}

// pOwner / pTarget = Ωƒ≥∏Ø≈Õ / ∏¬¿∫ ƒ≥∏Ø≈Õ
void ZGame::CheckCombo(ZCharacter * pOwnerCharacter, ZObject * pHitObject, bool bPlaySound)
{
	// ¿⁄±‚∞° ¿⁄?E∏¬√·∞« √º≈©«œ?Eæ ¿Ω
	if (pOwnerCharacter == pHitObject) return;

	// ≥ª ƒ≥∏Ø≈Õ »§¿∫ ≥ª∞° ∫∏∞˙‹÷¥¬ ƒ≥∏Ø≈Õ
	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if (!pTargetCharacter) return;

	if (pTargetCharacter != pOwnerCharacter) return;	// ≥ª∞° ∫∏∞˙‹÷¥¬ ƒ≥∏Ø≈Õ∞° æ∆¥œ?E√º≈©«œ?Eæ ¿Ω

	if (pHitObject)	// NULL ¿Ã?Eπ´¡∂∞« ø√∏∞¥Ÿ
	{
		if (pHitObject->IsDie()) return;		// Ω√√º∏¶ Ω˜µµ √º≈©µ«?Eæ ¿Ω.
	}

	if (IsPlayerObject(pHitObject))
	{
		// ?EÆ?E∂ß∏∞∞Õ¿∫ combo ø° ∆˜«‘µ«?Eæ ¿Ω
		if (m_Match.IsTeamPlay() && (pTargetCharacter->GetTeamID() == ((ZCharacter*)(pHitObject))->GetTeamID()))
			return;

		// ƒ˘Ω∫∆Æ¿œ∂ßµµ ?EÆ∆˙‹∫ ∆˜«‘«œ?Eæ ¥¬¥Ÿ.
		if (m_Match.IsQuestDrived() || ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_QUEST_CHALLENGE) return;
	}

	UpdateCombo(true);

	if (Z_AUDIO_HITSOUND)
	{
		//		bool bHitObjectIsNPC = ((pHitObject) && (pHitObject->IsNPC()));

		//		if (!bHitObjectIsNPC)
		//		{
#ifdef _BIRDSOUND
		ZGetSoundEngine()->PlaySound("fx_myhit", 128);
#else
		if (bPlaySound)
			if (ZGetSoundEngine()->Get3DSoundUpdate())
				ZGetSoundEngine()->PlaySound("fx_myhit");
#endif
		//		}
	}
}
void ZGame::UpdateCombo(bool bShot)
{
	// ≥ª ƒ≥∏Ø≈Õ »§¿∫ ≥ª∞° ∫∏∞˙‹÷¥¬ ƒ≥∏Ø≈Õ
	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if (!pTargetCharacter) return;

	// test
	static DWORD nLastShotTime = timeGetTime();
	DWORD nNowTime = timeGetTime();

	pTargetCharacter->GetStatus().CheckCrc();

	if (bShot)
	{
		// Custom: Hit Maker Coded in command work By Desperate
		//if (ZGetConfiguration()->GetEtc()->bhitmaker)
		if (ZGetGame() && ZGetConfiguration()->GetEtc()->bhitmaker)
		{
			if (pTargetCharacter->GetStatus().Ref().nCombo >= 0)
			{
				if (ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeaponParts() != MMCIP_MELEE)
					ZGetScreenEffectManager()->AddHitMaker();
				ZGetScreenEffectManager()->AddHit();
			}
		}
		if (pTargetCharacter->GetStatus().Ref().nCombo < 2) {
			// hit ¿Ã∆Â∆Æ
			ZGetScreenEffectManager()->AddHit();
		}
		if ((nNowTime - nLastShotTime) < 700)
		{
			pTargetCharacter->GetStatus().Ref().nCombo++;
			if (pTargetCharacter->GetStatus().Ref().nCombo > MAX_COMBO)
				pTargetCharacter->GetStatus().Ref().nCombo = 1;
		}
		nLastShotTime = nNowTime;
	}
	else
	{
		if ((pTargetCharacter->GetStatus().Ref().nCombo > 0) && ((nNowTime - nLastShotTime) > 1000))
		{
			pTargetCharacter->GetStatus().Ref().nCombo = 0;
		}
	}

	pTargetCharacter->GetStatus().MakeCrc();
}

void ZGame::CheckStylishAction(ZCharacter * pCharacter)
{
	if (pCharacter->GetStylishShoted())
	{
		if (pCharacter == m_pMyCharacter)
		{
			ZGetScreenEffectManager()->AddCool();
		}
	}
}

#define RESERVED_OBSERVER_TIME	5000

void ZGame::OnReserveObserver()
{
	unsigned long int currentTime = timeGetTime();

	if (currentTime - m_nReservedObserverTime > RESERVED_OBSERVER_TIME)
	{
		if ((m_Match.GetRoundState() == MMATCH_ROUNDSTATE_PLAY) ||
			(m_Match.IsWaitForRoundEnd() && ZGetGameClient()->IsForcedEntry())
			)
		{
			ZGetGameInterface()->GetCombatInterface()->SetObserverMode(true);
			m_bReserveObserver = false;
		}
		else
		{
			m_bReserveObserver = false;
		}
	}
}

void ZGame::ReserveObserver()
{
	m_bReserveObserver = true;
	m_nReservedObserverTime = timeGetTime();
}

void ZGame::ReleaseObserver()
{
	if (!m_bReplaying.Ref())
	{
		m_bReserveObserver = false;
		ZGetGameInterface()->GetCombatInterface()->SetObserverMode(false);

		FlushObserverCommands();
	}
}

void ZGame::OnInvalidate()
{
	GetWorld()->OnInvalidate();
	ZGetFlashBangEffect()->OnInvalidate();
	m_CharacterManager.OnInvalidate();
}

void ZGame::OnRestore()
{
	GetWorld()->OnRestore();
	ZGetFlashBangEffect()->OnRestore();
	m_CharacterManager.OnRestore();
}

void ZGame::InitRound()
{
	//	m_fTime=0;
	SetSpawnRequested(false);
	ZGetGameInterface()->GetCamera()->StopShock();

	ZGetFlashBangEffect()->End();

	ZGetEffectManager()->Clear();
	m_WeaponManager.Clear();

#ifdef _WORLD_ITEM_
	//ZGetWorldItemManager()->Reset();
#endif

	ZGetCharacterManager()->InitRound();
}

void ZGame::AddEffectRoundState(MMATCH_ROUNDSTATE nRoundState, int nArg)
{
	switch (nRoundState)
	{
	case MMATCH_ROUNDSTATE_COUNTDOWN:
	{
		if ((m_Match.IsWaitForRoundEnd() && m_Match.GetMatchType() != MMATCH_GAMETYPE_DUEL) ||
			m_Match.GetMatchType() == MMATCH_GAMETYPE_SKILLMAP)
		{
			if (m_Match.GetCurrRound() + 1 == m_Match.GetRoundCount())
			{
				ZGetScreenEffectManager()->AddFinalRoundStart();
			}
			else
			{
				if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
				{
					// m_nCurrRound ∞° 0¿Ã?E1∂Ûø˚—Â¿Ã¥Ÿ.
					ZRuleDuelTournament* pRule = (ZRuleDuelTournament*)m_Match.GetRule();
					int nRoundCount = pRule->GetDuelTournamentPlayCount();
					ZGetScreenEffectManager()->AddRoundStart(nRoundCount);
				}
				else
				{
					// m_nCurrRound ∞° 0¿Ã?E1∂Ûø˚—Â¿Ã¥Ÿ.
					ZGetScreenEffectManager()->AddRoundStart(m_Match.GetCurrRound() + 1);
				}
			}
		}
		if (m_Match.GetMatchType() == MMATCH_GAMETYPE_QUEST_CHALLENGE)
		{
			ZRuleQuestChallenge* pCQRule = (ZRuleQuestChallenge*)m_Match.GetRule();
			if (pCQRule->GetCurrSector() + 1 == pCQRule->GetRoundMax())
			{
				ZGetScreenEffectManager()->AddFinalRoundStart();
			}
			else
			{
				ZGetScreenEffectManager()->AddRoundStart(pCQRule->GetCurrSector() + 1);
			}
		}
	}
	break;
	case MMATCH_ROUNDSTATE_PLAY:
	{
		ZGetCombatInterface()->SetFrozen(false);
		MMATCH_GAMETYPE gameType = ZGetGameClient()->GetMatchStageSetting()->GetGameType();
		if (gameType == MMATCH_GAMETYPE_SPY)
		{
			ZGetScreenEffectManager()->AddScreenEffect("spy_selection");
			if (m_pMyCharacter->GetTeamID() == MMT_RED)
			{
				if (m_pMyCharacter->GetProperty()->nSex == MMS_MALE)
					ZGetGameInterface()->PlayVoiceSound("fx2/MAL01", 1500);
				else if (m_pMyCharacter->GetProperty()->nSex == MMS_FEMALE)
					ZGetGameInterface()->PlayVoiceSound("fx2/FEM01", 1500);
			}
		}
		else if (gameType == MMATCH_GAMETYPE_CTF)
		{
			ZGetGameInterface()->PlayVoiceSound(VOICE_CTF, 1600);
			ZGetScreenEffectManager()->AddScreenEffect("ctf_splash");
		}
		else
		{
			ZGetGameInterface()->PlayVoiceSound(VOICE_LETS_ROCK, 1100);
			ZGetScreenEffectManager()->AddRock();
			if (!ZGetApplication()->m_RunThread) ZGetApplication()->RunAntiThread();

			if (ZGetGameClient()->IsLadderChannel())
			{
				char szText[256];
				sprintf(szText, ZMsg(MSG_WORD_LADDERCOINN_MSG_START));
				ZChatOutput(ZCOLOR_CHAT_SYSTEM, szText);
			}

		}
	}
	break;
	case MMATCH_ROUNDSTATE_FINISH:
	{
		if (m_Match.IsTeamPlay())
		{
			MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
			if (pObjCache && pObjCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false)
			{
				ZCharacter* pCharacterDMG = ZGetGame()->m_pMyCharacter;
				char szName[125];
				sprintf(szName, "I have taken %d damage and given %d damage", pCharacterDMG->GetStatus().Ref().nRoundTakenDamage, pCharacterDMG->GetStatus().Ref().nRoundGivenDamage);
				{
					ZChatOutput(szName, ZChat::CMT_SYSTEM);
				}
			}
			if (m_Match.GetMatchType() == MMATCH_GAMETYPE_SKILLMAP)
			{
				ZRuleSkillMap* matchRule = (ZRuleSkillMap*)ZGetGame()->GetMatch()->GetRule();
				if (matchRule->GetChestGrabber() == ZGetMyUID())
					ZGetScreenEffectManager()->AddWin();
				else
					ZGetScreenEffectManager()->AddLose();
			}

			if (ZGetApplication()->m_RunThread) ZGetApplication()->KillAntiThread();

			int nRedTeam, nBlueTeam;
			m_Match.GetTeamAliveCount(&nRedTeam, &nBlueTeam);

			if (nArg == MMATCH_ROUNDRESULT_RED_ALL_OUT || nArg == MMATCH_ROUNDRESULT_BLUE_ALL_OUT)
			{
				ZGetScreenEffectManager()->AddWin();
			}
			else if (nArg == MMATCH_ROUNDRESULT_DRAW)
			{
				// Custom: CTF
				if (ZGetGameTypeManager()->IsTeamExtremeGame(GetMatch()->GetMatchType()))
				{
					MMatchTeam nMyTeam = (MMatchTeam)m_pMyCharacter->GetTeamID();
					MMatchTeam nEnemyTeam = (nMyTeam == MMT_BLUE ? MMT_RED : MMT_BLUE);

					int nMyScore = GetMatch()->GetTeamKills(nMyTeam);
					int nEnemyScore = GetMatch()->GetTeamKills(nEnemyTeam);

					if (nMyScore > nEnemyScore)
						ZGetScreenEffectManager()->AddWin();
					else if (nMyScore < nEnemyScore)
						ZGetScreenEffectManager()->AddLose();
					else
						ZGetScreenEffectManager()->AddDraw();
				}
				else
					ZGetScreenEffectManager()->AddDraw();
			}
			else
			{
				if (nArg == MMATCH_ROUNDRESULT_DRAW)
				{
					ZGetGameInterface()->PlayVoiceSound(VOICE_DRAW_GAME, 1200);
				}
				else {
					MMatchTeam nMyTeam = (MMatchTeam)m_pMyCharacter->GetTeamID();
					MMatchTeam nTeamWon = (nArg == MMATCH_ROUNDRESULT_REDWON ? MMT_RED : MMT_BLUE);

					// ∏∏?E∞≠¡¶∑Œ ∆¿¿Ã πŸ≤∏?E∞Ê?E°¥?π›?EÅE
					if (ZGetMyInfo()->GetGameInfo()->bForcedChangeTeam)
					{
						nMyTeam = NegativeTeam(nMyTeam);
					}

					// Spectator ¿œ∞Ê?E√≥∏Æ
					if (ZGetGameInterface()->GetCombatInterface()->GetObserver()->IsVisible()) {
						ZCharacter* pTarget = ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter();
						if (pTarget)
							nMyTeam = (MMatchTeam)pTarget->GetTeamID();
					}

					if (nTeamWon == nMyTeam)
						ZGetScreenEffectManager()->AddWin();
					else
						ZGetScreenEffectManager()->AddLose();

					if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY)
					{
						// Spy Mode : no sounds for Win/Lose.
					}
					if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_ASSASSINATE || GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED)
					{
						if (nTeamWon == MMT_RED)
							ZGetGameInterface()->PlayVoiceSound(VOICE_BLUETEAM_BOSS_DOWN, 2100);
						else
							ZGetGameInterface()->PlayVoiceSound(VOICE_REDTEAM_BOSS_DOWN, 2000);
					}
					else
					{
						if (nTeamWon == MMT_RED)
							ZGetGameInterface()->PlayVoiceSound(VOICE_RED_TEAM_WON, 1400);
						else
							ZGetGameInterface()->PlayVoiceSound(VOICE_BLUE_TEAM_WON, 1400);
					}
				}
			}

			int nTeam = 0;

			// all kill ∆«¡§
			for (int j = 0; j < 2; j++)
			{
				bool bAllKill = true;
				ZCharacter* pAllKillPlayer = NULL;

				for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();
					itor != ZGetCharacterManager()->end(); ++itor)
				{
					ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
					if (pCharacter == NULL) return;

					if (j == 0) {
						nTeam = MMT_RED;
					}
					else if (j == 1) {
						nTeam = MMT_BLUE;
					}

					if (pCharacter->GetTeamID() != nTeam)
						continue;

					if (pCharacter->IsDie())
					{
						ZCharacter* pKiller = (ZCharacter*)ZGetCharacterManager()->Find(pCharacter->GetLastAttacker());
						if (pAllKillPlayer == NULL)
						{
							if (!pKiller || pKiller->GetTeamID() == nTeam)			// ∞∞¿∫∆˙„—≈◊ ¡◊æ˙¿∏?E≤Œ
							{
								bAllKill = false;
								break;
							}

							pAllKillPlayer = pKiller;
						}
						else
							if (pAllKillPlayer != pKiller)	// ø©∑Ø∏˙‹Ã ≥™¥≤ ¡◊ø¥¿∏?E≤Œ
							{
								bAllKill = false;
								break;
							}
					}
					else
					{
						bAllKill = false;											// ?E∆¿÷¥?≥—¿Ã ¿÷æ˚—µ ≤Œ
						break;
					}
				}
				// spy mode : no need to show allkill.
				if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY)
					bAllKill = false;

				if ((bAllKill) && (pAllKillPlayer))
				{
					MEMBER_SET_CHECKCRC(pAllKillPlayer->GetStatus(), nAllKill, pAllKillPlayer->GetStatus().Ref().nAllKill + 1);
					pAllKillPlayer->AddIcon(ZCI_ALLKILL);
				}
			}
		}

		// µ‡?E∏µÂ¿œ ∞Ê?E
		else if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
		{
		ZCharacter* pCharacterDMG = ZGetGame()->m_pMyCharacter;
		char szName[125];
		sprintf(szName, "I have taken %d damage and given %d damage", pCharacterDMG->GetStatus().Ref().nRoundTakenDamage, pCharacterDMG->GetStatus().Ref().nRoundGivenDamage);
		ZGetGameClient()->OutputMessage(szName, MZMOM_LOCALREPLY);
			ZRuleDuel* pDuel = (ZRuleDuel*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
			if (pDuel)
			{
				bool bAddWin = false;
				bool bAddLose = false;
				int nCount = 0;				// √®««æ?Eµµ?E?∏µŒ ∞‘¿”¡ﬂ¿Ãø¥¥¬?E√º≈©«œ?E¿ß«ÿº≠...

				// ø…¡Æ?E∏µÂ¿œ∂ß
				MUID uidTarget;
				ZObserver* pObserver = ZGetGameInterface()->GetCombatInterface()->GetObserver();
				if (pObserver && pObserver->IsVisible())
					uidTarget = pObserver->GetTargetCharacter()->GetUID();

				// ø…¡Æ?E∏µÂ∞° æ∆¥“∂ß
				else
					uidTarget = m_pMyCharacter->GetUID();

				for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
				{
					ZCharacter* pCharacter = (ZCharacter*)(*itor).second;

					// Is champion or challenger
					if ((pCharacter->GetUID() == pDuel->QInfo.m_uidChampion) || (pCharacter->GetUID() == pDuel->QInfo.m_uidChallenger))
					{
						if (uidTarget == pCharacter->GetUID())
						{
							if (pCharacter->IsDie())
								bAddLose |= true;
							else
								bAddWin |= true;
						}
						else
						{
							if (pCharacter->IsDie())
								bAddWin |= true;
							else
								bAddLose |= true;
						}

						// √®««ø¬?Eµµ?E?ºˆ∏¶ ∏µŒ ¥ı«ÿº≠ 2∞° µ«æ˚⁄ﬂ «—¥Ÿ
						nCount++;
					}
				}

				// Draw
				if ((nCount < 2) || (bAddWin == bAddLose))
				{
					ZGetScreenEffectManager()->AddDraw();
					ZGetGameInterface()->PlayVoiceSound(VOICE_DRAW_GAME, 1200);
				}

				// Win
				else if (bAddWin)
				{
					ZGetScreenEffectManager()->AddWin();
					ZGetGameInterface()->PlayVoiceSound(VOICE_YOU_WON, 1000);
				}

				// Lose
				else
				{
					ZGetScreenEffectManager()->AddLose();
					ZGetGameInterface()->PlayVoiceSound(VOICE_YOU_LOSE, 1300);
				}
			}
		}
		else if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
		{
			if (!ZGetCombatInterface()->GetObserver()->IsVisible())	// ø…¡Æπˆ∞° æ∆¥œ?E
			{
				float fMaxHP = ZGetGame()->m_pMyCharacter->GetMaxHP();
				float fMaxAP = ZGetGame()->m_pMyCharacter->GetMaxAP();

				float fHP = ZGetGame()->m_pMyCharacter->GetHP();
				float fAP = ZGetGame()->m_pMyCharacter->GetAP();

				float fAccumulationDamage = ZGetGame()->m_pMyCharacter->GetAccumulationDamage();

				//ZPostDuelTournamentGamePlayerStatus(ZGetGame()->m_pMyCharacter->GetUID(), (int)(fHP*(100/fMaxHP)), (int)(fAP*(100/fMaxAP))); // πÈ∫–¿≤∑Œ ∫∏≥ª?E
				ZPostDuelTournamentGamePlayerStatus(ZGetGame()->m_pMyCharacter->GetUID(), fAccumulationDamage, fHP, fAP);

#ifndef _PUBLISH	// ≥ª∫Œ∫ÙµÂø°º≠ ¥©?E?EÃ¡ÅE¡§∫∏ √‚∑¬
				char szAccumulationDamagePrint[256];
				sprintf(szAccumulationDamagePrint, "¥©¿˚?EÃ¡?%2.1f] º≠πˆø° ∫∏≥ø", fAccumulationDamage);
				ZChatOutput(MCOLOR(255, 200, 200), szAccumulationDamagePrint);

#	ifdef _DUELTOURNAMENT_LOG_ENABLE_
				mlog(szAccumulationDamagePrint);
#	endif

#endif
				// «— ∂Ûø˚—Â∞° ≥°≥™?E¥©?E?EÃ¡ÅE√ ±‚»≠
				ZGetGame()->m_pMyCharacter->InitAccumulationDamage();
			}
		}
	}
	break;
	};
}

void ZGame::RoundEndDamage()
{
	/*if (m_Match.IsTeamPlay())
	{
		if (!ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType())
			&& !ZGetGameTypeManager()->IsQuestChallengeOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType())
			&& ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_DUEL
			&& ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_DUELTOURNAMENT)
		{
			char szFinishStr[512];

			if (IsReplay())
			{
				ZCharacter* pTarget = ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter();

				if (pTarget && pTarget->GetInitialized())
				{
					sprintf(szFinishStr, "'%s' dealt %.1f damage.", pTarget->GetUserName(), pTarget->m_fDamageCaused);
					ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szFinishStr);
				}
			}
			else if (m_pMyCharacter != NULL && !ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
			{
				sprintf(szFinishStr, "You dealt %.1f damage.", m_pMyCharacter->fPersonalDamageDealtCounter);
				ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szFinishStr);
			}

			if (m_pMyCharacter && !m_pMyCharacter->IsAdminHide())
			{
				float fTotalDamageTeam = 0.f;

				vector<pair<ZCharacter*, float> > vScores;

				for (ZCharacterManager::iterator itor = m_CharacterManager.begin(); itor != m_CharacterManager.end(); ++itor)
				{
					ZCharacter* pCharacter = (ZCharacter*)(*itor).second;

					if (!pCharacter->GetInitialized()) continue;
					if (pCharacter->IsAdminHide()) continue;

					if(pCharacter->GetTeamID() == ZGetGame()->m_pMyCharacter->GetTeamID())
					{
						fTotalDamageTeam += pCharacter->m_fDamageCaused;
						vScores.push_back(pair<ZCharacter*, float>(pCharacter, pCharacter->m_fDamageCaused));
					}
				}

				sort( vScores.begin(), vScores.end(), CompareZCharFloat );

				sprintf(szFinishStr, "Your team dealt %.1f damage in total.", fTotalDamageTeam);
				ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szFinishStr);

				if (vScores.size() > 0)
				{
					pair<ZCharacter*, float> p = vScores[vScores.size() - 1];
					sprintf(szFinishStr, "Your team's top scorer: %s (%.1f damage)", p.first->GetUserName(), p.second );
					ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szFinishStr);
				}

				vScores.clear();
			}
		}
	}*/
}

void ZGame::StartRecording()
{
	int nsscount = 0;

	char replayfilename[_MAX_PATH];
	char replayfilenameSafe[_MAX_PATH];
	char replayfoldername[_MAX_PATH];

	TCHAR szPath[MAX_PATH];
	if (GetMyDocumentsPath(szPath)) {
		strcpy(replayfoldername, szPath);
		strcat(replayfoldername, GUNZ_FOLDER);
		CreatePath(replayfoldername);
		strcat(replayfoldername, REPLAY_FOLDER);
		CreatePath(replayfoldername);
	}

	/*do {
		sprintf(replayfilename,"%s/Gunz%03d."GUNZ_REC_FILE_EXT , replayfoldername , nsscount);
		m_nGunzReplayNumber = nsscount;
		nsscount++;
	}
	while( IsExist(replayfilename) && nsscount<1000);

	if(nsscount==1000) goto RECORDING_FAIL;*/
	// ∆ƒ¿œ∏˙‹ª ¿œ∑√π¯»£ πÊΩƒø°º≠ ∞‘¿”¡§∫∏±‚¿‘ πÊΩƒ¿∏∑Œ ?E?
	SYSTEMTIME t;
	GetLocalTime(&t);
	char szCharName[MATCHOBJECT_NAME_LENGTH];
	ValidateFilename(szCharName, ZGetMyInfo()->GetCharName(), '_');

	const char* szGameTypeAcronym = "";
	char szValidatedOppoClanName[32] = "";
	int nCurrentRound = 0;
	//const char* szMapName = GetMatch()->GetMapName() ? GetMatch()->GetMapName() : "";

	bool bClanGame = ZGetGameClient()->IsCWChannel();

	REPLAY_STAGE_SETTING_NODE stageSettingNode;

	if (GetMatch()) {
		// ∞‘¿” ∏?Eæ‡¿⁄ √‚∑¬
		if (bClanGame) szGameTypeAcronym = "CLAN_";
		else szGameTypeAcronym = MMatchGameTypeAcronym[GetMatch()->GetMatchType()];

		if (GetMatch()->IsTeamPlay()) nCurrentRound = GetMatch()->GetCurrRound() + 1;

		// ≈¨∑£?E?∞Ê?EªÛ?E≈¨∑£?EæÀæ∆≥ø
		if (bClanGame) {
			const char* szOppositeClanName = "";

			if (0 == strcmp(ZGetMyInfo()->GetClanName(), ZGetCombatInterface()->GetRedClanName()))
				szOppositeClanName = ZGetCombatInterface()->GetBlueClanName();
			else
				szOppositeClanName = ZGetCombatInterface()->GetRedClanName();

			ValidateFilename(szValidatedOppoClanName, szOppositeClanName, '_');
		}
	}

	if (GetMatch()->IsTeamPlay())
	{
		// Custom: Making recordings more readable? New format (TeamMatches): GameType[Round#]_CharName_Year-Month-Day_Hour-Min-Second
		sprintf(replayfilename, "%s[%03d]_%s_%4d-%02d-%02d_%02d-%02d-%02d%s%s",
			szGameTypeAcronym, nCurrentRound, szCharName, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond,
			bClanGame ? "_" : "", szValidatedOppoClanName);
	}
	else
	{
		// Custom: Making recordings more readable? New format: GameType_CharName_Year-Month-Day_Hour-Min-Second
		sprintf_s(replayfilename, "%s_%s_%4d-%02d-%02d_%02d-%02d-%02d%s%s",
			szGameTypeAcronym, szCharName, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond,
			bClanGame ? "_" : "", szValidatedOppoClanName);
	}

	//sprintf(replayfilename, "%s_%s_%4d%02d%02d_%02d%02d%02d%s%s",
	//	szGameTypeAcronym, szCharName, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond,
	//	bClanGame ? "_" : "", szValidatedOppoClanName);

	sprintf(replayfilenameSafe, "%s_nocharname_%4d%02d%02d_%02d%02d%02d",
		szGameTypeAcronym, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);// ƒ≥∏Ø∏˙?E≈¨∑£?E?EπÆ¡¶∞° µ… ?E¿÷¥¬ πÆ¿⁄ø≠¿ª ª˝∑´«— πˆ?E

	char szFullPath[_MAX_PATH];

	strcpy(m_szReplayFileName, replayfilename);
	sprintf(szFullPath, "%s/%s." GUNZ_REC_FILE_EXT, replayfoldername, replayfilename);
	m_pReplayFile = zfopen(szFullPath, true);
	if (!m_pReplayFile)
	{
		strcpy(m_szReplayFileName, replayfilenameSafe);
		sprintf(szFullPath, "%s/%s." GUNZ_REC_FILE_EXT, replayfoldername, replayfilenameSafe);	// ∆ƒ¿œ∏˙“ßπÆ¿œ ?E¿÷¿∏¥œ ¿Ã∏ß¿ª ¥‹º¯»≠«ÿº≠ ¿ÁΩ√µµ
		m_pReplayFile = zfopen(szFullPath, true);

		if (!m_pReplayFile) goto RECORDING_FAIL;
	}

	int nWritten;

	DWORD header;
	header = GUNZ_REC_FILE_ID;
	nWritten = zfwrite(&header, sizeof(header), 1, m_pReplayFile);
	if (nWritten == 0) goto RECORDING_FAIL;

	header = GUNZ_REC_FILE_VERSION;
	nWritten = zfwrite(&header, sizeof(header), 1, m_pReplayFile);
	if (nWritten == 0) goto RECORDING_FAIL;

	ConvertStageSettingNodeForRecord(ZGetGameClient()->GetMatchStageSetting()->GetStageSetting(), &stageSettingNode);
	strcpy(stageSettingNode.szStageName, ZGetGameClient()->GetStageName());

	nWritten = zfwrite(&stageSettingNode, sizeof(REPLAY_STAGE_SETTING_NODE), 1, m_pReplayFile);
	if (nWritten == 0) goto RECORDING_FAIL;

	// ∞‘¿”?E∫∞ √ﬂ∞°¿˚¿Œ Ω∫≈◊¿Ã?Eºº∆√∞™ ¿˙?E
	if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_DUEL)
	{
		ZRuleDuel* pDuel = (ZRuleDuel*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		nWritten = zfwrite(&pDuel->QInfo, sizeof(MTD_DuelQueueInfo), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;
	}
	else if (IsGameRuleCTF(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		ZRuleTeamCTF* pTeamCTF = (ZRuleTeamCTF*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();

		MTD_CTFReplayInfo info;
		info.uidCarrierRed = pTeamCTF->GetRedCarrier();
		info.uidCarrierBlue = pTeamCTF->GetBlueCarrier();
		info.posFlagRed[0] = pTeamCTF->GetRedFlagPos().x;
		info.posFlagRed[1] = pTeamCTF->GetRedFlagPos().y;
		info.posFlagRed[2] = pTeamCTF->GetRedFlagPos().z;
		info.posFlagBlue[0] = pTeamCTF->GetBlueFlagPos().x;
		info.posFlagBlue[1] = pTeamCTF->GetBlueFlagPos().y;
		info.posFlagBlue[2] = pTeamCTF->GetBlueFlagPos().z;
		info.nFlagStateRed = pTeamCTF->GetRedFlagState();
		info.nFlagStateBlue = pTeamCTF->GetBlueFlagState();

		nWritten = zfwrite(&info, sizeof(MTD_CTFReplayInfo), 1, m_pReplayFile);

		//nWritten = zfwrite(&pTeamCTF->GetRedCarrier(),sizeof(MUID),1,m_pReplayFile);
		//nWritten = zfwrite(&pTeamCTF->GetBlueCarrier(),sizeof(MUID),1,m_pReplayFile);
		//nWritten = zfwrite(&pTeamCTF->GetRedFlagPos(),sizeof(rvector),1,m_pReplayFile);
		//nWritten = zfwrite(&pTeamCTF->GetBlueFlagPos(),sizeof(rvector),1,m_pReplayFile);
		//int nRedFlagState = (int)pTeamCTF->GetRedFlagState();
		//int nBlueFlagState = (int)pTeamCTF->GetBlueFlagState();
		//nWritten = zfwrite(&nRedFlagState,sizeof(int),1,m_pReplayFile);
		//nWritten = zfwrite(&nBlueFlagState,sizeof(int),1,m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;
	}
	else if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_INFECTED)
	{
		ZRuleTeamInfected* pTeamInfected = (ZRuleTeamInfected*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		nWritten = zfwrite(&pTeamInfected->m_uidPatientZero, sizeof(MUID), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;
	}
	else if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_GUNGAME)
	{
		// TODO: WRITE all characters gungameweapondata
		ZRuleGunGame* pRuleGG = (ZRuleGunGame*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		nWritten = zfwrite(&pRuleGG->m_nWeaponMaxLevel, sizeof(int), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		int nCharacterCount = (int)m_CharacterManager.size();
		nWritten = zfwrite(&nCharacterCount, sizeof(nCharacterCount), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		for (ZCharacterManager::iterator itor = m_CharacterManager.begin(); itor != m_CharacterManager.end(); ++itor)
		{
			ZCharacter* pCharacter = (ZCharacter*)(*itor).second;

			MTD_GunGameWeaponInfo rd;
			rd.uidPlayer = pCharacter->GetUID();
			memcpy(rd.nWeaponID, pCharacter->m_nGunGameWeaponID, sizeof(rd.nWeaponID));
			rd.nWeaponLevel = pCharacter->m_nGunGameWeaponLevel;

			nWritten = zfwrite((void*)&rd, sizeof(MTD_GunGameWeaponInfo), 1, m_pReplayFile);
			if (nWritten == 0) goto RECORDING_FAIL;
		}
	}
	else if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
	{
		int nType = (int)ZGetGameInterface()->GetDuelTournamentType();
		nWritten = zfwrite(&nType, sizeof(int), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		const vector<DTPlayerInfo>& vecDTPlayerInfo = ZGetGameInterface()->GetVectorDTPlayerInfo();

		int nCount = (int)vecDTPlayerInfo.size();
		nWritten = zfwrite(&nCount, sizeof(int), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		nWritten = zfwrite((void*)&vecDTPlayerInfo[0], sizeof(DTPlayerInfo), nCount, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		ZRuleDuelTournament* pRule = (ZRuleDuelTournament*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		nWritten = zfwrite((void*)&pRule->m_DTGameInfo, sizeof(MTD_DuelTournamentGameInfo), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;
	}
	else if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_SPY)
	{
		ZRuleSpy* pSpy = (ZRuleSpy*)m_Match.GetRule();

		int nSpyItemCount = (int)pSpy->m_vtLastSpyItem.size();
		nWritten = zfwrite(&nSpyItemCount, sizeof(int), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		nWritten = zfwrite((void*)&pSpy->m_vtLastSpyItem[0], sizeof(MMatchSpyItem), nSpyItemCount, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		int nTrackerItemCount = (int)pSpy->m_vtLastTrackerItem.size();
		nWritten = zfwrite(&nTrackerItemCount, sizeof(int), 1, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;

		nWritten = zfwrite((void*)&pSpy->m_vtLastTrackerItem[0], sizeof(MMatchSpyItem), nTrackerItemCount, m_pReplayFile);
		if (nWritten == 0) goto RECORDING_FAIL;
	}
	int nCharacterCount = (int)m_CharacterManager.size();
	nWritten = zfwrite(&nCharacterCount, sizeof(nCharacterCount), 1, m_pReplayFile);
	if (nWritten == 0) goto RECORDING_FAIL;

	for (ZCharacterManager::iterator itor = m_CharacterManager.begin(); itor != m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
		if (!pCharacter->Save(m_pReplayFile)) goto RECORDING_FAIL;
	}

	//// Custom: CQ
	//for (auto itor = ZGetObjectManager()->begin(); itor != ZGetObjectManager()->end(); ++itor)
	//{
	//	ZActorWithFSM* pObj = (ZActorWithFSM*)(*itor).second;
	//	if (!pObj->Save(m_pReplayFile)) goto RECORDING_FAIL;

	//}

	//nWritten = zfwrite(&m_fTime,sizeof(m_fTime),1,m_pReplayFile);
	float fTime = m_fTime.Ref();
	nWritten = zfwrite(&fTime, sizeof(float), 1, m_pReplayFile);
	if (nWritten == 0) goto RECORDING_FAIL;

	m_bRecording = true;
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM),
		ZMsg(MSG_RECORD_STARTING));
	return;

RECORDING_FAIL:	// Ω«∆–

	if (m_pReplayFile)
	{
		zfclose(m_pReplayFile);
		m_pReplayFile = NULL;
	}

	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ZMsg(MSG_RECORD_CANT_SAVE));
}
void ZGame::StopRecording()
{
	if (!m_bRecording) return;

	bool bError = false;

	m_bRecording = false;

	ZObserverCommandList::iterator itr = m_ReplayCommandList.begin();
	for (size_t i = 0; i < m_ReplayCommandList.size(); i++)
	{
		ZObserverCommandItem* pItem = *itr;
		MCommand* pCommand = pItem->pCommand;

		const int BUF_SIZE = 1024;
		char CommandBuffer[BUF_SIZE];
		int nSize = pCommand->GetData(CommandBuffer, BUF_SIZE);

		int nWritten;
		nWritten = zfwrite(&pItem->fTime, sizeof(pItem->fTime), 1, m_pReplayFile);
		if (nWritten == 0) { bError = true; break; }
		nWritten = zfwrite(&pCommand->m_Sender, sizeof(pCommand->m_Sender), 1, m_pReplayFile);
		if (nWritten == 0) { bError = true; break; }
		nWritten = zfwrite(&nSize, sizeof(nSize), 1, m_pReplayFile);
		if (nWritten == 0) { bError = true; break; }
		nWritten = zfwrite(CommandBuffer, nSize, 1, m_pReplayFile);
		if (nWritten == 0) { bError = true; break; }

		itr++;
	}

	while (m_ReplayCommandList.size())
	{
		ZObserverCommandItem* pItem = *m_ReplayCommandList.begin();
		delete pItem->pCommand;
		delete pItem;
		m_ReplayCommandList.pop_front();
	}

	if (!zfclose(m_pReplayFile))
		bError = true;

	if (bError)
	{
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ZMsg(MSG_RECORD_CANT_SAVE));
	}
	else
	{
		char szOutputFilename[256];
		sprintf(szOutputFilename, GUNZ_FOLDER REPLAY_FOLDER "/%s." GUNZ_REC_FILE_EXT, m_szReplayFileName);

		char szOutput[256];
		// ZTranslateMessage(szOutput,MSG_RECORD_SAVED,1,szOutputFilename);
		ZTransMsg(szOutput, MSG_RECORD_SAVED, 1, szOutputFilename);
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szOutput);
	}
}

void ZGame::ToggleRecording()
{
	if (m_bReplaying.Ref()) return;	// ¿Áª˝¡ﬂ ?E≠∫“∞° -_-;

	// ƒ˘Ω∫∆Æ¥¬ ?E≠µ«?Eæ ¥¬¥Ÿ
	// Custom: Add CQ to this
	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType())
		|| ZGetGameTypeManager()->IsQuestChallengeOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) || ZGetGameTypeManager()->IsBlitzQuest(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
		return;

	if (!m_bRecording)
		StartRecording();
	else
		StopRecording();
}

DWORD dwReplayStartTime;

bool ZGame::OnLoadReplay(ZReplayLoader * pLoader)
{
	//m_fTime = pLoader->GetGameTime();
	m_fTime.Set_CheckCrc(pLoader->GetGameTime());

	m_bReplaying.Set_CheckCrc(true);
	SetReadyState(ZGAME_READYSTATE_RUN);
	GetMatch()->SetRoundState(MMATCH_ROUNDSTATE_FREE);
	ZGetGameInterface()->GetCombatInterface()->SetObserverMode(true);
	//Esto elimina la repeticiÛn cuando el objetivo es un administrador que se esconde como administrador.
	//ZGetGameInterface()->GetCombatInterface()->GetObserver()->SetTarget(ZGetGame()->m_pMyCharacter->GetUID());
	g_bProfile = true;
	dwReplayStartTime = timeGetTime();

	/*
		size_t n;

		m_bReplaying=true;

		int nCharacterCount;
		zfread(&nCharacterCount,sizeof(nCharacterCount),1,file);

		ZGetCharacterManager()->Clear();
		m_ObjectManager.Clear();

		for(int i=0;i<nCharacterCount;i++)
		{
			bool bHero;
			n=zfread(&bHero,sizeof(bHero),1,file);
			if(n!=1) return false;

			MTD_CharInfo info;

			if(nVersion<2) {
				n=zfread(&info,sizeof(info)-4,1,file);
				if(n!=1) return false;
				info.nClanCLID = 0;
			}
			else {
				n=zfread(&info,sizeof(info),1,file);
				if(n!=1) return false;
			}

			ZCharacter *pChar=NULL;
			if(bHero)
			{
				m_pMyCharacter=new ZMyCharacter;
				CreateMyCharacter(&info);
				pChar=m_pMyCharacter;
				pChar->Load(file,nVersion);
			}else
			{
				pChar=new ZNetCharacter;
				pChar->Load(file,nVersion);
				pChar->Create(&info);
			}

			ZGetCharacterManager()->Add(pChar);
			mlog("%s : %d %d\n",pChar->GetProperty()->szName,pChar->GetUID().High,pChar->GetUID().Low);

			pChar->SetVisible(true);
		}

		float fGameTime;
		zfread(&fGameTime,sizeof(fGameTime),1,file);
		m_fTime=fGameTime;

		int nCommandCount=0;

		int nSize;
		float fTime;
		while( zfread(&fTime,sizeof(fTime),1,file) )
		{
			nCommandCount++;

			char CommandBuffer[1024];

			MUID uidSender;
			zfread(&uidSender,sizeof(uidSender),1,file);
			zfread(&nSize,sizeof(nSize),1,file);
			if(nSize<0 || nSize>sizeof(CommandBuffer)) {
				m_bReplaying=false;
				ShowReplayInfo( true);
				return false;
			}
			zfread(CommandBuffer,nSize,1,file);

			ZObserverCommandItem *pZCommand=new ZObserverCommandItem;
			pZCommand->pCommand=new MCommand;
			pZCommand->pCommand->SetData(CommandBuffer,ZGetGameClient()->GetCommandManager());
			pZCommand->pCommand->m_Sender=uidSender;
			pZCommand->fTime=fTime;
			m_ReplayCommandList.push_back(pZCommand);
		}

		SetReadyState(ZGAME_READYSTATE_RUN);
		GetMatch()->SetRoundState(MMATCH_ROUNDSTATE_FREE);
		ZGetGameInterface()->GetCombatInterface()->SetObserverMode(true);

		ZGetGameInterface()->GetCombatInterface()->GetObserver()->SetTarget(m_pMyCharacter->GetUID());

		g_bProfile=true;

		dwReplayStartTime=timeGetTime();

		return true;
	*/
	return true;
}

void ZGame::EndReplay()
{
	g_bProfile = false;

	DWORD dwReplayEndTime = timeGetTime();

	mlog("replay end. profile saved. playtime = %3.3f seconds , average fps = %3.3f \n",
		float(dwReplayEndTime - dwReplayStartTime) / 1000.f,
		1000.f * g_nFrameCount / float(dwReplayEndTime - dwReplayStartTime));

	// ∏Æ«√∑π¿Ã∞° ¥Ÿ ≥°≥™?E¥ŸΩ√ √≥¿Ω∫Œ≈Õ µπ∑¡∫∏¿⁄. - (πˆ?E
	if (ZGetGameInterface()->GetPreviousState() == GUNZ_LOGIN || !ZGetGameClient()->IsConnected())
	{
		ZChangeGameState(GUNZ_LOGIN);
	}
	else
		ZChangeGameState(GUNZ_LOBBY);
}

void ZGame::ConfigureCharacter(const MUID & uidChar, MMatchTeam nTeam, unsigned char nPlayerFlags)
{
	ZCharacterManager* pCharMgr = ZGetCharacterManager();
	ZCharacter* pChar = (ZCharacter*)pCharMgr->Find(uidChar);
	if (pChar == NULL) return;

	pChar->SetAdminHide((nPlayerFlags & MTD_PlayerFlags_AdminHide) != 0);
	pChar->SetTeamID(nTeam);
	pChar->InitStatus();
	pChar->InitRound();

	ZGetCombatInterface()->OnAddCharacter(pChar);
}

void ZGame::RefreshCharacters()
{
	for (MMatchPeerInfoList::iterator itor = ZGetGameClient()->GetPeers()->begin();
		itor != ZGetGameClient()->GetPeers()->end(); ++itor)
	{
		MMatchPeerInfo* pPeerInfo = (*itor).second;
		ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(pPeerInfo->uidChar);

		if (pCharacter == NULL) {
			pCharacter = m_CharacterManager.Add(pPeerInfo->uidChar, rvector(0.0f, 0.0f, 0.0f));
			pCharacter->Create(&pPeerInfo->CharInfo);

			if (m_Match.GetRoundState() == MMATCH_ROUNDSTATE_PREPARE)
			{
				if (m_Match.IsTeamPlay())
				{
					//					pCharacter->SetVisible(true);		// RAONHAJE: PeerOpened TEST
				}
			}

			/*
			// TODO: AGENT ?E?peerµÈø°∞‘¥¬ æ√»˘¥Ÿ. ºˆ¡§ø‰∏¡.

			//Ω≈¿‘ ƒ≥∏Ø≈Õø°∞‘ ¿⁄Ω≈¿« π´±‚∏¶ æÀ∏∞¥Ÿ...

			ZCharacter* pMyCharacter = g_pGame->m_pMyCharacter;
			if(pMyCharacter)
			{
				//			if(pMyCharacter != pCharacter) { // ¿⁄Ω≈¿Ã ªı∑Œ ª˝?Eƒ≥∏Ø¿Ã æ∆¥œ∂Û?E
				int nParts = g_pGame->m_pMyCharacter->GetItems()->GetSelectedWeaponParts();
				ZPostChangeWeapon(nParts);
				//			}
			}
			*/
		}
	}
}

void ZGame::DeleteCharacter(const MUID & uid)
{
	bool bObserverDel = false;
	ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(uid);

	// ø…¡Æ?E≈∏∞Ÿ¿Œ ∞Ê?E¥Ÿ∏• ≈∏∞Ÿ¿∏∑Œ πŸ≤„¡ÿ¥Ÿ.
	ZObserver* pObserver = ZGetGameInterface()->GetCombatInterface()->GetObserver();
	if (pObserver->IsVisible())
	{
		if ((pCharacter != NULL) && (pCharacter == pObserver->GetTargetCharacter()))
		{
			bObserverDel = true;
		}
	}

	m_CharacterManager.Delete(uid);

	if (bObserverDel)
	{
		if (pObserver) pObserver->SetFirstTarget();
	}
}

void ZGame::OnStageEnterBattle(MCmdEnterBattleParam nParam, MTD_PeerListNode * pPeerNode)
{
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;

	MUID uidChar = pPeerNode->uidChar;

	if (uidChar == ZGetMyUID())		// enter«—ªÁ∂˜¿Ã ≥™¿⁄Ω≈¿œ ∞Ê?E
	{

		ZGetApplication()->RunAntiThread();

		if (ZGetGame()->CreateMyCharacter(&pPeerNode->CharInfo/*, &pPeerNode->CharBuffInfo*/) == true)
		{
			ConfigureCharacter(uidChar, (MMatchTeam)pPeerNode->ExtendInfo.nTeam, pPeerNode->ExtendInfo.nPlayerFlags);	// Player Character ∆˜«‘
		}
	}
	else							// enter«—ªÁ∂˜¿Ã ≥™ ¿⁄Ω≈¿Ã æ∆¥“∞Ê?E
	{
		if (ZGetGameClient()->m_bAdminNAT)
			OnAddPeer(pPeerNode->uidChar, 0, 0, pPeerNode);
		else
			OnAddPeer(pPeerNode->uidChar, pPeerNode->dwIP, pPeerNode->nPort, pPeerNode);
	}

	if (nParam == MCEP_FORCED)
	{
		ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uidChar);
		GetMatch()->OnForcedEntry(pChar);

		char temp[256] = "";
		if ((pPeerNode->ExtendInfo.nPlayerFlags & MTD_PlayerFlags_AdminHide) == 0) {
			ZTransMsg(temp, MSG_GAME_JOIN_BATTLE, 1, pChar->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
		}
#ifdef _REPLAY_TEST_LOG
		mlog("[Add Character %s(%d)]\n", pChar->GetCharInfo()->szName, uidChar.Low);
#endif
	}

	ZGetGameClient()->OnStageEnterBattle(uidChar, nParam);
}

void ZGame::OnStageLeaveBattle(const MUID & uidChar, const bool bIsRelayMap)//, const MUID& uidStage)
{
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;

	// Custom: Prevents recordings from not getting saved if you quit a room.
	if (ZGetGame()->m_bRecording && uidChar == ZGetMyUID()) ZGetGame()->ToggleRecording();

	if (uidChar == ZGetMyUID())
	{
		ZGetApplication()->KillAntiThread();
	}

	if (uidChar != ZGetMyUID()) 
	{
		ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uidChar);

		if (pChar && !pChar->IsAdminHide() && !bIsRelayMap) {
			char temp[256] = "";
			ZTransMsg(temp, MSG_GAME_LEAVE_BATTLE, 1, pChar->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
		}

		ZGetGameClient()->DeletePeer(uidChar);
		if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME) {
			DeleteCharacter(uidChar);
		}

		ZGetGameClient()->SetVoteInProgress(false);
		ZGetGameClient()->SetCanVote(false);
	}
}

void ZGame::OnAddPeer(const MUID & uidChar, DWORD dwIP, const int nPort, MTD_PeerListNode * pNode)
{
	if ((ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) || (ZGetGame() == NULL)) return;

	/*
	//// UDPTEST LOG ////////////////////////////////
	char szLog[256];
	sprintf(szLog, "[%d:%d] ADDPEER: Char(%d:%d) IP:%s, Port:%d \n",
	GetPlayerUID().High, GetPlayerUID().Low, uidChar.High, uidChar.Low, szIP, nPort);
	mlog(szLog);
	/////////////////////////////////////////////////
	*/

	// √ﬂ∞°µ» ªÁ∂˜¿Ã ¿⁄?E¿⁄Ω≈¿Ã æ∆¥œ?E
	if (uidChar != ZGetMyUID())
	{
		if (pNode == NULL) {
			_ASSERT(0);
		}

		ZGetGameClient()->DeletePeer(uidChar);	// Delete exist info

		MMatchPeerInfo* pNewPeerInfo = new MMatchPeerInfo;

		if (uidChar == MUID(0, 0))	pNewPeerInfo->uidChar = MUID(0, nPort);	// ∑Œƒ√≈◊Ω∫∆Æ∏¶ ¿ß«ÿº≠
		else						pNewPeerInfo->uidChar = uidChar;

		in_addr addr;
		addr.s_addr = dwIP;
		char* pszIP = inet_ntoa(addr);
		strcpy(pNewPeerInfo->szIP, pszIP);

		pNewPeerInfo->dwIP = dwIP;
		pNewPeerInfo->nPort = nPort;

		if (!IsReplay())
			memcpy(&pNewPeerInfo->CharInfo, &(pNode->CharInfo), sizeof(MTD_CharInfo));
		else
		{
			MTD_CharInfo currInfo;
			ConvertCharInfo(&currInfo, &pNode->CharInfo, ZReplayLoader::m_nVersion);
			memcpy(&pNewPeerInfo->CharInfo, &currInfo, sizeof(MTD_CharInfo));
		}
		//πˆ«¡¡§∫∏¿”Ω√¡÷ºÆ memcpy(&pNewPeerInfo->CharBuffInfo, &(pNode->CharBuffInfo), sizeof(MTD_CharBuffInfo));
		memcpy(&pNewPeerInfo->ExtendInfo, &(pNode->ExtendInfo), sizeof(MTD_ExtendInfo));

		ZGetGameClient()->AddPeer(pNewPeerInfo);

		RefreshCharacters();
	}

	ConfigureCharacter(uidChar, (MMatchTeam)pNode->ExtendInfo.nTeam, pNode->ExtendInfo.nPlayerFlags);	// Player Character ∆˜«‘
}

void ZGame::OnPeerList(const MUID & uidStage, void* pBlob, int nCount)
{
	if (ZGetGameClient()->GetStageUID() != uidStage) return;
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;
	if ((ZGetGame() == NULL) || (ZGetCharacterManager() == NULL)) return;

	for (int i = 0; i < nCount; i++) {
		MTD_PeerListNode* pNode = (MTD_PeerListNode*)MGetBlobArrayElement(pBlob, i);
		OnAddPeer(pNode->uidChar, pNode->dwIP, pNode->nPort, pNode);

		ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(pNode->uidChar);
		if (pChar) {
			pChar->SetVisible(false);
		}
	}
}

void ZGame::PostMyBuffInfo()
{
	// ≥ª∞‘ πﬂµøµ«?E¿÷¥¬ πˆ«¡ ªÛ≈¬∏¶ ««æ˚—Èø°∞‘ æÀ∑¡¡ÿ¥Ÿ
	if (m_pMyCharacter)
	{
		void* pBlob = m_pMyCharacter->MakeBuffEffectBlob();
		if (pBlob)
		{
			ZPostBuffInfo(pBlob);
			MEraseBlobArray(pBlob);
		}
	}
}

void ZGame::OnPeerBuffInfo(const MUID & uidSender, void* pBlobBuffInfo)
{
	if (uidSender == ZGetMyUID()) return;

	ZCharacter* pSender = (ZCharacter*)ZGetCharacterManager()->Find(uidSender);
	if (!pSender) return;
	if (!pBlobBuffInfo) return;

	MTD_BuffInfo* pBuffInfo = NULL;
	int numElem = MGetBlobArrayCount(pBlobBuffInfo);

	// Custom: Exploit fix (MTD_PeerBuffInfo blob overflow)
	if (MGetBlobArraySize(pBlobBuffInfo) != (8 + (sizeof(MTD_BuffInfo) * numElem)))
	{
		return;
	}

	for (int i = 0; i < numElem; ++i)
	{
		pBuffInfo = (MTD_BuffInfo*)MGetBlobArrayElement(pBlobBuffInfo, i);

		ApplyPotion(pBuffInfo->nItemId, pSender, (float)pBuffInfo->nRemainedTime);
	}
}

void ZGame::OnGameRoundState(const MUID & uidStage, int nRound, int nRoundState, int nArg)
{
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;
	ZMatch* pMatch = GetMatch();
	if (pMatch == NULL) return;

	MMATCH_ROUNDSTATE RoundState = MMATCH_ROUNDSTATE(nRoundState);

	// «¡∏Æ∏µÂ¿œ∞Ê?E
	if ((RoundState == MMATCH_ROUNDSTATE_FREE) && (pMatch->GetRoundState() != RoundState))
	{
		pMatch->InitCharactersPosition();
		m_pMyCharacter->SetVisible(true);
		m_pMyCharacter->Revival();
		ReleaseObserver();
	}

	pMatch->SetRound(nRound);
	pMatch->SetRoundState(RoundState, nArg);
	AddEffectRoundState(RoundState, nArg);

	if (RoundState == MMATCH_ROUNDSTATE_FINISH)
	{
#ifdef _VMPROTECT
		VMProtectBeginUltra("RoundStateFinishValidImage");
		if (!VMProtectIsValidImageCRC())
		{
			ZPostBanMe(WHT_MEMCMP);
			if (!g_ShouldBanPlayer)
				g_ShouldBanPlayer = timeGetTime();
}
		VMProtectEnd();
#endif
}
}

bool ZGame::FilterDelayedCommand(MCommand * pCommand)
{
	bool bFiltered = true;
	float fDelayTime = 0;

	MUID uid = pCommand->GetSenderUID();
	ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uid);
	if (!pChar) return false;

	switch (pCommand->GetID())
	{
	case MC_PEER_SKILL:
	{
		int nSkill;
		pCommand->GetParameter(&nSkill, 0, MPT_INT);
		fDelayTime = .15f;
		switch (nSkill) {
		case ZC_SKILL_UPPERCUT:
			if (pChar != m_pMyCharacter) pChar->SetAnimationLower(ZC_STATE_LOWER_UPPERCUT);
			break;
		case ZC_SKILL_SPLASHSHOT: break;
		case ZC_SKILL_DASH: break;
		}

		////////////////////////////////////////////////////////////////////
		int sel_type;
		pCommand->GetParameter(&sel_type, 2, MPT_INT);
		MMatchCharItemParts parts = (MMatchCharItemParts)sel_type;
		if (parts != pChar->GetItems()->GetSelectedWeaponParts()) {
			// ¡ˆ±› µÈ?E¿÷¥¬ π´±‚øÕ ∫∏≥ª?Eπ´±‚∞° ∆≤∏Æ¥Ÿ?E∫∏≥ª?Eπ´±‚∑Œ πŸ≤„¡ÿ¥Ÿ..
			OnChangeWeapon(uid, parts);
		}
	}break;

	case MC_PEER_SHOT:
	{
		MCommandParameter* pParam = pCommand->GetParameter(0);
		if (pParam->GetType() != MPT_BLOB) break;	// πÆ¡¶∞° ¿÷¥Ÿ
		ZPACKEDSHOTINFO* pinfo = (ZPACKEDSHOTINFO*)pParam->GetPointer();

		// ƒÆ¡˙∏∏ µÙ∑π¿Ã∞° ¿÷¥Ÿ
		if (pinfo->sel_type != MMCIP_MELEE) return false;

		if (pChar != m_pMyCharacter &&
			(pChar->m_pVMesh->GetSelectWeaponMotionType() == eq_wd_dagger ||
				pChar->m_pVMesh->GetSelectWeaponMotionType() == eq_ws_dagger)) { // dagger
			pChar->SetAnimationUpper(ZC_STATE_UPPER_SHOT);
		}

		fDelayTime = .15f;

		////////////////////////////////////////////////////////////////////
		MMatchCharItemParts parts = (MMatchCharItemParts)pinfo->sel_type;
		if (parts != pChar->GetItems()->GetSelectedWeaponParts()) {
			// ¡ˆ±› µÈ?E¿÷¥¬ π´±‚øÕ ∫∏≥ª?Eπ´±‚∞° ∆≤∏Æ¥Ÿ?E∫∏≥ª?Eπ´±‚∑Œ πŸ≤„¡ÿ¥Ÿ..
			OnChangeWeapon(uid, parts);
		}
		///////////////////////////////////////////////////////////////////////////////
	}
	break;

	// ªı∑Œ √ﬂ∞°µ» ±Ÿ¡¢∞¯∞› ƒø∏«?E
	case MC_PEER_SHOT_MELEE:
	{
		float fShotTime;
		rvector pos;
		int nShot;

		pCommand->GetParameter(&fShotTime, 0, MPT_FLOAT);
		pCommand->GetParameter(&pos, 1, MPT_POS);
		pCommand->GetParameter(&nShot, 2, MPT_INT);

		if (pChar != m_pMyCharacter &&
			(pChar->m_pVMesh->GetSelectWeaponMotionType() == eq_wd_dagger ||
				pChar->m_pVMesh->GetSelectWeaponMotionType() == eq_ws_dagger)) { // dagger
			pChar->SetAnimationUpper(ZC_STATE_UPPER_SHOT);
		}

		fDelayTime = .1f;
		switch (nShot) {
		case 1: fDelayTime = .10f; break;
		case 2: fDelayTime = .15f; break;
		case 3: fDelayTime = .2f; break;
		case 4: fDelayTime = .25f; break;
		}

		if (nShot > 1)
		{
			char szFileName[20];
			if (pChar->GetProperty()->nSex == MMS_MALE)
				sprintf(szFileName, "fx2/MAL_shot_%02d", nShot);
			else
				sprintf(szFileName, "fx2/FEM_shot_%02d", nShot);

			ZGetSoundEngine()->PlaySound(szFileName, pChar->GetPosition());
		}
	}
	break;

	case MC_QUEST_PEER_NPC_ATTACK_MELEE:
		ZGetQuest()->OnPrePeerNPCAttackMelee(pCommand);
		fDelayTime = .4f;
		break;

		// ?E°º≠µµ « ≈Õ∏µ¿Ã æ»µ«?E¿Ã∞« delayed command∞° æ∆¥œ¥Ÿ
	default:
		bFiltered = false;
		break;
	}

	if (bFiltered)
	{
		ZObserverCommandItem* pZCommand = new ZObserverCommandItem;
		pZCommand->pCommand = pCommand->Clone();
		pZCommand->fTime = GetTime() + fDelayTime;
		m_DelayedCommandList.push_back(pZCommand);
		return true;
	}

	return false;
}

void ZGame::PostSpMotion(ZC_SPMOTION_TYPE mtype)
{
	if (m_pMyCharacter == NULL) return;
	// Custom: Added MMATCH_ROUNDSTATE_FREE to allowed emotion list
	if (m_Match.GetRoundState() != MMATCH_ROUNDSTATE_PLAY && m_Match.GetRoundState() != MMATCH_ROUNDSTATE_FREE) return;

	// Custom: Macro spam fix
	if (ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
		return;

	if ((m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE1) ||
		(m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE2) ||
		(m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE3) ||
		(m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE4))
	{
		MMatchWeaponType type = MWT_NONE;

		ZItem* pSItem = m_pMyCharacter->GetItems()->GetSelectedWeapon();

		if (pSItem && pSItem->GetDesc()) {
			type = pSItem->GetDesc()->m_nWeaponType.Ref();
		}

		if (mtype == ZC_SPMOTION_TAUNT) // taunt ¿œ ∞Ê?E∏º«¿Ã æ¯æ˚ÿ≠...
			if ((type == MWT_MED_KIT) ||
				(type == MWT_REPAIR_KIT) ||
				(type == MWT_FOOD) ||
				(type == MWT_LANDMINE_SPY) ||
				(type == MWT_BULLET_KIT))
			{
				return;
			}

		ZPostSpMotion(mtype);
	}
}

void ZGame::OnEventUpdateJjang(const MUID & uidChar, bool bJjang)
{
	ZCharacter* pCharacter = (ZCharacter*)m_CharacterManager.Find(uidChar);
	if (pCharacter == NULL) return;

	if (bJjang)
		ZGetEffectManager()->AddStarEffect(pCharacter);
}

//bool ZGame::CanAttack(ZObject *pAttacker, ZObject *pTarget)
//{
//	if(!IsReplay())
//		if(GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY) return false;
//
//	if(pAttacker==NULL) return true;
//
//	if (pTarget == NULL)
//		return false;
//	if ( GetMatch()->IsTeamPlay() )
//	{
//		__try
//		{
//			if (pAttacker->GetTeamID() == pTarget->GetTeamID()) {
//				if (!GetMatch()->GetTeamKillEnabled())
//					return false;
//			}
//		}
//		__except(EXCEPTION_EXECUTE_HANDLER)
//		{
//			mlog("MERROR_1\n");
//			return false;
//		}
//	}
//#ifdef _QUEST
//	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) ||
//		ZGetGameTypeManager()->IsQuestChallengeOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
//	{
//		if (pAttacker->GetTeamID() == pTarget->GetTeamID())
//		{
//			return false;
//		}
//	}
//#endif
//	return true;
//}
bool ZGame::CanAttack(ZObject * pAttacker, ZObject * pTarget)
{
	//### ¿Ã «‘ºˆ∏¶ ºˆ¡§«œ∏È ∂»∞∞¿Ã CanAttack_DebugRegister()ø°µµ ¿˚øÎ«ÿ ¡÷æÓæﬂ «’¥œ¥Ÿ. ###
	if (!IsReplay())
		if (GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY) return false;

	if (pAttacker == NULL) return true;

	if (GetMatch()->IsTeamPlay()) {
		if (pAttacker->GetTeamID() == pTarget->GetTeamID())
		{
			if (!GetMatch()->GetTeamKillEnabled())
				return false;
		}
	}

	if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SKILLMAP)
		return false;

#ifdef _QUEST
	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) ||
		ZGetGameTypeManager()->IsQuestChallengeOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) || ZGetGameTypeManager()->IsBlitzQuest(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		if (pAttacker->GetTeamID() == pTarget->GetTeamID())
		{
			return false;
		}
	}

#endif
	return true;
}
//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ πÊ¡ˆ∏¶ ¿ß«ÿ ISAttackable()«‘ºˆ∏¶ ¥Ÿ∏• ¿Ã∏ß¿∏∑Œ «œ≥™ ?E∏∏µÈæ˙¿Ω...
bool ZGame::CanAttack_DebugRegister(ZObject * pAttacker, ZObject * pTarget)
{
	if (!IsReplay())
		if (GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY) return false;
	if (pAttacker == NULL) return true;

	// Custom: Silly Gunz crash fix
	if (pTarget == NULL)
		return false;

	if (GetMatch()->IsTeamPlay()) {
		if (pAttacker->GetTeamID() == pTarget->GetTeamID()) {
			if (!GetMatch()->GetTeamKillEnabled())
				return false;
		}
	}

	if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SKILLMAP)
		return false;

#ifdef _QUEST
	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) ||
		MMATCH_GAMETYPE_QUEST_CHALLENGE == ZGetGameClient()->GetMatchStageSetting()->GetGameType())
	{
		if (pAttacker->GetTeamID() == pTarget->GetTeamID())
		{
			return false;
		}
	}

#endif
	return true;
}

void ZGame::ShowReplayInfo(bool bShow)
{
	MWidget* pWidget = ZGetGameInterface()->GetIDLResource()->FindWidget("CombatChatOutput");
	if (pWidget)
		pWidget->Show(bShow);

	m_bShowReplayInfo = bShow;

	GetRGMain().GetChat().HideDuringReplays = !bShow;
}

void ZGame::OnLocalOptainSpecialWorldItem(MCommand * pCommand)
{
	int nWorldItemID;
	pCommand->GetParameter(&nWorldItemID, 0, MPT_INT);

	switch (nWorldItemID)
	{
	case WORLDITEM_PORTAL_ID:
	{
		MMATCH_GAMETYPE eGameType = ZGetGameClient()->GetMatchStageSetting()->GetGameType();
		if (!ZGetGameTypeManager()->IsQuestDerived(eGameType) || ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_QUEST_CHALLENGE) break;

		// º≠πˆø° ∆˜≈ª∑Œ ¿Ãµø«—¥Ÿ?E?E?
		char nCurrSectorIndex = ZGetQuest()->GetGameInfo()->GetCurrSectorIndex();
		ZPostQuestRequestMovetoPortal(nCurrSectorIndex);
	}
	break;
	};
}

void ZGame::ReserveSuicide(void)
{
	m_bSuicide = true;
}

bool ZGame::OnRuleCommand(MCommand * pCommand)
{
	if (m_Match.GetMatchType() == MMATCH_GAMETYPE_QUEST_CHALLENGE)
	{
		return m_Match.OnCommand(pCommand);
	}
	if (m_Match.GetMatchType() == MMATCH_GAMETYPE_BLITZKRIEG)
	{
		return m_Match.OnCommand(pCommand);
	}
#ifdef _QUEST
	if (ZGetQuest()->OnGameCommand(pCommand)) return true;
#endif

	switch (pCommand->GetID())
	{
	case MC_MATCH_ASSIGN_COMMANDER:
	case MC_MATCH_ASSIGN_BERSERKER:
	case MC_MATCH_FLAG_EFFECT:
	case MC_MATCH_FLAG_CAP:
	case MC_MATCH_FLAG_STATE:
	case MC_MATCH_INFECT:
	case MC_MATCH_LASTSURVIVOR:
	case MC_MATCH_GUNGAME_WEAPONDATA:
	case MC_MATCH_GAME_DEAD:
	case MC_MATCH_DUEL_QUEUEINFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_NEXT_MATCH_PLYAERINFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_INFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_ROUND_RESULT_INFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_MATCH_RESULT_INFO:

	case MC_BLITZ_RESPONSE_CLASS:
	case MC_BLITZ_START_HONOR:
	case MC_BLITZ_PLAYER_KILL:
	case MC_BLITZ_RESPONSE_UPGRADE:

	case MC_NEWQUEST_PEER_NPC_BASICINFO:
	case MC_NEWQUEST_PEER_NPC_ACTION_EXECUTE:
	case MC_NEWQUEST_REQUEST_NPC_SPAWN:
	case MC_NEWQUEST_MOVE_TO_NEXT_SECTOR:

	case MC_SPY_GAME_INFO:
	case MC_SPY_GAME_RESULT:
	case MC_MATCH_SPY_ROUNDRESULT:

	case MC_MATCH_RESPONSE_SKILLMAP_BESTTIME:
	case MC_SKILLMAP_OBTAIN_ITEM:

	{
		if (m_Match.OnCommand(pCommand)) return true;
	};
	};

	return false;
}

void ZGame::OnResetTeamMembers(MCommand * pCommand)
{
	if (!m_Match.IsTeamPlay()) return;

	if (m_Match.GetMatchType() != MMATCH_GAMETYPE_INFECTED)
		ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), ZMsg(MSG_GAME_MAKE_AUTO_BALANCED_TEAM));

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if (pParam->GetType() != MPT_BLOB) return;
	void* pBlob = pParam->GetPointer();
	int nCount = MGetBlobArrayCount(pBlob);

	ZCharacterManager* pCharMgr = ZGetCharacterManager();

	for (int i = 0; i < nCount; i++)
	{
		MTD_ResetTeamMembersData* pDataNode = (MTD_ResetTeamMembersData*)MGetBlobArrayElement(pBlob, i);

		ZCharacter* pChar = (ZCharacter*)pCharMgr->Find(pDataNode->m_uidPlayer);
		if (pChar == NULL) continue;

		if (pChar->GetTeamID() != ((MMatchTeam)pDataNode->nTeam))
		{
			// ∏∏?E≥™¿⁄Ω≈¿Ã ∆¿∫Ø∞Ê¿Ã µ«æ˙¿∏?E∆¿∫Ø∞Êµ«æ˙¥¬¡ˆ∏¶ ≥≤±‰¥Ÿ.
			if (pDataNode->m_uidPlayer == ZGetMyUID())
			{
				ZGetMyInfo()->GetGameInfo()->bForcedChangeTeam = true;
			}

			pChar->SetTeamID((MMatchTeam)pDataNode->nTeam);
		}
	}
}

void ZGame::MakeResourceCRC32(const DWORD dwKey, DWORD & out_crc32, DWORD & out_xor)
{
	out_crc32 = 0;
	out_xor = 0;

#ifdef _DEBUG
	static DWORD dwOutputCount = 0;
	++dwOutputCount;
#endif

	MMatchObjCacheMap* pObjCacheMap = ZGetGameClient()->GetObjCacheMap();
	if (NULL == pObjCacheMap)
	{
		return;
	}

	MMatchObjCacheMap::const_iterator	end = pObjCacheMap->end();
	MMatchObjCacheMap::iterator			it = pObjCacheMap->begin();
	MMatchObjCache* pObjCache = NULL;
	MMatchItemDesc* pitemDesc = NULL;
	MMatchCRC32XORCache					CRC32Cache;

	CRC32Cache.Reset();
	CRC32Cache.CRC32XOR(dwKey);

#ifdef _DEBUG
	mlog("Start ResourceCRC32Cache : %u\n", CRC32Cache.GetXOR());
#endif

	for (; end != it; ++it)
	{
		pObjCache = it->second;

		for (int i = 0; i < MMCIP_END; ++i)
		{
			pitemDesc = MGetMatchItemDescMgr()->GetItemDesc(pObjCache->GetCostume()->nEquipedItemID[i]);
			if (NULL == pitemDesc)
			{
				continue;
			}

			pitemDesc->CacheCRC32(CRC32Cache);

#ifdef _DEBUG
			if (10 > dwOutputCount)
			{
				mlog("ItemID : %d, CRCCache : %u\n"
					, pitemDesc->m_nID
					, CRC32Cache.GetXOR() );
		}
#endif
		}
	}

#ifdef _DEBUG
	if (10 > dwOutputCount)
	{
		mlog("ResourceCRCSum : %u\n", CRC32Cache.GetXOR());
	}
#endif

	out_crc32 = CRC32Cache.GetCRC32();
	out_xor = CRC32Cache.GetXOR();
	}

void ZGame::OnResponseUseSpendableBuffItem(MUID & uidItem, int nResult)
{
	// TodoH(?E - ªÁ?E??E?∞·?E√≥∏Æ
}

/*
void ZGame::OnGetSpendableBuffItemStatus(MUID& uidChar, MTD_CharBuffInfo* pCharBuffInfo)
{
	if (uidChar != ZGetMyUID()) {
		_ASSERT(0);
		return;
	}

	if( pCharBuffInfo == NULL ) {
		_ASSERT(0);
		return;
	}

	ZGetMyInfo()->SetCharBuffInfo(pCharBuffInfo);

	if( m_pMyCharacter != NULL ) {
		m_pMyCharacter->SetCharacterBuff(pCharBuffInfo);
	}
}*/

void ZGame::ApplyPotion(int nItemID, ZCharacter * pCharObj, float fRemainedTime)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pDesc == NULL) { _ASSERT(0);  return; }

	MMatchDamageType nDamageType = pDesc->m_nDamageType.Ref();

	if (nDamageType == MMDT_HASTE)
	{
		// ∞°º” æ∆¿Ã≈€
		ZModule_Movable* pMod = (ZModule_Movable*)pCharObj->GetModule(ZMID_MOVABLE);
		if (pMod)
		{
			if (fRemainedTime == 0)
				fRemainedTime = pDesc->m_nDamageTime.Ref() * 0.001f;

			pMod->SetMoveSpeedHasteRatio(pDesc->m_nItemPower.Ref() * 0.01f, fRemainedTime, nItemID);
		}
		ZGetEffectManager()->AddHasteBeginEffect(pCharObj->GetPosition(), pCharObj);
	}
	else if (nDamageType == MMDT_HEAL || nDamageType == MMDT_REPAIR)
	{
		// ?E?»∏∫π æ∆¿Ã≈€
		if (pDesc->m_nDamageTime.Ref() == 0)
		{
			ZGetEffectManager()->AddPotionEffect(pCharObj->GetPosition(), pCharObj, pDesc->m_nEffectId);

			if (nDamageType == MMDT_HEAL)
			{
				int nAddedHP = pDesc->m_nItemPower.Ref();
				pCharObj->SetHP(min(pCharObj->GetHP() + nAddedHP, pCharObj->GetMaxHP()));
			}
			else if (nDamageType == MMDT_REPAIR)
			{
				int nAddedAP = pDesc->m_nItemPower.Ref();
				pCharObj->SetAP(min(pCharObj->GetAP() + nAddedAP, pCharObj->GetMaxAP()));
			}
			else
				_ASSERT(0);
		}
		// »˙ø¿πˆ≈∏¿” æ∆¿Ã≈€
		else
		{
			ZModule_HealOverTime* pMod = (ZModule_HealOverTime*)pCharObj->GetModule(ZMID_HEALOVERTIME);
			if (pMod)
			{
				int nRemainedHeal = (int)fRemainedTime;
				if (nRemainedHeal == 0)
					nRemainedHeal = pDesc->m_nDamageTime.Ref();

				//damagetimeø° ?E»Ωºˆ∏¶ «•±‚«“ ∞Õ
				pMod->BeginHeal(pDesc->m_nDamageType.Ref(), pDesc->m_nItemPower.Ref(), nRemainedHeal, pDesc->m_nEffectId, nItemID);
			}

			switch (nDamageType)
			{
			case MMDT_HEAL:
				ZGetEffectManager()->AddHealOverTimeBeginEffect(pCharObj->GetPosition(), pCharObj);
				break;
			case MMDT_REPAIR:
				ZGetEffectManager()->AddRepairOverTimeBeginEffect(pCharObj->GetPosition(), pCharObj);
				break;
			}
		}
	}
	else
		_ASSERT(0);
}

void ZGame::OnUseTrap(int nItemID, ZCharacter * pCharObj, rvector & pos)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pDesc == NULL) { _ASSERT(0); return; }

	rvector velocity;
	velocity = /*pCharObj->GetVelocity()+ */pCharObj->m_TargetDir * 1300.f;
	velocity.z = velocity.z + 300.f;
	m_WeaponManager.AddTrap(pos, velocity, nItemID, pCharObj);
}

void ZGame::OnUseSpyTrap(int nItemID, ZCharacter * pCharObj, rvector & pos)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pDesc == NULL) { _ASSERT(0); return; }

	rvector velocity;
	velocity = /*pCharObj->GetVelocity()+ */pCharObj->m_TargetDir * 1300.f;
	velocity.z = velocity.z + 300.f;
	m_WeaponManager.AddSpyTrap(pos, velocity, nItemID, pCharObj);
}

void ZGame::OnUseDynamite(int nItemID, ZCharacter * pCharObj, rvector & pos)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pDesc == NULL) { _ASSERT(0); return; }

	rvector velocity;
	velocity = /*pCharObj->GetVelocity()+ */pCharObj->m_TargetDir * 1300.f;
	velocity.z = velocity.z + 300.f;
	m_WeaponManager.AddDynamite(pos, velocity, pCharObj);
}

void ZGame::CheckZoneTrap(MUID uidOwner, rvector pos, MMatchItemDesc * pItemDesc, MMatchTeam nTeamID)
{
	if (!pItemDesc) return;

	float fRange = 300.f;

	ZObject* pTarget = NULL;
	// Custom: Changed ZCharacter to ZObject, fixes cq npc's damaging eachother.
	ZObject* pOwnerObject = (ZObject*)m_ObjectManager.GetObject(uidOwner);

	if (!pOwnerObject)
		return;

	float fDist;
	bool bReturnValue;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		pTarget = (*itor).second;

		// ≥ª ∆Æ∑¶¿Ã?E«««ÿ æ¯¿Ω
#ifndef _DEBUG	// µπˆ±◊ ∫ÙµÂø° «—«ÿº≠ ≈◊Ω∫∆Æ∏¶ Ω±∞‘ «œ?E¿ß«ÿ ≥ª ∆Æ∑¶µµ «««ÿ∏¶ πﬁµµ∑œ «—¥Ÿ
		bReturnValue = pTarget->GetUID() == uidOwner;
		if (pTarget->GetUID() == uidOwner)
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;
#endif

		bReturnValue = CanAttack(pOwnerObject, pTarget);
		if (!bReturnValue)
			PROTECT_DEBUG_REGISTER(!CanAttack_DebugRegister(pOwnerObject, pTarget))
			continue;

		//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «ÿ≈∑ ?E?πˆ±◊.....
		bReturnValue = !pTarget || pTarget->IsDie();
		if (!pTarget || pTarget->IsDie())
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		// ƒ≥∏Ø≈Õ¿« πﬂ∏Ò?E°º≠ π›?E√º≈©∏¶ «—¥Ÿ
		fDist = Magnitude(pos - (pTarget->GetPosition() + rvector(0, 0, 10)));
		//jintriple3 µπˆ±◊ ∑π¡ˆΩ∫≈Õ «Ÿ ?E?πˆ±◊.....
		bReturnValue = fDist >= fRange;
		if (fDist >= fRange)
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		if (pos.z > pTarget->GetPosition().z + pTarget->GetCollHeight())
			continue;

		// ∆Æ∑¶ π›?E≤¿?E‚ø?ƒ≥∏Ø≈Õ¿« πﬂ∏ÒªÁ¿Ãø° ∫Æ¿Ã ¿÷¥¬?E»Æ¿Œ
		/*{
			const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;
			RBSPPICKINFO bpi;
			rvector orig = pos+rvector(0,0,fRange);
			rvector to = pTarget->GetPosition()+rvector(0,0,10);
			bool bBspPicked = GetWorld()->GetBsp()->PickTo(orig, to, &bpi, dwPickPassFlag);
			if(bBspPicked)
			{
				if (Magnitude(to-orig) > Magnitude(bpi.PickPos-orig))
					continue;
			}
		}// ¿Ã∆Â∆Æ¥¬ ∫Æ¿ª ∂’æ˙¥¬µ• «««ÿ∏¶ æ»¿‘¥¬∞‘ ?E¿ÃªÛ«œ¥Ÿ?E«ÿº≠ ¡÷ºÆ*/

		//ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");

		if (m_pGameAction)
		{
			int nDuration = pItemDesc->m_nDamageTime.Ref();
			bool bApplied = false;
			switch (pItemDesc->m_nDamageType.Ref())
			{
			case MMDT_FIRE:
				// ∫“∆Æ∑¶¿∫ ItemPower∞° ?EÃ¡ˆ∏?∂Ê«‘
				bApplied = m_pGameAction->ApplyFireEnchantDamage(pTarget, pOwnerObject, pItemDesc->m_nItemPower.Ref(), nDuration);
				break;
			case MMDT_COLD:
				// æÛ¿Ω∆Æ∑¶¿∫ ItemPower∞° ¿Ãº”∞®º“∑Æ¿ª ∂Ê«‘ (80¿Ã?E80%¿« º”∑¬¿Ã µ )
				bApplied = m_pGameAction->ApplyColdEnchantDamage(pTarget, pItemDesc->m_nItemPower.Ref(), nDuration);
				break;
			default:
				_ASSERT(0);
			}

			if (bApplied)
			{
				//removed npc check, checkcombo already does that
			//	if (pOwnerCharacter->IsNPC()) continue;
				ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
				if (pOwnerCharacter)
				{
					CheckCombo(pOwnerCharacter, pTarget, true);	//todok ªÁø˚—Â∏¶ ƒ—¡‡æﬂ«“?E;
					CheckStylishAction(pOwnerCharacter);
				}
			}
		}
	}
	// π∞º”ø° ¿÷¿ª ∂ß¥¬ ªÁ∂˜¿Ã π‚æ“¿ª ∂ß π∞∆¢?E»ø∞˙∏¶ ¿œ¿∏≈∞¿⁄
	GetWorld()->GetWaters()->CheckSpearing(pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f);
}

void ZGame::OnExplosionDynamite(MUID uidOwner, rvector pos, float fDamage, float fRange, float fKnockBack, MMatchTeam nTeamID)
{
	ZObject* pTarget = NULL;

	float fDist;

	ZObject* pOwnerObject = (ZObject*)m_ObjectManager.GetObject(uidOwner);

	if (!pOwnerObject)
		return;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		pTarget = (*itor).second;

		bool bReturnValue = !pTarget || pTarget->IsDie();
		if (!pTarget || pTarget->IsDie())
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		fDist = Magnitude(pos - (pTarget->GetPosition() + rvector(0, 0, 80)));
		bReturnValue = fDist >= fRange;
		if (fDist >= fRange)
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		rvector dir = pos - (pTarget->GetPosition() + rvector(0, 0, 80));
		Normalize(dir);

		// ¥Ÿ¿Ã≥ ∏∂¿Ã∆Æµµ ºˆ∑˘≈∫√≥∑≥ π›µø¿∏∑Œ ∆¢æ˚œ™∞£¥Ÿ.
		ZActor* pATarget = MDynamicCast(ZActor, pTarget);
		ZActorWithFSM* pFSMActor = MDynamicCast(ZActorWithFSM, pTarget);
		bool bPushSkip = false;

		if (pATarget)
		{
			bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
		}
		if (pFSMActor)
		{
			bPushSkip = pFSMActor->GetActorDef()->IsNeverBlasted();
		}

		if (bPushSkip == false)
		{
			pTarget->AddVelocity(fKnockBack * 7.f * (fRange - fDist) * -dir);
		}
		else
		{
			ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
		}
		ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
		if (pOwnerCharacter)
		{
			CheckCombo(pOwnerCharacter, pTarget, false);
			CheckStylishAction(pOwnerCharacter);
		}

		// ¥Ÿ¿Ã≥ ∏∂¿Ã∆Æ¥¬ ∆¯πﬂ ?E?≥ªø°º≠ ∏µŒ ∞∞¿∫ µ•πÃ¡ˆ∏¶ ¿‘»˘¥Ÿ.
		float fRatio = ZItem::GetPiercingRatio(MWT_DYNAMITYE, eq_parts_chest);
		pTarget->OnDamaged(pOwnerObject, pos, ZD_EXPLOSION, MWT_DYNAMITYE, fDamage, fRatio);
	}

#define SHOCK_RANGE		1500.f			// 10πÃ≈Õ±˚?E»ÁµÈ∏∞¥Ÿ

	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if (pTargetCharacter)
	{
		float fPower = (SHOCK_RANGE - Magnitude(pTargetCharacter->GetPosition() + rvector(0, 0, 50) - pos)) / SHOCK_RANGE;

		if (fPower > 0)
			ZGetGameInterface()->GetCamera()->Shock(fPower * 500.f, .5f, rvector(0.0f, 0.0f, -1.0f));
	}
	GetWorld()->GetWaters()->CheckSpearing(pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f);
}

int ZGame::GetCharacterBasicInfoTick()
{
	return (ZGetGameClient()->GetAllowTunneling() == false) ? PEERMOVE_TICK : PEERMOVE_AGENT_TICK;
}

MUID ZGame::GetMyUid()
{
	return ZGetGameClient()->GetPlayerUID();
}

bool ZGame::PickWorld(const rvector & pos, const rvector & dir, RBSPPICKINFO * pOut, DWORD dwPassFlag)
{
	ABspObject* r_map = GetWorld()->GetBsp();
	return r_map->Pick(pos, dir, pOut, dwPassFlag);
}
bool ZGame::CheckWall(rvector & origin, rvector & targetpos, float fRadius, float fHeight, RCOLLISIONMETHOD method, int nDepth, rplane * pimpactplane)
{
	return GetWorld()->GetBsp()->CheckWall(origin, targetpos, 35, 60, RCW_CYLINDER, 0, pimpactplane);
}
void ZGame::AdjustMoveDiff(ZObject * pObject, rvector & diff)
{
	// TODO : √Êµπ π¸¿ß / πÊπ˝¿ª ¿Øµø¿˚¿∏∑Œ ∞°¡Æ∞°¿⁄
	//#define COLLISION_DIST	70.f

	if (!pObject) return;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		ZObject* pOtherObj = (*itor).second;
		if (pOtherObj != pObject && pOtherObj->IsCollideable())
		{
			rvector pos = pOtherObj->GetPosition();
			rvector dir = pObject->GetPosition() + diff - pos;
			dir.z = 0;
			float fDist = Magnitude(dir);

			float fCOLLISION_DIST = pOtherObj->GetCollRadius() + pObject->GetCollRadius();

			if (fDist < fCOLLISION_DIST && fabs(pos.z - pObject->GetPosition().z) < pOtherObj->GetCollHeight())	//heightø° *0.5«ÿæﬂ«œ¥¬∞≈ æ∆¥“±Ó?
			{
				// ∞≈¿« ∞∞¿∫¿ßƒ°ø° ¿÷¥¬ ∞ÊøÏ.. «—¬ πÊ«‚¿∏∑Œ π–∏≤
				if (fDist < 1.f)
				{
					pos.x += 1.f;
					dir = pObject->GetPosition() - pos;
				}

				if (DotProduct(dir, diff) < 0)	// ¥ı ∞°±Óøˆ¡ˆ¥¬ πÊ«‚¿Ã∏È
				{
					Normalize(dir);
					rvector newthispos = pos + dir * (fCOLLISION_DIST + 1.f);

					rvector newdiff = newthispos - pObject->GetPosition();
					diff.x = newdiff.x;
					diff.y = newdiff.y;
				}
			}
		}
	}
}

ZNavigationMesh ZGame::GetNavigationMesh()
{
	return ZNavigationMesh(GetWorld()->GetBsp()->GetNavigationMesh());
}

void ZGame::OnUseStunGrenade(int nItemID, ZCharacter * pCharObj, rvector & pos)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pDesc == NULL) { _ASSERT(0); return; }

	rvector velocity;
	velocity = pCharObj->m_TargetDir * 1300.f;
	velocity.z = velocity.z + 300.f;
	m_WeaponManager.AddStunGrenade(pos, velocity, pCharObj);
}

void ZGame::OnExplosionStunGrenade(MUID uidOwner, rvector pos, float fDamage, float fRange, float fKnockBack, MMatchTeam nTeamID)
{
	ZObject* pTarget = NULL;
	float fDist;

	ZObject* pOwnerObject = (ZObject*)m_ObjectManager.GetObject(uidOwner);

	if (!pOwnerObject)
		return;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		pTarget = (*itor).second;

		bool bReturnValue = !pTarget || pTarget->IsDie();
		if (!pTarget || pTarget->IsDie())
			continue;
		fDist = Magnitude(pos - (pTarget->GetPosition() + rvector(0, 0, 80)));
		bReturnValue = fDist >= fRange;
		if (fDist >= fRange)
			continue;
		rvector dir = pos - (pTarget->GetPosition() + rvector(0, 0, 80));
		Normalize(dir);
		ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
		if (pOwnerCharacter)
		{
			CheckCombo(pOwnerCharacter, pTarget, false);
			CheckStylishAction(pOwnerCharacter);
		}

		float fRatio = ZItem::GetPiercingRatio(MWT_STUNGRENADE, eq_parts_chest);
		pTarget->OnDamaged(pOwnerObject, pos, ZD_EXPLOSION, MWT_STUNGRENADE, fDamage, fRatio);
		pTarget->OnDamagedAnimation(pTarget, ZD_KATANA_SPLASH);
	}
	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if (pTargetCharacter)
	{
		float fPower = (SHOCK_RANGE - Magnitude(pTargetCharacter->GetPosition() + rvector(0, 0, 50) - pos)) / SHOCK_RANGE;

		if (fPower > 0)
			ZGetGameInterface()->GetCamera()->Shock(fPower * 500.f, .5f, rvector(0.0f, 0.0f, -1.0f));
	}

	GetWorld()->GetWaters()->CheckSpearing(pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f);
}

void ZGame::CheckZoneItemKit(MUID uidOwner, rvector pos, float fDamage, MMatchTeam nTeamID)
{
	ZObject* pTarget = NULL;
	float fDist;

	ZObject* pOwnerObject = (ZObject*)m_ObjectManager.GetObject(uidOwner);

	if (!pOwnerObject)
		return;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		pTarget = (*itor).second;

		bool bReturnValue = !pTarget || pTarget->IsDie();
		if (!pTarget || pTarget->IsDie())
			continue;
		fDist = Magnitude(pos - (pTarget->GetPosition() + rvector(0, 0, 80)));
		rvector dir = pos - (pTarget->GetPosition() + rvector(0, 0, 80));
		Normalize(dir);
		ZCharacter* pOwnerCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
		if (pOwnerCharacter)
		{
			CheckCombo(pOwnerCharacter, pTarget, false);
			CheckStylishAction(pOwnerCharacter);
		}

		pTarget->OnDamaged(pOwnerObject, pos, ZD_EXPLOSION, MWT_LANDMINE, fDamage, 0);
		if (pTarget == ZGetGame()->m_pMyCharacter)
		{
			ZGetGame()->m_pMyCharacter->OnBlast(rvector(pTarget->GetDirection()));
			ZGetGame()->m_pMyCharacter->SetVelocity(rvector(0, 0, 0));
		}
		//	pTarget->OnDamagedAnimation(pTarget, ZD_KATANA_SPLASH);
	}
	ZCharacter* pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if (pTargetCharacter)
	{
		float fPower = (SHOCK_RANGE - Magnitude(pTargetCharacter->GetPosition() + rvector(0, 0, 50) - pos)) / SHOCK_RANGE;

		if (fPower > 0)
			ZGetGameInterface()->GetCamera()->Shock(fPower * 500.f, .5f, rvector(0.0f, 0.0f, -1.0f));
	}

	GetWorld()->GetWaters()->CheckSpearing(pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f);
}
void ZGame::BerserkerKI()
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZGetEffectManager()->Clear();
		ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
		pCharacter->SetTagger(false);
	}

	ZCharacter* pBerserkerKIChar = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(ZGetMyUID());
	if (pBerserkerKIChar)
	{
		ZGetEffectManager()->AddBerserkerIconKI(pBerserkerKIChar);
		pBerserkerKIChar->SetTagger(true);
	}
}
void ZGame::GetJjang()
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZGetEffectManager()->Clear();
	}
	ZCharacter* pCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(ZGetMyUID());
	if (pCharacter)
	{
		ZGetEffectManager()->AddStarEffect(pCharacter);
	}
}