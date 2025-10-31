#include "stdafx.h"
#ifdef _VOICECHAT
#include "MMatchTeamSpeak.h"
#endif
#define _VOICECHAT2 1

#ifndef _VOICECHAT2
void TSServer_Start(int port) {}
void TSServer_Stop() {}
WORD TSServer_GetPassword() {}
#endif

#ifdef ENABLE_TEAMSPEAK
#include <public_definitions.h>
#include <public_errors.h>
#include <serverlib_publicdefinitions.h>
#include <serverlib.h>

#pragma comment(lib, "../sdk/TeamSpeak/lib/ts3server_win32.lib")

static unsigned int	error;
#define PROVISIONING_SLOTS_BUFFER	1	
#define CHECK_ERROR(x) if((error = x) != ERROR_ok) { goto on_error; }

static int TSHandleError(const char* func = "")
{
	char* errormsg;
	if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok)
	{
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Error: %s %x %s\n", func, error, errormsg);
		ts3server_freeMemory(errormsg);
	}
	else
	{
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Error: %s %x\n", func, error);
	}
	return 0;
}

static int readKeyPairFromFile(const char *fileName, char *keyPair)
{
	FILE *file;

	file = fopen(fileName, "r");
	if(file == NULL) {
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Could not open file '%s' for reading keypair\n", fileName);
		return -1;
	}

	fgets(keyPair, BUFSIZ, file);
	if(ferror(file) != 0) {
		fclose (file);
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Error reading keypair from file '%s'.\n", fileName);
		return -1;
	}
	fclose (file);

	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Read keypair '%s' from file '%s'.\n", keyPair, fileName);
	return 0;
}

static int writeKeyPairToFile(const char *fileName, const char* keyPair)
{
	FILE *file;

	file = fopen(fileName, "w");
	if(file == NULL) {
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Could not open file '%s' for writing keypair\n", fileName);
		return -1;
	}
    
	fputs(keyPair, file);
	if(ferror(file) != 0) {
		fclose (file);
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Error writing keypair to file '%s'.\n", fileName);
		return -1;
	}
	fclose (file);

	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Wrote keypair '%s' to file '%s'.\n", keyPair, fileName);
	return 0;
}

static void onClientConnected(uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError)
{
	char* clientName = "";
	if((error = ts3server_getClientVariableAsString(serverID, clientID, CLIENT_NICKNAME, &clientName)) != ERROR_ok)
	{
		*removeClientError = ERROR_client_not_logged_in;
		return;
	}
	MUID STAGE = MGetMatchServer()->TsSrv.GetStageMUID(channelID);
	MUID Player = MUID(0, 0);
	if(MUID(0, 0) != STAGE)
	{
		MMatchStage* pStage = MGetMatchServer()->FindStage(STAGE);
		if(pStage)
		{
			for(MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
			{
				MMatchObject* pObj = (MMatchObject*)((*i).second);
				if(pObj && pObj->GetCharInfo() &&memcmp(clientName, pObj->GetCharInfo()->m_szName, strlen(clientName)) == 0)
				{
					Player = i->first;
					break;
				}
			}
		} 
		else
		{
			MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]%s Stage Not Found\n", clientName);
			*removeClientError = ERROR_client_not_logged_in;
			ts3server_freeMemory(clientName);
			return;
		}
	} 
	else
	{
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]%s Channel Not Found\n", clientName);
		*removeClientError = ERROR_client_not_logged_in;
		ts3server_freeMemory(clientName);
		return;
	}
	if(Player != MUID(0, 0))
	{
		if(MGetMatchServer()->TsSrv.PM2C.find(Player) == MGetMatchServer()->TsSrv.PM2C.end())
		{
			MGetMatchServer()->TsSrv.PM2C.insert(PlayerMUIDToClientIDMap::value_type(Player, clientID));
			MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Client %u '%s' joined channel %llu on virtual server %llu\n", clientID, clientName, (unsigned long long) channelID, (unsigned long long)serverID);
			MGetMatchServer()->TsSrv.m_curClients++;
		}
	}
	else
	{
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]%s Client Not Found\n", clientName);
		*removeClientError = ERROR_client_not_logged_in;
		ts3server_freeMemory(clientName);
		return;
	}
	ts3server_freeMemory(clientName);
}


