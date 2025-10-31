#include "stdafx.h"

#include "ZChat.h"
#include "MChattingFilter.h"
#include "ZApplication.h"
#include "ZGameInterface.h"
#include "ZPost.h"
#include "ZCombatChat.h"
#include "ZCombatInterface.h"
#include "ZIDLResource.h"
#include "MListBox.h"
#include "MLex.h"
#include "MTextArea.h"
#include "ZMyInfo.h"

#include "ZChat_CmdID.h"
#include "ZConfiguration.h"
#define ZCHAT_CHAT_DELAY				1000		
#define ZCHAT_CHAT_ABUSE_COOLTIME		(1000 * 60)		// 1ｺﾐ
#define ZCHAT_CLEAR_DELAY				(1000 * 10)		// ｹﾝｺｹ ﾀﾎｽﾄ ﾁｦｰﾅ ｽﾃｰ｣..10ﾃﾊ


void ZChatOutput(const char* szMsg, ZChat::ZCHAT_MSG_TYPE msgtype, ZChat::ZCHAT_LOC loc,MCOLOR _color)
{
	ZGetGameInterface()->GetChat()->Output(szMsg, msgtype, loc , _color);
	void(*thisCall)(const char* , ZChat::ZCHAT_MSG_TYPE , ZChat::ZCHAT_LOC ,MCOLOR );
	thisCall = ZChatOutput;
	CHECK_RETURN_CALLSTACK(thisCall);
}

void ZChatOutput(MCOLOR color, const char* szMsg, ZChat::ZCHAT_LOC loc)
{
	ZGetGameInterface()->GetChat()->Output(color, szMsg, loc);
}

void ZChatOutputMouseSensitivityChanged(int old, int neo)
{
	char sz[256] = "";
	sprintf(sz, "%s [%d] -> [%d]", ZStr(std::string("UI_OPTION_20")), old, neo);
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), sz);
}

void ZChatOutputMouseSensitivityCurrent(int i)
{
	char sz[256] = "";
	sprintf(sz, "%s [%d]", ZStr(std::string("UI_OPTION_20")), i);
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), sz);
}

#define DWSIZEOF(x) ((sizeof(x) + sizeof(DWORD) - 1) & ~(sizeof(DWORD) - 1))
#define GETVA(lastarg) ((va_list)&lastarg + DWSIZEOF(lastarg))

void ZChatOutputF(const char* szFormat, ...)
{
	char buffer[256] = { 0 };
	vsnprintf(buffer, sizeof(buffer), szFormat, GETVA(szFormat));
	ZChatOutput(buffer, ZChat::CMT_NORMAL);
}

ZChat::ZChat()
{
	m_nLastInputTime = 0;
	m_nSameMsgCount = 0;
	m_nLastInputMsg[0] = 0;
	m_nLastAbuseTime = 0;
	m_nAbuseCounter = 0;
	m_szWhisperLastSender[0] = 0;

	InitCmds();
}

ZChat::~ZChat()
{

}

bool ZChat::CheckRepeatInput(string szMsg)
{
#ifndef _PUBLISH
	return true;
#endif

	if (!stricmp(m_nLastInputMsg, szMsg.c_str())) m_nSameMsgCount++;
	else m_nSameMsgCount = 0;

	DWORD this_time = timeGetTime();

	if(this_time-m_nLastInputTime > ZCHAT_CLEAR_DELAY) {//10ﾃﾊｰ｡ ﾁｪｸ・ｰｰﾀｺ ｸﾞｽﾃﾁ・ﾀﾎﾁ､ｾﾈﾇﾔ..
		m_nSameMsgCount = 0;
	}

	m_nLastInputTime = this_time;

	strcpy(m_nLastInputMsg, szMsg.c_str());

	if (m_nSameMsgCount >= 2)
	{
		Output( ZErrStr(MERR_CANNOT_INPUT_SAME_CHAT_MSG), 
			CMT_SYSTEM);
		return false;
	}
	CHECK_RETURN_CALLSTACK(CheckRepeatInput);
	return true;
}

