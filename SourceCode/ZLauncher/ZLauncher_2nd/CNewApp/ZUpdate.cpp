#include "stdafx.h"
#include "ZGlobal.h"
#include "ZUpdate.h"
#include "MXml.h"
#include "FileInfo.h"
#include "MZip.h"
#include "MMD5.h"
#include <shlwapi.h>
#include <ctime>
#include <sstream>
#include <string>
#include <TlHelp32.h>

CString GetFilteredFileName(const char* szFileName)
{
	CString str = szFileName;

	char szDir[_MAX_DIR];
	GetCurrentDirectory(sizeof(szDir), szDir);

	if (strncmp(szFileName, szDir, strlen(szDir)) == 0)
	{
		str = ".";
		str += szFileName + (int)strlen(szDir);
	}

	return str;
}

std::string GetCRC(const char* szFileName, CString* pstrErrorMsg)
{
	char *pFileName = _strlwr(_strdup(szFileName));
	bool bIsMRSFile = (strstr(pFileName, ".mrs") != NULL) ? true : false;
	delete pFileName;

	DWORD dwCRC = 0;
	char szMsg[1024];

	MMD5 m;
	unsigned char md5[16];
	char szMd5[64];
	m.md5_file((char*)szFileName, md5);

	for (int i = 0, j = 0; i < 16; i++, j += 2)
		sprintf(szMd5 + j, "%02x", md5[i]);

	return szMd5;
}

void GetTempFilePathName(const char* szFileName, char* szTmpFileName)
{
	char szDir[_MAX_DIR];
	GetCurrentDirectory(sizeof(szDir), szDir);

	char drive[_MAX_PATH], dir[_MAX_PATH], fname[_MAX_PATH], ext[_MAX_PATH];
	_splitpath(szFileName, drive, dir, fname, ext);

	sprintf(szTmpFileName, "%s\\%s\\%s%s_", szDir, STR_TEMP_DOWNLOADFOLDER, fname, ext);
}

bool IsRebootingFile(const char* szFileName)
{
	bool bIsReboot = false;

	char *pFileName = _strlwr(_strdup(szFileName));

	if ((strstr(szFileName, "gunzlauncher") || strstr(szFileName, "GunzLauncher")) != NULL)
		bIsReboot = true;

	delete pFileName;
	return bIsReboot;
}

ZUpdatePatchNode::ZUpdatePatchNode(const char* pszName, unsigned long nSize, string sChecksum)
{
	strcpy(m_szFileName, pszName);
	m_nSize = nSize;
	m_sCheckSum = sChecksum;
	m_bValidate = false;
	m_bPatchSuccess = true;
}

ZUpdatePatchNode::~ZUpdatePatchNode()
{
}

bool ZUpdatePatchNode::CheckValid(CString* pstrErrorMsg)
{
	m_bValidate = false;

	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	if ((hFind = FindFirstFile(GetFileName() + 2, &FindData)) == INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);

		char szMsg[512];
		char buffer[1024];
		GetCurrentDirectory(1024, buffer);
		sprintf(szMsg, "[ZUpdatePatchNode] Needs to update : %s/%s (not exist)", buffer, GetFileName() + 2);
		PutLog(szMsg);

		return false;
	}

	FindClose(hFind);

	string sMd5 = GetCRC(GetFileName(), pstrErrorMsg);
	if (stricmp(sMd5.c_str(), GetChecksum().c_str()))
	{
		char szMsg[512];
#ifdef _DEBUG
		sprintf(szMsg, "[ZUpdatePatchNode] Needs to update : %s, remote(%u), local(%u)", GetFileName(), GetChecksum(), sMd5.c_str());
#else
		sprintf(szMsg, "[ZUpdatePatchNode] Needs to update : %s", GetFileName());
#endif
		PutLog(szMsg);
	}
	else
		m_bValidate = true;

	return true;
}

ZUpdate::ZUpdate()
{
	m_bInitialize = false;
	m_szAddress[0] = 0;
	m_nPort = 0;
	m_szID[0] = 0;
	m_szPassword[0] = 0;
	m_pUpdatePatchList.clear();
	m_bStopUpdate = false;
	m_bPatchComplete = false;
	m_bReboot = false;
}

ZUpdate::~ZUpdate()
{
	if (m_bInitialize)
		Destroy();
}

