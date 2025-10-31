#ifndef MSHAREDCOMMANDTABLE_H
#define MSHAREDCOMMANDTABLE_H

class MCommandManager;


#define MCOMMAND_VERSION	59 // Custom: Updated CMD version



#define MSCT_MASTER			1
#define MSCT_CLIENT			2
#define MSCT_MATCHSERVER	4
#define MSCT_AGENT			8
#define MSCT_CHECKER		16
#define MSCT_ALL			(MSCT_MASTER+MSCT_CLIENT+MSCT_AGENT)


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ¡÷¿« : ƒø∏«µÅEID∏¶ ºˆ¡§«œ∏ÅE±‚¡∏ø° ¿˙¿Â«— ∏Æ«√∑π¿Ã ∆ƒ¿œ¿Ã ¿Áª˝µ«¡ÅEæ ¿ª ºÅE¿÷Ω¿¥œ¥Ÿ.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// ∏µÅEƒøπ¬¥œƒ…¿Ã≈Õ∞° ∞¯¿Ø«œ¥¬ ƒø∏«µÅE
void MAddSharedCommandTable(MCommandManager* pCommandManager, int nSharedType);


/// º≠πÅE≈¨∂Û¿Ãæ∆Æ ∞¯¿Ø ªÛºÅE
#define MATCH_CYCLE_CHECK_SPEEDHACK	20000


// ±‚∫ª ƒø∏«µÅE∏ÆΩ∫∆Æ
#define MC_HELP						0
#define MC_VERSION					1

#define MC_DEBUG_TEST				100

#define MC_LOCAL_INFO				201		///< Local ¡§∫∏∏¶ æÚ¥¬¥Ÿ.
#define MC_LOCAL_ECHO				202		///< Local Echo ≈◊Ω∫∆Æ
#define MC_LOCAL_LOGIN				203		///< Login √≥∏Æ Local Command



#define MC_NET_ENUM					301		///< ø¨∞·«“ ºÅE¿÷¥¬ ƒøπ¬¥œƒ…¿Ã≈Õ Enum
#define MC_NET_CONNECT				302		///< ƒøπ¬¥œƒ…¿Ã≈ÕøÕ ø¨∞ÅE
#define MC_NET_DISCONNECT			303		///< ƒøπ¬¥œƒ…¿Ã≈ÕøÕ ø¨∞ÅE«ÿ¡¶
#define MC_NET_CLEAR				304		///< ƒøπ¬¥œƒ…¿Ã≈ÕøÕ ø¨∞ÅE«ÿ¡¶ »ƒ ∞ÅE√¿⁄øÅE√≥∏Æ
#define MC_NET_ONCONNECT			311		///< ƒøπ¬¥œƒ…¿Ã≈ÕøÕ ø¨∞·µ«æ˙¿ª ∂ß
#define MC_NET_ONDISCONNECT			312		///< ƒøπ¬¥œƒ…¿Ã≈ÕøÕ ø¨∞·¿Ã ≤˜∞Â¿ª ∂ß
#define MC_NET_ONERROR				313		///< ƒøπ¬¥œƒ…¿Ã≈ÕøÕ ø¨∞ÅEø°∑Ø
#define MC_NET_CHECKPING			321		///< ƒøπ¬¥œƒ…¿Ã≈Õ π›¿¿º”µµ∞ÀªÅE
#define MC_NET_PING					322		///< ƒøπ¬¥œƒ…¿Ã≈Õ π›¿¿º”µµ∞ÀªÅEø‰√ª
#define MC_NET_PONG					323		///< ƒøπ¬¥œƒ…¿Ã≈Õ π›¿¿º”µµ∞ÀªÅE¿¿¥ÅE

#define MC_HSHIELD_PING				324		///< «ŸΩ«µÅE∏ﬁºº¡ÅEø‰√ª(∏≈ƒ°º≠πÅE-> ≈¨∂Û¿Ãæ∆Æ)
#define MC_HSHIELD_PONG				325		///< «ŸΩ«µÅE∏ﬁºº¡ÅE¿¿¥ÅE≈¨∂Û¿Ãæ∆Æ -> ∏≈ƒ°º≠πÅE

#define MC_NET_BANPLAYER_FLOODING	326		///< CommandBuilderø°º≠ Flooding ∞À√‚Ω√ ∏∏µÂ¥¬ Command

#define MC_NET_CONNECTTOZONESERVER	331		///< 127.0.0.1:6000 ø¨∞ÅE

#define MC_NET_REQUEST_INFO			341		///< Net ¡§∫∏∏¶ ø‰√ª«—¥Ÿ.
#define MC_NET_RESPONSE_INFO		342		///< Net ¡§∫∏∏¶ æÚ¥¬¥Ÿ.
#define MC_NET_REQUEST_UID			343		///< MUID∏¶ ø‰√ª
#define MC_NET_RESPONSE_UID			344		///< MUID∏¶ »Æ∫∏«ÿº≠ µπ∑¡¡ÿ¥Ÿ.
#define MC_NET_ECHO					351		///< Net Echo ≈◊Ω∫∆Æ

#define MC_CLOCK_SYNCHRONIZE		361		///< ≈¨∑∞ ΩÃ≈©
#define	MC_SERVER_PING				362     ///Server Ping


// ∏≈ƒ°º≠πÅE∞ÅE√ ∏˙”…æÅE
#define MC_MATCH_NOTIFY						401		///< æÀ∏≤ ∏ﬁΩ√¡ÅE
#define MC_MATCH_ANNOUNCE					402		///< ∞¯¡ÅE
#define MC_MATCH_RESPONSE_RESULT			403		///< requestø° ¥ÅE— ∞·∞˙∏¶ æÀ∏≤(S -> C)
// ∞ÅEÆ¿⁄ ¿ÅEÅE∏˙”…æÅE
#define MC_ADMIN_ANNOUNCE					501		///< ¿ÅEº ¡¢º”¿⁄ø°∞‘ πÊº€
#define MC_SERVER_ANNOUNCE					503
#define MC_ADMIN_REQUEST_SERVER_INFO		505		///< º≠πÅE¡§∫∏ ø‰√ª
#define MC_ADMIN_RESPONSE_SERVER_INFO		506		///< º≠πÅE¡§∫∏ ¿¿¥ÅE
#define MC_ADMIN_SERVER_HALT				511		///< º≠πˆ∏¶ ¡æ∑ÅE«—¥Ÿ
#define MC_ADMIN_TERMINAL					512		///< ≈ÕπÃ≥Ø
#define MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE	513	///< ∞Ë¡§ ±««— ∫Ø∞ÅEø‰√ª
#define MC_ADMIN_RESPONSE_UPDATE_ACCOUNT_UGRADE	514	///< ∞Ë¡§ ±««— ∫Ø∞ÅE¿¿¥ÅE
#define MC_ADMIN_REQUEST_KICK_PLAYER			515		///< ≈∏∞Ÿ ¡¢º” ≤˜±ÅEø‰√ª
#define MC_ADMIN_RESPONSE_KICK_PLAYER			516		///< ≈∏∞Ÿ ¡¢º” ≤˜±ÅE¿¿¥ÅE
#define MC_ADMIN_REQUEST_MUTE_PLAYER			517		///< ≈∏∞Ÿ √§∆√ ±›¡ÅEø‰√ª
#define MC_ADMIN_RESPONSE_MUTE_PLAYER			518		///< ≈∏∞Ÿ √§∆√ ±›¡ÅE¿¿¥ÅE
#define MC_ADMIN_REQUEST_BLOCK_PLAYER			519		///< ≈∏∞Ÿ ¡¢º” ∫˙”∞ ø‰√ª
#define MC_ADMIN_RESPONSE_BLOCK_PLAYER			520		///< ≈∏∞Ÿ ¡¢º” ∫˙”∞ ¿¿¥ÅE
#define MC_ADMIN_PING_TO_ALL				521		///< Garbage Connection √ªº“ »Æ¿Œ¿ª ¿ß«— ºˆµø∏˙”…
#define MC_ADMIN_REQUEST_SWITCH_LADDER_GAME	522		///< ≈¨∑£¿ÅEΩ≈√ª ∞°¥…ø©∫Œ º≥¡§
#define MC_ADMIN_HIDE						531		///< ø˚€µ¿⁄ æ»∫∏¿Ã±ÅE
#define MC_ADMIN_RELOAD_CLIENT_HASH			532		///< reload XTrap Hashmap
#define MC_ADMIN_RESET_ALL_HACKING_BLOCK	533		///< ∏µÅE«ÿ≈∑ ∫˙”∞¿ª √ÅE“«—¥Ÿ.
#define MC_ADMIN_RELOAD_GAMBLEITEM			534		///< ∞◊∫ÅEæ∆¿Ã≈€ ¡§∫∏ ¥ŸΩ√ ¿–±ÅE
#define MC_ADMIN_DUMP_GAMBLEITEM_LOG		535		///< «ˆ¡¶ ∞◊∫ÅEæ∆¿Ã≈€ ¡§∫∏∏¶ Log∆ƒ¿œø° √‚∑¬«—¥Ÿ.
#define MC_ADMIN_ASSASIN					536		///< GM¿Ã ƒø∏«¥ı∞° µ»¥Ÿ.
#define MC_ADMIN_KICKALL					537		


// ¿Ã∫•∆Æ ∏∂Ω∫≈Õ ∏˙”…æÅE
#define MC_EVENT_CHANGE_MASTER				601		///< πÊ¿Â±««—¿ª ªØæ˚€¬¥Ÿ
#define MC_EVENT_CHANGE_PASSWORD			602		///< πÊ¿« ∫Òπ–π¯»£∏¶ πŸ≤€¥Ÿ
#define MC_EVENT_REQUEST_JJANG				611		///< ¬Ø∏∂≈© ∫Œø©
#define MC_EVENT_REMOVE_JJANG				612		///< ¬Ø∏∂≈© »∏ºÅE
#define MC_EVENT_UPDATE_JJANG				613		///< ¬Ø∏∂≈© æÀ∏≤


// ∏≈ƒ°º≠πÅE≈¨∂Û¿Ãæ∆Æ
#define MC_MATCH_LOGIN							1001	///< ∑Œ±◊¿Œ
#define MC_MATCH_RESPONSE_LOGIN					1002	///< ∑Œ±◊¿Œ ¿¿¥ÅE
#define MC_MATCH_LOGIN_NETMARBLE				1003	///< ≥›∏∂∫˙€°º≠ ∑Œ±◊¿Œ
#define MC_MATCH_LOGIN_NETMARBLE_JP				1004	///< ¿œ∫ª≥›∏∂∫˙€°º≠ ∑Œ±◊¿Œ
#define MC_MATCH_LOGIN_FROM_DBAGENT				1005	///< DBAgent∑Œ∫Œ≈Õ¿« ∑Œ±◊¿Œ ¿¿¥ÅE¿œ∫ª≥›∏∂∫ÅE¿ÅEÅE
#define MC_MATCH_LOGIN_FROM_DBAGENT_FAILED		1008	///< DBAgent∑Œ∫Œ≈Õ¿« ∑Œ±◊¿Œ Ω«∆–(¿œ∫ª≥›∏∂∫ÅE¿ÅEÅE
#define MC_MATCH_FIND_HACKING					1009	///< «ÿ≈∑ ∞À√ÅE
#define MC_MATCH_DISCONNMSG						1010	///< Discconst∏ﬁΩ√¡ÅE
#define MC_MATCH_LOGIN_NHNUSA					1011	///< πÃ±π NHNø°º≠ ∑Œ±◊¿Œ.
#define MC_MATCH_LOGIN_GAMEON_JP				1012	///< ¿œ∫ª gameonø°º≠ ∑Œ±◊¿Œ

#define MC_MATCH_BRIDGEPEER						1006	///< MatchServerø° Peer¡§∫∏ æÀ∏≤
#define MC_MATCH_BRIDGEPEER_ACK					1007	///< Peer¡§∫∏ ºˆΩ≈»Æ¿Œ

#define MC_MATCH_OBJECT_CACHE					1101	///< ø¿∫ÅEß∆Æ ƒ≥Ω√

