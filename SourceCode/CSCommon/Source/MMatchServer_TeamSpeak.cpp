#include "stdafx.h"
#include "MMatchServer.h"


#ifdef ENABLE_TEAMSPEAK
SOCKET conn;
bool MMatchServer::ConnectToTeamSpeak()//add connection retry should be as simple as setting a bool if connected
{
	const char* servername=""; //IP Server
	WSADATA wsaData;
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;
	int wsaret=WSAStartup(0x101,&wsaData);
	if(wsaret)	
		return false;
	conn=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(conn==INVALID_SOCKET)
		return false;
	if(inet_addr(servername)==INADDR_NONE)
	{
		hp=gethostbyname(servername);
	}
	else
	{
		addr=inet_addr(servername);
		hp=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
	}
	if(hp==NULL)
	{
		closesocket(conn);
		return false;
	}
	server.sin_addr.s_addr=*((unsigned long*)hp->h_addr);
	server.sin_family=AF_INET;
	server.sin_port=htons(20248);
	if(connect(conn,(struct sockaddr*)&server,sizeof(server)))
	{
		closesocket(conn);
		return false;	
	}
	return true;
}

void MMatchServer::DisconnectTeamSpeak()
{
	closesocket(conn);
	WSACleanup();
	TsConnected = false;
}

void MMatchServer::SendToTeamSpeak(char* Command)
{
	if(TsConnected == true && SOCKET_ERROR == send(conn, Command, strlen(Command), 0))
		DisconnectTeamSpeak();
}
#endif