bool ZUpdate::Create(const char* pszAddress, unsigned long nPort, const char* pszDefDirectory, const char* pszID, const char* pszPassword)
{
	m_strErrorMsg = "";

	PutLog("[ZUpdate] Create.");

	strcpy(m_szAddress, pszAddress);
	m_nPort = nPort;
	strcpy( m_szID, pszID);
	strcpy( m_szPassword, pszPassword);

	HANDLE m_hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	int cant = 0;

	if( m_hSnapshot == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	PROCESSENTRY32 pe32;
	while ( Process32Next( m_hSnapshot, &pe32 ) ) 
	{
		if(strstr(pe32.szExeFile, "Gunz.exe") || strstr(pe32.szExeFile, "Gunzd.exe"))
		{
			HANDLE hProc = OpenProcess(PROCESS_TERMINATE, false, pe32.th32ProcessID);

			TerminateProcess(hProc, 0);
		}
	}
	CloseHandle( m_hSnapshot );

	// Create File Transfer
	if ( !m_FileTransfer.Create( pszAddress, nPort, pszID, pszPassword))
	{
		char szMsg[ 1024];
		sprintf( szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n",	/*The update server is not responding or is not running right now.*/ ZGetString( "STR_128"),
																/*Please check your firewall.*/ ZGetString( "STR_129"),
																/*Please try again after a while.*/ ZGetString( "STR_130"));
		m_strErrorMsg += szMsg;

		PutLog("[ZUpdate] ERROR : Cannot create file transfer.");
		PutLog("[ZUpdate] ERROR : Create FAILED!!!");

		return false;
	}

	if (!m_FileTransfer.OpenConnection())
	{
		char szMsg[1024];
		sprintf(szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n", ZGetString("STR_131"),
			ZGetString("STR_129"),
			ZGetString("STR_132"));
		m_strErrorMsg += szMsg;

		PutLog("[ZUpdate] ERROR : Cannot open file transfer.");
		PutLog("[ZUpdate] ERROR : Create FAILED!!!");

		return false;
	}

	if (!m_FileTransfer.ChangeDirectory(pszDefDirectory))
	{
		char szMsg[1024];
		sprintf(szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n", ZGetString("STR_128"),
			ZGetString("STR_129"),
			ZGetString("STR_130"));
		m_strErrorMsg += szMsg;

		PutLog("[ZUpdate] ERROR : Cannot change default directory.");
		PutLog("[ZUpdate] ERROR : Create FAILED!!!");

		return false;
	}

	m_UpdateInfo.SetTransferUI(m_FileTransfer.GetFileTransferUI());
	m_bInitialize = true;
	PutLog("[ZUpdate] Create successfully compete.");

	return true;
}

bool ZUpdate::Destroy()
{
	PutLog("[ZUpdate] Destroy.");

	if (!m_FileTransfer.Destroy())
	{
		PutLog("[ZUpdate] WARNING : Destroy FAILED!!!");
	}

	m_szAddress[0] = 0;
	m_nPort = 0;
	m_szID[0] = 0;
	m_szPassword[0] = 0;
	m_bInitialize = false;

	for (ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin(); itr != m_pUpdatePatchList.end(); itr++)
		delete (*itr);

	m_pUpdatePatchList.clear();

	PutLog("[ZUpdate] Destroy successfully complete.");

	return true;
}

#include <MMSystem.h>
bool ZUpdate::CheckUpdate(const char* pszPatchFileName)
{
	m_strErrorMsg = "";

	if (!m_bInitialize)
	{
		PutLog("[ZUpdate] ERROR : Do not Created.");
		PutLog("[ZUpdate] ERROR : Start update FAILED!!!");

		return false;
	}
	PutLog("[ZUpdate] Start update.");

	m_bStopUpdate = false;
	m_bPatchComplete = false;
	m_bReboot = false;
	m_UpdateInfo.ClearTotalPatchFileSize();
	m_UpdateInfo.ClearCurrPatchedFileSize();
	m_UpdateInfo.ClearTotalPatchFileCount();
	m_UpdateInfo.ClearCurrPatchedFileCount();
	m_UpdateInfo.ClearPatchFailedCount();

	if (!m_FileTransfer.DownloadFile(pszPatchFileName, pszPatchFileName, 0))
	{
		char szMsg[1024];
		sprintf(szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n", ZGetString("STR_128"),
			ZGetString("STR_129"),
			ZGetString("STR_130"));
		m_strErrorMsg += szMsg;

		PutLog("[ZUpdate] ERROR : Start update FAILED!!!");
		return false;
	}

	if (!GetUpdateInfo(pszPatchFileName))
	{
		PutLog("[ZUpdate] ERROR : Start update FAILED!!!");
		return false;
	}

	DeleteFile(pszPatchFileName);

	// Update files
	if (!CheckValidFromPatchList())
	{
		PutLog("[ZUpdate] ERROR : Start update FAILED!!!");
		return false;
	}

	return true;
}

bool ZUpdate::StartUpdate()
{
	m_strErrorMsg = "";

	if (!PatchFiles())
	{
		PutLog("[ZUpdate] ERROR : Start update FAILED!!!");
		return false;
	}

	if (!MovePatchFiles())
	{
		PutLog("[ZUpdate] ERROR : Cannot move patch files.");
		return false;
	}

	PutLog("[ZUpdate] Start update successfully complete.");

	m_bPatchComplete = true;

	return true;
}

bool ZUpdate::StopUpdate()
{
	PutLog("[ZUpdate] Stop update.");

	m_FileTransfer.StopDownload();
	m_bStopUpdate = true;

	PutLog("[ZUpdate] Stop update successfully complete.");

	char szMsg[1024];
	sprintf(szMsg, "%s\n     [Tip] %s\n", ZGetString("STR_133"),
		ZGetString("STR_134"));
	m_strErrorMsg += szMsg;

	return true;
}

#define MPTOK_PATCHINFO		"PATCHINFO"
#define MPTOK_VERSION		"VERSION"
#define MPTOK_PATCHNODE		"PATCHNODE"
#define MPTOK_SIZE			"SIZE"
#define MPTOK_WRITETIMEHIGH	"WRITETIMEHIGH"
#define MPTOK_WRITETIMELOW	"WRITETIMELOW"
#define MPTOK_CHECKSUM		"CHECKSUM"
#define MPTOK_ATTR_FILE		"file"

bool ZUpdate::GetUpdateInfo(const char* pszPatchFileName)
{
	PutLog("[ZUpdate] Get update info.");

	for (ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin(); itr != m_pUpdatePatchList.end(); itr++)
		delete (*itr);

	m_pUpdatePatchList.clear();

	MXmlDocument	xmlConfig;
	MXmlElement		rootElement, aPatchInfo;

	xmlConfig.Create();
	if (!xmlConfig.LoadFromFile(pszPatchFileName))
	{
		xmlConfig.Destroy();
		PutLog("[ZUpdate] ERROR : Get update info FAILED!!!");

		return false;
	}

	rootElement = xmlConfig.GetDocumentElement();
	if (rootElement.IsEmpty())
	{
		xmlConfig.Destroy();
		PutLog("[ZUpdate] ERROR : Get update info FAILED!!!");

		return false;
	}

	if (rootElement.FindChildNode(MPTOK_PATCHINFO, &aPatchInfo) == false)
	{
		xmlConfig.Destroy();
		PutLog("[ZUpdate] ERROR : Get update info FAILED!!!");

		return false;
	}

	int iCount = aPatchInfo.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		MXmlElement aPatchNode = aPatchInfo.GetChildNode(i);

		char szBuf[256];
		aPatchNode.GetTagName(szBuf);
		if (szBuf[0] == '#') continue;

		if (!stricmp(szBuf, MPTOK_PATCHNODE))
		{
			char szFileName[_MAX_DIR] = "";
			int nSize = 0;
			FILETIME tmWrite;
			char sChecksum[_MAX_DIR] = "";

			if (aPatchNode.GetAttribute(szFileName, MPTOK_ATTR_FILE) == false)
				continue;

			if (aPatchNode.GetChildContents(&nSize, MPTOK_SIZE) == false)
				continue;

			if (aPatchNode.GetChildContents(sChecksum, MPTOK_CHECKSUM) == false)
				continue;

#ifdef _DEBUG
			if (strstr(szFileName, "GunzLauncher.exe") != NULL)
				continue;
#endif

			m_pUpdatePatchList.push_back(new ZUpdatePatchNode(szFileName, nSize, sChecksum));
		}
		else if (!stricmp(szBuf, MPTOK_VERSION))
		{
			char szVersion[128] = "";
			aPatchNode.GetContents(szVersion);
		}
	}
	xmlConfig.Destroy();

	PutLog("[ZUpdate] Get update info successfully complete.");

	return true;
}

