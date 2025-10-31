#include "resource.h"
#include "CDDBDialog.h"

class CNewAppAboutBox : public CDDBDialog
{
private:
	CStatic			m_cMessage;
	CDDBButton		m_cOK;
	CDDBButton		m_cCancel;
	CString			m_strCaption;
	CString			m_strMessage;
	UINT			m_nButtonType;

	CDialog*		m_pParentDialog;

public:
	CNewAppAboutBox(CWnd* pParent = NULL);

	void SetDialog(CDialog* pDialog, CString strCaption, CString strMessage, UINT nButtonType = MB_OKCANCEL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
protected:
	DECLARE_MESSAGE_MAP()
};
