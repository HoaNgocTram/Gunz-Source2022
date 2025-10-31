#pragma once
#ifdef _VOICECHAT
#include <Windows.h>
typedef unsigned __int64 uint64;
typedef map<uint64, MUID> ChannelToStageMapTemp;
typedef map<MUID, uint64> PlayerMUIDToClientIDMap;
class Ts3Srv
{
  public:
	uint64		serverID;
	int		m_serverPort;
	HANDLE		m_hThread;
	volatile int	m_stopRequest;
	ChannelToStageMapTemp Chan2Stage;
	PlayerMUIDToClientIDMap PM2C;
	int		m_curClients;
	int		m_serverSlots;
	
  public:
	Ts3Srv();
	~Ts3Srv();
	
	int		Startup();
	int		Shutdown();
	void	Tick();
	MUID GetStageMUID(uint64 ChannelID);
	uint64 GetClientByMuid(MUID Player);
	uint64	CreateVirtualServer(const char* name, int port, int maxClients);
	uint64	CreateChannel(const char* name, const char* Password, const int Players, MUID Stage);
	void	deleteChannel(uint64 newChannelID);
	void	moveClient(uint64 NewChannelID, uint64 clientID);
};

extern	void		TSServer_Start(int port);
extern	void		TSServer_Stop();
#endif