#include "resource.h"
#include "CDDBDialog.h"

class CNewAppWarnVista : public CDDBDialog
{
private:
	CStatic			m_cMessage;
	CButton			m_cCheck;
	CDDBButton		m_cOK;
	CDDBButton		m_cCancel;
	CString			m_strCaption;
	CString			m_strMessage;
	UINT			m_nButtonType;

	bool			m_bCheck;

public:
	CNewAppWarnVista(CWnd* pParent = NULL);

	void SetDialog(CString strCaption, CString strMessage, UINT nButtonType = MB_OKCANCEL);

	bool GetChecked() { return m_bCheck; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
protected:
	afx_msg void OnOK();
	DECLARE_MESSAGE_MAP()
};
