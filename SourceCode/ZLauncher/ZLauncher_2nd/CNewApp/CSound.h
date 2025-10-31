#ifndef _INCLUDED_CSOUND_
#define _INCLUDED_CSOUND_

class CSound
{
public:
	void PlayWavSound(UINT nIDResource);
	inline void PlayWavSound(LPCTSTR lpszSound);

protected:
};

#endif