static void onClientDisconnected(uint64 serverID, anyID clientID, uint64 channelID)
{
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]voip: Client %u left channel %llu on virtual server %llu\n", clientID, (unsigned long long)channelID, (unsigned long long)serverID);
	for(PlayerMUIDToClientIDMap::iterator i = MGetMatchServer()->TsSrv.PM2C.begin(); i != MGetMatchServer()->TsSrv.PM2C.end();)
	{
		if(i->second == clientID)
		{
			MGetMatchServer()->TsSrv.PM2C.erase(i);
			MGetMatchServer()->TsSrv.m_curClients--;
			break;
		} else i++;
	}
}

static void onChannelCreated(uint64 serverID, anyID invokerClientID, uint64 channelID)
{
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Channel %llu created by %u on virtual server %llu\n", (unsigned long long)channelID, invokerClientID, (unsigned long long)serverID);
	if(invokerClientID == 0)
		return;
		
	char* name = NULL;
	if((error = ts3server_getChannelVariableAsString(serverID, channelID, CHANNEL_NAME, &name)) != ERROR_ok)
		TSHandleError("getChannelName");

	//dc player due to makeing a channel or implement groups
	
	if(name) ts3server_freeMemory(name);
}

static void onChannelEdited(uint64 serverID, anyID invokerClientID, uint64 channelID)
{
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Channel %llu edited by %u on virtual server %llu\n", (unsigned long long)channelID, invokerClientID, (unsigned long long)serverID);

	if(invokerClientID == 0)
		return;
	
	//dc player due to editing a channel or implement groups
}

static void onChannelDeleted(uint64 serverID, anyID invokerClientID, uint64 channelID)
{
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Channel %llu deleted by %u on virtual server %llu\n", (unsigned long long)channelID, invokerClientID, (unsigned long long)serverID);

	if(invokerClientID == 0)
		return;
	
	//dc player due to deleteing a channel or implement groups
}

static void onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString)
{
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]VOICE:[%d] %s\n", logLevel, completeLogString);
}

static void onAccountingErrorEvent(uint64 serverID, unsigned int errorCode)
{
	char* errorMessage = NULL;
	if(ts3server_getGlobalErrorMessage(errorCode, &errorMessage) != ERROR_ok)
		TSHandleError("onAccountingErrorEvent error");

	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]voip: !!!!!! onAccountingErrorEvent serverID=%llu, errorCode=%u: %s\n", (unsigned long long)serverID, errorCode, errorMessage ? errorMessage : "unknown");

	 //dc player ?
	if(errorMessage)
		ts3server_freeMemory(errorMessage);
}

static void onCustomPacketEncryptEvent(char** dataToSend, unsigned int* sizeOfData)
{
#ifdef USE_CUSTOM_ENCRYPTION
	for(unsigned int i = 0; i < *sizeOfData; i++) {
		(*dataToSend)[i] ^= CUSTOM_CRYPT_KEY;
	}
#endif
}

static void onCustomPacketDecryptEvent(char** dataReceived, unsigned int* dataReceivedSize)
{
#ifdef USE_CUSTOM_ENCRYPTION
	for(unsigned int i = 0; i < *dataReceivedSize; i++) {
		(*dataReceived)[i] ^= CUSTOM_CRYPT_KEY;
	}
#endif
}

MUID Ts3Srv::GetStageMUID(uint64 ChannelID)
{
	ChannelToStageMapTemp::iterator i = Chan2Stage.find(ChannelID);
	if(i != Chan2Stage.end()) return i->second; else return MUID(0, 0);
}

uint64 Ts3Srv::GetClientByMuid(MUID Player)
{
	PlayerMUIDToClientIDMap::iterator i = PM2C.find(Player);
	if(i !=PM2C.end()) return i->second; else return -1;
}

