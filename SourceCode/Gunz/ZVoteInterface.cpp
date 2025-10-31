#include "stdafx.h"
#include "ZVoteInterface.h"
#include "ZGameClient.h"
#include "RMesh.h"
#include "RVisualMeshMgr.h"
#include "MMatchObject.h"
#include "ZCharacterManager.h"
#include "ZPost.h"
#include "ZApplication.h"
#include "ZCombatInterface.h"

#include "ZApplication.h"


bool ZVoteInterface::OnVoteRun(int nTargetIndex)
{
	if (nTargetIndex < 0 || nTargetIndex >= (int)m_TargetList.size())
		return false;

	m_strTargetName = m_TargetList[nTargetIndex];

	ZPOSTCMD2(MC_MATCH_CALLVOTE, MCmdParamStr("kick"), MCmdParamStr(m_strTargetName.c_str()) );
	ZPOSTCMD0(MC_MATCH_VOTE_YES);	

	Clear();
	return true;
}

void ZVoteInterface::Clear()
{
	ShowTargetList(false);
#ifdef _RADIO
	SetRadioCommandList(false);
#endif
	m_szDiscuss[0] = NULL;
	m_TargetList.clear();
}
#ifdef _RADIO
bool ZVoteInterface::OnRadioCommand(int nTargetCommand)
{
	char buff[128];
	string strTarget = m_RadioCommandList[nTargetCommand];
	sprintf(buff, "%s", strTarget.c_str());
	ZChatOutput(buff);

	Clear();
	return true;
}
void ZVoteInterface::CallRadioCommandList()
{
	if (GetRadioCommandList())
		return;

	Clear();

	ShowTargetList(false);
	SetRadioCommandList(true);

	m_RadioCommandList.push_back("Go! - Go, go, go.");
	m_RadioCommandList.push_back("Taking Fire, Need Assistance!");
	m_RadioCommandList.push_back("Cover Me!");
	m_RadioCommandList.push_back("Get in Position");
	m_RadioCommandList.push_back("Affirmative/Roger");
	m_RadioCommandList.push_back("Enemy Spotted");
	m_RadioCommandList.push_back("Need Backup");
	m_RadioCommandList.push_back("I'm in Position");
	m_RadioCommandList.push_back("Enemy Down");
}
#endif
void ZVoteInterface::CallVote(const char* pszDiscuss)
{
	if (GetShowTargetList())
		return;

	Clear();

	SetDiscuss(pszDiscuss);
#ifdef _RADIO
	SetRadioCommandList(false);
#endif

	// Custom: Enhanced Kick System (disable)
	//ShowTargetList(true);

	// Push Target List
	for(ZCharacterManager::iterator i=ZGetCharacterManager()->begin(); i!=ZGetCharacterManager()->end();i++)
	{
		ZCharacter *pChar = (ZCharacter*) i->second;
		if(pChar->IsAdminHide()) continue;
		m_TargetList.push_back(pChar->GetProperty()->GetName());
	}

	if (m_TargetList.size() <= 16)
		ShowTargetList(true);
	else
		ZGetGameInterface()->ShowKickPlayerDialog( true);
}
void TextRelative(MDrawContext* pDC, float x, float y, const char* szText, bool bCenter);
void ZVoteInterface::DrawVoteTargetlist(MDrawContext* pDC)
{
	if(GetShowTargetList() == false) return;

	MFont *pFont=ZGetGameInterface()->GetCombatInterface()->GetGameFont();
	pDC->SetFont(pFont);

	float y = 0.3f;
	float linespace = (float)pFont->GetHeight() * 1.1 / (float)MGetWorkspaceHeight();

	pDC->SetColor(MCOLOR(0xFFFFFFFF));

	TextRelative( pDC, .05f, y, ZMsg(MSG_VOTE_SELECT_PLAYER_TO_KICK) );

	y+=2.f*linespace;

	for (int i=0; i<(int)m_TargetList.size(); i++) 
	{
		string& strName = m_TargetList[i];

		char szBuffer[256];
		sprintf(szBuffer,"[%c] %s", ConvIndexToKey(i), strName.c_str());

		pDC->SetColor(MCOLOR(0xFFFFFFFF));
		TextRelative(pDC,.05f,y,szBuffer);

		y+=linespace;
	}

	y+=linespace;
	pDC->SetColor(MCOLOR(0xFFFFFFFF));
	
	TextRelative(pDC,.05f,y, ZMsg(MSG_VOTE_SELECT_PLAYER_CANCEL));
}
#ifdef _RADIO
void ZVoteInterface::DrawRadioCommandList(MDrawContext* pDC)
{
	if (GetRadioCommandList() == false) return;

	MFont* pFont = ZGetGameInterface()->GetCombatInterface()->GetGameFont();
	pDC->SetFont(pFont);

	float y = 0.18f;
	float x = 0.22f;
	float linespace = (float)pFont->GetHeight() * 1.1 / (float)MGetWorkspaceHeight();

	pDC->SetColor(MCOLOR(0xFFFFFFFF));

	//TextRelative(pDC, .015f, y, "Choose a command from the list:");
	TextRelative(pDC, y, x, "Choose a command from the list:");

	y += 2.f * linespace;

	for (int i = 0; i < (int)m_RadioCommandList.size(); i++)
	{
		string& strName = m_RadioCommandList[i];

		char szBuffer[256];
		sprintf(szBuffer, "[%c] %s", ConvIndexToKey(i), strName.c_str());

		pDC->SetColor(MCOLOR(0xFFFFFFFF));
		//TextRelative(pDC, .015f, y, szBuffer);
		TextRelative(pDC, 0.22f, y, szBuffer);

		y += linespace;
	}

	y += linespace;
	pDC->SetColor(MCOLOR(0xFFFFFFFF));

//	TextRelative(pDC, .015f, y, "Press a key to use the command.");
	TextRelative(pDC, 0.18f, 0.22f, "Press a key to use the command.");
	// Background
	MCOLOR BG1 = MCOLOR(28, 28, 28, 160);
	pDC->SetColor(BG1);
	pDC->FillRectangle(595 * MGetWorkspaceWidth() / 800.f, 171 * MGetWorkspaceHeight() / 600.f, 195 * MGetWorkspaceWidth() / 800.f, 250 * MGetWorkspaceHeight() / 600.f);

	//////////////// BG1 ////////////////
	MCOLOR BG2 = MCOLOR(31, 138, 201, 34);
	pDC->SetColor(BG2);
	pDC->FillRectangle(595 * MGetWorkspaceWidth() / 800.f, 141 * MGetWorkspaceHeight() / 600.f, 195 * MGetWorkspaceWidth() / 800.f, 30 * MGetWorkspaceHeight() / 600.f);

}
#endif
void ZVoteInterface::DrawVoteMessage(MDrawContext* pDC)		// 투표가 진행중일때 메시지
{
	// 투표 진행중일때 메시지
	if ( (ZGetGameInterface()->GetState() == GUNZ_GAME) &&
		ZGetGameClient() &&
		ZGetGameClient()->IsVoteInProgress() && 
		ZGetGameClient()->CanVote() ) 
	{
		MFont *pFont=ZGetGameInterface()->GetCombatInterface()->GetGameFont();
		pDC->SetFont(pFont);
		pDC->SetColor(MCOLOR(0x80ffffff));
		TextRelative(pDC,300.f/800.f,550/600.f,ZGetGameClient()->GetVoteMessage());

		if(timeGetTime()/500 % 2 == 0 ) {

			TextRelative(pDC,300.f/800.f,565/600.f, ZMsg(MSG_VOTE_YESNO));
		}
	}
}
int ZVoteInterface::ConvKeyToIndex(char nChar)
{
	return (nChar >= 'A') ? nChar-'A'+10 : nChar-'0';
}

