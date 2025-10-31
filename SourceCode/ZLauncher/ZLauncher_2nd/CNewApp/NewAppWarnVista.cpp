#include "stdafx.h"
#include "NewAppWarnVista.h"

CNewAppWarnVista::CNewAppWarnVista(CWnd* pParent) : CDDBDialog(IDD_WARNVISTA, pParent)
{
	CDDBDLGINFO DlgInfo;
	DlgInfo.nWidth = 380;
	DlgInfo.nHeight = 230;
	DlgInfo.IDSkinResource = IDR_BMP_POPUPDLG;
	DlgInfo.nTextColor = RGB(255, 255, 255);
	DlgInfo.nTextBkColor = RGB(38, 38, 38);
	SetDialogInfo(DlgInfo);

	m_bCheck = false;
}

void CNewAppWarnVista::DoDataExchange(CDataExchange* pDX)
{
	CDDBDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_CHECK, m_cCheck);
	DDX_Control(pDX, IDOK, m_cOK);
	DDX_Control(pDX, IDCANCEL, m_cCancel);
}

BEGIN_MESSAGE_MAP(CNewAppWarnVista, CDDBDialog)
END_MESSAGE_MAP()

BOOL CNewAppWarnVista::OnInitDialog()
{
	CDDBDialog::OnInitDialog();

	CRect rect;
	GetWindowRect(&rect);
	int x = rect.left + rect.Width() / 2 - GetWidth() / 2;
	int y = rect.top + rect.Height() / 2 - GetHeight() / 2;
	MoveWindow(x, y, GetWidth(), GetHeight());

	DrawPanel(6, 25, GetWidth() - 12, GetHeight() - 31);

	m_cOK.InitDDBButton(IDR_BMP_BUTTON, IDR_WAV_PUSHBUTTON);
	m_cCancel.InitDDBButton(IDR_BMP_BUTTON, IDR_WAV_PUSHBUTTON);

	SetWindowText(m_strCaption);
	GetScreen()->PutText(9, 5, 17, "Tahoma", m_strCaption, RGB(0, 0, 0), TS_BOLD);
	GetScreen()->PutText(10, 6, 17, "Tahoma", m_strCaption, RGB(238, 156, 0), TS_BOLD);

	m_cMessage.SetWindowText(m_strMessage);

	m_cCheck.MoveWindow(25, GetHeight() - 70, 15, 15);
	GetScreen()->PutText(42, GetHeight() - 70, 15, "Tahoma", "No mostrar este mensaje.", GetTextColor());

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

	return true;
}

void CNewAppWarnVista::SetDialog(CString strCaption, CString strMessage, UINT nButtonType)
{
	m_strCaption = strCaption;
	m_strMessage = strMessage;
	m_nButtonType = nButtonType;
}

void CNewAppWarnVista::OnOK()
{
	if (m_cCheck.GetCheck() > 0)
		m_bCheck = true;
	else
		m_bCheck = false;

	CDDBDialog::OnOK();
}