#define MC_MATCH_REQUEST_RECOMMANDED_CHANNEL	1201	///< √÷√  ¬ÅE°«“ √§≥Œø°¥ÅEÿ √ﬂ√µ¿ª ø‰√ª«—¥Ÿ.
#define MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL	1202	///< √÷√  ¬ÅE°«“ √§≥Œø°¥ÅEÿ √ﬂ√µ«—¥Ÿ.
#define MC_MATCH_CHANNEL_REQUEST_JOIN			1205	///< √§≥Œø° ¬ÅE°(C -> S)
#define MC_MATCH_CHANNEL_REQUEST_JOIN_FROM_NAME	1206	///< √§≥Œ¿Ã∏ß¿∏∑Œ √§≥Œ ¡∂¿Œ ø‰√ª
#define MC_MATCH_CHANNEL_RESPONSE_JOIN			1207	///< √§≥Œ ¡∂¿Œ ¿¿¥ÅES -> C)
#define MC_MATCH_CHANNEL_LEAVE					1208	///< √§≥Œ ≈ª≈ÅE
#define MC_MATCH_CHANNEL_LIST_START				1211	///< √§≥Œ ∏Ò∑œ ¿ÅE€Ω√¿€ ø‰√ª
#define MC_MATCH_CHANNEL_LIST_STOP				1212	///< √§≥Œ ∏Ò∑œ ¿ÅE€¡ﬂ¡ÅEø‰√ª
#define MC_MATCH_CHANNEL_LIST					1213	///< √§≥Œ ∏Ò∑œ
#define MC_MATCH_CHANNEL_REQUEST_PLAYER_LIST	1221	///< «√∑π¿ÃæÅE∏ÆΩ∫∆Æ ø‰√ª
#define MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST	1222	///< «√∑π¿ÃæÅE∏ÆΩ∫∆Æ ¿¿¥ÅE
#define MC_MATCH_CHANNEL_REQUEST_CHAT			1225	///< √§≥Œø° ¥ÅE≠ ø‰√ª
#define MC_MATCH_CHANNEL_CHAT					1226	///< √§≥Œø°º≠ ¥ÅE≠ ¿¿¥ÅE
#define MC_MATCH_CHANNEL_DUMB_CHAT				1227	///< √§≥Œø°º≠ ¥ÅE≠ ¬˜¥‹, ∫°æ˚‘Æ ±‚¥… - »´±‚¡÷(2009.08.05)

#define MC_MATCH_CHANNEL_REQUEST_RULE			1230	///< √§≥Œ±‘ƒ¢ ø‰√ª
#define MC_MATCH_CHANNEL_RESPONSE_RULE			1231	///< √§≥Œ±‘ƒ¢ ¿¿¥ÅE
#define MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST		1232	///< √§≥Œ¿« ∑Œ∫Òø° ¿÷¥¬ ∏µÅE«√∑π¿ÃæÅE∏ÆΩ∫∆Æ ø‰√ª
#define MC_MATCH_CHANNEL_RESPONSE_ALL_PLAYER_LIST		1233	///< √§≥Œ¿« ∑Œ∫Òø° ¿÷¥¬ «√∑π¿ÃæÅE∏ÆΩ∫∆Æ ø‰√ª


#define MC_MATCH_STAGE_CREATE					1301	///< Ω∫≈◊¿Ã¡ÅEª˝º∫
#define MC_MATCH_RESPONSE_STAGE_CREATE			1302	///< Ω∫≈◊¿Ã¡ÅEª˝º∫ ¿¿¥ÅE
#define MC_MATCH_STAGE_JOIN						1303	///< Ω∫≈◊¿Ã¡ÅE¬ÅE°
#define MC_MATCH_REQUEST_STAGE_JOIN				1304	///< Ω∫≈◊¿Ã¡ÅE¬ÅE° ø‰√ª
#define MC_MATCH_REQUEST_PRIVATE_STAGE_JOIN		1305	///< ∫Òπ– Ω∫≈◊¿Ã¡ÅE¬ÅE° ø‰√ª
#define MC_MATCH_RESPONSE_STAGE_JOIN			1306	///< Ω∫≈◊¿Ã¡ˆø° ¡∂¿Œø©∫Œ∏¶ æÀ∏≤
#define MC_MATCH_STAGE_LEAVE					1307	///< Ω∫≈◊¿Ã¡ÅE≈ª≈ÅE
#define MC_MATCH_STAGE_REQUEST_PLAYERLIST		1308	///< Ω∫≈◊¿Ã¡ˆ¿« ¿Ø¿˙∏Ò∑œ ø‰√ª
#define MC_MATCH_STAGE_FOLLOW					1309	///< ¡ˆ¡§ ¿Ø¿˙∏¶ µ˚∂Ûº≠ Ω∫≈◊¿Ã¡ÅE¬ÅE°
#define MC_MATCH_RESPONSE_STAGE_FOLLOW			1310	///< ¡ˆ¡§ ¿Ø¿˙∏¶ µ˚∂Ûº≠ Ω∫≈◊¿Ã¡ÅE¬ÅE° ¿¿¥ÅE
#define MC_MATCH_REQUEST_STAGE_LIST				1311	///< ≈¨∂Û¿Ãæ∆Æ∞° Ω∫≈◊¿Ã¡ÅE∏ÆΩ∫∆Æ ø‰√ª
#define MC_MATCH_STAGE_LIST_START				1312	///< Ω∫≈◊¿Ã¡ÅE∏Ò∑œ ¿ÅE€Ω√¿€ ø‰√ª
#define MC_MATCH_STAGE_LIST_STOP				1313	///< Ω∫≈◊¿Ã¡ÅE∏Ò∑œ ¿ÅE€¡ﬂ¡ÅEø‰√ª
#define MC_MATCH_STAGE_LIST						1314	///< Ω∫≈◊¿Ã¡ÅE∏Ò∑œ
#define MC_MATCH_STAGE_CHAT						1321	///< Ω∫≈◊¿Ã¡ÅE¥ÅE≠
#define MC_MATCH_STAGE_REQUEST_QUICKJOIN		1322	///< ƒÅE∂¿Œ ø‰√ª
#define MC_MATCH_STAGE_RESPONSE_QUICKJOIN		1323	///< ƒÅE∂¿Œ ¿¿¥ÅE
#define MC_MATCH_STAGE_GO						1331	///< Ω∫≈◊¿Ã¡ÅEπ¯»£∑Œ ¬ÅE°
#define MC_MATCH_STAGE_REQUIRE_PASSWORD			1332	///< ∫Òπ–πÊ¿Ã∂ÅE∆–Ω∫øˆµÂ∞° « ø‰«œ¥Ÿ.(S -> C)


#define MC_MATCH_STAGE_REQUEST_ENTERBATTLE		1401	///< Ω∫≈◊¿Ã¡ÅE¿ÅEÅE¬ÅE°«—¥Ÿ∞ÅEø‰√ª
#define MC_MATCH_STAGE_ENTERBATTLE				1402	///< Ω∫≈◊¿Ã¡ÅE¿ÅEı¬ÅE°
#define MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER	1403	///< Ω∫≈◊¿Ã¡ÅE¿ÅEı≈ª≈ÅEC -> S)
#define MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT	1404	///< Ω∫≈◊¿Ã¡ÅE¿ÅEı≈ª≈ÅES -> C)

#define MC_MATCH_REQUEST_STAGESETTING			1411	///< Ω∫≈◊¿Ã¡ˆ¿« º≥¡§¿ª ø‰√ª
#define MC_MATCH_RESPONSE_STAGESETTING			1412	///< Ω∫≈◊¿Ã¡ˆ¿« º≥¡§¿ª æÀ∏≤
#define MC_MATCH_STAGESETTING					1413	///< Ω∫≈◊¿Ã¡ÅEº≥¡§
#define MC_MATCH_STAGE_MAP						1414	///< Ω∫≈◊¿Ã¡ÅE∏  º≥¡§
#define MC_MATCH_STAGE_REQUEST_FORCED_ENTRY		1415	///< Ω∫≈◊¿Ã¡ˆø°º≠ ∞‘¿”ø° ≥≠¿‘ ø‰√ª
#define MC_MATCH_STAGE_RESPONSE_FORCED_ENTRY	1416	///< Ω∫≈◊¿Ã¡ÅE∞‘¿” ≥≠¿‘ ¿¿¥ÅE
#define MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE	1417	///< Ω∫≈◊¿Ã¡ÅE∏±∑π¿Ã∏  ø‰º“(≈∏¿‘, »∏¬ÅE æ˜µ•¿Ã∆Æ
#define MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE	1418	///< Ω∫≈◊¿Ã¡ÅE∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ æ˜µ•¿Ã∆Æ

#define MC_MATCH_STAGE_MASTER					1421	///< Ω∫≈◊¿Ã¡ÅEπÊ¿ÅEæÀ∏≤
#define MC_MATCH_STAGE_PLAYER_STATE				1422	///< Ω∫≈◊¿Ã¡ÅE∑πµÅEµ˚‹« «√∑π¿ÃæÅEªÛ≈¬ ¡§∫∏
#define MC_MATCH_STAGE_TEAM						1423	///< Ω∫≈◊¿Ã¡ÅE∆¿
#define MC_MATCH_STAGE_START					1431	///< Ω∫≈◊¿Ã¡ÅEΩ√¿€(ƒ´ø˚‚Æ¥ŸøÅE
#define MC_MATCH_STAGE_LAUNCH					1432	///< Ω∫≈◊¿Ã¡ÅEΩ««ÅE
#define MC_MATCH_STAGE_RELAY_LAUNCH				1433	///< ∏±∑π¿Ãµ» Ω∫≈◊¿Ã¡ÅEΩ««ÅE

#define MC_MATCH_LOADING_COMPLETE				1441	///< ∑Œµ˘¿Ã ≥°≥µ¿Ω
#define MC_MATCH_STAGE_FINISH_GAME				1442	///< Ω∫≈◊¿Ã¡ÅE∞‘¿” ¡æ∑ÅE

#define MC_MATCH_REQUEST_GAME_INFO				1451	///< ∞‘¿”æ» ¡§∫∏∏¶ ¥ﬁ∂ÅEø‰√ª
#define MC_MATCH_RESPONSE_GAME_INFO				1452	///< ∞‘¿”æ» ¡§∫∏∏¶ æÀ∑¡¡ÿ¥Ÿ
#define MC_MATCH_RESPONSE_GAME_INFO_V2			1453	// Custom: Larger Game Info Packet

#define MC_MATCH_REQUEST_PEERLIST				1461	///< ¬ÅE©¡ﬂ¿Œ ¿Ø¿˙µÈ¿« Peer¡§∫∏∏¶ ø‰√ª
#define MC_MATCH_RESPONSE_PEERLIST				1462	///< ¬ÅE©¡ﬂ¿Œ ¿Ø¿˙µÈ¿« Peer¡§∫∏∏¶ æÀ∏≤

#define MC_MATCH_REQUEST_PEER_RELAY				1471	///< ∏ﬁƒ°º≠πˆø° «ÿ¥ÁPeerø° ¥ÅE— ¡ﬂ∞Ëø‰√ª
#define MC_MATCH_RESPONSE_PEER_RELAY			1472	///< «ÿ¥ÁPeerø° ¥ÅE— ¡ﬂ∞Ë«„∞° ≈ÅE∏

// ∞‘¿” ∞ÅE√
#define MC_MATCH_GAME_ROUNDSTATE				1501	///< ∂Ûø˚—Â¿« ªÛ≈¬∏¶ µø±‚»≠
#define MC_MATCH_ROUND_FINISHINFO				1502	///< ∂Ûø˚—ÅE¡æ∑·Ω√ ¡§∫∏ æ˜µ•¿Ã∆Æ (C <- S)
#define MC_MATCH_GAME_KILL						1511	///< ƒ≥∏Ø≈Õ ªÁ∏¡ (C -> S)
#define MC_MATCH_GAME_DEAD						1512	///< ƒ≥∏Ø≈Õ ¡◊æ˙¥Ÿ (S -> C)
#define MC_MATCH_GAME_LEVEL_UP					1513	///< ∞‘¿”¡ﬂ ∑π∫ßæÅE
#define MC_MATCH_GAME_LEVEL_DOWN				1514	///< ∞‘¿”¡ﬂ ∑π∫ß¥ŸøÅE
#define MC_MATCH_GAME_REQUEST_SPAWN				1515	///< ƒ≥∏Ø≈Õ ∫Œ»∞ø‰√ª (C -> S)
#define MC_MATCH_GAME_RESPONSE_SPAWN			1516	///< ƒ≥∏Ø≈Õ ∫Œ»∞ø‰√ª (C -> S)
#define MC_MATCH_GAME_TEAMBONUS					1517	///< ∆¿ ∫∏≥ Ω∫(S -> C)
#define MC_MATCH_GAME_CHAT						1518	// Custom: in-game chat server sided.

