#ifndef _MMATCHSHOP_H
#define _MMATCHSHOP_H

#include "winsock2.h"
#include "MXml.h"
#include "MUID.h"
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

#include "MMatchItem.h"
#include "MQuestItem.h"
#include "MMatchDBGambleItem.h"

enum MSHOPMODE
{
	SHOP_NORMAL = 1,
	SHOP_CASH,
	SHOP_MEDAL,
	SHOP_EVENT,
};

struct ShopItemNode
{
	unsigned int	nItemID;
	int				nItemCount;
	bool			bIsRentItem;
	int				nRentPeriodHour;
	float			fStockRate;
	bool			bIsMedalItem;
	int				nMedalItem;
	
	


	ShopItemNode() : nItemID(0), nItemCount(0), bIsRentItem(false), nRentPeriodHour(0), bIsMedalItem(false), nMedalItem(0), fStockRate(0.0) {}
	};

class MMatchShop
{
private:
protected:
	vector<ShopItemNode*>					m_ItemNodeVector;
	map<unsigned int, ShopItemNode*>		m_ItemNodeMap;

	void ParseSellItem(MXmlElement& element);
	bool ReadXml(const char* szFileName);


public:
	MMatchShop();
	virtual ~MMatchShop();
	bool Create(const char* szDescFileName);
	void Destroy();

	void Clear();
	int GetCount() { return static_cast<int>(m_ItemNodeVector.size()); }
	bool IsSellItem(const unsigned long int nItemID);
	ShopItemNode* GetSellItemByIndex(int nListIndex);
	ShopItemNode* GetSellItemByItemID(int nItemID);

	static MMatchShop* GetInstance();


#ifdef _DEBUG
	void MakeShopXML();
#endif
};

inline MMatchShop* MGetMatchShop() { return MMatchShop::GetInstance(); }

#define MTOK_SELL					"SELL"
#define MTOK_SELL_ITEMID			"itemid"
#define MTOK_SELL_ITEM_COUNT		"item_Count"		
#define MTOK_SELL_RENT_PERIOD_HOUR	"rent_period"
#define MTOK_SELL_MEDAL_ITEM		"medalitem"
#define MTOK_SELL_STOCK_RATE		"stockrate"



#endif