bool ZUpdate::CheckValidFromPatchList()
{
	PutLog("[ZUpdate] Check valid from patch list.");

	for (ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin(); itr != m_pUpdatePatchList.end(); itr++)
	{
		(*itr)->CheckValid(&m_strErrorMsg);

		if (m_bStopUpdate)
		{
			PutLog("[ZUpdate] ERROR : Check valid STOPED!!!");
			PutLog("[ZUpdate] ERROR : Check valid FAILED!!!");

			return false;
		}
	}

	for (ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin(); itr != m_pUpdatePatchList.end(); itr++)
	{
		if (!(*itr)->IsValid())
		{
			m_UpdateInfo.AddTotalPatchFileSize((*itr)->GetSize());
			m_UpdateInfo.AddTotalPatchFileCount(1);
		}
	}

	PutLog("[ZUpdate] Check valid from patch list successfully complete.");
	CString str;
	str.Format("[ZUpdate] + Total patch file count : %u", m_UpdateInfo.GetTotalPatchFileCount());
	PutLog(str);
	str.Format("[ZUpdate] + Total patch file size : %u bytes", m_UpdateInfo.GetTotalPatchFileSize());
	PutLog(str);

	return true;
}

bool ZUpdate::PatchFiles()
{
	PutLog("[ZUpdate] Patch files.");

	int numErrores = 0;

	char szMsg[1024];
	for (ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin(); itr != m_pUpdatePatchList.end(); itr++)
	{
		if (!(*itr)->IsValid())
		{
			TCHAR szFullPath[_MAX_DIR];
			GetTempFilePathName((*itr)->GetFileName(), szFullPath);

			bool bExistFile = false;
			WIN32_FIND_DATA findfile;
			HANDLE hFind = FindFirstFile(_T(szFullPath), &findfile);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				if ((*itr)->GetChecksum() == GetCRC(szFullPath, &m_strErrorMsg))
				{
					sprintf(szMsg, "[ZUpdate] Already exist patch file : %s", GetFilteredFileName(szFullPath));
					PutLog(szMsg);

					bExistFile = true;
				}

				FindClose(hFind);
			}

			if (!bExistFile)
			{
#define DOWNLOAD_RETRY_COUNT	2
				for (int nRetry = 1; nRetry <= DOWNLOAD_RETRY_COUNT; nRetry++)
				{
					if (m_bStopUpdate)
						break;

					if (m_FileTransfer.DownloadFile((*itr)->GetFileName(), szFullPath, (*itr)->GetSize()))
					{
						sprintf(szMsg, "[ZUpdate] Successfully download '%s'", GetFilteredFileName(szFullPath));
						PutLog(szMsg);

						break;
					}
					else if (nRetry == DOWNLOAD_RETRY_COUNT)
					{
						m_UpdateInfo.AddPatchFailedCount(1);
						(*itr)->SetPatchSuccess(false);

						sprintf(szMsg, "[ZUpdate] ERROR : Fail to download file '%s'", GetFilteredFileName(szFullPath));
						PutLog(szMsg);

						sprintf(szMsg, "%s : %s\n     [Tip] %s\n", ZGetString("STR_135"), GetFilteredFileName(szFullPath),
							ZGetString("STR_136"));
						m_strErrorMsg += szMsg;
						numErrores = numErrores + 1;
					}
					else
					{
						sprintf(szMsg, "[ZUpdate] WARNNING : Retry patch file : %s , %d retry", GetFilteredFileName(szFullPath), nRetry);
						PutLog(szMsg);
					}
				}
			}

			m_UpdateInfo.AddCurrPatchedFileSize((*itr)->GetSize());
			m_UpdateInfo.AddCurrPatchedFileCount(1);
		}

		if (m_bStopUpdate)
		{
			PutLog("[ZUpdate] Patch files STOP!!!");

			return false;
		}
	}

	PutLog("[ZUpdate] Patch files successfully complete.");

	if (numErrores > 0) {
		return false;
	}

	return true;
}

