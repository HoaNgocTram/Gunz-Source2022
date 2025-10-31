#ifndef MMATCHGUNGAME_H
#define MMATCHGUNGAME_H

struct GunGameItemData
{
	int nItemID;
	int nItemParts;
};

struct GunGameData
{
	//int nItemID;
	int nLevel;
	bool bSpecial;
	vector<GunGameItemData> itemData;
	//int nItemParts;
};

typedef vector<GunGameData> MGunGameData;

bool InitializeGunGame();
int* GetGunGameItemSet(MGunGameData data, int nLevel);
int GetGunGameMaxLevel();
bool GetGunGameRuleRandomize();
vector<GunGameData> GetGunGameData();
bool IsSpecialSortedFirst();
int GetSpecialCount();


#endif