#define MC_MATCH_GAME_REQUEST_TIMESYNC			1521	///< Matchº≠πˆø° Ω√∞£µø±‚»≠ ø‰√ª
#define MC_MATCH_GAME_RESPONSE_TIMESYNC			1522	///< Ω√∞£µø±‚»≠
#define MC_MATCH_GAME_REPORT_TIMESYNC			1523	///< Ω√∞£µø±ÅE∞ÀªÅE

#define MC_MATCH_REQUEST_SUICIDE				1531	///< ¿⁄ªÅEø‰√ª
#define MC_MATCH_RESPONSE_SUICIDE				1532	///< ¿⁄ªÅE¿¿¥ÅE
#define MC_MATCH_RESPONSE_SUICIDE_RESERVE		1533	///< ¿⁄ªÅEøπæÅE
#define MC_MATCH_NOTIFY_THROW_TRAPITEM			1535	///< ∆Æ∑¶æ∆¿Ã≈€ ≈ı√¥ æÀ∏≤(C -> S)
#define MC_MATCH_NOTIFY_ACTIVATED_TRAPITEM		1536	///< ∆Æ∑¶æ∆¿Ã≈€ πﬂµø æÀ∏≤(C -> S)
#define MC_MATCH_NOTIFY_ACTIATED_TRAPITEM_LIST	1437	///< ≥≠¿‘«— ¿Ø¿˙ø°∞‘ «ˆ¿ÅEπﬂµø ¡ﬂ¿Œ ∆Æ∑¶æ∆¿Ã≈€ ∏Ò∑œ æÀ∑¡¡÷±ÅES -> C)
#define MC_MATCH_REQUEST_OBTAIN_WORLDITEM		1541	///< æ∆¿Ã≈€ ∏‘µµ∑œ ø‰√ª
#define MC_MATCH_OBTAIN_WORLDITEM				1542	///< æ∆¿Ã≈€ ∏‘¿Ω ∫ÅEŒµÂƒ≥Ω∫∆Æ(S -> C)
#define MC_MATCH_SPAWN_WORLDITEM				1543	///< æ∆¿Ã≈€¿Ã Ω∫∆˘µ 
#define MC_MATCH_REQUEST_SPAWN_WORLDITEM		1544	///< æ∆¿Ã≈€¿ª ∂≥±∏µµ∑œ ø‰√ª(C -> S)
#define MC_MATCH_REMOVE_WORLDITEM				1545	///< æ∆¿Ã≈€ æ¯æ˚›ÅES -> C)
#define MC_MATCH_ASSIGN_COMMANDER				1551	///< æœªÅEÅE¥ÅEÂ¿∏∑Œ ¿”∏ÅE
#define MC_MATCH_RESET_TEAM_MEMBERS				1552	///< ∆¿∏‚πÅE¥ŸΩ√ ºº∆√
#define MC_MATCH_SET_OBSERVER					1553	///< ∞≠¡¶∑Œ ø…¿˙πˆ∏µÂ∑Œ ¿ÅEØ


// µø¿«∞ÅE√
#define MC_MATCH_REQUEST_PROPOSAL				1561	///< ¥Ÿ∏• ªÁ∂˜¿« µø¿« ø‰√ª
#define MC_MATCH_RESPONSE_PROPOSAL				1562	///< µø¿« ø‰√ª ¿¿¥ÅE
#define MC_MATCH_ASK_AGREEMENT					1563	///< ¥Ÿ∏• ªÁ∂˜ø°∞‘ µø¿« ¡˙¿«(S -> C)
#define MC_MATCH_REPLY_AGREEMENT				1564	///< µø¿« ¿¿¥ÅEC -> S)

// ∑°¥ÅE∞ÅE√ (≈¨∑£¿ÅE
#define MC_MATCH_LADDER_REQUEST_CHALLENGE		1571	///< ∑°¥ÅEµµ¿ÅEø‰√ª(C -> S)
#define MC_MATCH_LADDER_RESPONSE_CHALLENGE		1572	///< ∑°¥ÅEµµ¿ÅE¿¿¥ÅE
#define MC_MATCH_LADDER_SEARCH_RIVAL			1574	///< ∑°¥ÅE∞Àªˆ¡ﬂ		
#define MC_MATCH_LADDER_REQUEST_CANCEL_CHALLENGE	1575	///< ∑°¥ÅEµµ¿ÅE√ÅE“ø‰√ª
#define MC_MATCH_LADDER_CANCEL_CHALLENGE		1576	///< ∑°¥ÅEµµ¿ÅE√ÅE“
#define MC_MATCH_LADDER_PREPARE					1578	///< ∑°¥ÅEΩ√¿€¡ÿ∫ÅE
#define MC_MATCH_LADDER_LAUNCH					1579	///< ∑°¥ÅE∑±ƒ° (S -> C)
#define MC_MATCH_STAGE_REQUEST_REJOIN			1580    ////  _REJOIN_NEW

// ¿«ªÁº“≈ÅE∞ÅE√
#define MC_MATCH_USER_WHISPER					1601	///< ±”º”∏ª
#define MC_MATCH_USER_WHERE						1602	///< ¿ßƒ°»Æ¿Œ
#define MC_MATCH_USER_OPTION					1605	///< ∞¢¡æø…º«(±”∏ª∞≈∫Œ,√ ¥ÅE≈∫Œ,ƒ£±∏∞≈∫Œ µ˚—ÅE
#define MC_MATCH_CHATROOM_CREATE				1651	///< √§∆√∑ÅE∞≥º≥
#define MC_MATCH_CHATROOM_JOIN					1652	///< √§∆√∑ÅE¬ÅE°
#define MC_MATCH_CHATROOM_LEAVE					1653	///< √§∆√∑ÅE≈ª≈ÅE
#define MC_MATCH_CHATROOM_INVITE				1661	///< √§∆√∑ÅE√ ¥ÅE
#define MC_MATCH_CHATROOM_CHAT					1662	///< √§∆√
#define MC_MATCH_CHATROOM_SELECT_WRITE			1665	///< √§∆√∑ÅEº±≈√

// ƒ≥∏Ø≈Õ ∞ÅE√
#define MC_MATCH_REQUEST_ACCOUNT_CHARLIST		1701	///< ∞Ë¡§¿« ¿ÅEº ƒ≥∏Ø≈Õ ∏ÆΩ∫∆Æ∏¶ ø‰√ª
#define MC_MATCH_RESPONSE_ACCOUNT_CHARLIST		1702	///< ∞Ë¡§¿« ¿ÅEº ƒ≥∏Ø≈Õ ∏ÆΩ∫∆Æ∏¶ ¿¿¥ÅE
#define MC_MATCH_REQUEST_SELECT_CHAR			1703	///< ƒ≥∏Ø≈Õ º±≈√ ø‰√ª
#define MC_MATCH_RESPONSE_SELECT_CHAR			1704	///< ƒ≥∏Ø≈Õ º±≈√ ¿¿¥ÅE
#define MC_MATCH_REQUEST_MYCHARINFO				1705	///< ≥ª ƒ≥∏Ø≈Õ ¡§∫∏ ø‰√ª
#define MC_MATCH_RESPONSE_MYCHARINFO			1706	///< ≥ª ƒ≥∏Ø≈Õ ¡§∫∏ ¿¿¥ÅE
#define MC_MATCH_REQUEST_CREATE_CHAR			1711	///< ≥ª ƒ≥∏Ø≈Õ ª˝º∫ ø‰√ª
#define MC_MATCH_RESPONSE_CREATE_CHAR			1712	///< ≥ª ƒ≥∏Ø≈Õ ª˝º∫ ¿¿¥ÅE
#define MC_MATCH_REQUEST_DELETE_CHAR			1713	///< ≥ª ƒ≥∏Ø≈Õ ªË¡¶ ø‰√ª
#define MC_MATCH_RESPONSE_DELETE_CHAR			1714	///< ≥ª ƒ≥∏Ø≈Õ ªË¡¶ ¿¿¥ÅE
#define MC_MATCH_REQUEST_COPY_TO_TESTSERVER		1715	///< ƒ≥∏Ø≈Õ ≈◊Ω∫∆Æº≠πˆ∑Œ ¿ÅE€ ø‰√ª - πÃ±∏«ÅE
#define MC_MATCH_RESPONSE_COPY_TO_TESTSERVER	1716	///< ƒ≥∏Ø≈Õ ≈◊Ω∫∆Æº≠πˆ∑Œ ¿ÅE€ ¿¿¥ÅE- πÃ±∏«ÅE
#define MC_MATCH_REQUEST_CHARINFO_DETAIL		1717	///< ¥Ÿ∏• «√∑π¿ÃæÅE¡§∫∏∫∏±ÅEµ˚‹« ¿⁄ºº«— «√∑π¿ÃæÅE¡§∫∏ ø‰√ª
#define MC_MATCH_RESPONSE_CHARINFO_DETAIL		1718	///< ¥Ÿ∏• «√∑π¿ÃæÅE¡§∫∏∫∏±ÅEµ˚‹« ¿⁄ºº«— «√∑π¿ÃæÅE¡§∫∏ ¿¿¥ÅE
#define MC_MATCH_REQUEST_ACCOUNT_CHARINFO		1719	///< ∞Ë¡§¿« «— ƒ≥∏Ø≈Õ ¡§∫∏ ø‰√ª
#define MC_MATCH_RESPONSE_ACCOUNT_CHARINFO		1720	///< ∞Ë¡§¿« «— ƒ≥∏Ø≈Õ ¡§∫∏ ¿¿¥ÅE
#define MC_MATCH_REQUEST_CREATE_ACCOUNT			1721	/// Custom: Creating Account from in game
#define MC_MATCH_RESPONSE_CREATE_ACCOUNT		1722	/// Custom: Respond to account creation request
// æ∆¿Ã≈€ ∞ÅE√
#define MC_MATCH_REQUEST_SIMPLE_CHARINFO		1801	///< ƒ≥∏Ø≈Õ¿« ∞£¥‹«— ¡§∫∏ ø‰√ª	
#define MC_MATCH_RESPONSE_SIMPLE_CHARINFO		1802	///< ƒ≥∏Ø≈Õ¿« ∞£¥‹«— ¡§∫∏ ¿¿¥ÅE
#define MC_MATCH_REQUEST_MY_SIMPLE_CHARINFO		1803	///< ≥ª ƒ≥∏Ø≈Õ XP, BPµÅE¡§∫∏ ø‰√ª
#define MC_MATCH_RESPONSE_MY_SIMPLE_CHARINFO	1804	///< ≥ª ƒ≥∏Ø≈Õ XP, BPµÅE¡§∫∏ ¿¿¥ÅE

#define MC_MATCH_REQUEST_BUY_ITEM				1811	///< æ∆¿Ã≈€ ±∏∏≈ ø‰√ª
#define MC_MATCH_RESPONSE_BUY_ITEM				1812	///< æ∆¿Ã≈€ ±∏∏≈ ¿¿¥ÅE
#define MC_MATCH_REQUEST_SELL_ITEM				1813	///< æ∆¿Ã≈€ ∆«∏≈ ø‰√ª
#define MC_MATCH_RESPONSE_SELL_ITEM				1814	///< æ∆¿Ã≈€ ∆«∏≈ ¿¿¥ÅE
#define MC_MATCH_REQUEST_SHOP_ITEMLIST			1815	///< º•ø°º≠ ∆«∏≈«œ¥¬ æ∆¿Ã≈€ ∏ÆΩ∫∆Æ ø‰√ª
#define MC_MATCH_RESPONSE_SHOP_ITEMLIST			1816	///< º•ø°º≠ ∆«∏≈«œ¥¬ æ∆¿Ã≈€ ∏ÆΩ∫∆Æ ¿¿¥ÅE