char ZVoteInterface::ConvIndexToKey(int nIndex)
{
	return (nIndex >= 10) ? 'A'+nIndex-10 : '0'+nIndex;
}

void ZVoteInterface::VoteInput(char nKey)
{
	if (GetShowTargetList()) {	
		// 1~0 , A~F 까지 - 누른키에 해당하는 플레이어 없을 수도 있음
		int nIndex = ConvKeyToIndex(nKey);
		if (OnVoteRun(nIndex)) {
			ZGetGameClient()->SetCanVote(false);
			ShowTargetList(false);
			return;
		}
	}

	if (ZGetGameClient()->CanVote()) {
		if (nKey == 'Y') {
			ZPOSTCMD0(MC_MATCH_VOTE_YES);
			ZGetGameClient()->SetCanVote(false);
			ShowTargetList(false);
		} else if (nKey == 'N') {
			ZPOSTCMD0(MC_MATCH_VOTE_NO);
			ZGetGameClient()->SetCanVote(false);
			ShowTargetList(false);
		}
		return;
	}
}
#ifdef _RADIO
void ZVoteInterface::RadioCommandInput(char nKey)
{
	if (GetRadioCommandList()) {
		int nIndex = ConvKeyToIndex(nKey);
		if (OnRadioCommand(nIndex)) {
			return;
		}
	}
}
#endif
void ZVoteInterface::CancelVote()
{
	Clear();
}
