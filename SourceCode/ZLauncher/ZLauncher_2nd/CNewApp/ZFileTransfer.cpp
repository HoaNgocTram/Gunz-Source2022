#include "stdafx.h"
#include "ZGlobal.h"
#include "ZFileTransfer.h"
#include "FileInfo.h"
#include "Log.h"

MFTSession::MFTSession(LPCTSTR pszAppName, int nMethod)
	: CInternetSession(pszAppName, 1, nMethod)
{
	EnableStatusCallback(TRUE);
}

void MFTSession::OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInfomration, DWORD dwStatusInformationLen)
{
}

ZFileTransfer::ZFileTransfer() : m_Session("ZUpdate")
{
	m_bInitialize = false;
	m_szAddress[0] = 0;
	m_szDefDir[0] = 0;
	m_nPort = 0;
	m_szID[0] = 0;
	m_szPassword[0] = 0;
	m_szLastError[0] = 0;

	m_pConnection = NULL;
	m_bOpenConnection = false;
	m_bStopDownload = false;
}

ZFileTransfer::~ZFileTransfer()
{
	if (m_bOpenConnection)
		CloseConnection();
}

bool ZFileTransfer::Create(const char* pszAddress, unsigned long nPort, const char* pszID, const char* pszPassword)
{
	PutLog("[ZFileTransfer] Create.");

	strcpy(m_szAddress, pszAddress);
	m_nPort = nPort;
	strcpy(m_szID, pszID);
	strcpy(m_szPassword, pszPassword);

	m_bInitialize = true;
	PutLog("[ZFileTransfer] Create successfully complete.");

	return true;
}

bool ZFileTransfer::Destroy()
{
	PutLog("[ZFileTransfer] Destroy.");

	if (m_bOpenConnection)
	{
		if (!CloseConnection())
			PutLog("[ZFileTransfer] WARNING : Destroy FAILED!!!");
	}

	m_szAddress[0] = 0;
	m_nPort = 0;
	m_szID[0] = 0;
	m_szPassword[0] = 0;

	m_bInitialize = false;
	PutLog("[ZFileTransfer] Destroy successfly complete.");

	return true;
}

bool ZFileTransfer::OpenConnection()
{
	PutLog("[ZFileTransfer] Open connection.");

	if (m_bOpenConnection)
	{
		PutLog("[ZFileTransfer] ERROR : Already open connection.");
		PutLog("[ZFileTransfer] ERROR : Open connection FAILED!!!");
		return false;
	}

	CHttpConnection* pConnection = NULL;
	try
	{
		pConnection = m_Session.GetHttpConnection(m_szAddress, (INTERNET_PORT)m_nPort);
		if (pConnection != NULL)
		{
			m_pConnection = pConnection;
		}
	}
	catch (CInternetException* pEx)
	{
		pEx->GetErrorMessage(m_szLastError, 1024);
		pEx->Delete();

		if (pConnection)
		{
			pConnection->Close();
			delete pConnection;
		}

		CString str;
		str.Format("[ZFileTransfer] ERROR : %s", m_szLastError);
		PutLog(str);

		return false;
	}

	m_bOpenConnection = true;
	PutLog("[ZFileTransfer] Open connection successfully complete.");

	return true;
}

bool ZFileTransfer::CloseConnection()
{
	PutLog("[ZFileTransfer] Close connection.");

	if (!m_bOpenConnection)
	{
		PutLog("[ZFileTransfer] WARNING : Connection are not opened.");
		PutLog("[ZFileTransfer] WARNING : Close connection FAILED!!!");

		return false;
	}

	GetConnection()->Close();

	delete m_pConnection;
	m_pConnection = NULL;

	m_bOpenConnection = false;
	PutLog("[ZFileTransfer] Close connection successfully complete.");

	return true;
}

string ZFileTransfer::UrlEncode(const string &value)
{
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
	{
		string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '/' || c == '\\')
		{
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char)c);
		escaped << nouppercase;
	}

	return escaped.str();
}

