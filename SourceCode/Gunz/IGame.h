#pragma once
#include "ZCharacterManager.h"

class ZNavigationMesh;

struct ZPICKINFO 
{
	ZObject* pObject;
	RPickInfo	info;

	bool bBspPicked;
	int nBspPicked_DebugRegister;
	RBSPPICKINFO bpi;
};


class IGame
{
public:
	virtual float GetTime() = 0;
	//virtual ZMyCharacter* GetMyCharacter() = 0;
	virtual MUID GetMyUid() = 0;

	virtual ZCharacterManagerBase* GetCharacterMgr() = 0;

	virtual bool CanAttack(ZObject* pAttacker, ZObject* pTarget) = 0;
	virtual bool CanAttack_DebugRegister(ZObject* pAttacker, ZObject* pTarget) = 0;

	virtual bool Pick(ZObject* pOwnerObject, rvector& origin, rvector& dir, ZPICKINFO* pickinfo, DWORD dwPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE, bool bMyChar = false) = 0;

	virtual bool PickWorld(const rvector& pos, const rvector& dir, RBSPPICKINFO* pOut, DWORD dwPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_USEOPACITY | RM_FLAG_HIDE) = 0;
	virtual bool CheckWall(rvector& origin, rvector& targetpos, float fRadius, float fHeight = 0.f, RCOLLISIONMETHOD method = RCW_CYLINDER, int nDepth = 0, rplane* pimpactplane = NULL) = 0;
	virtual rvector GetFloor(rvector pos, rplane* pimpactplane = NULL, MUID uID = MUID(0, 0)) = 0;
	virtual void AdjustMoveDiff(ZObject* pObject, rvector& diff) = 0;

	virtual ZNavigationMesh GetNavigationMesh() = 0;

	virtual bool InRanged(ZObject* pAttacker, ZObject* pVictim) = 0;
	virtual bool InRanged(ZObject* pAttacker, ZObject* pVictim, int& nDebugRegister) = 0;


	virtual bool IsWallBlocked(ZObject* pObj1, ZObject* pObj2, bool bCoherentToPeer = false) = 0;
	virtual bool IsWallBlocked(ZObject* pObj1, ZObject* pObj2, int& nDebugRegister, bool bCoherentToPeer = false) = 0;

	virtual bool IsExceptedFromNpcTargetting(const ZCharacter* pChar) = 0;
	virtual bool IsEnabledToWarpNpcWhenStucked() { return false; }

	virtual int GetCharacterBasicInfoTick() { return 100; }
};