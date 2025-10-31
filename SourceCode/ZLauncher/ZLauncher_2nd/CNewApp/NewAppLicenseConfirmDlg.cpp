#include "stdafx.h"
#include "NewAppLicenseConfirmDlg.h"
#include <Mmsystem.h>

CNewAppLicenseConfirmDlg::CNewAppLicenseConfirmDlg(CWnd* pParent) : CDDBDialog(IDD_LICENSE_CONFIRM, pParent)
{
	CDDBDLGINFO DlgInfo;
	DlgInfo.nWidth = 640;
	DlgInfo.nHeight = 400;
	DlgInfo.IDSkinResource = IDR_BMP_POPUPDLG;
	DlgInfo.bEnableMoveDlg = FALSE;
	DlgInfo.nTextColor = RGB(166, 156, 153);
	DlgInfo.nTextBkColor = RGB(36, 33, 32);
#ifdef LOCALE_NHNUSA
	DlgInfo.nTextColor = RGB(46, 47, 48);
	DlgInfo.nTextBkColor = RGB(150, 150, 150);
#endif
	SetDialogInfo(DlgInfo);

	m_pParentDialog = NULL;
}

void CNewAppLicenseConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDDBDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LICENSE, m_cLicense);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDOK, m_cAgree);
	DDX_Control(pDX, IDCANCEL, m_cDeny);
}

BEGIN_MESSAGE_MAP(CNewAppLicenseConfirmDlg, CDDBDialog)
END_MESSAGE_MAP()

BOOL CNewAppLicenseConfirmDlg::OnInitDialog()
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
	else
	{
		int sx = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
		int sy = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON);

		MoveWindow((sx - GetWidth()) / 2, (sy - GetHeight()) / 2, GetWidth(), GetHeight());
	}

	m_cAgree.InitDDBButton(IDR_BMP_BUTTON, IDR_WAV_PUSHBUTTON);
	m_cDeny.InitDDBButton(IDR_BMP_BUTTON, IDR_WAV_PUSHBUTTON);

	SetWindowText(m_strCaption);
	GetScreen()->PutText(9, 5, 17, "Tahoma", m_strCaption, RGB(0, 0, 0), TS_BOLD);
	GetScreen()->PutText(10, 6, 17, "Tahoma", m_strCaption, RGB(238, 156, 0), TS_BOLD);

	GetScreen()->PutText(0, 0, GetWidth(), GetHeight(), 20, "Tahoma", "No se encontró la licencia.", GetTextColor(), 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	char szCurrDir[512];
	GetCurrentDirectory(sizeof(szCurrDir), szCurrDir);

	char szAddress[_MAX_DIR];
	sprintf(szAddress, "file://%s\\%s", szCurrDir, m_strFileName.GetBuffer());

	char szFullPath[_MAX_DIR];
	sprintf(szFullPath, "%s\\%s", szCurrDir, m_strFileName.GetBuffer());

	WIN32_FIND_DATA findfile;
	HANDLE hFind = FindFirstFile(szFullPath, &findfile);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		m_cLicense.Navigate(szAddress, NULL, NULL, NULL, NULL);
		m_cLicense.MoveWindow(13, 31, GetWidth() - 26, GetHeight() - 120);
	}
	else
	{
		m_cLicense.MoveWindow(13, 31, 0, 0);
	}
	FindClose(hFind);

	m_cMessage.MoveWindow(13, GetHeight() - 80, GetWidth() - 26, 40);
	m_cMessage.SetWindowText(m_strMessage);

	m_cAgree.SetWindowText(m_strAgree);
	m_cAgree.ShowWindow(SW_SHOW);
	m_cAgree.EnableWindow(TRUE);
	m_cAgree.SetBlanking(TRUE, 500);
	m_cAgree.MoveWindow(GetWidth() / 2 - m_cAgree.GetWidth() - 5, GetHeight() - 35);

	m_cDeny.SetWindowText(m_strDeny);
	m_cDeny.ShowWindow(SW_SHOW);
	m_cDeny.EnableWindow(TRUE);
	m_cDeny.MoveWindow(GetWidth() / 2 + 5, GetHeight() - 35);

	return true;
}

void CNewAppLicenseConfirmDlg::SetDialog(CDialog* pDialog, CString strCaption, CString strFileName, CString strMessage, CString strAgree, CString strDeny)
{
	m_pParentDialog = pDialog;
	m_strCaption = strCaption;
	m_strFileName = strFileName;
	m_strMessage = strMessage;
	m_strAgree = strAgree;
	m_strDeny = strDeny;
}