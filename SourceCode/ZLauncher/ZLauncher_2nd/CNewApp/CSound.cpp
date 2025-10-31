#include <stdafx.h>
#include "CSound.h"
#include <MMSystem.h>

void CSound::PlayWavSound(UINT nIDResource)
{
	PlayWavSound(MAKEINTRESOURCE(nIDResource));
}

inline void CSound::PlayWavSound(LPCTSTR lpszSound)
{
	HRSRC hRes;
	HGLOBAL hData;
	BOOL bOk = false;
	if ((hRes = ::FindResource(AfxGetResourceHandle(), lpszSound, _T("WAVE"))) != NULL &&
		(hData = ::LoadResource(AfxGetResourceHandle(), hRes)) != NULL)
	{
		bOk = sndPlaySound((LPCTSTR)::LockResource(hData), SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
		FreeResource(hData);
	}
}