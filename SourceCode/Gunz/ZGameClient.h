#ifndef _ZGAMECLIENT_H
#define _ZGAMECLIENT_H

#include "ZPrerequisites.h"
#include "MMatchClient.h"
#include "MSharedCommandTable.h"
#include "MMatchStage.h"
#include "ZChannelRule.h"
#include "ZGame.h"
#include "ZNetAgreementBuilder.h"
#include "MEmblemMgr.h"
#include "ZCommandUDPHackShield.h"
#include "MChattingFilter.h"


typedef bool(ZONCOMMANDCALLBACK)(MCommand* pCommand);
class MListBox;
class ZCharacterViewList;
class UPnP;


class ZGameClient : public MMatchClient
{
protected:
	char				m_szChannel[256];
	char				m_szChannelRule[128];
	char				m_szStageName[256];
	char				m_szChatRoomInvited[64];
	unsigned int		m_nRoomNo;
	int					m_nStageCursor;
	bool				m_bLadderWars;
	bool				m_bLadderGame;
	MCHANNEL_TYPE		m_CurrentChannelType;
	bool				m_bEnableInterface;
	char				m_szVoteText[256];

private:
	unsigned long int		m_nPrevClockRequestAttribute;
	
	int						m_nBridgePeerCount;
	unsigned long int		m_tmLastBridgePeer;

	int						m_nCountdown;
	unsigned long int		m_tmLastCountdown;

	int						m_nRequestID;			///< 클랜 생성등에서 쓰이는 RequestID
	MUID					m_uidRequestPlayer;		///< 클랜 생성등의 요청자 
	ZNetAgreementBuilder	m_AgreementBuilder;		///< 동의빌더
	MMatchProposalMode		m_nProposalMode;

	// 투표인터페이스 관련
	bool					m_bVoteInProgress;		///< 투표가 진행중인가
	bool					m_bCanVote;				///< 투표권이 있는가

	// 클랜 앰블럼 관련
	MEmblemMgr				m_EmblemMgr;

	// 기타옵션 관련
	bool					m_bPriorityBoost;		///< 성능최대화
	bool					m_bRejectNormalChat;	///< 일반챗 허용
	bool					m_bRejectTeamChat;		///< 팀챗 허용
	bool					m_bRejectClanChat;		///< 클랜챗 허용
	bool					m_bRejectWhisper;		///< 귓말허용
	bool					m_bRejectInvite;		///< 초대허용
	int						m_nPingLimitStrikes;
#ifdef _SRVRPNG
	DWORD					m_ServerPing;
#endif
	bool                    m_bEnableCrash;

	// Custom: Ignore list
	list<string>			m_IgnoreList;

public:
#ifdef _REJOIN_NEW
	bool IsRejoin;
#endif
	bool m_LobbyChat;
	bool m_bAdminNAT;
#ifdef _CW_VOTE
	int LastVoteID;
	bool bMatching;
#endif // _CW_VOTE
	bool isPreGame;
	//bool ShowDamage;
	MBitmapR2* m_pSelectorImg0;
	MBitmapR2* m_pSelectorImg1;
	MBitmapR2* m_pSelectorImg2;


public:
	// 듀얼토너먼트 내 전적정보 (로그인할때만 1회 받음, 언어변경시 여기에 기억된 값으로 ui갱신)
	struct DTCHARINFO {
		int tournamentPoint, wins, losses, ranking, winners, lastWeekGrade;
	};
	struct LADDERWARSCHARINFO
	{
		int Ranking, Wins, Losses, Draws, Score;
	};
	const LADDERWARSCHARINFO* GetMyLadderWarsCharInfo() { return &m_LadderWarsCharInfo; };
	const DTCHARINFO* GetMyDuelTournamentCharInfo()		{ return &m_dtCharInfo; }
	const DTCHARINFO* GetMyDuelTournamentCharInfoPrev() { return &m_dtCharInfoPrev; }
	uint64 scHandlerID;
private:
	DTCHARINFO				m_dtCharInfo, m_dtCharInfoPrev;
	LADDERWARSCHARINFO              m_LadderWarsCharInfo;

protected:
	void SetChannelRuleName(const char* pszName)	{ strcpy(m_szChannelRule, pszName); }
	int GetBridgePeerCount()			{ return m_nBridgePeerCount; }
	void SetBridgePeerCount(int nCount)	{ m_nBridgePeerCount = nCount; }
	void UpdateBridgePeerTime(unsigned long int nClock)	{ m_tmLastBridgePeer = nClock; }
	void StartBridgePeer();