#define MC_MATCH_REQUEST_CHARACTER_ITEMLIST		1821	///< ≥ª æ∆¿Ã≈€ ∏ÆΩ∫∆Æ ¡÷Ω√ø¿
#define MC_MATCH_RESPONSE_CHARACTER_ITEMLIST	1822	///< æ∆¿Ã≈€ ∏ÆΩ∫∆Æ ø©ÉÖø¿
#define MC_MATCH_REQUEST_EQUIP_ITEM				1823	///< æ∆¿Ã≈€ ¿Â∫ÅEø‰√ª
#define MC_MATCH_RESPONSE_EQUIP_ITEM			1824	///< æ∆¿Ã≈€ ¿Â∫ÅE¿¿¥ÅE
#define MC_MATCH_REQUEST_TAKEOFF_ITEM			1825	///< æ∆¿Ã≈€ «ÿ¡¶ ø‰√ª
#define MC_MATCH_RESPONSE_TAKEOFF_ITEM			1826	///< æ∆¿Ã≈€ «ÿ¡¶ ¿¿¥ÅE
#define MC_RESPONSE_GAMBLEITEMLIST				1827	///< DB¿« ∞◊∫ÅEæ∆¿Ã≈€ ¡§∫∏ ¿ÅE€.
#define MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK	1828	///< ƒ≥∏Ø≈Õ¿« Look∏∏µÅEæ˜µ•¿Ã∆Æ«“∂ß ªÁøÅE—¥Ÿ.
#define MC_MATCH_REQUEST_CHARACTER_ITEMLIST_FORCE	1829	///< ≥ª æ∆¿Ã≈€ ∏ÆΩ∫∆Æ ¡÷Ω√ø¿ : ¡ﬂ∫π ø‰√ªµµ π´Ω√«œ¡ÅEæ ¥¬ πˆ¿ÅE¡÷¿««ÿº≠ ªÁøÅE“∞Õ)

#define MC_MATCH_REQUEST_ACCOUNT_ITEMLIST			1831	///< ≥ª √¢∞ÅEæ∆¿Ã≈€ ∏ÆΩ∫∆Æ ¡÷Ω√ø¿
#define MC_MATCH_RESPONSE_ACCOUNT_ITEMLIST			1832	///< √¢∞ÅEæ∆¿Ã≈€ ∏ÆΩ∫∆Æ ø©ÉÖø¿
#define MC_MATCH_REQUEST_BRING_ACCOUNTITEM			1833	///< √¢∞ÅEæ∆¿Ã≈€¿ª ≥ª ƒ≥∏Ø≈Õ∑Œ ∞°¡Æø¿±ÅE
#define MC_MATCH_RESPONSE_BRING_ACCOUNTITEM			1834	///< √¢∞ÅEæ∆¿Ã≈€ ∞°¡Æø¿±ÅE¿¿¥ÅE
#define MC_MATCH_REQUEST_BRING_BACK_ACCOUNTITEM		1835	///< ≥ª ƒ≥Ω¨æ∆¿Ã≈€ √¢∞˙”Œ ø≈±‚±ÅEø‰√ª
#define MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM	1836	///< ≥ª ƒ≥Ω¨æ∆¿Ã≈€ √¢∞˙”Œ ø≈±‚±ÅE¿¿¥ÅE
#define MC_MATCH_EXPIRED_RENT_ITEM					1837	///< æ∆¿Ã≈€¿Ã ±‚∞£ ∏∏∑·µ«æ˙Ω¿¥œ¥Ÿ.(S -> C)

#define MC_MATCH_REQUEST_GAMBLE					1841	///< ªÃ±ÅE(C -> S)
#define MC_MATCH_RESPONSE_GAMBLE				1842	///< ªÃ±ÅE∞·∞ÅE(S -> C)

#define MC_MATCH_REWARD_BATTLE_TIME				1843	///< πË∆≤ Ω√∞£ ∫∏ªÅE∞¯¡ÅES->C)

// æ∆¿Ã≈€ ∞ÅE√(º“∏º∫)
#define MC_MATCH_REQUEST_USE_SPENDABLE_NORMAL_ITEM		1850	///< ¿œπ› º“∏º∫ æ∆¿Ã≈€ ªÁøÅEø‰√ª(C -> S)
#define MC_MATCH_REQUEST_USE_SPENDABLE_BUFF_ITEM		1860	///< πˆ«¡ º“∏º∫ æ∆¿Ã≈€ ªÁøÅEø‰√ª(C -> S)
#define MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM		1861	///< πˆ«¡ º“∏º∫ æ∆¿Ã≈€ ªÁøÅE∞·∞ÅES -> C)
#define MC_MATCH_SPENDABLE_BUFF_ITEM_STATUS				1865	///< πˆ«¡ º“∏º∫ æ∆¿Ã≈€ ªÛ≈¬(S -> C)
#define MC_MATCH_REQUEST_USE_SPENDABLE_COMMUNITY_ITEM	1870	///< ƒøπ¬¥œ∆º º“∏º∫ æ∆¿Ã≈€ ªÁøÅEø‰√ª(C -> S)

// ƒ£±∏ ∞ÅE√
#define MC_MATCH_FRIEND_ADD						1901	///< ƒ£±∏ √ﬂ∞°
#define MC_MATCH_FRIEND_REMOVE					1902	///< ƒ£±∏ ªË¡¶
#define MC_MATCH_FRIEND_LIST					1903	///< ƒ£±∏ ∏Ò∑œ ø‰√ª
#define MC_MATCH_RESPONSE_FRIENDLIST			1904	///< ƒ£±∏ ∏Ò∑œ ¿ÅE€
#define MC_MATCH_FRIEND_MSG						1905	///< ƒ£±∏ √§∆√


// ≈¨∑£∞ÅE√
#define MC_MATCH_CLAN_REQUEST_CREATE_CLAN			2000	///< ≈¨∑£ ª˝º∫ ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_CREATE_CLAN			2001	///< ≈¨∑£ ª˝º∫ ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_ASK_SPONSOR_AGREEMENT			2002	///< πﬂ±‚¿Œø°∞‘ ≈¨∑£ ª˝º∫ µø¿« ¡˙¿«(S -> C)
#define MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT		2003	///< πﬂ±‚¿Œ¿Ã µø¿«∏¶ ¿¿¥ÅEC -> S)
#define MC_MATCH_CLAN_REQUEST_AGREED_CREATE_CLAN	2004	///< πﬂ±‚¿Œ¿Ã µø¿««— ≈¨∑£ ª˝º∫ ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN	2005	///< πﬂ±‚¿Œ¿Ã µø¿««— ≈¨∑£ ª˝º∫ ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_REQUEST_CLOSE_CLAN			2006	///< ∏∂Ω∫≈Õ∞° ≈¨∑£ ∆ÛºÅEø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN			2007	///< ∏∂Ω∫≈Õ¿« ≈¨∑£ ∆ÛºÅE¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_REQUEST_JOIN_CLAN				2008	///< ø˚€µ¿⁄∞° ∞°¿‘¿⁄¿« ∞°¿‘¿ª ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_JOIN_CLAN			2009	///< ø˚€µ¿⁄¿« ∞°¿‘¿⁄ ∞°¿‘¿« ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_ASK_JOIN_AGREEMENT			2010	///< ∞°¿‘¿⁄ø°∞‘ ∞°¿‘ µø¿« ¡˙¿«(S -> C)
#define MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT			2011	///< ∞°¿‘¿⁄¿« ∞°¿‘ µø¿« ¿¿¥ÅEC -> S)
#define MC_MATCH_CLAN_REQUEST_AGREED_JOIN_CLAN		2012	///< ∞°¿‘¿⁄∞° µø¿««— ≈¨∑£ ∞°¿‘ ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN		2013	///< ∞°¿‘¿⁄∞° µø¿««— ≈¨∑£ ∞°¿‘ ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_REQUEST_LEAVE_CLAN			2014	///< ≈¨∑£ø¯¿Ã ≈ª≈∏¶ ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN			2015	///< ≈¨∑£ø¯¿« ≈ª≈¿« ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_UPDATE_CHAR_CLANINFO			2016	///< ƒ≥∏Ø≈Õ¿« ≈¨∑£¡§∫∏∞° æ˜µ•¿Ã∆Æµ (S -> C)
#define MC_MATCH_CLAN_MASTER_REQUEST_CHANGE_GRADE	2017	///< ∏‚πˆ¿« ±««— ∫Ø∞ÅEø‰√ª(C -> S)
#define MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE	2018	///< ∏‚πˆ¿« ±««— ∫Ø∞ÅE¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_ADMIN_REQUEST_EXPEL_MEMBER	2019	///< ∏‚πˆ∏¶ ≈ª≈ÅEø‰√ª(C -> S)
#define MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER	2020	///< ∏‚πˆ∏¶ ≈ª≈ÅE¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_REQUEST_MSG					2021	///< ≈¨∑£ √§∆√ ø‰√ª(C -> S)
#define MC_MATCH_CLAN_MSG							2022	///< ≈¨∑£ √§∆√(S -> C)
#define MC_MATCH_CLAN_REQUEST_MEMBER_LIST			2023	///< ≈¨∑£ ∏‚πÅE∏ÆΩ∫∆Æ ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_MEMBER_LIST			2024	///< ≈¨∑£ ∏‚πÅE∏ÆΩ∫∆Æ ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_REQUEST_CLAN_INFO				2025	///< ≈¨∑£ ¡§∫∏ ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_CLAN_INFO			2026	///< ≈¨∑£ ¡§∫∏ ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_STANDBY_CLAN_LIST				2027	///< ≈¨∑£¿ÅE¥ÅE‚¡ﬂ¿Œ ≈¨∑£ ∏ÆΩ∫∆Æ ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_MEMBER_CONNECTED				2028	///< ≈¨∑£ø¯¿Ã ¡¢º”«‘(S -> C)
#define MC_MATCH_FRIEND_CONNECTED					2029	///< Friend >
#define MC_MATCH_CLAN_REQUEST_EMBLEMURL				2051	///< ≈¨∑£∏∂≈© URL ø‰√ª(C -> S)
#define MC_MATCH_CLAN_RESPONSE_EMBLEMURL			2052	///< ≈¨∑£∏∂≈© URL ¿¿¥ÅES -> C)
#define MC_MATCH_CLAN_LOCAL_EMBLEMREADY				2055	///< ≈¨∑£∏∂≈© ¥Ÿø˚”ŒµÅEøœ∑ÅEC -> C)
#define MC_MATCH_CLAN_ACCOUNCE_DELETE				2056	///< ≈¨∑£ ∆ÛºÅEø‰√ª ¡¢ºÅE¡§∫∏ ∞¯¡ÅE(S -> C)


// ≈ı«•∞ÅE√
#define MC_MATCH_CALLVOTE							2100	///< ≈ı«•∏¶ ¡¶æ»«—¥Ÿ. (C -> S)
#define MC_MATCH_NOTIFY_CALLVOTE					2101	///< ≈ı«•¡¶æ»¿ª æÀ∏∞¥Ÿ. (S -> C)
#define MC_MATCH_NOTIFY_VOTERESULT					2102	///< ≈ı«•∞·∞˙∏¶ æÀ∏∞¥Ÿ. (S -> C)
#define MC_MATCH_VOTE_YES							2105	///< ≈ı«• (C -> S)
#define MC_MATCH_VOTE_NO							2106	///< ≈ı«• (C -> S)
#define MC_MATCH_VOTE_RESPONSE						2107	///< ≈ı«• øπø‹.(S -> C)
#define MC_MATCH_VOTE_STOP							2108	///< ≈ı«• √ÅE“. (S -> C)

// πÊº€∞ÅE√
#define MC_MATCH_BROADCAST_CLAN_RENEW_VICTORIES			2200	///< ≈¨∑£¿Ã ∞Ëº” ø¨Ω¬¡ﬂ¿‘¥œ¥Ÿ.(S -> C)
#define MC_MATCH_BROADCAST_CLAN_INTERRUPT_VICTORIES		2201	///< ≈¨∑£¿« ø¨Ω¬¿ª ¿˙¡ˆ«œø¥Ω¿¥œ¥Ÿ.(S -> C)
#define MC_MATCH_BROADCAST_DUEL_RENEW_VICTORIES			2202	///< xx¥‘¿Ã xx√§≥Œ¿« xxπÊø°º≠ xø¨Ω¬¡ﬂ¿‘¥œ¥Ÿ.(S -> C)
#define MC_MATCH_BROADCAST_DUEL_INTERRUPT_VICTORIES		2203	///< xx¥‘¿Ã xx¥‘¿« xxø¨Ω¬¿ª ¿˙¡ˆ«ﬂΩ¿¥œ¥Ÿ. (S -> C)


