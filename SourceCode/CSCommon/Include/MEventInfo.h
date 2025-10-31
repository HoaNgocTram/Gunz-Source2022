#include <MXml.h>
#include <map>

struct MEventInfo
{
	string strEventName;
	string strSecondaryName;
	string strTertiaryName;
	string strQuarteraryName;
	string strEventObjective;
	string strEventDesc;
	unsigned int nObjColor[3];
	unsigned int nDescColor[3];
};


class MEvents
{
protected:
	std::vector<MEventInfo*> m_pEventInfo;
public:
	MEvents() ;
	virtual ~MEvents();

	bool ReadXml(const char* szFileName, MZFileSystem* pFileSystem);
	static MEvents* GetInstance();
	void LoadEventInfo(MXmlElement& element);

	MEventInfo* Find(string strName)
	{
		for (auto i = m_pEventInfo.begin(); i != m_pEventInfo.end(); ++i)
		{
			MEventInfo* node = (*i);
			if (StrStrIA(strName.c_str(), node->strEventName.c_str()) || StrStrIA(strName.c_str(), node->strSecondaryName.c_str()) ||
				StrStrIA(strName.c_str(), node->strTertiaryName.c_str())|| StrStrIA(strName.c_str(), node->strQuarteraryName.c_str()))
				return node;
		}
		return nullptr;
	}
}; 
inline MEvents* MGetMatchEventInfo() { return MEvents::GetInstance(); }

