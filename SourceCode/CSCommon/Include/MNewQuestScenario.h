#pragma once

class MNewQuestSpawn
{
	string m_strPosTag;		// 맵 메쉬에 박혀 있는 스폰노드 이름
	int m_num;				// 이런 이름의 노드가 몇 개 있는가
	string m_strActor;		// 여기서 스폰될 액터종류
	int m_dropTableId;		// 드랍아이템 인덱스
	bool bIsBoss;
public:
	void SetPosTag(const char* szPosTag) { m_strPosTag = szPosTag; }
	void SetBoss(bool b) { bIsBoss = b; }
	void SetNum(int num) { m_num = num; }
	void SetActor(const char* szActor) { m_strActor = szActor; }
	void SetDropTableId(int id) { m_dropTableId = id; }

	const char* GetPosTag() { return m_strPosTag.c_str(); }
	bool GetBoss() { return bIsBoss; }
	int GetNum() { return m_num; }
	const char* GetActor() { return m_strActor.c_str(); }
	int GetDropTableId() { return m_dropTableId; }
};

class MNewQuestSector
{
	string m_strMapName;
	int m_rewardXp;
	int m_rewardBp;

	typedef map<string, MNewQuestSpawn*>	MapSpawn;
	typedef MapSpawn::iterator				ItorSpawn;
	MapSpawn m_mapSpawn;

public:
	MNewQuestSector() : m_rewardXp(0), m_rewardBp(0) {}
	~MNewQuestSector()
	{
		for (ItorSpawn it = m_mapSpawn.begin(); it != m_mapSpawn.end(); ++it)
			delete it->second;
	}

	const char* GetMapName() { return m_strMapName.c_str(); }

	void SetMap(const char* szMap) { m_strMapName = szMap; }
	void SetRewardXp(int rewardXp) { m_rewardXp = rewardXp; }
	void SetRewardBp(int rewardBp) { m_rewardBp = rewardBp; }
	int GetRewardXp() { return m_rewardXp; }
	int GetRewardBp() { return m_rewardBp; }

	bool AddSpawn(MNewQuestSpawn* pSpawn);
	MNewQuestSpawn* GetSpawnByIndex(int index);
	int GetNumSpawnType() { return (int)m_mapSpawn.size(); }
};

class MNewQuestScenario
{
	int m_nID;
	string m_strName;
	string m_strDescription;
	DWORD m_rewardItem;
	int   m_maxPlayers;
	DWORD m_levelLimit;
	DWORD m_GoodTime;

	typedef vector<MNewQuestSector*>	VecSector;
	typedef VecSector::iterator			ItorSector;
	VecSector m_vecSector;

public:
	MNewQuestScenario() : m_rewardItem(0) {}
	~MNewQuestScenario()
	{
		for (int i = 0; i < (int)m_vecSector.size(); ++i)
			delete m_vecSector[i];
	}

	void SetID(int n) { m_nID = n; }
	int GetID() { return m_nID; }

	void SetName(const char* szName) { m_strName = szName; }
	const char* GetName() { return m_strName.c_str(); }

	void SetRewardItem(int itemId) { m_rewardItem = itemId; }
	int GetRewardItem() { return m_rewardItem; }

	void SetMaxPlayers(int maxPlayers) { m_maxPlayers = maxPlayers; }
	int GetMaxPlayers() { return m_maxPlayers; }

	void SetLevelLimit(int levelLimit) { m_levelLimit = levelLimit; }
	int GetLevelLimit() { return m_levelLimit; }

	void SetDescription(const char* szDescription) { m_strDescription = szDescription; }
	const char* GetDescription() { return m_strDescription.c_str(); }

	void SetGoodTime(int nTime) { m_GoodTime = nTime; }
	int GetGoodTime() { return m_GoodTime; }

	void AddSector(MNewQuestSector* pSector) { m_vecSector.push_back(pSector); }
	int GetNumSector() { return (int)m_vecSector.size(); }
	MNewQuestSector* GetSector(int index) {
		if (index < 0 || GetNumSector() <= index) return NULL;
		return m_vecSector[index];
	}
};

class MNewQuestScenarioManager
{
	typedef map<string, MNewQuestScenario*>	MapScenario;
	typedef MapScenario::iterator			ItorScenario;
	MapScenario m_mapScenario;
	MQuestDropTable* m_pDropTable;

public:
	MNewQuestScenarioManager();
	~MNewQuestScenarioManager();

	void Clear();
	bool AddScenario(MNewQuestScenario* pScenario);
	MNewQuestScenario* GetScenario(const char* szName);
	void GetScenarioNameList(vector<string>& out);
	const char* GetDefaultScenarioName();
	const char* GetDefaultScenarioDescription();

	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName, MQuestDropTable* pDropTable);
	bool ParseXmlFromStr(const char* szXmlContent);
	MNewQuestScenario* ParseScenario(MXmlElement& elem);
	MNewQuestSector* ParseSector(MXmlElement& elem);
	MNewQuestSpawn* ParseSpawn(MXmlElement& elem);
};
