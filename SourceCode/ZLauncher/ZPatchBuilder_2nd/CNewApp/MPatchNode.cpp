#include "StdAfx.h"
#include "MPatchNode.h"
#include <io.h>

//// MPatchNode /////////////////////////////////////
MPatchNode::MPatchNode(const char* pszName, unsigned long nSize, FILETIME tmWrite, string sChecksum)
{
	strcpy(m_szName, pszName);
	m_nSize = nSize;
	m_tmWrite = tmWrite;
	m_sChecksum = sChecksum;
	m_bValidate = false;
}

MPatchNode::~MPatchNode()
{
}

void MPatchNode::MakeChecksum()
{
}

bool MPatchNode::CheckValid()
{
	m_bValidate = false;

	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	if ((hFind = FindFirstFile(GetName(), &FindData)) == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		if ((GetSize() == FindData.nFileSizeLow) &&
			(CompareFileTime(&m_tmWrite, &FindData.ftLastWriteTime) == 0))
			m_bValidate = true;

		FindClose(hFind);
	}

	return m_bValidate;
}