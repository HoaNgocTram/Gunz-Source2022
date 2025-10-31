#include "stdafx.h"
#include "ZMessages.h"
#include "ZApplication.h"
#include "ZClanListBox.h"

ZClanListBox::ZClanListBox(const char* szName, MWidget* pParent, MListener* pListener) 
			: MWidget(szName, pParent, pListener)
{
	m_iNumRoom		= 0;
	m_RoomWidth		= 0;
	m_RoomHeight	= 0;

	m_pRoomFrame = MBitmapManager::Get("banner_clan.bmp");

	for (int i = 0; i < NUM_DISPLAY_CLAN; i++)
	{
		ZCLANINFO *pInfo = m_pClanInfo+i;
		pInfo->bEmpty = true;
		pInfo->nClanEmblemID = 0;
	}
}

ZClanListBox::~ZClanListBox()
{
	ClearAll();
}

void ZClanListBox::OnDraw( MDrawContext* pDC )
{
	const int nWidth = 376;
	float fRatio = (float)m_Rect.w / (float)nWidth;

	const int nGap = 7;

	for(int i=0;i<NUM_DISPLAY_CLAN;i++) 
	{
		ZCLANINFO *pInfo = m_pClanInfo+i;
		if(!pInfo->bEmpty) 
		{
			if(m_pRoomFrame)
			{
				pDC->SetBitmap(m_pRoomFrame);
				int y = (int)(fRatio * (m_pRoomFrame->GetHeight() + nGap ) * i) ;
				pDC->Draw(0, y , (int)(fRatio * m_pRoomFrame->GetWidth()), (int)(fRatio * m_pRoomFrame->GetHeight()));
				pDC->SetColor(MCOLOR(0xffffffff));
				char szBuffer[256];
				sprintf(szBuffer,ZMsg( MSG_LOBBY_WAITING ), pInfo->nPlayers, pInfo->nPlayers );
				pDC->Text((int)(fRatio*160) , (int)(y + fRatio*10) , szBuffer);
			}
		}
	}
}

void ZClanListBox::SetInfo(int nIndex, int nEmblemID, const char *szName, int nPlayers)
{
	if(nIndex<0 || nIndex>=NUM_DISPLAY_CLAN) return;

	ZCLANINFO *pInfo = m_pClanInfo+nIndex;

	ZGetEmblemInterface()->AddClanInfo(nEmblemID);
	ZGetEmblemInterface()->DeleteClanInfo(pInfo->nClanEmblemID);
	pInfo->nClanEmblemID = nEmblemID;
	strcpy(pInfo->szClanName , szName);
	pInfo->nPlayers = nPlayers;
	pInfo->bEmpty = false;

}

void ZClanListBox::Clear(int nIndex)
{
	if(nIndex<0 || nIndex>=NUM_DISPLAY_CLAN) return;

	ZCLANINFO *pInfo = m_pClanInfo+nIndex;
	pInfo->bEmpty = true;
	ZGetEmblemInterface()->DeleteClanInfo(pInfo->nClanEmblemID);
	pInfo->nClanEmblemID = 0;
}

void ZClanListBox::ClearAll()
{
	for (int i = 0; i < NUM_DISPLAY_CLAN; i++)
	{
		Clear(i);
	}
}