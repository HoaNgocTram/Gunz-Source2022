#include "resource.h"
#include "CDDBDialog.h"
#include "explorer.h"

class CNewAppLicenseConfirmDlg : public CDDBDialog
{
private:
	CExplorer		m_cLicense;
	CStatic			m_cMessage;
	CDDBButton		m_cAgree;
	CDDBButton		m_cDeny;
	CDialog*		m_pParentDialog;

	CString			m_strCaption;
	CString			m_strFileName;
	CString			m_strMessage;
	CString			m_strAgree;
	CString			m_strDeny;

public:
	CNewAppLicenseConfirmDlg(CWnd* pParent = NULL);

	void SetDialog(CDialog* pDialog, CString strCaption, CString strFileName, CString strMessage, CString strAgree, CString strDeny);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
