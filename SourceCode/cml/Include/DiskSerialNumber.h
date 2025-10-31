// DiskSerialNumber.h: interface for the CDiskSerialNumber class. 
// 
////////////////////////////////////////////////////////////////////// 
 
#ifndef DISKSERIALNUMBER_H
#define DISKSERIALNUMBER_H 
 
#if _MSC_VER > 1000 
#pragma once 
#endif // _MSC_VER > 1000 

#include <Windows.h>
 
class CDiskSerialNumber   
{ 
public: 
	CDiskSerialNumber(); 
	virtual ~CDiskSerialNumber(); 
 
	BOOL GetFirstDiskSerial(LPSTR szDiskSerial); 
	BOOL GetNextDiskSerial(LPSTR szDiskSerial); 
 
private: 
	BOOL ReadDrivePortsInWin9X(); 
	BOOL ReadPhysicalDriveInNTWithAdminRights(); 
	BOOL ReadIdeDriveAsScsiDriveInNT(); 
	BOOL ReadPhysicalDriveInNTWithZeroRights(); 
 
	void ConvertToString(LPSTR szDiskSerial, DWORD dwDiskData[], int nSize); 
	void flipAndCodeBytes(LPSTR result, LPCSTR str); 
	void fileBlankSpace(LPSTR result, LPCSTR str); 
 
	int m_nSerialCount; 
	int m_nReadCount; 
	char m_szSerials[32][64]; 
}; 
 
#endif // DISKSERIALNUMBER_H