// Duel Tournament ∞ÅE√.. Added by »´±‚¡÷
#define MC_MATCH_DUELTOURNAMENT_REQUEST_JOINGAME				2300	///< µ‡æÅE≈‰≥ ∏’∆Æ¿« ¬ÅE° Ω≈√ª(C -> S)
#define MC_MATCH_DUELTOURNAMENT_RESPONSE_JOINGAME				2301	///< µ‡æÅE≈‰≥ ∏’∆Æ¿« ¬ÅE° Ω≈√ªø° ¥ÅE— ¿¿¥ÅES -> C)
#define MC_MATCH_DUELTOURNAMENT_REQUEST_CANCELGAME				2302	///< µ‡æÅE≈‰≥ ∏’∆Æ¿« ¬ÅE° Ω≈√ª √ÅE“(C -> S)
#define MC_MATCH_DUELTOURNAMENT_CHAR_INFO						2303	///< µ‡æÅE≈‰≥ ∏’∆Æ¿« ƒ…∏Ø≈Õ ¡§∫∏(S -> C)
#define MC_MATCH_DUELTOURNAMENT_PREPARE_MATCH					2304	///< µ‡æÅE≈‰≥ ∏’∆Æ Ω√¿€ ¿ÅE
#define MC_MATCH_DUELTOURNAMENT_LAUNCH_MATCH					2305	///< µ‡æÅE≈‰≥ ∏’∆Æ Ω√¿€! (S - > C)
#define MC_MATCH_DUELTOURNAMENT_CANCEL_MATCH					2306	///< µ‡æÅE≈‰≥ ∏’∆Æ √ÅE“! (S - > C)
#define MC_MATCH_DUELTOURNAMENT_CHAR_INFO_PREVIOUS				2307	///< µ‡æÅE≈‰≥ ∏’∆Æ ƒ≥∏Ø≈Õ ¡ˆ≥≠¡÷ ¡§∫∏(S -> C)
#define MC_MATCH_DUELTOURNAMENT_NOT_SERVICE_TIME				2308	///< «ˆ¿ÅEµ‡æÛ≈‰≥ ∏’∆Æ º≠∫ÒΩ∫ Ω√∞£¿Ã æ∆¥’¥œ¥Ÿ.(ø¿«¬Ω√∞£ %s%dΩ√00∫– ~ %s%dΩ√59∫–) (S - > C)

#define MC_MATCH_DUELTOURNAMENT_REQUEST_SIDERANKING_INFO		2400	///< ƒ…∏Ø≈Õ ∑©≈∑¿« ±Ÿ√≥ ∑©≈∑ ¡§∫∏ ø‰√ª(C -> S)
#define MC_MATCH_DUELTOURNAMENT_RESPONSE_SIDERANKING_INFO		2401	///< ƒ…∏Ø≈Õ ∑©≈∑¿« ±Ÿ√≥ ∑©≈∑ ¡§∫∏ ¿¿¥ÅES -> C)
#define MC_MATCH_DUELTOURNAMENT_GROUPRANKING_INFO				2402	///< µ‡æÅE≈‰≥ ∏’∆Æ¿« ±◊∑ÅE∑©≈∑ ¡§∫∏(S -> C)

#define MC_MATCH_DUELTOURNAMENT_GAME_INFO						2500	///< µ‡æÅE≈‰≥ ∏’∆Æ ∞‘¿” ≥ªø°º≠¿« ¡§∫∏
#define MC_MATCH_DUELTOURNAMENT_GAME_ROUND_RESULT_INFO			2501	///< µ‡æÅE≈‰≥ ∏’∆Æ ∞‘¿” ≥ªø°º≠¿« ∂Ûø˚—ÅEΩ¬∆– ∞·∞ÅE¡§∫∏
#define MC_MATCH_DUELTOURNAMENT_GAME_MATCH_RESULT_INFO			2502	///< µ‡æÅE≈‰≥ ∏’∆Æ ∞‘¿” ≥ªø°º≠¿« ∏≈ƒ° Ω¬∆– ∞·∞ÅE¡§∫∏
#define MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS				2503	///< µ‡æÅE≈‰≥ ∏’∆Æ ∞‘¿” ≥ªø°º≠¿« ∂Ûø˚—Â∞° ¡æ∑·µ… ∂ß, «ÿ¥ÅE∞‘¿Ã∏”¿« ªÛ≈¬∏¶ º≠πˆ∑Œ ∫∏≥ø(C -> S)
#define MC_MATCH_DUELTOURNAMENT_GAME_NEXT_MATCH_PLYAERINFO		2504	///< µ‡æÅE≈‰≥ ∏’∆Æ ∞‘¿” ≥ªø°º≠¿« ¥Ÿ¿Ω ∏≈ƒ°æ˜¿« «√∑π¿ÃæÅE¡§∫∏

// ∞‘¿” ∑ÅE∞ÅE√
// πˆº≠ƒø ∏µÅE
#define MC_MATCH_ASSIGN_BERSERKER					3001	///< πˆº≠ƒø ¿”∏ÅES -> C)


#define MC_MATCH_DUEL_QUEUEINFO						3100	///< ¥ÅE‚ø≠ º¯πÅEæÀ∏≤(S -> C)
#define MC_MATCH_DUEL_QUEUEINFO_V2					3101	// Custom: Duel Queue Info V2


// ∏≈ƒ° ø°¿Ã¡Ø∆Æ ∞ÅE√ ∏˙”…æÅE
#define MC_MATCH_REGISTERAGENT				5001	///< ø°¿Ã¡Ø∆Æ µ˚”œ
#define MC_MATCH_UNREGISTERAGENT			5002	///< ø°¿Ã¡Ø∆Æ «ÿ¡ÅE
#define MC_MATCH_AGENT_REQUEST_LIVECHECK	5011	///< ø°¿Ã¡Ø∆Æ »Æ¿Œø‰√ª
#define MC_MATCH_AGENT_RESPONSE_LIVECHECK	5012	///< ø°¿Ã¡Ø∆Æ »Æ¿Œ¿¿¥ÅE
#define MC_AGENT_ERROR						5013	///< ø°¿Ã¿ÅEÆ ∞ÅE√ ø°∑Ø

#define MC_AGENT_CONNECT					5021	///< ∏≈ƒ°º≠πˆø° ø°¿Ã¡Ø∆Æ º≠πÅEµ˚”œ
#define MC_AGENT_DISCONNECT					5022	///< ∏≈ƒ°º≠πˆø° ø°¿Ã¡Ø∆Æ º≠πÅEµ˚”œ«ÿ¡ÅE
#define MC_AGENT_LOCAL_LOGIN				5023	///< ≈¨∂Û¿Ãæ∆Æ ¡¢º”√≥∏Æ
#define MC_AGENT_RESPONSE_LOGIN				5024	///< ≈¨∂Û¿Ãæ∆Æø° ∑Œ±◊¿Œ ≈ÅEÅE
//#define MC_AGENT_MATCH_VALIDATE				5031	///< ∏≈ƒ°º≠πˆøÕ¿« ¡¢º”»Æ¿Œ
#define MC_AGENT_STAGE_RESERVE				5051	///< ø°¿Ã¿ÅEÆø° Ω∫≈◊¿Ã¡ÅEπË¡§
#define MC_AGENT_STAGE_RELEASE				5052	///< ø°¿Ã¿ÅEÆø° Ω∫≈◊¿Ã¡ÅE«ÿ¡ÅE
#define MC_AGENT_STAGE_READY				5053	///< ø°¿Ã¿ÅEÆø° Ω∫≈◊¿Ã¡ÅE¡ÿ∫Òøœ∑ÅE
#define MC_AGENT_LOCATETO_CLIENT			5061	///< ≈¨∂Û¿Ãæ∆Æø°∞‘ Agent æÀ∏≤
#define MC_AGENT_RELAY_PEER					5062	///< ø°¿Ã¿ÅEÆø° ¡ﬂ∞Ë¡ˆΩ√
#define MC_AGENT_PEER_READY					5063	///< ∏ﬁƒ°º≠πˆø° ««æ˚›ÿ∫ÅEæÀ∏≤
#define MC_AGENT_PEER_BINDTCP				5071	///< ≈¨∂Û¿Ãæ∆Æ Bind TCP
#define MC_AGENT_PEER_BINDUDP				5072	///< ≈¨∂Û¿Ãæ∆Æ Bind UDP
#define MC_AGENT_PEER_UNBIND				5073	///< ≈¨∂Û¿Ãæ∆Æ Unbind
#define MC_GUNZ_SEND_COUNTRYCODE			5074    ///< Banderas
#define MC_PEER_KILLSTREAK					5075    ///< KillerStreak
#define MC_AGENT_TUNNELING_TCP				5081	///< TCP ≈Õ≥Œ∏µ
#define MC_AGENT_TUNNELING_UDP				5082	///< UDP ≈Õ≥Œ∏µ
#define MC_AGENT_ALLOW_TUNNELING_TCP		5083	///< TCP ≈Õ≥Œ∏µ «„øÅE
#define MC_AGENT_ALLOW_TUNNELING_UDP		5084	///< UDP ≈Õ≥Œ∏µ «„øÅE
#define MC_AGENT_DEBUGPING					5101	///< µπˆ±ÅEÅE
#define MC_AGENT_DEBUGTEST					5102	///< µπˆ±ÅEÅE

#define MC_TEST_BIRDTEST1					60001	///< πˆµÅE≈◊Ω∫∆Æ1
#define MC_TEST_PEERTEST_PING				60002	///< Target Peer Test
#define MC_TEST_PEERTEST_PONG				60003	///< Target Peer Test



/// ƒø∏«µÅEºº∫Œ ø…º«

// MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST¿« ºº∫Œø…º«
#define MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_NORMAL		0		
#define MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_NONCLAN		1
#define MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_MYCLAN		2


// ƒ˘Ω∫∆Æ ∞ÅE√ ∏˙”…æÅEº≠πŸ¿ÃπÅE∏µÅE ƒ˘Ω∫∆Æ ∏µÅE∞¯≈ÅE
#define MC_QUEST_NPC_SPAWN					6000	///< NPC Ω∫∆ÅES -> C)
#define MC_QUEST_ENTRUST_NPC_CONTROL		6001	///< NPC¿« ¡∂¡æ¿ª ««æ˚€°∞‘ ¿ß¿”(S -> C)
#define MC_QUEST_CHECKSUM_NPCINFO			6002	///< NPC¡§∫∏µÈ¿« √º≈©º∂¿ª º≠πˆø°∞‘ ∫∏≥Ω¥Ÿ(C -> S)
#define MC_QUEST_REQUEST_NPC_DEAD			6003	///< NPC ¡◊¿Ω ø‰√ª(C -> S)
#define MC_QUEST_NPC_DEAD					6004	///< NPC ¡◊¿Ω(S -> C)
#define MC_QUEST_REFRESH_PLAYER_STATUS		6005	///< ¡◊æ˙¿∏∏ÅE¥ŸΩ√ ≈¬æ˚œ™∞ÅE ∏µÅEªÛ≈¬∏¶ √÷ªÛ¿∏∑Œ ∫π±Õ(S -> C)
#define MC_QUEST_NPC_ALL_CLEAR				6006	///< NPC ∏µŒ ªË¡¶(S -> C)
#define MC_MATCH_QUEST_REQUEST_DEAD			6007	///< «√∑π¿ÃæÅE¡◊æ˙¥Ÿ∞ÅEø‰√ª(C -> S) ƒ˘Ω∫∆Æ∏µÂø°º≠¥¬ MC_MATCH_GAME_KILL¥ÅE≈ ∫∏≥Ω¥Ÿ.
#define MC_MATCH_QUEST_PLAYER_DEAD			6008	///< «√∑π¿ÃæÅE¡◊¿Ω(S -> C)
#define MC_QUEST_OBTAIN_QUESTITEM			6009	///< ƒ˘Ω∫∆Æ æ∆¿Ã≈€ »πµÅES -> C)
#define MC_QUEST_STAGE_MAPSET				6010	///< Ω∫≈◊¿Ã¡ÅE∏ º¬ ∫Ø∞ÅES <-> C)
#define MC_QUEST_OBTAIN_ZITEM				6011	///< ¿œπ› æ∆¿Ã≈€ »πµÅES -> C)
#define MC_QUEST_PING						6012	///< º≠πÅE>≈¨∂ÅE«Œ (S -> C)
#define MC_QUEST_PONG						6013	///< ≈¨∂ÅE>º≠πÅE«Œ (C -> S)
#define MC_QUEST_NPCLIST					6014	///< «ˆ¡¶ ƒ˘Ω∫∆Æø°º≠ ªÁøÅE“ NPC¿« ¡§∫∏.(S->C) 
///<  ≈¨∂Û¥¬ º≠πˆ¿« ¡§∫∏∏¶ ∞°¡ˆ∞ÅENPC∏¶ √ ±‚»≠ «—¥Ÿ.