bool ZChat::Input(char* szMsg)
{
	if( 0 == strlen(szMsg) ) {
		return false;
	}

	GunzState state = ZApplication::GetGameInterface()->GetState();

#ifdef _PUBLISH
	if ((timeGetTime() - m_nLastInputTime) < ZCHAT_CHAT_DELAY)
	{
		ZGetSoundEngine()->PlaySound("if_error");
		return false;
	}
#endif
	
	// ﾄｿｸﾇｵ・ｸ昞ﾉｾ・ﾃｳｸｮ //////////////////////
	bool bMsgIsCmd = false;
	if (szMsg[0] == '/')
	{
		if (strlen(szMsg) >= 2)
		{
			if (((szMsg[1] > 0) && (isspace(szMsg[1]))) == false)
			{
				ZChatCmdFlag nCurrFlag = CCF_NONE;

				switch (state)
				{
					case GUNZ_LOBBY: nCurrFlag = CCF_LOBBY; break;
					case GUNZ_STAGE: nCurrFlag = CCF_STAGE; break;
					case GUNZ_GAME:  nCurrFlag = CCF_GAME;  break;
				}

				int nCmdInputFlag = ZChatCmdManager::CIF_NORMAL;

				// ｰ・ｮﾀﾚﾀﾎﾁ・ﾆﾇｺｰ
				// Custom: Event Team
				if ((ZGetMyInfo()->GetUGradeID() == MMUG_ADMIN) || 
					(ZGetMyInfo()->GetUGradeID() == MMUG_DEVELOPER) ||
#ifdef _NEWGRADE
					(ZGetMyInfo()->GetUGradeID() == MMUG_JORK) ||
#endif
					(ZGetMyInfo()->GetUGradeID() == MMUG_EVENTMASTER) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_EVENTTEAM)||
					(ZGetMyInfo()->GetUGradeID() == MMUG_MANAGER))
				{
					nCmdInputFlag |= ZChatCmdManager::CIF_ADMIN;
				}
#ifdef _EVENTCMD
				if ((ZGetMyInfo()->GetUGradeID() == MMUG_ADMIN) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_DEVELOPER) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_JORK) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_EVENTMASTER) ||
					(ZGetMyInfo()->GetUGradeID() == MMUG_JORK))
				{
					nCmdInputFlag |= ZChatCmdManager::CIF_EVENT;
				}
#endif
				// Custom: Quest/Clan Server
				if ((ZIsLaunchDevelop()) /*&& 
					((ZGetGameClient()->GetServerMode() == MSM_TEST)*/ || (!ZGetGameClient()->IsConnected())) //)
				{
					nCmdInputFlag |= ZChatCmdManager::CIF_TESTER;
				}

				bool bRepeatEnabled = m_CmdManager.IsRepeatEnabled(&szMsg[1]);
				if (!bRepeatEnabled)
				{
					if (!CheckRepeatInput(szMsg)) return false;
				}

				if (m_CmdManager.DoCommand(&szMsg[1], nCurrFlag, ZChatCmdManager::CmdInputFlag(nCmdInputFlag)))
				{
					return true;
				}
				else
				{
					bMsgIsCmd = false;

//					return true;			// ﾃ､ﾆﾃﾃ｢ｿ｡ ﾇ･ｽﾃ ｾﾈｵﾇｰﾔ ﾇﾑｴﾙ.
				}
			}
		}

		ZChatOutput("Unrecognized command.");
		return false;
	}

	
	if (!bMsgIsCmd)
	{
		if (!CheckRepeatInput(szMsg)) return false;
	}



	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		// Custom: Made CMT_SYSTEM
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
		return false;
	}

	// ｿ蟷戝・//////////////////////////////////

	string strMsg = szMsg; //copy contents of szMsg to strMsg (easier to manipulate)

	if (!CheckChatFilter(szMsg)) return false;
	// ﾆ暲ﾇ ﾄｿｸﾇｵ・ｸ昞ﾉｾ・//////////////////////
	bool bTeamChat = false;
	if (strMsg.c_str()[0] == '!') {	// Team Chat
		bTeamChat = true;
	} else if (strMsg.c_str()[0] == '@') {	// ChatRoom
		ZPostChatRoomChat(&strMsg.c_str()[1]);
		return true;
	} else if (strMsg.c_str()[0] == '#') {	// Clan Chat
		ZPostClanMsg(ZGetGameClient()->GetPlayerUID(), &strMsg.c_str()[1]);
		return true;
	} else if (strMsg.c_str()[0] == '$') { // Lobby Chat
		ZPostChannelChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetChannelUID(), &strMsg.c_str()[1]);
		return true;
	}
#ifdef _STAFFCHAT
	else if (szMsg[0] == '$') {		// Staff Chat
		ZPostStaffChat(&szMsg[1]);
		return true;
	}
