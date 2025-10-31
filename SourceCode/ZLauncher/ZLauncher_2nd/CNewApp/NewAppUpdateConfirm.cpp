#include "stdafx.h"
#include "NewAppUpdateConfirm.h"
#include <Mmsystem.h>

CNewAppUpdateConfirm::CNewAppUpdateConfirm(CWnd* pParent) : CDDBDialog(IDD_UPDATE_CONFIRM, pParent)
{
	CDDBDLGINFO DlgInfo;
	DlgInfo.nWidth = 330;
	DlgInfo.nHeight = 180;
	DlgInfo.IDSkinResource = IDR_BMP_POPUPDLG;
	DlgInfo.nTextColor = RGB(30, 30, 30);
	DlgInfo.nTextBkColor = RGB(166, 156, 153);
#ifdef LOCALE_NHNUSA
	DlgInfo.nTextColor = RGB(150, 150, 150);
	DlgInfo.nTextBkColor = RGB(46, 47, 48);
#endif
	SetDialogInfo(DlgInfo);

	m_pParentDialog = NULL;
}

void CNewAppUpdateConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDDBDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDOK, m_cOK);
	DDX_Control(pDX, IDCANCEL, m_cCancel);
}

BEGIN_MESSAGE_MAP(CNewAppUpdateConfirm, CDDBDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CNewAppUpdateConfirm::OnInitDialog()
{
	CDDBDialog::OnInitDialog();

	if (m_pParentDialog != NULL)
	{
		CRect rect;
		m_pParentDialog->GetWindowRect(&rect);
		int x = rect.left + rect.Width() / 2 - GetWidth() / 2;
		int y = rect.top + rect.Height() / 2 - GetHeight() / 2;
		MoveWindow(x, y, GetWidth(), GetHeight());
	}

	DrawPanel(6, 25, GetWidth() - 12, GetHeight() - 31);

	m_cOK.InitDDBButton(IDR_BMP_BUTTON, IDR_WAV_PUSHBUTTON);
	m_cCancel.InitDDBButton(IDR_BMP_BUTTON, IDR_WAV_PUSHBUTTON);

	SetWindowText(m_strCaption);
	GetScreen()->PutText(9, 5, 17, "Tahoma", m_strCaption, RGB(0, 0, 0), TS_BOLD);
	GetScreen()->PutText(10, 6, 17, "Tahoma", m_strCaption, RGB(238, 156, 0), TS_BOLD);

	m_cMessage.MoveWindow(13, 35, GetWidth() - 26, GetHeight() - 70);
	PutMessage(10);

	if (m_nButtonType == MB_OKCANCEL)
	{
		m_cOK.SetWindowText("OK");
		m_cOK.ShowWindow(SW_SHOW);
		m_cOK.EnableWindow(TRUE);
		m_cOK.MoveWindow(GetWidth() / 2 - m_cOK.GetWidth() - 5, GetHeight() - 35);

		m_cCancel.SetWindowText("Cancel");
		m_cCancel.ShowWindow(SW_SHOW);
		m_cCancel.EnableWindow(TRUE);
		m_cCancel.MoveWindow(GetWidth() / 2 + 5, GetHeight() - 35);
	}
	else if (m_nButtonType == MB_YESNO)
	{
		m_cOK.SetWindowText("Yes");
		m_cOK.ShowWindow(SW_SHOW);
		m_cOK.EnableWindow(TRUE);
		m_cOK.MoveWindow(GetWidth() / 2 - m_cOK.GetWidth() - 5, GetHeight() - 35);

		m_cCancel.SetWindowText("No");
		m_cCancel.ShowWindow(SW_SHOW);
		m_cCancel.EnableWindow(TRUE);
		m_cCancel.MoveWindow(GetWidth() / 2 + 5, GetHeight() - 35);
	}
	else if (m_nButtonType == MB_OK)
	{
		m_cOK.SetWindowText("OK");
		m_cOK.ShowWindow(SW_SHOW);
		m_cOK.EnableWindow(TRUE);
		m_cOK.MoveWindow(GetWidth() / 2 - m_cOK.GetWidth() / 2, GetHeight() - 35);

		m_cCancel.ShowWindow(SW_HIDE);
	}

	m_dwTime = timeGetTime();

	SetTimer(0, 500, NULL);

	return true;
}

void CNewAppUpdateConfirm::SetDialog(CDialog* pDialog, CString strCaption, CString strMessage, UINT nButtonType)
{
	m_pParentDialog = pDialog;
	m_strCaption = strCaption;
	m_strMessage = strMessage;
	m_nButtonType = nButtonType;
}

void CNewAppUpdateConfirm::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 0)
	{
		DWORD dwCurrTime = timeGetTime() - m_dwTime;

		if (dwCurrTime > 10000)
		{
			KillTimer(0);

			OnOK();

			return;
		}

		PutMessage(10 - (dwCurrTime / 1000) - 1);
	}

	CDDBDialog::OnTimer(nIDEvent);
}

void CNewAppUpdateConfirm::PutMessage(int nTime)
{
	CString strMsg;
	strMsg.Format(m_strMessage.GetBuffer(), nTime);
	m_cMessage.SetWindowText(strMsg);
}