	void UpdateStageSetting(MSTAGE_SETTING_NODE* pSetting, STAGE_STATE nStageState, const MUID& uidMaster);
	void SetCountdown(int nCountdown)	{ m_nCountdown = nCountdown; m_tmLastCountdown = 0; }
	int GetCountdown()					{ return m_nCountdown; }
	bool Countdown(int nClock) {
		if (nClock - m_tmLastCountdown > 1000) {
			m_nCountdown--;
			m_tmLastCountdown = nClock;
			return true;
		}
		return false;
	}

	static int FindListItem(MListBox* pListBox, const MUID& uid);

protected:
	ZONCOMMANDCALLBACK*		m_fnOnCommandCallback;
	
	bool					m_bIsBigGlobalClock;	// 글로벌 클럭이 로컬보다 더 크면 true
	unsigned long int		m_nClockDistance;	///< 글로벌 클럭과의 시간차

	MMatchStageSetting		m_MatchStageSetting;
	bool					m_bForcedEntry;		///< 난입해서 들어가는지 여부
protected:
	virtual bool OnCommand(MCommand* pCommand);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockConnect(SOCKET sock);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);
	virtual int OnConnected(SOCKET sock, MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp);
	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	virtual void OnPrepareCommand(MCommand* pCommand);	///< 커맨드를 처리하기 전에
	virtual int OnResponseMatchLogin(const MUID& uidServer, int nResult, const char* szServerName, const MMatchServerMode nServerMode,
									 const char* szAccountID, const MMatchUserGradeID nUGradeID, const MMatchPremiumGradeID nPGradeID, int nCountryFlag, int nCash, int nEvent,
									 const MUID& uidPlayer, bool bEnabledSurvivalMode, bool bEnabledDuelTournament, unsigned char* szEncryptMsg);	// update sgk 0409
	virtual void OnBridgePeerACK(const MUID& uidChar, int nCode);
	virtual void OnObjectCache(unsigned int nType, void* pBlob, int nCount);
	virtual void OnAgentError(int nError);

	virtual void OnPrepareRun(void);

protected:
	void OnMatchNotify(unsigned int nMsgID);
//	void OnPeerList(const MUID& uidStage, void* pBlob, int nCount);
//	void OnAddPeer(const MUID& uidChar, const char* szIP, const int nPort =				
//		MATCHCLIENT_DEFAULT_UDP_PORT, MTD_CharInfo* pCharInfo = NULL, int nTeam = 0);
	void OnAnnounce(unsigned int nType, char* szMsg);
	void OnResponseResult(const int nResult);
	void OnStageSettingUpdate(DWORD ping);

	void OnChannelResponseJoin(const MUID& uidChannel, MCHANNEL_TYPE nChannelType, const char* szChannelName, bool bEnableInterface);
	void OnChannelChat(const MUID& uidChannel, char* szName, char* szChat,int nGrade);
	void OnChannelList(void* pBlob, int nCount);
	void OnChannelResponseRule(const MUID& uidchannel, const char* pszRuleName);

	void OnLadderPrepare(const MUID& uidStage, const int nTeam);
	void OnLadderLaunch(const MUID& uidStage, const char* pszMapName, bool LadderWars);
	void OnLadderResponseChallenge(const int nResult);

	void OnStageJoin(const MUID& uidChar, const MUID& uidStage, unsigned int nRoomNo, char* szStageName);
	void OnStageLeave(const MUID& uidChar, const MUID& uidStage);