#endif
	switch (state)
	{
	case GUNZ_GAME:
		{
			ZCombatInterface* pCombatInterface = ZGetGameInterface()->GetCombatInterface();
			int nTeam = MMT_ALL;
			if (pCombatInterface->IsTeamChat() || bTeamChat)
				nTeam = ZGetGame()->m_pMyCharacter->GetTeamID();
			if (strMsg.c_str()[0] == '!' && nTeam != MMT_ALL) // remove the ! from the team chat
			{
				ZPostGameChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), (char*)&strMsg.c_str()[1], nTeam);
			}
			else
			{
				ZPostGameChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), (char*)strMsg.c_str(), nTeam);
			}
		}
		break;
	case GUNZ_LOBBY:
		{
			ZPostChannelChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetChannelUID(), strMsg.c_str());
		}
		break;
	case GUNZ_STAGE:
		{
			ZPostStageChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), (char*)strMsg.c_str());
		}
		break;
	}

	return true;
}

void ZChat::Output(const char* szMsg, ZCHAT_MSG_TYPE msgtype, ZCHAT_LOC loc,MCOLOR _color)
{
	GunzState state = ZApplication::GetGameInterface()->GetState();

	char szOutput[512];

	char szBuffer[512];
	memset( szBuffer, 0, sizeof( szBuffer ) );

	strcat_s(szBuffer, szMsg);

	if (strlen(szBuffer) < sizeof(szOutput)-2) strcpy(szOutput, szBuffer);
	else {
		_ASSERT(0);	// ﾃ､ﾆﾃ ｹﾛｰ｡ ｳﾊｹｫ ﾀﾛﾀｽ
		char temp[32];strncpy(temp,szMsg,30);temp[30]=0;temp[31]=0;
		mlog("warning : chat buffer overflow : %s\n",temp);
		strncpy(szOutput, szMsg, sizeof(szOutput)-2);
		szOutput[sizeof(szOutput)-1]=0;
		szOutput[sizeof(szOutput)-2]=0;
	}
	if (msgtype == CMT_CURSOUR)
	{
		MCOLOR color = MCOLOR(255, 0, 0);
		Output(color, szOutput, loc);
		return;
	}
	else if (msgtype == CMT_STAFFHELP)
	{
		MCOLOR color = MCOLOR(255, 255, 255);
		Output(color, szOutput, loc);
		return;
	}
#ifdef _STAFFCHAT
	else if (msgtype == CMT_STAFFCHAT)
	{
		MCOLOR color = MCOLOR(255, 251, 0);
		Output(color, szOutput, loc);
		return;
	}
#endif
	string strMsg = szOutput;
	if (ZGetConfiguration()->GetEtc()->bChatFilter)
	{
		for (int i = 0; i < strMsg.length(); ++i)
		{
			MAbuseWord* pAbuseWord = MGetChattingFilter()->Find(strMsg.c_str());
			if (pAbuseWord)
			{
				string::size_type pos = strMsg.find(pAbuseWord->GetAbuseWord());
				char szWarning[64];

				if (pos != string::npos)
				{
					strMsg.replace(pos, strlen(pAbuseWord->GetAbuseWord()), "*beep*");
				}
			}
		}
	}

	if (msgtype == CMT_SYSTEM)
	{
		MCOLOR color = MCOLOR(ZCOLOR_CHAT_SYSTEM_GAME);

		if (((loc == CL_CURRENT) && (state==GUNZ_GAME)) || (loc==CL_GAME))
		{
			color = MCOLOR(ZCOLOR_CHAT_SYSTEM_GAME);
		}
		else if (((loc == CL_CURRENT) && (state==GUNZ_LOBBY)) || (loc==CL_LOBBY))
		{
			color = MCOLOR(ZCOLOR_CHAT_SYSTEM_LOBBY);
		}
		else if (((loc == CL_CURRENT) && (state==GUNZ_STAGE)) || (loc==CL_STAGE))
		{
			color = MCOLOR(ZCOLOR_CHAT_SYSTEM_STAGE);
		}

		Output(color, strMsg.c_str(), loc);
		return;
	}
	else if (msgtype == CMT_BROADCAST)
	{
		MCOLOR color = MCOLOR(ZCOLOR_CHAT_BROADCAST_GAME);

		if (((loc == CL_CURRENT) && (state==GUNZ_GAME)) || (loc==CL_GAME))
		{
			color = MCOLOR(ZCOLOR_CHAT_BROADCAST_GAME);
		}
		else if (((loc == CL_CURRENT) && (state==GUNZ_LOBBY)) || (loc==CL_LOBBY))
		{
			color = MCOLOR(ZCOLOR_CHAT_BROADCAST_LOBBY);
		}
		else if (((loc == CL_CURRENT) && (state==GUNZ_STAGE)) || (loc==CL_STAGE))
		{
			color = MCOLOR(ZCOLOR_CHAT_BROADCAST_STAGE);
		}

		Output(color, strMsg.c_str(), loc);
		return;
	}
	else if ( msgtype == CMT_NORMAL)
	{
		if (((loc == CL_CURRENT) && (state==GUNZ_GAME)) || (loc==CL_GAME))
		{
			ZGetCombatInterface()->OutputChatMsg(szOutput);
		}
		else if (((loc == CL_CURRENT) && (state==GUNZ_LOBBY)) || (loc==CL_LOBBY))
		{
			if(_color.GetARGB() == ZCOLOR_CHAT_SYSTEM )// ｱ篌ｻｻﾎ ｰ豼・ｷﾎｺﾇ ｱ篌ｻｻ・
				LobbyChatOutput(strMsg.c_str());
			else 
				LobbyChatOutput(strMsg.c_str(),_color);	// ｵ﨧ﾏﾇﾑｻ・.
		}
		else if (((loc == CL_CURRENT) && (state==GUNZ_STAGE)) || (loc==CL_STAGE))
		{
			if(_color.GetARGB() == ZCOLOR_CHAT_SYSTEM )// ｱ篌ｻｻﾎ ｰ豼・ｽｺﾅﾗﾀﾌﾁﾇ ｱ篌ｻｻ・
				StageChatOutput(strMsg.c_str());
			else 
				StageChatOutput(strMsg.c_str(),_color);	// ｵ﨧ﾏﾇﾑｻ・.
		}

		m_ReportAbuse.OutputString(szMsg);
	}
}

