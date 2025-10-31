#include "stdafx.h"
#include "MMatchUtil.h"
#include "MZFileSystem.h"


unsigned long int MGetTimeDistance(unsigned long int nTimeA, unsigned long int nTimeB)
{
	if(nTimeB >= nTimeA)
	{
		return nTimeB - nTimeA;
	}
	else
	{
		return nTimeB + (UINT_MAX - nTimeA + 1);
	}

	return 0;
}

unsigned long MGetMemoryChecksum(char *pBuffer, int nLen)
{
	unsigned long nChecksum = 0;
	for (int i=0; i<nLen; i++)
		nChecksum += pBuffer[i];

	return nChecksum;
}

unsigned long MGetMZFileChecksum(const char* pszFileName) 
{
	MZFile mzf;
	if(!mzf.Open(pszFileName)) 
		return 0;

	char* pBuffer = NULL;
	int nLen = mzf.GetLength();
	pBuffer = new char[mzf.GetLength()+1];
	pBuffer[nLen] = 0;
	mzf.Read(pBuffer, nLen);
	mzf.Close();

	unsigned long nChecksum = MGetMemoryChecksum(pBuffer,nLen);
	delete[] pBuffer;
	return nChecksum;
}

void MMakeSeedKey(MPacketCrypterKey* pKey, const MUID& uidServer, const MUID& uidClient, unsigned int nTimeStamp)
{
	unsigned char* p = (unsigned char*)pKey->szKey;
	// key
	memset(p, 0, sizeof(MPacketCrypterKey));
	int nUIDSize = sizeof(MUID);

	memcpy(p, &nTimeStamp, sizeof(unsigned int));
	memcpy(p+sizeof(unsigned int), &uidServer.Low, sizeof(unsigned int));
	memcpy(p+nUIDSize, &uidClient, nUIDSize);

	// Custom: Updated XOR data
	const BYTE XOR[16] = {11, 54, 5, 69, 93, 5, 30, 25, 1, 32, 63, 84, 21, 11, 133, 7};
	//const BYTE XOR[16] = {87, 2, 91, 4, 52, 6, 1, 8, 55, 10, 18, 105, 65, 56, 15, 120};

	for (int i = 0; i < 16; i++)
	{
		p[i] ^= XOR[i];
	}

	p = p + 16;

	// Custom: Updated IV
	for( int i = 0; i < 16; ++i )
	{
		if( p[i] != 14 )
			p[i] = (36 + ((XOR[i]>>2) % 0xFF)) % 0xFF;
		else
			p[i] = (5 * (MCOMMAND_VERSION << 1)) % 0xFF;
	}
	
}

