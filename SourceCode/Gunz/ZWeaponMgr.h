#ifndef _ZWeaponMgr_h
#define _ZWeaponMgr_h

#include <list>
using namespace std;

#include "RTypes.h"
#include "RVisualMeshMgr.h"
#include "ZWeapon.h"


#include "ZActorAction.h"
// ｼｺ
// ｽﾃﾀﾛﾀｧﾄ｡ﾁﾖｰ・
// ｹ貮簔ﾖｰ・(ｰ｡ｼﾓｵｵ)
// define ｼﾒｸ・ﾃｰ｣ ( ｸﾊｰ惕ﾇ ﾀｧﾄ｡ｿｬｵｿ , ﾅｺｼｺ )
// ｴｩｱｸ ｼｺﾀﾎﾁ・ｾﾋｾﾆｾﾟ ﾇﾏｴﾂｰ｡? ( ｳｪﾁﾟｿ｡ ﾁ｡ｼ・ﾁﾖｱ・ﾀｧﾇﾘ,ﾅｳｼｦ ｿﾃｸｮｰﾅｳｪ.. ) ( ｽ・ﾇ･ｱ・)
// Effect
enum eCrashType{
	et_crash_none = 0,
	et_crash_wall,
	et_crash_floor
};

class ZCharacter;
class ZEffectBillboardSource;

typedef list<ZWeapon*>			z_weapon_list;
typedef z_weapon_list::iterator	z_weapon_node;

class ZWeaponMgr {
public:
	ZWeaponMgr();
	~ZWeaponMgr();

public:
	void Add(ZWeapon* pWeapon) {
		m_list.push_back(pWeapon);
	}
	void Clear();
	void AddGrenade(rvector &pos,rvector &velocity,ZObject* pC);
	void AddRocket(rvector &pos,rvector &dir,ZObject* pC);
	void AddMagic(ZSkill* pSkill, const rvector &pos, const rvector &dir,ZObject* pOwner);
	void AddFlashBang(rvector &pos,rvector &dir,ZObject* pC);
	void AddSmokeGrenade(rvector &pos,rvector &velocity,ZObject* pC);

	//void AddMedikit(rvector &pos,rvector &velocity,ZObject* pC,float delay);
	//void AddRepairkit(rvector &pos,rvector &velocity,ZObject* pC,float delay);
	void AddKit(rvector &pos, rvector &velocity, ZCharacter* pC, float Delaytime, char *szMeshName, int nLinkedWorldItemID);
	void AddTrap(rvector &pos,rvector &velocity,int nItemId,ZObject* pC);
	void AddTrapAlreadyActivated(rvector& pos, float fActivatedTime, int nItemId, ZObject* pOwner);
	void AddDynamite(rvector &pos, rvector &velocity, ZObject* pC);
	void AddStunGrenade(rvector &pos, rvector &velocity, ZObject* pC);
	//////////SPY MODE /////////////
	void AddSpyFlashBang(rvector& pos, rvector& dir, ZObject* pC);
	void AddSpySmokeGrenade(rvector& pos, rvector& velocity, ZObject* pC);
	void AddSpyTrap(rvector& pos, rvector& velocity, int nItemId, ZObject* pC);

	void AddNewQuestProjectile(const ZActorActionRangeShot* pRangeShot, const rvector &pos, const rvector &dir, ZObject* pOwner);
	void DeleteWeaponHasTarget(const MUID& uidTargetChar);	// ﾆｯﾁ､ ﾄｳｸｯﾅﾍｸｦ ﾅｸｰﾙﾀｸｷﾎ ﾇﾏｰ・ﾀﾖｴﾂ ﾅｺﾀｻ ﾁｦｰﾅﾇﾑｴﾙ
	void EnableRender(bool b) { m_bEnableRender = b; }

	void Update();
	void Render();

	ZMovingWeapon*	UpdateWorldItem(int nItemID,rvector& pos);//ｿ・ｾﾆﾀﾌﾅﾛｰ・ｺｸｿｩﾁﾂ ｵｿｱ・ｸﾂﾃ箍ﾍｵ・..
	ZWeapon*		GetWorldItem(int nItemID);

	z_weapon_list m_list;

protected:

	bool	SamePoint(rvector& p1,rvector& p2);
	float	m_fLastTime;
	bool	m_bEnableRender;

};

#endif//_ZWeaponMgr_h