void ZChat::Output(MCOLOR color, const char* szMsg, ZCHAT_LOC loc)
{
	m_ReportAbuse.OutputString(szMsg);

	GunzState state = ZApplication::GetGameInterface()->GetState();

	char szOutput[512];
	if (strlen(szMsg) < sizeof(szOutput)-2) strcpy(szOutput, szMsg);
	else {
		_ASSERT(0);	// ﾃ､ﾆﾃ ｹﾛｰ｡ ｳﾊｹｫ ﾀﾛﾀｽ
		char temp[32];strncpy(temp,szMsg,30);temp[30]=0;temp[31]=0;
		mlog("warning : chat buffer overflow : %s\n",temp);
		strncpy(szOutput, szMsg, sizeof(szOutput)-2);
		szOutput[sizeof(szOutput)-1]=0;
		szOutput[sizeof(szOutput)-2]=0;
	}

	string strMsg = szOutput;
	if (ZGetConfiguration()->GetEtc()->bChatFilter)
	{
		for (int i = 0; i < strMsg.length(); ++i)
		{
			MAbuseWord* pAbuseWord = MGetChattingFilter()->Find(strMsg.c_str());
			if (pAbuseWord)
			{
				string::size_type pos = strMsg.find(pAbuseWord->GetAbuseWord());
				char szWarning[64];

				if (pos != string::npos)
				{
					if (ZGetConfiguration()->GetEtc()->bChatFilter)
					{
						strMsg.replace(pos, strlen(pAbuseWord->GetAbuseWord()), "*beep*");
					}
				}
			}
		}
	}

	if (((loc == CL_CURRENT) && (state==GUNZ_GAME)) || (loc==CL_GAME))
	{
		ZCombatInterface* pCombat = ZGetCombatInterface();
		if (pCombat)
			pCombat->OutputChatMsg(color, strMsg.c_str());
	}
	else if (((loc == CL_CURRENT) && (state==GUNZ_LOBBY)) || (loc==CL_LOBBY))
	{
		LobbyChatOutput( strMsg.c_str() , color );
	}
	else if (((loc == CL_CURRENT) && (state==GUNZ_STAGE)) || (loc==CL_STAGE))
	{
		StageChatOutput( strMsg.c_str() , color );
	}
}

void ZChat::Clear(ZCHAT_LOC loc)
{
	GunzState state = ZApplication::GetGameInterface()->GetState();

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if (((loc == CL_CURRENT) && (state==GUNZ_GAME)) || (loc==CL_GAME))
	{
	}
	else if (((loc == CL_CURRENT) && (state==GUNZ_LOBBY)) || (loc==CL_LOBBY))
	{
		MTextArea* pWidget = (MTextArea*)pResource->FindWidget("ChannelChattingOutput");
		if (pWidget != NULL) pWidget->Clear();
	}
	else if (((loc == CL_CURRENT) && (state==GUNZ_STAGE)) || (loc==CL_STAGE))
	{
		MTextArea* pWidget = (MTextArea*)pResource->FindWidget("StageChattingOutput");
		if (pWidget != NULL) pWidget->Clear();
	}
}

