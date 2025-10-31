#ifndef _ZVOTEINTERFACE_H
#define _ZVOTEINTERFACE_H

//#pragma once

#include <string>
#include <vector>
using namespace std;

class MDrawContext;


class ZVoteInterface {
protected:
	bool			m_bShowTargelist;
	char			m_szDiscuss[128];
	vector<string>	m_TargetList;
	string			m_strTargetName;
#ifdef _RADIO
	bool			m_bShowRadioCommandList;
	vector<string>	m_RadioCommandList;
#endif
protected:
	const char* GetDiscuss()				{ return m_szDiscuss; }
	void SetDiscuss(const char* pszDiscuss)	{ strcpy(m_szDiscuss, pszDiscuss); }

	int ConvKeyToIndex(char nChar);
	char ConvIndexToKey(int nIndex);
	
	bool OnVoteRun(int nTargetIndex);
#ifdef _RADIO
	bool OnRadioCommand(int nTargetCommand);
#endif
public:
	ZVoteInterface()					{ Clear(); }
	~ZVoteInterface()					{ Clear(); }

	bool GetShowTargetList()		{ return m_bShowTargelist; }
	void ShowTargetList(bool bVal)	{ m_bShowTargelist = bVal; }
#ifdef _RADIO
	bool GetRadioCommandList() { return m_bShowRadioCommandList; }
	void SetRadioCommandList(bool bVal) { m_bShowRadioCommandList = bVal; }
#endif
	void Clear();
	void CallVote(const char* pszDiscuss);
#ifdef _RADIO
	void CallRadioCommandList();
#endif
	void VoteInput(char nKey);
#ifdef _RADIO
	void RadioCommandInput(char nKey);
#endif
	void CancelVote();

	void DrawVoteTargetlist(MDrawContext* pDC);
	void DrawVoteMessage(MDrawContext* pDC);
#ifdef _RADIO
	void DrawRadioCommandList(MDrawContext* pDC);
#endif
	const char* GetTargetName() { return m_strTargetName.c_str(); }
};


#endif