#define MC_QUEST_PEER_NPC_BASICINFO			6040	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ NPC ±‚∫ª¡§∫∏
#define MC_QUEST_PEER_NPC_HPINFO			6041	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ NPC √º∑¬¡§∫∏
#define MC_QUEST_PEER_NPC_ATTACK_MELEE		6042	///< ±Ÿ¡¢ ∞¯∞›
#define MC_QUEST_PEER_NPC_ATTACK_RANGE		6043	///< ø¯∞≈∏Æ ∞¯∞›
#define MC_QUEST_PEER_NPC_SKILL_START		6044	///< Ω∫≈≥ ƒ≥Ω∫∆√ Ω√¿€
#define MC_QUEST_PEER_NPC_SKILL_EXECUTE		6045	///< Ω∫≈≥ ƒ≥Ω∫∆√º∫∞ÅE»ø∞ÅEΩ√¿€
#define MC_QUEST_PEER_NPC_DEAD				6046	///< ¡◊¿∫ ±◊ Ω√¡°ø° ««æ˚—Èø°∞‘ ∫∏≥Ω¥Ÿ. 
#define MC_QUEST_PEER_NPC_BOSS_HPAP			6047	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ NPC ∫∏Ω∫ √º∑¬ æ∆∏”

// ƒ˘Ω∫∆Æ ∞‘¿” ¡¯«ÅE∞ÅE√(ƒ˘Ω∫∆Æ ∏µÅE
#define MC_QUEST_GAME_INFO					6051	///< ∞‘¿” ¡§∫∏. ∑Œµ˘«“ ∏Æº“Ω∫ µ˚‹ª æÀ∏≤(S -> C)
#define MC_QUEST_COMBAT_STATE				6052	///< ƒ˘Ω∫∆Æ ∞‘¿”≥ª ªÛ≈¬ ¡§∫∏(S -> C)
#define MC_QUEST_SECTOR_START				6053	///< ∏ ºΩ≈Õ ∞‘¿” Ω√¿€(S -> C)
#define MC_QUEST_COMPLETED					6054	///< «ÿ¥ÅEƒ˘Ω∫∆Æ ≈¨∏ÆæÅES -> C)
#define MC_QUEST_FAILED						6055	///< ƒ˘Ω∫∆Æ Ω«∆–(S -> C)

#define MC_QUEST_REQUEST_MOVETO_PORTAL		6057	///< ∆˜≈ª∑Œ ¿Ãµø«‘(C -> S)
#define MC_QUEST_MOVETO_PORTAL				6058	///< ∆˜≈ª∑Œ ¿Ãµø(S -> C)
#define MC_QUEST_READYTO_NEWSECTOR			6059	///< ªı∑ŒøÅEºΩ≈Õ ¡ÿ∫ÅEøœ∑ÅES <-> C)

#define MC_GAME_START_FAIL					6060	///< ƒ˘Ω∫∆Æ Ω√¿€¿ª Ω«∆–.(S -> C)
#define MC_QUEST_STAGE_GAME_INFO			6061	///< ¥ÅE‚¡ﬂ Ω∫≈◊¿Ã¡ÅE¡§∫∏ æÀ∏≤(QL, Ω√≥™∏Æø¿ID). (S -> C)
#define MC_QUEST_SECTOR_BONUS				6062	///< ºΩ≈Õ ∫∏≥ Ω∫(S -> C)

// ƒ˘Ω∫∆Æ ∞‘¿” ¡¯«ÅE∞ÅE√(º≠πŸ¿ÃπÅE∏µÅE
#define MC_QUEST_ROUND_START				6100	///< ªı∑ŒøÅE∂Ûø˚—ÅEΩ√¿€(S -> C)
#define MC_QUEST_REQUEST_QL					6101	///< «ˆ¿ÅEƒ˘Ω∫∆Æ Ω∫≈◊¿Ã¡ˆ¿« QL¡§∫∏∏¶ ø‰√ª.(C -> S)
#define MC_QUEST_RESPONSE_QL				6102	///< «ˆ¿ÅEƒ˘Ω∫∆Æ Ω∫≈◊¿Ã¡ˆ¿« QL¡§∫∏∏¶ æÀ∏≤.(S -> C)
#define MC_QUEST_SURVIVAL_RESULT			6103	///< º≠πŸ¿ÃπÅE∞·∞ÅE≈ÅE∏(S -> C)
#define MC_SURVIVAL_RANKINGLIST				6104	///< º≠πŸ¿ÃπÅEªÛ¿ß ∑©≈∑ ∏Ò∑œ ¿ÅE€(S -> C)
#define MC_SURVIVAL_PRIVATERANKING			6105	///< ¿Ø¿˙¿« º≠πŸ¿ÃπÅE∑©≈∑ ¡§∫∏ ¿ÅE€(S -> C)


// ƒ˘Ω∫∆Æ ≈◊Ω∫∆ÆøÅE
#define MC_QUEST_TEST_REQUEST_NPC_SPAWN		6901	///< NPC Ω∫∆ÅE∫Œ≈π(C -> S)
#define MC_QUEST_TEST_REQUEST_CLEAR_NPC		6902	///< NPC Clear ∫Œ≈π(C -> S)
#define MC_QUEST_TEST_REQUEST_SECTOR_CLEAR	6903	///< ºΩ≈Õ Clear(C -> S)	
#define MC_QUEST_TEST_REQUEST_FINISH		6904	///< ƒ˘Ω∫∆Æ Complete(C -> S)

/// Ω≈±‘ƒ˘Ω∫∆ÆøÅE(√ß∏∞¡ÅEƒ˘Ω∫∆Æ)
#define MC_NEWQUEST_NPC_SPAWN				6301	///< NPC Ω∫∆˘ (S -> C)
#define MC_NEWQUEST_REQUEST_NPC_SPAWN		6304	///< NPC Ω∫∆ÅE(C -> S)
#define MC_NEWQUEST_REQUEST_NPC_DEAD		6302	///< NPC ¡◊¿Ω ø‰√ª(C -> S)
#define MC_NEWQUEST_NPC_DEAD				6303	///< NPC ¡◊¿Ω(S -> C)
#define MC_NEWQUEST_MOVE_TO_NEXT_SECTOR		6341	///< ¥Ÿ¿Ω ºΩ≈Õ∑Œ ¿Ãµø (S -> C)
#define MC_NEWQUEST_PEER_NPC_BASICINFO		6351	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ NPC ±‚∫ª¡§∫∏
#define MC_NEWQUEST_PEER_NPC_ACTION_EXECUTE	6404	///< NPC æ◊º« Ω√¿€
#define MC_NEWQUEST_PEER_NPC_ATTACK_MELEE	6405	///< NPC ±Ÿ¡¢∞¯∞›	//todok del ¿Ã∞« æ◊º« Ω««‡≈Î¡ˆ∏¶ πﬁ¿∏∏È peerµÈ¿Ã ∞¢¿⁄ √≥∏Æ«œ∞‘ µ∆¥Ÿ
#define MC_NEWQUEST_NEW_CONTROLLER			6406
#define MC_NEWQUEST_LATE_NPC_SPAWN			6407
#define MC_NEWQUEST_STAGE_GAME_INFO			6408
#define MC_MATCH_LATEJOIN_CQ				6410
#define MC_MATCH_LATEJOIN_QUEST				6411


// MatchServer Schedule∞ÅE√.
#define MC_MATCH_SCHEDULE_ANNOUNCE_MAKE				7001	///< Ω∫ƒ…¡ÅE∞¯¡ÅEª˝º∫.
#define MC_MATCH_SCHEDULE_ANNOUNCE_SEND				7002	///< Ω∫ƒ…¡ÅE∞¯¡ÅE¿ÅE€.
#define MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_DOWN	7003	///< ≈¨∑£º≠πÅE≈¨∑£¿ÅE∫Ò»∞º∫»≠.
#define MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_ON		7004	///< ≈¨∑£º≠πÅE≈¨∑£¿ÅE»∞º∫»≠.
#define MC_MATCH_SCHEDULE_STOP_SERVER				7005	///< º≠πÅE¡æ∑ÅE


// x-trap
#define MC_REQUEST_XTRAP_HASHVALUE					8001	///< ¿Ã¿ÅE˙¿« »£»Ø¿∏∑Œ ¡∏¿Á∏∏(ªÁøÅEœ¡ÅEæ ¥¬¥Ÿ.)
#define MC_RESPONSE_XTRAP_HASHVALUE					8002	///< ¿Ã¿ÅE˙¿« »£»Ø¿∏∑Œ ¡∏¿Á∏∏(ªÁøÅEœ¡ÅEæ ¥¬¥Ÿ.)
#define MC_REQUEST_XTRAP_SEEDKEY					8003	///< æœ»£≈∞ ø‰√ª(S -> C)
#define MC_RESPONSE_XTRAP_SEEDKEY					8004	///< æœ»£≈∞ ¿¿¥ÅEC -> S)
#define MC_REQUEST_XTRAP_DETECTCRACK				8005	///< ¿Ã¿ÅE˙¿« »£»Ø¿∏∑Œ ¡∏¿Á∏∏(ªÁøÅEœ¡ÅEæ ¥¬¥Ÿ.)

// gameguard
#define MC_REQUEST_GAMEGUARD_AUTH					9001
#define MC_RESPONSE_GAMEGUARD_AUTH					9002
#define MC_REQUEST_FIRST_GAMEGUARD_AUTH				9003
#define MC_RESPONSE_FIRST_GAMEGUARD_AUTH			9004

// ¿⁄ºÅE
#define MC_REQUEST_GIVE_ONESELF_UP					9101	///< ¿⁄ºˆ«œ¥Ÿ.


//// ««æ˚·ı««æÅE∞ÅE√ ∏˙”…æÅE/////////////////////////////////////////////////////////////
#define MC_PEER_PING				10001	///< peerµÅEªÁ¿Ã¿« network latency(ping)¿ª √¯¡§«œ±‚¿ß«— command
#define MC_PEER_PONG				10002	///< pingø° ¥ÅE— ¿¿¥ÅE
#define MC_PEER_UDPTEST				10005	///< Peer to Peer UDP ¿ÅE€¿Ã ∞°¥…«—¡ÅE∞ÀªÁ«—¥Ÿ.
#define MC_PEER_UDPTEST_REPLY		10006	///< Peer to Peer UDP ¿ÅE€¿Ã ∞°¥…«—¡ÅE∞ÀªÁ«—¥Ÿ.

#define MC_UDP_PING					10007	///< ClientøÕ AgentµÅEªÁ¿Ã¿« ping¿ª √¯¡§«œ±‚¿ß«— command
#define MC_UDP_PONG					10008	///< pingø° ¥ÅE— ¿¿¥ÅE

#define MC_PEER_OPENED				10011	///< ∞≥∫∞ Peer ≈ÅE≈ ∞≥≈ÅEÀ∏≤
#define MC_PEER_BASICINFO			10012	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ ƒ≥∏Ø≈Õ ±‚∫ª¡§∫∏ ≈ı«•∆«¡§¿« ±Ÿ∞≈∞° µ»¥Ÿ
#define MC_PEER_HPINFO				10013	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ ƒ≥∏Ø≈Õ √º∑¬¡§∫∏ ≈ı«• ∆«¡§¿« ±Ÿ∞≈∞° µ»¥Ÿ
#define MC_PEER_HPAPINFO			10014	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ ƒ≥∏Ø≈Õ √º∑¬/æ∆∏” ¡§∫∏
#define MC_PEER_DUELTOURNAMENT_HPAPINFO		10016	///< ¡§±‚¿˚¿∏∑Œ æ˜µ•¿Ã∆Æµ«¥¬ µ‡æÛ≈‰≥ ∏’∆Æ ¿ÅEÅEƒ≥∏Ø≈Õ √º∑¬/æ∆∏” ¡§∫∏

