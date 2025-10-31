#ifndef _INCLUDED_CNEWAPP_
#define _INCLUDED_CNEWAPP_

class CNewApp : public CWinApp
{
public:
	CNewApp();

public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

#endif
