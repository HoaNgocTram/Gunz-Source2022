#include "resource.h"
#include "CDDBDialog.h"

class CNewAppErrorDlg : public CDDBDialog
{
private:
	CStatic			m_cMessage;
	CListBox		m_cErrorList;
	CDDBButton		m_cOK;
	CDDBButton		m_cShowLog;
	CString			m_strCaption;
	UINT			m_nButtonType;
	CString			m_strMessage;
	CString*		m_pstrErrorMessage;

	CDialog*		m_pParentDialog;

public:
	CNewAppErrorDlg(CWnd* pParent = NULL);

	void SetDialog(CDialog* pDialog, CString strCaption, CString strMessage, CString* pstrErrorMessage, UINT nButtonType = MB_OKCANCEL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
protected:
	afx_msg void OnBnClickedShowlog();
	DECLARE_MESSAGE_MAP()
};