bool ZUpdate::MovePatchFiles()
{
	PutLog("[ZUpdate] Move patch files.");

	char szMsg[1024];
	bool bFail = false;
	for (ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin(); itr != m_pUpdatePatchList.end(); itr++)
	{
		if (!(*itr)->IsValid() && (*itr)->IsPatchSuccess())
		{
			TCHAR szFullPath[_MAX_DIR];
			PathSearchAndQualify((*itr)->GetFileName(), szFullPath, _MAX_DIR);

			TCHAR szTmpFullPath[_MAX_DIR];
			GetTempFilePathName((*itr)->GetFileName(), szTmpFullPath);

			string sMd5 = GetCRC(szTmpFullPath, &m_strErrorMsg);

			if (_stricmp((*itr)->GetChecksum().c_str(), sMd5.c_str()))
			{
#ifdef _DEBUG
				sprintf(szMsg, "[ZUpdate] ERROR : Invalid MD5 : '%s' (org:%u / curr:%u)", GetFilteredFileName(szTmpFullPath), (*itr)->GetChecksum(), sMd5.c_str());
#else
				sprintf(szMsg, "[ZUpdate] ERROR : Invalid MD5 '%s'", GetFilteredFileName(szTmpFullPath));
#endif
				PutLog(szMsg);

				sprintf(szMsg, "%s : %s\n     [Tip] %s\n", ZGetString("STR_137"), GetFilteredFileName(szTmpFullPath),
					ZGetString("STR_138"));
				m_strErrorMsg += szMsg;

				bFail = true;

				continue;
			}

			if (IsRebootingFile(GetFilteredFileName(szTmpFullPath)))
			{
				sprintf(szMsg, "[ZUpdate] This file is must restart : '%s'", GetFilteredFileName(szTmpFullPath));
				PutLog(szMsg);

				m_bReboot = true;

				continue;
			}

			sprintf(szMsg, "[ZUpdate] Copy patch file from '%s'  to  '%s'", GetFilteredFileName(szTmpFullPath), GetFilteredFileName(szFullPath));
			PutLog(szMsg);

			if (!MakePath(szFullPath))
			{
				sprintf(szMsg, "[ZUpdate] WARNNING : This file cannot made path : %s", GetFilteredFileName(szFullPath));
				PutLog(szMsg);

				sprintf(szMsg, "%s : %s\n     [Tip] %s\n", ZGetString("STR_139"), GetFilteredFileName(szFullPath),
					ZGetString("STR_140"));
				m_strErrorMsg += szMsg;
			}

			BOOL bRetVal = CopyFile(szTmpFullPath, szFullPath, FALSE);

			if (bRetVal == FALSE)
			{
				LPVOID lpMsgBuf;
				DWORD error = GetLastError();
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&lpMsgBuf, 0, NULL);

				sprintf(szMsg, "[ZUpdate] ERROR : File copy error : %s : %s", GetFilteredFileName(szFullPath), lpMsgBuf);
				PutLog(szMsg);

				sprintf(szMsg, "%s : %s : %s\n     [Tip] %s\n", ZGetString("STR_141"), GetFilteredFileName(szFullPath), lpMsgBuf,
					ZGetString("STR_125"));
				m_strErrorMsg += szMsg;

				LocalFree(lpMsgBuf);

				bFail = true;

				continue;
			}

			sMd5 = GetCRC(szFullPath, &m_strErrorMsg);

			if (_stricmp((*itr)->GetChecksum().c_str(), sMd5.c_str()))
			{
#ifdef _DEBUG
				sprintf(szMsg, "[ZUpdate] ERROR : MD5 error '%s' (current : %u  /  original : %u)", GetFilteredFileName(szFullPath), sMd5.c_str(), (*itr)->GetChecksum());
#else
				sprintf(szMsg, "[ZUpdate] ERROR : MD5 error '%s'", GetFilteredFileName(szFullPath));
#endif
				PutLog(szMsg);

				sprintf(szMsg, "%s : %s\n     [Tip] %s\n", ZGetString("STR_142"), GetFilteredFileName(szFullPath),
					ZGetString("STR_125"));
				m_strErrorMsg += szMsg;

				bFail = true;
			}

			else
				DeleteFile(szTmpFullPath);
		}
	}

	PutLog("[ZUpdate] Move patch files successfully complete.");

	if (bFail)
		return false;

	return true;
}
string convertToString(double num) {
	ostringstream convert;
	convert << num;
	return convert.str();
}
double roundOff(double n) {
	double d = n * 100.0;
	int i = d + 0.5;
	d = (float)i / 100.0;
	return d;
}

string convertSize(size_t size) {
	static const char *SIZES[] = { "B", "KB", "MB", "GB" };
	int div = 0;
	size_t rem = 0;

	while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
		rem = (size % 1024);
		div++;
		size /= 1024;
	}

	double size_d = (float)size + (float)rem / 1024.0;
	string result = convertToString(roundOff(size_d)) + " " + SIZES[div];
	return result;
}