bool ZFileTransfer::DownloadFile(const char* pszRemoteFileName, const char* pszLocalFileName, DWORD nFileSize, int retry)
{
	CString str;
	str.Format("[ZFileTransfer] Download file : %s  to  %s", pszRemoteFileName, pszLocalFileName);
	PutLog(str);

	CHttpFile* pFile = NULL;
	CFile LocalFile;
	m_bStopDownload = false;

	if (!m_bOpenConnection)
	{
		PutLog("[ZFileTransfer] ERROR : Connection are not opened.");
		str.Format("[ZFileTransfer] ERROR : Failed download : %s", pszRemoteFileName);
		PutLog(str);

		return false;
	}

	if (!MakePath(pszLocalFileName))
	{
	}

	if (!DeleteFile(pszLocalFileName))
	{
	}

	CFileException ex;
	if (!LocalFile.Open(pszLocalFileName, CFile::modeCreate | CFile::modeWrite, &ex))
	{
		ex.GetErrorMessage(m_szLastError, 1024);

		sprintf(m_szLastError, "%s : Access denied or unknown error.\n", pszLocalFileName);
		int i = (int)strlen(m_szLastError);
		for (; i >= 0; i--)
		{
			if ((m_szLastError[i] == '/') || (m_szLastError[i] == '\\'))
				break;
		}
		memcpy(m_szLastError, m_szLastError + (i + 1), (int)strlen(m_szLastError) - i);
		m_szLastError[i + 1] = 0;

		str.Format("[ZFileTransfer] ERROR : %s", m_szLastError);
		PutLog(str);
		str.Format("[ZFileTransfer] ERROR : Download File : %s FAILED!!!", pszRemoteFileName);
		PutLog(str);

		DeleteFile(pszLocalFileName);

		return false;
	}

	// Set full path name
	char szFullPathName[ 512];
	sprintf( szFullPathName, "\\%s%s", m_szDefDir, pszRemoteFileName);

	string url(szFullPathName);

	url = UrlEncode(url);

	memset(szFullPathName, 0, 512);

	memcpy(szFullPathName, url.c_str(), url.size());

	try
	{
		CString strHeaders = "Accept: */*\r\n";
		strHeaders += "Accept-Encoding: *\r\n";

		// Open request
		pFile = GetConnection()->OpenRequest(CHttpConnection::HTTP_VERB_GET, szFullPathName, NULL, 1, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD);
		if ( !pFile)
		{
			str.Format( "[ZFileTransfer] ERROR : Cannot open remote file.");
			PutLog( str);
			str.Format( "[ZFileTransfer] ERROR : Failed download : %s", pszRemoteFileName);
			PutLog( str);

			// Delete file
			DeleteFile( pszLocalFileName);

			return false;
		}
		pFile->AddRequestHeaders(strHeaders);
		pFile->SendRequest();

		bool bError = false;
		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_DENIED)
		{
			str.Format("[ZFileTransfer] ERROR : Access denied.");
			PutLog(str);
			sprintf(m_szLastError, "(%03u) %s\n", dwRet, ZGetString("STR_143"));

			bError = true;
		}
		else if (dwRet == HTTP_STATUS_NOT_FOUND)
		{
			str.Format("[ZFileTransfer] ERROR : File not found.");
			PutLog(str);
			sprintf(m_szLastError, "(%03u) %s\n", dwRet, ZGetString("STR_144"));

			bError = true;
		}
		else if ((dwRet >= 300) && (dwRet < 400))
		{
			str.Format("[ZFileTransfer] ERROR : Information error.");
			PutLog(str);
			sprintf(m_szLastError, "(%03u) %s\n", dwRet, ZGetString("STR_145"));

			bError = true;
		}
		else if ((dwRet >= 400) && (dwRet < 500))
		{
			str.Format("[ZFileTransfer] ERROR : File request error.");
			PutLog(str);
			sprintf(m_szLastError, "(%03u) %s\n", dwRet, ZGetString("STR_146"));

			bError = true;
		}
		else if ((dwRet >= 500) && (dwRet < 600))
		{
			str.Format("[ZFileTransfer] ERROR : Server error.");
			PutLog(str);
			sprintf(m_szLastError, "(%03u) %s\n", dwRet, ZGetString("STR_147"));

			bError = true;
		}

		if (bError)
		{
			str.Format("[ZFileTransfer] ERROR : Failed download : %s", pszRemoteFileName);
			PutLog(str);

			delete pFile;

			DeleteFile(pszLocalFileName);

			return false;
		}

		m_FileTransferUI.SetDownloadFileName(pszRemoteFileName);
		m_FileTransferUI.SetTotalDownloadSize(nFileSize);
		m_FileTransferUI.ClearCurrDownloadSize();
#define MBUF_SIZE	2048
		BYTE Buffer[MBUF_SIZE];
		DWORD dwRead = 0;
		DWORD dwTotalRead = 0;
		do
		{
			dwRead = pFile->Read(Buffer, MBUF_SIZE);
			LocalFile.Write(Buffer, dwRead);

			dwTotalRead += dwRead;

			m_FileTransferUI.AddCurrDownloadSize(dwRead);
		} while ((dwRead > 0) && !m_bStopDownload);
	}
	catch (CInternetException* pEx)
	{
		pEx->GetErrorMessage(m_szLastError, 1024);
		pEx->Delete();
		str.Format("[ZFileTransfer] ERROR : %s", m_szLastError);
		PutLog(str);
		LocalFile.Close();
		pFile->Close();
		delete pFile;
		DeleteFile(pszLocalFileName);

		return false;
	}

	if (m_bStopDownload)
	{
		str.Format("[ZFileTransfer] Download File : %s STOP!!!", pszRemoteFileName);
		PutLog(str);
		m_bStopDownload = false;

		LocalFile.Close();
		pFile->Close();
		delete pFile;

		DeleteFile(pszLocalFileName);

		return false;
	}

	str.Format("[ZFileTransfer] Download File : %s successfuly complete.", pszRemoteFileName);
	PutLog(str);

	LocalFile.Close();
	pFile->Close();

	delete pFile;

	m_FileTransferUI.ClearDownloadFileName();
	m_FileTransferUI.ClearCurrDownloadSize();
	m_FileTransferUI.SetTotalDownloadSize(0);

	return true;
}

bool ZFileTransfer::ChangeDirectory(const char* pszDirectory)
{
	PutLog("[ZFileTransfer] Change directory.");

	if (!m_bOpenConnection)
	{
		PutLog("[ZFileTransfer] ERROR : Connection are not opened.");
		PutLog("[ZFileTransfer] ERROR : Change directory FAILED!!!");

		return false;
	}

	strcpy(m_szDefDir, pszDirectory);

	CString str;
#ifdef _DEBUG
	str.Format("[ZFileTransfer] Change directory successfully complete. : %s", pszDirectory);
#else
	str.Format("[ZFileTransfer] Change directory successfully complete.");
#endif
	PutLog(str);

	return true;
}