#define MC_PEER_CHANGECHARACTER		10015
#define MC_PEER_MOVE				10021   ///< ø° ≥™¡ﬂø° MC_OBJECT_MOVE∑Œ πŸ≤˚⁄˚⁄ﬂ«“µÅE.
#define MC_PEER_CHANGE_WEAPON		10022   
#define MC_PEER_CHANGE_PARTS		10023
#define MC_PEER_ATTACK				10031
#define MC_PEER_DAMAGE				10032
#define MC_PEER_RELOAD				10033
#define MC_PEER_SHOT				10034
#define MC_PEER_SHOT_SP				10035
#define MC_PEER_SKILL				10036
#define MC_PEER_SHOT_MELEE			10037	///< ±Ÿ¡¢∞¯∞›
#define MC_PEER_DIE					10041
#define MC_PEER_SPAWN				10042
#define MC_PEER_DASH				10045
#define MC_PEER_SPMOTION			10046
#define MC_PEER_CHAT				10052
#define MC_PEER_CHAT_ICON			10053	///< √§∆√ Ω√¿€/≥°. ∏”∏Æø° æ∆¿Ãƒ‹¿ª ∫∏ø©¡÷∞≈≥™ ∞®√·¥Ÿ.
#define MC_PEER_REACTION			10054	///< ¿Ã∆Â∆Æ∏¶ ¿ß«ÿ æÀ∑¡¡ÿ¥Ÿ 
#define MC_PEER_ENCHANT_DAMAGE		10055	///< ¿Œ√¶∆Æ µ•πÃ¡ˆ∏¶ ∏¬æ“¥Ÿ
#define MC_PEER_BUFF_INFO			10056	///< πˆ«¡ ¿Ã∆Â∆Æ∏¶ ¿ÅEﬁ(ºÙπˆ«¡¿« ∞ÊøÅE πﬂµøΩ√ ªÛ¥ÅEµ ≈ÅEˆ∏¶ πﬁ¿∏π«∑Œ µ˚∑Œ ∫∏≥ª¡÷¡ÅEæ æ∆µµ µ»¥Ÿ)
											///< ±◊∑Ø≥™ ≥≠¿‘¿⁄ø°∞‘¥¬ ¿⁄Ω≈¿« πﬂµø ¡ﬂ¿Œ ¿Ã∆Â∆Æ∏¶ æÀ∑¡¡÷æ˚⁄ﬂ «œπ«∑Œ ±◊∂ß ªÁøÅE—¥Ÿ)

#define MC_PEER_VAMPIRE				10066
#define MC_PEER_SNIFER				10067

#define MC_ADMIN_FREEZE				10068
#define MC_ADMIN_SPAWN_RESPONSE		10069
#define MC_ADMIN_SUMMON				10070
#define MC_ADMIN_SLAP				10071
#define MC_ADMIN_SPAWN				10072	
#define MC_ADMIN_PKICK				10073
#define MC_ADMIN_NAT				10074
#define MC_VIP_ANNOUNCE				10077

#define MC_MATCH_FLAG_EFFECT		10080
#define MC_MATCH_FLAG_CAP			10081
#define MC_MATCH_REQUEST_FLAG_CAP	10082
#define MC_MATCH_FLAG_STATE			10083

#define MC_ADMIN_GOTO				10084
#define MC_ADMIN_SILENCE			10085

#define MC_MATCH_REQUEST_INFECT		10086
#define MC_MATCH_INFECT				10087
#define MC_MATCH_LASTSURVIVOR		10088

#define MC_MATCH_GUNGAME_WEAPONDATA 10089
#define MC_ADMIN_TEAM				10094
#define MC_MATCH_REQUEST_ROLL		10095
#define MC_MATCH_RESPONSE_ROLL		10096
#define MC_REQUEST_RESOURCE_CRC32				11001
#define MC_RESPONSE_RESOURCE_CRC32				12002


// ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ∞ÅE√ ∫Œ∫–.
#define MC_MATCH_REQUEST_CHAR_QUEST_ITEM_LIST		21000	///< ≥™¿« ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ∏Ò∑œ¿ª ø‰√ª.(C -> S)
#define MC_MATCH_RESPONSE_CHAR_QUEST_ITEM_LIST		21001	///< ≥™¿« ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ∏Ò∑œ¿ª ¿ÅE€.(S -> C)
#define MC_MATCH_REQUEST_BUY_QUEST_ITEM				21002	///< ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ±∏¿‘ ø‰√ª.(C -> S)
#define MC_MATCH_RESPONSE_BUY_QUEST_ITEM			21003	///< ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ±∏¿‘ øœ∑ÅE(S -> C)
#define MC_MATCH_REQUEST_SELL_QUEST_ITEM			21004	///< ≥™¿« ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ∆«∏≈ ø‰√ª.(C -> S)
#define MC_MATCH_RESPONSE_SELL_QUEST_ITEM			21005	///< ≥™¿« ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ∆«∏≈ øœ∑ÅE(S -> C)
#define MC_MATCH_USER_REWARD_QUEST					21006	///< ƒ˘Ω∫∆Æ∞° øœ∑·µ»»ƒ ∫∏ªÛµ» ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ¡§∫∏.
#define MC_MATCH_REQUEST_DROP_SACRIFICE_ITEM		21007	///< »Òª˝ æ∆¿Ã≈€¿ª ΩΩ∑‘ø° ≥ı±ÅEø‰√ª.(C -> S)
#define MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM		21008	///< »Òª˝ æ∆¿Ã≈€¿ª ΩΩ∑‘ø° ≥ı±ÅE¿¿¥ÅE(S -> C)
#define MC_MATCH_REQUEST_CALLBACK_SACRIFICE_ITEM	21009	///< ΩΩ∑‘ø° ø√∑¡≥ı¿∫ »Òª˝ æ∆¿Ã≈€¿ª »∏ºÅEø‰√ª.(C -> S)
#define MC_MATCH_RESPONSE_CALLBACK_SACRIFICE_ITEM	21010	///< Ω∫∑‘ø° ø√∑¡≥ı¿∫ »Òª˝ æ∆¿Ã≈€¿ª »÷ºÅE¿¿¥ÅE(S -> C)
#define MC_MATCH_REQUEST_SLOT_INFO					21011	///< »Òª˝ æ∆¿Ã≈€ ΩΩ∑‘ ¡§∫∏∏¶ ø‰√ª.(C -> S)
#define MC_MATCH_RESPONSE_SLOT_INFO					21012	///< »Òª˝ æ∆¿Ã≈€ ΩΩ∑‘ ¡§∫∏∏¶ æÀ∏≤.(S -> C)
#define MC_MATCH_NEW_MONSTER_INFO					21013	///< ∏ÛΩ∫≈Õ µµ∞®ø° √ﬂ∞°µ… ªı∑ŒøÅE∏ÛΩ∫≈Õ ¡§∫∏.
#define MC_MATCH_REQUEST_MONSTER_BIBLE_INFO			21014	///< ∏ÛΩ∫≈Õ µµ∞®¿« ≥ªøÅEø‰√ª.(C -> S)
#define MC_MATCH_RESPONSE_MONSTER_BIBLE_INFO		21015	///< ∏ÛΩ∫≈Õ µµ∞®¿« ≥ªøÅEæÀ∏≤.(S -> C)


/// KeeperøÕ Keeper ManagerøÕ ≈ÅE≈«œ¥¬µ• « ø‰«— ∏˙”…. 2005.06.03 ƒø∏«µÅE¡§∏Æ « øÅE
#define MC_KEEPER_MANAGER_CONNECT				31001	///< keeper Managerø°º≠ keeper∑Œ ¡¢º” ø‰√ª.
#define MC_CHECK_KEEPER_MANAGER_PING			31002	///< KeeperøÕ Keeper Manager∞° ¡§ªÛ¿˚¿∏∑Œ ø¨∞·µ«æÅE¿÷¥¬¡ÅE∞ÀªÅE
#define MC_REQUEST_MATCHSERVER_STATUS			31004	///< MatchServer¿« ¡§∫∏∏¶ ø‰√ª.(Keeper -> MatchServer)
#define MC_RESPONSE_MATCHSERVER_STATUS			31005	///< Keeper∑Œ MatchServer¿« ¡§∫∏∏¶ ¿ÅE€«ÿ ¡‹.(MatchServer -> Keeper)
// 2005.06.01 ~
#define MC_RESPONSE_KEEPER_MANAGER_CONNECT		31006	///< Keeper manager¿« ¡¢º”¿Ã ¡§ªÛ¿˚¿∏∑Œ ¿Ã∑ÁæÅE¡ÅEª æÀ∏≤.( Keeper -> Keeper Manager )
#define MC_REQUEST_CONNECTION_STATE				31007	///< «ˆ¡¶ º≠πˆ¿« ªÛ≈¬.(Keeper Manager -> Keeper)
#define MC_RESPONSE_CONNECTION_STATE			31008	///< º≠πˆªÛ≈¬ ¿¿¥ÅE(Keeper -> Keeper Manager)
#define MC_REQUEST_SERVER_HEARBEAT				31009	///< º≠πˆ∞° µø¿€«œ∞ÅE¿÷¥¬¡ÅE¿¿¥ÅEø‰√ª.(Keeper -> MatchServer)
#define MC_RESPONSE_SERVER_HEARHEAT				31010	///< º≠πˆ∞° µø¿€«œ∞ÅE¿÷¿ª¿ª ¿¿¥ÅE(MatchServer -> Keeper)
#define MC_REQUEST_KEEPER_CONNECT_MATCHSERVER	31011	///< Keeper∞° MatchServer∑Œ ¡¢º”«œµµ∑œ ø‰√ª«‘(Keeper Manager -> Keeper -> MatchServer )
#define MC_RESPONSE_KEEPER_CONNECT_MATCHSERVER	31012	///< Keeper∞° MatchServerø° ¡¢º”«ﬂ¥Ÿ¥¬ ¿¿¥ÅE(MatchServer -> Keeper -> Keeper Manager )
#define MC_REQUEST_REFRESH_SERVER				31013	///< Keeper Manager¿« º≠πÅE∏ÆΩ∫∆Æ ºˆµø ∞ªΩ≈¿ª ¿ß«ÿº≠ Keeper∑Œ MatchServer¿« Heartbaet√§≈© ø‰√ª.(Keeper Manager -> Keeepr)
#define MC_REQUEST_LAST_JOB_STATE				31014	///< ∏∂¡ˆ∏∑ Ω««‡µ» ¿œ¿« ∞·∞˙∏¶ ø‰√ª.(Keeper -> Keeper Manager )
#define MC_RESPONSE_LAST_JOB_STATE				31015	///< ¿€æÅE∞·∞ÅEø‰√ª ¿¿¥ÅE(Keeper -> Keeper Manager)
#define MC_REQUEST_CONFIG_STATE					31016	///< Keeperø° ¿˙¿Âµ» º≥¡§ ¡§∫∏ ø‰√ª.(Keeper Manager -> Keeper)
#define MC_RESPONSE_CONFIG_STATE				31017	///< Keeperø° ¿˙¿Âµ» º≥¡§ ¡§∫∏ ø‰√ª ¿¿¥ÅE(Keeper -> Keeper Manager)
#define MC_REQUEST_SET_ONE_CONFIG				31018	///< Config«œ≥™ º≥¡§ ø‰√ª.(Keeper Manager -> Keeper)
#define MC_RESPONSE_SET_ONE_CONFIG				31019	///< Config«œ≥™ º≥¡§ ø‰√ª ¿¿¥ÅE(Keeper -> Keeper Manager)
#define MC_REQUEST_KEEPERMGR_ANNOUNCE			31020	///< Keeper Managerø°º≠ ∞¢ º≠πˆ∑Œ ∞¯¡ˆ∏¶ ∫∏≥ªµµ∑œ Keeperø° ø‰√ª«‘.(Keeper Manager -> Keeper)
#define MC_REQUEST_KEEPER_ANNOUNCE				31021	///< Keeperø°º≠ MatchServer∞° ∞¯¡ˆªÁ«◊¿ª æÀ∏Æµµ∑œ ø‰√ª«‘.(Keeper -> MatchServer)
#define MC_REQUEST_RESET_PATCH					31022	///< «ˆ¡¶+±˚›ÅE¡¯«‡«— ∆–ƒ° ¿€æ˜ªÛ≈¬∏¶ √ ±‚»≠«‘.
#define MC_REQUEST_DISCONNECT_SERVER			31023	///< KeeperøÕ MatchServerøÕ¿« ø¨∞ÅE¡æ∑ÅE
#define MC_REQUEST_REBOOT_WINDOWS				31024	///< ¿©µµøÅE¿Á∫Œ∆√.(Keeper Manager -> Keeper)
#define MC_REQUEST_ANNOUNCE_STOP_SERVER			31025	///< ∞¯¡ˆªÁ«◊∞ÅE«‘∞‘ º≠πˆ∏¶ ¡æ∑ÅEΩ√≈¥ ø‰√ª.
#define MC_RESPONSE_ANNOUNCE_STOP_SERVER		31026	///< ∞¯¡ˆªÁ«◊∞ÅE«‘∞‘ º≠πˆ∏¶ ¡æ∑ÅEΩ√≈¥ ø‰√ª ¿¿¥ÅE
#define MC_REQUEST_SERVER_AGENT_STATE			31027	///< «ˆ¡¶ º≠πˆøÕ ø°¿Ã¿ÅEÆ¿« Ω««ÅEªÛ≈¬ ø‰√ª.
#define MC_RESPONSE_SERVER_AGENT_STATE			31028	///< «ˆ¡¶ º≠πˆøÕ ø°¿Ã¿ÅEÆ¿« Ω««ÅEªÛ≈¬ ø‰√ª ¿¿¥ÅE
#define MC_REQUEST_WRITE_CLIENT_CRC				31029	///< ≈¨∂Û¿Ãæ∆Æ¿« CRC CheckSum¿ª º≠πÅEini∆ƒ¿œø° ¿˙¿ÅEø‰√ª.(KeeperManager -> Keeper)
#define MC_RESPONSE_WRITE_CLIENT_CRC			31030	///< ≈¨∂Û¿Ãæ∆Æ¿« CRC CheckSum¿ª º≠πÅEini∆ƒ¿œø° ¿˙¿ÅEø‰√ª ¿¿¥ÅE(Keeper -> Keeper Manager)
#define MC_REQUEST_KEEPER_RELOAD_SERVER_CONFIG	31031	///< KeeperøÕ ø¨∞·µ«æ˚‹÷¥¬ º≠πˆ∞° º≥¡§ ∆ƒ¿œ¿ª ¥ŸΩ√ ∑ŒµÂ«œµµ∑œ ø‰√ª«‘.(KeeperManager -> Keeper)
#define MC_REQUEST_RELOAD_CONFIG				31032	///< º≠πˆ¿« º≥¡§ ∆ƒ¿œ¿ª ¥ŸΩ√ ∑Œ¥Ÿ«œµµ∑œ ø‰√ª.(Keeper -> server)
#define MC_REQUEST_KEEPER_ADD_HASHMAP			31033	
#define MC_RESPONSE_KEEPER_ADD_HASHMAP			31034
#define MC_REQUEST_ADD_HASHMAP					31035
#define MC_RESPONSE_ADD_HASHMAP					31036

