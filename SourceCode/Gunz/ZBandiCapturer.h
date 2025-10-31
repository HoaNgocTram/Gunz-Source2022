#pragma once
// Custom: Fix bandicam support
#include "../sdk/BandiCaptureLibrary/Include/bandicap.h"
class ZBandiCapturer
{
public:
	ZBandiCapturer() : m_bInited(false), m_hHandle(0), m_pDevice(NULL) {}
	~ZBandiCapturer() {}

	bool Init(HWND nHwnd, LPDIRECT3DDEVICE9 pDevice);

	bool Start(HRESULT& hr);
	void Stop();
	void ToggleStart();
	void DrawCapture(MDrawContext* pDC);
	bool IsCapturing();
	void SetResolution(int iResolution);
	int GetResolution();
	bool CaptureImage();

private:
	HWND					m_hHandle;
	LPDIRECT3DDEVICE9		m_pDevice;
	bool					m_bInited;
	CBandiCaptureLibrary	m_bandiCaptureLibrary;
	TCHAR					m_pathName[MAX_PATH];
	int						m_iResolution;
	long					m_lFileSize;
	float					m_fToggleFileSize;

	void OnDrawCapture(MDrawContext* pDC);
	BCAP_CONFIG GetConfig();
	void GetFolder(TCHAR* szoutFolder);

	int get_file_length(char* fn);
};

void SetBandiCaptureConfig(int iResolution);
void SetBandiCaptureFileSize(int iFileSize);