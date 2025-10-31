#include "stdafx.h"
#include "ZApplication.h"
#include "ZEnemy.h"
#include "ZObject.h"
#include "ZObjectManager.h"
#include "Physics.h"
#include "ZReplay.h"
#include "ZTestGame.h"
#include "ZGameClient.h"
#include "MMath.h"
#include "ZMapDesc.h"

#include "ZModule_Skills.h"

void CreateTestGame(char *mapname, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5)
{
	ZApplication::GetStageInterface()->SetMapName(mapname);
	ZGetGameClient()->GetMatchStageSetting()->SetMapName(mapname);
	ZGetGameInterface()->SetState(GUNZ_GAME);

	if(!ZGetGame()) return;

	ZGetGame()->SetReadyState(ZGAME_READYSTATE_RUN);

	ZGetGame()->GetMatch()->SetRoundState(MMATCH_ROUNDSTATE_PLAY);

	MTD_CharInfo info;
	memset(&info, 0, sizeof(MTD_CharInfo));
	strcpy(info.szName,"Maiet");
	info.szClanName[0]=0;
	info.nSex=1;
	info.nHP = 100;
	info.nAP = 100;

	for(int i=0;i<MMCIP_END;i++)
		info.nEquipedItemDesc[i]=0;
	//info.nEquipedItemDesc[MMCIP_MELEE] = 1;		// 구식단검
	info.nEquipedItemDesc[MMCIP_MELEE] = nParam1;		// 구식장검
	info.nEquipedItemCount[MMCIP_MELEE] = nParam1 > 0;		// 구식장검
	//	info.nEquipedItemDesc[MMCIP_MELEE] = 31;		// 용월랑
	//	info.nEquipedItemDesc[MMCIP_PRIMARY] = 5018;
	//	info.nEquipedItemDesc[MMCIP_PRIMARY] = 4514;	// 자우르스 B x2
	info.nEquipedItemDesc[MMCIP_PRIMARY] = nParam2;	// LX 44
	info.nEquipedItemCount[MMCIP_PRIMARY] = nParam2 > 0;		// 구식장검
	//	info.nEquipedItemDesc[MMCIP_SECONDARY] = 6001;	// 샷건
	info.nEquipedItemDesc[MMCIP_SECONDARY] = nParam3;	// 머신건
	info.nEquipedItemCount[MMCIP_SECONDARY] = nParam3 > 0;		// 구식장검
	info.nEquipedItemDesc[MMCIP_CUSTOM1] = nParam4;
	info.nEquipedItemCount[MMCIP_CUSTOM1] = nParam4 > 0;		// 구식장검
	info.nEquipedItemDesc[MMCIP_CUSTOM2] = nParam5;	// 카노푸스의 눈
	info.nEquipedItemCount[MMCIP_CUSTOM2] = nParam5 > 0;		// 구식장검		

	ZGetGame()->CreateMyCharacter(&info);
	ZGetGame()->m_pMyCharacter->Revival();
	MUID uid;
	ZCharacter* pCharacter = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uid);
	rvector pos=rvector(0,0,0), dir=rvector(0,1,0);

	ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetSoloRandomData();
	if (pSpawnData != NULL)
	{
		pos = pSpawnData->m_Pos;
		dir = pSpawnData->m_Dir;
	}

	ZGetGame()->m_pMyCharacter->SetPosition(pos);
	ZGetGame()->m_pMyCharacter->SetDirection(dir);
}