//	void OnStageEnterBattle(const MUID& uidChar, const MUID& uidStage, MCmdEnterBattleParam nParam, MTD_PeerListNode* pPeerNode);
//	void OnStageLeaveBattle(const MUID& uidChar, const MUID& uidStage);
	void OnStageStart(const MUID& uidChar, const MUID& uidStage, int nCountdown);
	void OnStageRelayStart();
	void OnStageLaunch(const MUID& uidStage, const char* pszMapName);
	void OnStageFinishGame(const MUID& uidStage, const bool bIsRelayMapUnFinish);
	void OnStageMap(const MUID& uidStage, char* szMapName, bool bIsRelayMap = false);
	void OnStageTeam(const MUID& uidChar, const MUID& uidStage, unsigned int nTeam);
	void OnStagePlayerState(const MUID& uidChar, const MUID& uidStage, MMatchObjectStageState nStageState);
	void OnStageMaster(const MUID& uidStage, const MUID& uidChar);
	void OnStageChat(const MUID& uidChar, const MUID& uidStage, char* szChat);
	void OnStageList(int nPrevStageCount, int nNextStageCount, void* pBlob, int nCount);
	void OnStageRelayMapListUpdate(int nRelayMapType, int nRelayMapRepeatCount, void* pStageRelayMapListBlob);
	void OnStageRelayMapElementUpdate(int nRelayMapType, int nRelayMapRepeatCount);
	void OnResponseFriendList(void* pBlob, int nCount);
	void OnChannelPlayerList(int nTotalPlayerCount, int nPage, void* pBlob, int nCount);
	void OnChannelAllPlayerList(const MUID& uidChannel, void* pBlob, int nBlobCount);
	void OnResponseStageSetting(const MUID& uidStage, void* pStageBlob, int nStageCount, void* pCharBlob, 
		                        int nCharCount, STAGE_STATE nStageState, const MUID& uidMaster);
	void OnResponseRecommandedChannel(const MUID& uidChannel);
	void OnResponsePeerRelay(const MUID& uidPeer);
	void OnResponseGameInfo(const MUID& uidStage, void* pGameInfoBlob, void* pRuleInfoBlob, void* pPlayerInfoBlob, bool bNew);
	void OnResponseCharInfoDetail(void* pBlob);

	void OnLoadingComplete(const MUID& uidChar, int nPercent);
	void OnForcedEntryToGame();

	void OnUserWhisper(char* pszSenderName, char* pszTargetName, char* pszMessage);
	void OnChatRoomJoin(char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomLeave(char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomSelectWrite(char* pszChatRoomName);
	void OnChatRoomInvite(char* pszSenderName, char* pszRoomName);
	void OnChatRoomChat(char* pszChatRoomName, char* pszPlayerName, char* pszChat);

	void OnResponseUpdateStageEquipLook( const MUID& uidPlayer, const int nParts, const int nItemID );

	// 따라가기 음답.
	void OnFollowResponse( const int nMsgID );
	void OnExpiredRentItem(void* pBlob);	///< 기간제 아이템 사용 만료 통지받음

	void OnBirdTest();
protected:
	// 클랜관련
	void OnResponseCreateClan(const int nResult, const int nRequestID);
	void OnResponseAgreedCreateClan(const int nResult);
	void OnClanAskSponsorAgreement(const int nRequestID, const char* szClanName, MUID& uidMasterObject, const char* szMasterName);
	void OnClanAnswerSponsorAgreement(const int nRequestID, const MUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer);
	void OnClanResponseCloseClan(const int nResult);
	void OnClanResponseJoinClan(const int nResult);
	void OnClanAskJoinAgreement(const char* szClanName, MUID& uidClanAdmin, const char* szClanAdmin);
	void OnClanAnswerJoinAgreement(const MUID& uidClanAdmin, const char* szJoiner, const bool bAnswer);
	void OnClanResponseAgreedJoinClan(const int nResult);
	void OnClanUpdateCharClanInfo(void* pBlob);
	void OnClanResponseLeaveClan(const int nResult);
	void OnClanResponseChangeGrade(const int nResult);
	void OnClanResponseExpelMember(const int nResult);
	void OnClanMsg(const char* szSenderName, const char* szMsg);
	void OnClanMemberList(void* pBlob);
	void OnClanResponseClanInfo(void* pBlob);
	void OnClanStandbyClanList(int nPrevStageCount, int nNextStageCount, void* pBlob);
	void OnClanMemberConnected(const char* szMember, const MMatchClanGrade nMemberGrade);

	// 동의관련
	void OnResponseProposal(const int nResult, const MMatchProposalMode nProposalMode, const int nRequestID);
	void OnAskAgreement(const MUID& uidProposer, 
		                void* pMemberNamesBlob, 
						const MMatchProposalMode nProposalMode, 
						const int nRequestID);
	void OnReplyAgreement(const MUID& uidProposer, 
		                  const MUID& uidChar, 
						  const char* szReplierName, 
						  const MMatchProposalMode nProposalMode,
					      const int nRequestID, 
						  const bool bAgreement);
	void ReplyAgreement(const MUID& uidProposer, const MMatchProposalMode nProposalMode, bool bAgreement);
protected:
#ifdef _UPREWARDLEVEL
	void OnGameLevelUp(const MUID& uidChar, const int nBounty, const int nCash, const int nItemID);
#else
	void OnGameLevelUp(const MUID& uidChar);
#endif
	void OnGameLevelDown(const MUID& uidChar);

public:				// 젠장 -_-; 리플레이에서 이 이벤트들 처리해야 함;
	void OnSpawnWorldItem(void* pBlob);
	void OnObtainWorldItem(const MUID& uidChar, const int nItemUID);
	void OnRemoveWorldItem(const int nItemUID);

	void OnNotifyActivatedTrapItemList(void* pBlob);

protected:
	// Local
	void OnLocalReport119();
protected:
	void OnAdminAnnounce(const char* szMsg, const ZAdminAnnounceType nType);
	void OnPlayerNotify(const char* szReason);
#ifdef _ANTISPAM
	void OnAdminWall(const char* szMsg, const char* szCharName);
#endif
	void OnStaffHelp(const char* szMsg, const char* szUsr, const int UgradeID);
public:
	ZGameClient();
	virtual ~ZGameClient();

	void PriorityBoost(bool bBoost);
	bool GetPriorityBoost()				{ return m_bPriorityBoost; }
	bool GetRejectNormalChat()			{ return m_bRejectNormalChat; }
	void SetRejectNormalChat(bool bVal)	{ m_bRejectNormalChat = bVal; }
	bool GetRejectTeamChat()			{ return m_bRejectTeamChat; }
	void SetRejectTeamChat(bool bVal)	{ m_bRejectTeamChat = bVal; }
	bool GetRejectClanChat()			{ return m_bRejectClanChat; }
	void SetRejectClanChat(bool bVal)	{ m_bRejectClanChat = bVal; }
	bool GetRejectWhisper()				{ return m_bRejectWhisper; }
	void SetRejectWhisper(bool bVal)	{ m_bRejectWhisper = bVal; }
	bool GetRejectInvite()				{ return m_bRejectInvite; }
	void SetRejectInvite(bool bVal)		{ m_bRejectInvite = bVal; }
	int	GetPingLimitStrikes() { return m_nPingLimitStrikes; }
	void SetPingLimitStrikes(int nlimit) { m_nPingLimitStrikes = nlimit; }
	bool GetCrash() { return m_bEnableCrash; }
	void SetCrash(bool bVal) { m_bEnableCrash = bVal; }
#ifdef _SRVRPNG
	DWORD GetServerPing() { return m_ServerPing; }
#endif
#ifdef _SRVRPNG
	void OnServerPing(DWORD ping, bool ServerType = true);
#endif

	// Custom: Ignore List
	bool IsUserIgnored( const char* szCharName );
	void SetIgnoreUser( const char* szCharName, bool bIgnore );
	list<string> GetIgnoreList()		{ return m_IgnoreList; }
	int GetIgnoreListCount()			{ return m_IgnoreList.size(); }

	unsigned long int GetClockCount(void) { return timeGetTime(); }		// 로컬 클럭 반환
	unsigned long int GetGlobalClockCount(void);		///< 서버와 동기된 클럭을 반환한다.

	virtual void OutputMessage(const char* szMessage, MZMOMType nType=MZMDM_GENERAL);
	
	void SetOnCommandCallback(ZONCOMMANDCALLBACK pCallback) { m_fnOnCommandCallback = pCallback;}

//	void InitializeGame();
//	void StartGame();
//	void FinalizeGame();

	void Tick(void);
	void Disconnect()							{ MMatchClient::Disconnect(m_Server); }
	
	MMatchStageSetting* GetMatchStageSetting() { return &m_MatchStageSetting; }
	bool IsForcedEntry() { return m_bForcedEntry; }
	bool IsAllChannel() { return (m_CurrentChannelType == MCHANNEL_TYPE_MAX); }
	bool IsLadderChannel() { return (m_CurrentChannelType == MCHANNEL_TYPE_LADDER); }
	bool IsCWChannel() { return (m_CurrentChannelType == MCHANNEL_TYPE_CLAN); }
	bool IsDTChannel() { return (m_CurrentChannelType == MCHANNEL_TYPE_DUELTOURNAMENT); }
	bool IsDuelTournamentGame() { return m_MatchStageSetting.GetGameType() == MMATCH_GAMETYPE_DUELTOURNAMENT; }
	
	void ReleaseForcedEntry();
	void ClearStageSetting();
public:
	void RequestPrevStageList();
	void RequestNextStageList();
	void RequestStageList(int nPage);
	void StartStageList();
	void StopStageList();
	void StartChannelList(MCHANNEL_TYPE nChannelType);
	void StopChannelList();
	//void SetChannelType(MCHANNEL_TYPE type);

	const char*		GetChannelName()		{ return m_szChannel; }
	MCHANNEL_TYPE	GetChannelType()		{ return m_CurrentChannelType; }
	bool			GetEnableInterface()	{ return m_bEnableInterface; }
	const char*		GetChannelRuleName()	{ return m_szChannelRule; }
	const char*		GetStageName()			{ return m_szStageName; }
	int				GetStageNumber()		{ return m_nRoomNo; }

	void SetStageName( const char* pszStageName ) { strcpy( m_szStageName, pszStageName); } // Custom: For replays
	
	
	
	const char* GetChatRoomInvited(){ return m_szChatRoomInvited; }
	void SetChatRoomInvited(const char* pszRoomName)	{ strcpy(m_szChatRoomInvited, pszRoomName); }

	bool AmIStageMaster() { return (m_MatchStageSetting.GetMasterUID() == GetPlayerUID()); }

	const char* GetVoteMessage() { return m_szVoteText; }
public:
	// 클랜 관련
	void AnswerSponsorAgreement(bool bAnswer);
	void AnswerJoinerAgreement(bool bAnswer);
	void RequestCreateClan(char* szClanName, char** ppMemberCharNames);

	// 동의 관련
	void RequestProposal(const MMatchProposalMode nProposalMode, char** ppReplierCharNames, const int nReplierCount);
	void ReplyAgreement(bool bAgreement);
public:	// 투표관련
	bool IsVoteInProgress()				{ return m_bVoteInProgress;	}
	void SetVoteInProgress(bool bVal)	{ m_bVoteInProgress = bVal; }
	bool CanVote()						{ return m_bCanVote; }
	void SetCanVote(bool bVal)			{ m_bCanVote = bVal; }

public:
	void RequestGameSuicide();
	// game 에서 불러준다
	//void OnGameRoundState(const MUID& uidStage, int nRoundState, int nRound);
	void OnStageEnterBattle(const MUID& uidChar, MCmdEnterBattleParam nParam);
public:
	// Validate 씨리즈
	int ValidateRequestDeleteChar();
public:
	// Request 씨리즈
	void RequestChannelJoin(const MUID& uidChannel);
	void RequestChannelJoin(const MCHANNEL_TYPE nChannelType, char* szChannelName);
	void RequestOnLobbyCreated();		// 로비에 들어올때 요청하는 메시지들
	void RequestOnGameDestroyed();		// 게임이 끝났을때 요청하는 메시지들
protected:
	void OnNotifyCallVote(const char* pszDiscuss, const char* pszArg);
	void OnNotifyVoteResult(const char* pszDiscuss, int nResult);
	void OnVoteAbort( const int nMsgCode );
protected:
	void OnBroadcastClanRenewVictories(const char* pszWinnerClanName, const char* pszLoserClanName, int nVictories);
	void OnBroadcastClanInterruptVictories(const char* pszWinnerClanName, const char* pszLoserClanName, int nVictories);
	void OnBroadcastDuelRenewVictories(const char* pszChampionName, const char* pszChannelName, int nRoomno, int nVictories);
	void OnBroadcastDuelInterruptVictories(const char* pszChampionName, const char* pszInterrupterName, int nVictories);
protected:
	// Emblem 관련
	void ProcessEmblem(unsigned int nCLID, unsigned int nChecksum);
	void RequestEmblemURL(unsigned int nCLID);
	void OnClanResponseEmblemURL(unsigned int nCLID, unsigned int nEmblemChecksum, const char* szEmblemURL);
	void OnClanEmblemReady(unsigned int nCLID, const char* szURL);

public:
	MEmblemMgr *GetEmblemManager() { return &m_EmblemMgr; }	

	// UPnP
protected:
	UPnP *m_pUPnP;

public:
	bool CreateUPnP(unsigned short nUDPPort);
	bool DestroyUPnP();
	UPnP* GetUDPInfo()		{ return m_pUPnP; }

//	void ChangeQuestStage();
	void OnRecieveGambleItem( unsigned int nRecvItem, unsigned int nCnt, unsigned int nTime);

	void GetEncryptMD5HashValue(char* szEncryptMD5Value);
#ifdef _HWID
	void GetHWID(char* hdID);
#endif
protected:
	void OnDuelTournamentPrepare(MDUELTOURNAMENTTYPE nType, MUID uidStage, void* pBlobPlayerInfo);
	void OnDuelTournamentLaunch(const MUID& uidStage, const char* pszMapName);

protected:
	ZCommandUDPHackShield m_UPDCommadHackShield;
public:
	bool IsUDPCommandValidationCheck(int nCommandID);

protected:
	void OnAdminResponseKickPlayer(int nResult);
	void OnAdminResponseBlockPlayer(int nResult);
	void OnAdminResponseMutePlayer(int nResult);

public:
	void OnMedalsReceivedResponse(const MUID& uidObj);
};





bool ZPostCommand(MCommand* pCmd);

// 내 플레이어의 uid
#define ZGetMyUID() (ZGetGameClient() ? ZGetGameClient()->GetPlayerUID() : MUID(0,0))


MCommand* ZNewCmd(int nID);

unsigned long int ZGetClockDistance(unsigned long int nGlobalClock, unsigned long int nLocalClock);

#ifdef _TDD
bool ZTddNeverPostCmd();
#endif

// Post Command Macro For Convenience
#define ZPOSTCMD0(_ID)									{ MCommand* pC=ZNewCmd(_ID); ZPostCommand(pC); }
#define ZPOSTCMD1(_ID, _P0)								{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); ZPostCommand(pC); }
#define ZPOSTCMD2(_ID, _P0, _P1)						{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); ZPostCommand(pC); }
#define ZPOSTCMD3(_ID, _P0, _P1, _P2)					{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ZPostCommand(pC); }
#define ZPOSTCMD4(_ID, _P0, _P1, _P2, _P3)				{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ZPostCommand(pC); }
#define ZPOSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)			{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ZPostCommand(pC); }
#define ZPOSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ZPostCommand(pC); }
#define ZPOSTCMD7(_ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); pC->AP(_P6); ZPostCommand(pC); }

#ifdef _ANTILEADNEW
#define ZPOSTCMDLEAD(_ID, _UID, _P0)						{ MCommand* pC=ZNewCmd(_ID); pC->AddParameter(new _P0); pC->m_Receiver = _UID; ZPostCommand(pC); }
#endif

#define ZPOSTDMGTAKEN(_P0, _P1, _UID)	{ MCommand* pC=ZNewCmd(MC_GUNZ_DMGGIVEN); pC->AP(_P0); pC->AP(_P1); pC->SetReceiverUID(_UID); ZPostCommand(pC); }
#define ZPOSTLASTDMG(_P0, _P1)	{ MCommand* pC=ZNewCmd(MC_GUNZ_LASTDMG); pC->AP(_P0); pC->AP(_P1); ZPostCommand(pC); }

#define HANDLE_COMMAND(message, fn)    \
	case (message): return fn(pCommand);

bool GetUserInfoUID(MUID uid,MCOLOR& _color,char* sp_name,MMatchUserGradeID& gid);


#endif