// MatchServer ∆–ƒ° ∞ÅE√.
#define MC_REQUEST_DOWNLOAD_SERVER_PATCH_FILE	32024	///< Keeper∞° FTPº≠πˆø°º≠ Patch∆ƒ¿œ¿ª ¥Ÿø˚”ŒµÅEΩ√≈¥.(Keeper Manager -> Keeper )
#define MC_REQUEST_START_SERVER					32026	///< MatchServerΩ««ÅEø‰√ª.(Keeper Manager -> Keeper)
#define MC_REQUEST_STOP_SERVER					32028	///< º≠πÅE¡§¡ÅEø‰√ª.(Keeper -> Keeper Manager)
#define MC_REQUEST_PREPARE_SERVER_PATCH			32030	///< ∆–ƒ° ¡ÿ∫ÅE¿€æÅE(æ–√ÅE«Æ±ÅE ¿”Ω√ ∆ƒ¿œ ¡¶∞≈.)
#define MC_REQUEST_SERVER_PATCH					32032	///< ∆–ƒ° ∆ƒ¿œ¿ª ∫πªÁ«‘.(Keeper Manager -> Keeper )
// MatchAgent ∆–ƒ° ∞ÅE√.
#define MC_REQUEST_STOP_AGENT_SERVER			33034	///< Agent server¡§¡ÅEø‰√ª.(Keeper Manager -> Keeper )
#define MC_REQUEST_START_AGENT_SERVER			33036	///< Agent serverΩ√¿€ ø‰√ª.(Keeper Manager -> Keeper)
#define MC_REQUEST_DOWNLOAD_AGENT_PATCH_FILE	33038	///< request agent patch file download.(Keeper Manager -> Keeper)
#define MC_REQUEST_PREPARE_AGENT_PATCH			33040	///< reqeust prepare agent patch.(Keeper Manager -> Keeper)
#define MC_REQUEST_AGENT_PATCH					33042	///< request agent patch.(Keeper Manager -> Keeper)
#define MC_REQUEST_SERVER_STATUS				33043	///< serverøÕ agent¿« ¡§∫∏∏¶ Keeper∑Œ ø‰√ª«‘.(Keeper Manager -> keeper)
#define MC_RESPONSE_SERVER_STATUS				33044	///< serverøÕ agetn¿« ¡§∫∏ ø‰√ª ¿¿¥ÅE(Keeper -> Keeper Manager)
// Keeper manager schedule.
#define MC_REQUEST_KEEPER_MANAGER_SCHEDULE		34001	///< request keeper manager schedule.(Keeper Manager -> Keeper -> MatchServer)
#define MC_RESPONSE_KEEPER_MANAGER_SCHEDULE		34002	///< Keeper Manager¿« Ω∫ƒ…¡ÅE˚”œ ø‰√ª ¿¿¥ÅE
#define MC_REQUEST_START_SERVER_SCHEDULE		34003	///< ¥ŸΩ√Ω√¿€ Ω∫ƒ…¡Ÿø°º≠ º≠πˆ∏¶ Ω√¿€«œ¥¬ Ω√ƒ…¡Ÿ. Ω√¿€«“∂ß±˚›ÅEΩ∫ƒ…¡Ÿ¿Ã ¡¶∞≈µ«¡ÅEæ ¿Ω.
///


/// Locator∞ÅE√.
#define MC_REQUEST_SERVER_LIST_INFO				40001	/// ¡¢º”∞°¥…«— º≠πˆ¿« ∏ÆΩ∫∆Æ ¡§∫∏ ø‰√ª.(Client -> Locator)
#define MC_RESPONSE_SERVER_LIST_INFO			40002	/// ¡¢º”∞°¥…«— º≠πÅE∏ÆΩ∫∆Æ ¡§∫∏ ¿¿¥ÅE(Locator -> Client)
#define MC_RESPONSE_BLOCK_COUNTRY_CODE_IP		40003	/// ¡¢º”∫“∞° ±π∞° ƒ⁄µÂ¿« IP¿¿¥ÅE(Locator -> Client)


// filter.
#define MC_RESPONSE_BLOCK_COUNTRYCODE			50001	/// ∫˙”∞µ» ±π∞°ƒ⁄µÂ¿« IP∞° ¡¢º”Ω√ ≈ÅE∏øÅE
#define MC_LOCAL_UPDATE_USE_COUNTRY_FILTER		50002
#define MC_LOCAL_GET_DB_IP_TO_COUNTRY			50003
#define MC_LOCAL_GET_DB_BLOCK_COUNTRY_CODE		50004
#define MC_LOCAL_GET_DB_CUSTOM_IP				50005
#define MC_LOCAL_UPDATE_IP_TO_COUNTRY			50006
#define MC_LOCAL_UPDATE_BLOCK_COUTRYCODE		50007
#define MC_LOCAL_UPDATE_CUSTOM_IP				50008
#define MC_LOCAL_UPDATE_ACCEPT_INVALID_IP		50009
#define MC_ADMIN_GIVE_COINSCASH					50010
#define MC_ADMIN_GIVE_COINSMEDALS				50011
#define MC_ADMIN_SENDITEM					    50012
#define MC_STAFF_CHAT							50013

#define MC_GUNZ_LASTDMG							50023
#define MC_GUNZ_DMGGIVEN						50024
#define MC_KILL_ALL                             50025
#define MC_ADMIN_TELEPORT                       50026
#define MC_TELEPORT_ALL							50027
#define MC_ADMIN_FREEZE_ALL						50028
#define MC_REPORT_USER							50029
#define MC_ADMIN_STAGE_JOIN						50030

////////// Spy Mode //////////
#define MC_SPY_STAGE_BAN_MAP_LIST					50032
#define MC_SPY_STAGE_ACTIVATE_MAP					50033
#define MC_SPY_STAGE_REQUEST_START					50034
#define MC_SPY_GAME_INFO							50035
#define MC_SPY_GAME_RESULT							50036
#define MC_SPY_GAME_SCORE							50037
#define MC_SPY_STAGE_REQUEST_BAN_MAP_LIST			50038
#define MC_MATCH_FREEZESPY							50039
#define MC_MATCH_UNFREEZESPY					    50040
#define MC_MATCH_SPY_ROUNDRESULT			    	50041


//////////////////LadderWars/////////////////////////////
#define MC_MATCH_LADDERWARS_CHARINFO			50052
#define MC_MATCH_JOIN_LADDERWARS				50053
#define MC_MATCH_LADDERWARS_COUNTER			50054
#define MC_MATCH_LADDERWARS_SIDERANK			50055
#define MC_MATCH_LADDERWARS_INVITED			50056
#define MC_MATCH_LADDERWARS_FRIENDINVITE		50057
#define MC_MATCH_LADDERWARS_FRIENDACCEPT		50058
#define MC_MATCH_LADDERWARS_FRIENDLEAVE		50059

///////////////////BlitzKriegs///////////////////////////
#define MC_BLITZ_START_HONOR				    50075
#define MC_BLITZ_PLAYER_KILL				    50076
#define MC_BLITZ_REQUEST_UPGRADE			    50077
#define MC_BLITZ_RESPONSE_UPGRADE			    50078
#define MC_BLITZ_REQUEST_CLASS				    50079
#define MC_BLITZ_RESPONSE_CLASS				    50080

#define MC_MATCH_VOTE				50085
#define MC_MATCH_VOTE_UPDATE		50086
#define MC_MATCH_RANDOM_MAPS		50087

////////////////////////////////////////////////////////
#define MC_ADMIN_WALL							50060
#define MC_UGRADEID_SEND                        50061
#define MC_ADMIN_GIVE_COINSMEDALS_RESPONSE		50065

//////////////////ChatVoice/////////////////////////////
#define MC_MATCH_SEND_VOICE_CHAT               50067
#define MC_MATCH_RECEIVE_VOICE_CHAT            50068
////////////////////////////////////////////////////////
#define MC_FIND_WEAPON						   50069 //Kill Feed
#define MC_PEER_SET_SWORD_COLOR                50070
#define MC_MATCH_RESPONSE_SPEC                 50071
#define MC_PEER_SPEC                           50072
#define MC_MATCH_REQUEST_SPEC                  50073
#define MC_PEER_MOVE_DELTA                     50074

/////////////////////////no lead//////////////////////////
#define MC_PEER_NOLEADBLOB			           50075
#define MC_PEER_NOLEAD				           50076

///////////////////autobanned////////////////////////////
#define MC_MATCH_BANME				            10091
#define MC_ADMIN_SBTEST				            10092
#define MC_MATCH_BANACCOUNT                     10106

///////////////////Information Player////////////////////////////
#define MC_MATCH_INFORMATION					10100
#define MC_MATCH_INFORMATION_RECEIVE			10095
#define MC_MATCH_INFORMATION_HPAP				10096
#define MC_MATCH_INFORMATION_HPAP_GO			10097
#define MC_MATCH_INFORMATION_CLOTHES			10098
#define MC_MATCH_INFORMATION_CLOTHES_GO			10099

/////////////////////////Skill Map//////////////////////////////
#define MC_MATCH_UPDATE_SKILLMAP_BESTTIME	    10101
#define MC_MATCH_REQUEST_SKILLMAP_BESTTIME	    10102
#define MC_MATCH_RESPONSE_SKILLMAP_BESTTIME	    10103
#define MC_SKILLMAP_OBTAIN_ITEM				    10105

///////////////////Comandos Staff//////////////////////////////

#define MC_ALL_CMD								10107
#define MC_ADMIN_CRASH							10108
#define MC_MATCH_UPDATE_CLIENT_SETTINGS         10109

///////////////////ANTILEADNEW////////////////////////////////
#define MC_GUNZ_ANTI_LEAD_JORK					10110

#endif