uint64 Ts3Srv::CreateChannel(const char* name, const char* Password, const int Players, MUID Stage)
{
	uint64 newChannelID;
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, 0, CHANNEL_NAME,                name));
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, 0, CHANNEL_PASSWORD,			   Password));
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, 0, CHANNEL_TOPIC,               ""));
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, 0, CHANNEL_DESCRIPTION,         ""));
	CHECK_ERROR(ts3server_setChannelVariableAsInt   (serverID, 0, CHANNEL_FLAG_PERMANENT,      0));
	CHECK_ERROR(ts3server_setChannelVariableAsInt   (serverID, 0, CHANNEL_FLAG_SEMI_PERMANENT, 1));
	CHECK_ERROR(ts3server_setChannelVariableAsInt   (serverID, 0, CHANNEL_FLAG_PASSWORD,	   1));
	CHECK_ERROR(ts3server_setChannelVariableAsInt   (serverID, 0, CHANNEL_MAXCLIENTS,			Players));
	CHECK_ERROR(ts3server_flushChannelCreation(serverID, 0, &newChannelID));
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Created channel '%s': %llu\n", name, (unsigned long long)newChannelID);
	if(Chan2Stage.find(newChannelID) == Chan2Stage.end())
		Chan2Stage.insert(ChannelToStageMapTemp::value_type(newChannelID, Stage));
	return newChannelID;

on_error:
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Error creating channel: %d\n", error);
	return 0;
}

void Ts3Srv::deleteChannel(uint64 newChannelID) 
{
	unsigned int error;
	if((error = ts3server_channelDelete(serverID, newChannelID, 1)) != ERROR_ok) 
	{
		char* errormsg;
		if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Error deleting channel: %s\n ", errormsg);
			ts3server_freeMemory(errormsg);
		} 
	} 
	else		
	{
		MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Channel %llu Deleted.\n", (unsigned long long)newChannelID);
		ChannelToStageMapTemp::iterator i = Chan2Stage.find(newChannelID);
		if(i != Chan2Stage.end())
			Chan2Stage.erase(i);
	}
}

void Ts3Srv::moveClient(uint64 NewChannelID, uint64 clientID) 
{
	anyID clientIDArray[2];
	clientIDArray[0] = clientID;
	clientIDArray[1] = 0;  
	if((error = ts3server_clientMove(serverID, NewChannelID, clientIDArray)) != ERROR_ok) 
	{
		char* errormsg;
		if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) 
		{
			MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "Error moving client: %s\n ", errormsg);
			ts3server_freeMemory(errormsg);
		} else MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "Moved %llu To Channel: %llu.\n", (unsigned long long)clientID, (unsigned long long)NewChannelID);
		return;
	}
}

Ts3Srv::Ts3Srv()
{
	serverID      = 0;
	m_serverSlots = 0;
	m_curClients  = 0;
	m_stopRequest = 0;
	m_hThread     = 0;
	m_serverPort  = 0;
}

Ts3Srv::~Ts3Srv()
{
	TSServer_Stop();
}

int Ts3Srv::Startup()
{
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Starting Up...\n");
	if(m_serverPort <= 0) return 0;
	char* version;
	if((error = ts3server_getServerLibVersion(&version)) != ERROR_ok)
		return TSHandleError("ts3server_getServerLibVersion");
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Server lib version: %s\n", version);
	ts3server_freeMemory(version);
	struct ServerLibFunctions funcs;
	memset(&funcs, 0, sizeof(struct ServerLibFunctions));
	funcs.onClientConnected          = onClientConnected;
	funcs.onClientDisconnected       = onClientDisconnected;
	funcs.onChannelCreated           = onChannelCreated;
	funcs.onChannelEdited            = onChannelEdited;
	funcs.onChannelDeleted           = onChannelDeleted;
	funcs.onUserLoggingMessageEvent  = onUserLoggingMessageEvent;
	funcs.onAccountingErrorEvent     = onAccountingErrorEvent;
	funcs.onCustomPacketEncryptEvent = onCustomPacketEncryptEvent;
	funcs.onCustomPacketDecryptEvent = onCustomPacketDecryptEvent;
	if((error = ts3server_initServerLib(&funcs, LogType_USERLOGGING, NULL)) != ERROR_ok)
		return TSHandleError("ts3server_initServerLib");
	serverID = CreateVirtualServer("streakgunz", m_serverPort, PROVISIONING_SLOTS_BUFFER);
	if(!serverID)
		return 0;
	ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_WELCOMEMESSAGE, "StreakGunZ");
	ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_PASSWORD, "Lg5AndZe");
	if((error = ts3server_flushVirtualServerVariable(serverID)) != ERROR_ok)
		TSHandleError("ts3server_flushVirtualServerVariable on init\n");
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Server running\n");
	if((error = ts3server_setChannelVariableAsInt(serverID, 1, CHANNEL_CODEC, 0)) != ERROR_ok) // Narrowband 
		TSHandleError("CHANNEL_CODEC");
	if((error = ts3server_setChannelVariableAsInt(serverID, 1, CHANNEL_CODEC_QUALITY, 3)) != ERROR_ok) // Narrowband 8 kHz
		TSHandleError("CHANNEL_CODEC_QUALITY");
	if((error = ts3server_flushChannelVariable(serverID, 1)) != ERROR_ok) // Narrowband 8 kHz
		TSHandleError("ts3server_flushChannelVariable");
	
	return 1;
}

