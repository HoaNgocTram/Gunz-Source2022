#include <stdafx.h>
#include "NewApp.h"
#include "NewAppDlg.h"

CNewApp theApp;

CNewApp::CNewApp()
{
}

BEGIN_MESSAGE_MAP(CNewApp, CWinApp)
END_MESSAGE_MAP()

HANDLE g_hMutex = NULL;
BOOL CNewApp::InitInstance()
{
	BOOL bFound = FALSE;
	g_hMutex = ::CreateMutex(NULL, TRUE, "ZGunzLauncherMutex");

	if (::GetLastError() == ERROR_ALREADY_EXISTS)
		bFound = TRUE;

	if (g_hMutex)
		::ReleaseMutex(g_hMutex);

	if (bFound)
		return FALSE;

	AfxEnableControlContainer();
	AfxInitRichEdit();

	CNewAppDlg dlg;
	dlg.DoModal();

	return false;
}