#define MAX_CHAT_LINES	100

void ZChat::LobbyChatOutput(const char* szChat,MCOLOR color)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MTextArea* pWidget = (MTextArea*)pResource->FindWidget("ChannelChattingOutput");
	if(!pWidget) return;

	pWidget->AddText(szChat,color);
	while(pWidget->GetLineCount()>MAX_CHAT_LINES)
		pWidget->DeleteFirstLine();

}

void ZChat::StageChatOutput(const char* szChat,MCOLOR color)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MTextArea* pWidget = (MTextArea*)pResource->FindWidget("StageChattingOutput");
	if(!pWidget) return;

	pWidget->AddText(szChat,color);
	while(pWidget->GetLineCount()>MAX_CHAT_LINES)
		pWidget->DeleteFirstLine();

}

void ZChat::Report112(const char* szReason)
{
	m_ReportAbuse.Report(szReason);
}

bool ZChat::CheckChatFilter(const char* szMsg)
{
	if (m_nLastAbuseTime > 0) 
	{
		if ((timeGetTime() - m_nLastAbuseTime) < ZCHAT_CHAT_ABUSE_COOLTIME)
		{
			char szOutput[512];
			sprintf(szOutput, 
				ZErrStr(MERR_CHAT_PENALTY_FOR_ONE_MINUTE) );
			Output(szOutput, CMT_SYSTEM);
			return false;
		}
		else
		{
			m_nLastAbuseTime = 0;
		}
	}
	MAbuseWord* pAbuseWord = MGetChattingFilter()->Find(szMsg);
	if ( pAbuseWord && pAbuseWord->GetFilteringLevel() > 1)
	{
#ifndef _DEBUG
		m_nAbuseCounter++;

		if ( m_nAbuseCounter >= 3)			// 3ｹ・ｿｬｼﾓ ｿ衂ﾏｸ・1ｺﾐｰ｣ ﾃ､ﾆﾃｱﾝﾁ・
			m_nLastAbuseTime = timeGetTime();
#endif

		char szOutput[512];
		sprintf( szOutput, "%s (%s)", ZErrStr( MERR_CANNOT_ABUSE), pAbuseWord->GetAbuseWord());
		Output( szOutput, CMT_SYSTEM);

		return false;
	}
	else
	{
		m_nAbuseCounter = 0;
	}

	return true;
}

bool _InsertString(char* szTarget, const char* szInsert, int nPos, int nMaxTargetLen=-1)
{
	int nTargetLen = (int)strlen(szTarget);
	int nInsertLen = (int)strlen(szInsert);
	if(nPos>nTargetLen) return false;
	if(nMaxTargetLen>0 && nTargetLen+nInsertLen>=nMaxTargetLen) return false;

	char* temp = new char[nTargetLen-nPos+2];
	strcpy(temp, szTarget+nPos);
	strcpy(szTarget+nPos, szInsert);
	strcpy(szTarget+nPos+nInsertLen, temp);
	delete[] temp;

	return true;
}

// ﾃ､ﾆﾃﾃ｢ｿ｡ｼｭ '/r ' ﾀｻ ﾄ｡ｸ・ﾀﾌﾀ・｡ ｱﾓｸｻﾀﾌ ｿﾔｴ・ｻ邯ﾑﾅﾗ ｹﾙｷﾎ ｱﾓｸｻ ｸ昞ﾉｾ﨧ﾎ ｺｯｰ・
void ZChat::FilterWhisperKey(MWidget* pWidget)
{
	char text[256];
	strcpy(text, pWidget->GetText());

	if ((!stricmp(text, "/r ")) || (!stricmp(text, "/､｡ ")))
	{
		char msg[128] = "";

		ZChatCmd* pWhisperCmd = GetCmdManager()->GetCommandByID(CCMD_ID_WHISPER);
		if (pWhisperCmd)
		{
			sprintf(msg, "/%s ", pWhisperCmd->GetName());
		}

		//strcpy(msg, "/ｱﾓｸｻ ");

		if (m_szWhisperLastSender[0])
		{
			strcat(msg, m_szWhisperLastSender);
			strcat(msg, " ");
		}
		pWidget->SetText(msg);
	}


}
///////////////////////////////////////////////////////////////////////////