int Ts3Srv::Shutdown()
{
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Shutting down...\n");
	uint64* ids;
	if((error = ts3server_getVirtualServerList(&ids)) != ERROR_ok)
	{
		if(error == ERROR_serverlibrary_not_initialised)
			return 1;
		return TSHandleError("ts3server_getVirtualServerList");
	}

	for(int i=0; ids[i]; i++) 
	{
		if((error = ts3server_stopVirtualServer(ids[i])) != ERROR_ok)
			TSHandleError("ts3server_stopVirtualServer");
	}
	ts3server_freeMemory(ids);
	if((error = ts3server_destroyServerLib()) != ERROR_ok)
		TSHandleError("ts3server_destroyServerLib");
	
	return 1;
}

void Ts3Srv::Tick()
{
	if(m_curClients + PROVISIONING_SLOTS_BUFFER != m_serverSlots)
	{
		m_serverSlots = m_curClients + PROVISIONING_SLOTS_BUFFER;
		if((error = ts3server_setVirtualServerVariableAsInt(serverID, VIRTUALSERVER_MAXCLIENTS, m_serverSlots)) != ERROR_ok)
			TSHandleError("VIRTUALSERVER_MAXCLIENTS");
		if((error = ts3server_flushVirtualServerVariable(serverID)) != ERROR_ok)
			TSHandleError("VIRTUALSERVER_MAXCLIENTS flush");
	}
}

uint64 Ts3Srv::CreateVirtualServer(const char* name, int port, int maxClients)
{
	char buffer[BUFSIZ] = { 0 };
	char filename[BUFSIZ];
	char port_str[20];
	char *keyPair;
	uint64 serverID;
	strcpy(filename, "ts_keypair_");
	sprintf(port_str, "%d", port);
	strcat(filename, port_str);
	strcat(filename, ".txt");
	if(readKeyPairFromFile(filename, buffer) == 0) {
		keyPair = buffer;
	} else {
		keyPair = "";
	}
	MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]Creating virtual server with %d slots\n", maxClients);
	if((error = ts3server_createVirtualServer(port, "0.0.0.0", name, keyPair, maxClients, &serverID)) != ERROR_ok)
		return TSHandleError("ts3server_createVirtualServer");
	if(!*buffer)
	{
		if((error = ts3server_getVirtualServerKeyPair(serverID, &keyPair)) != ERROR_ok)
		{
			return TSHandleError("ts3server_getVirtualServerKeyPair");
		}
		if(writeKeyPairToFile(filename, keyPair) != 0)
		{
			MGetMatchServer()->LOG(MGetMatchServer()->LOG_PROG, "[TS3]!!! was unable to write keypair\n");
		}
		ts3server_freeMemory(keyPair);
	}
	m_serverSlots = maxClients;
	return serverID;
}


static unsigned int WINAPI TSServer_Thread(void* in_ptr)
{
	MGetMatchServer()->TsSrv.Startup();
	while(MGetMatchServer()->TsSrv.m_stopRequest == 0)
	{
		::Sleep(200);
		MGetMatchServer()->TsSrv.Tick();
	}
	MGetMatchServer()->TsSrv.Shutdown();
	return 0;
}


void TSServer_Start(int port)
{
	MGetMatchServer()->TsSrv.m_serverPort = port;
	MGetMatchServer()->TsSrv.m_hThread = (HANDLE)_beginthreadex(NULL, 0, TSServer_Thread, &MGetMatchServer()->TsSrv, 0, NULL);
}

void TSServer_Stop()
{
	if(MGetMatchServer()->TsSrv.m_hThread)
	{
		MGetMatchServer()->TsSrv.m_stopRequest = 1;
		WaitForSingleObject(MGetMatchServer()->TsSrv.m_hThread, 2000);
	}
}
#endif