#pragma once

// Sort this stuff sometime maybe '__'

#include <mutex>
#include <queue>
#include <memory>
#include "VoiceChat.h"
#include "NewChat.h"
#include "optional.h"

class ZChatCmdManager;
class MEvent;
class ZIDLResource;

//void LoadRGCommands(ZChatCmdManager &CmdManager);
enum class PlayerFoundStatus
{
	FoundOne,
	NotFound,
	TooManyFound,
};

std::pair<PlayerFoundStatus, ZCharacter*> FindSinglePlayer(const char* NameSubstring);

class RGMain
{
public:
	RGMain();
	RGMain(const RGMain&) = delete;
	~RGMain();

	void OnUpdate(double Elapsed);
	bool OnEvent(MEvent *pEvent);
	void OnReset();
	void OnCreateDevice();
	void OnAppCreate();

#ifdef _VOICE_CHAT
	auto MutePlayer(const MUID& UID) { return m_VoiceChat.MutePlayer(UID); }
#endif

	void OnDrawLobby(MDrawContext* pDC);
	void OnDrawGame();
	void OnDrawGameInterface(MDrawContext* pDC);
	void OnGameCreate();
	bool OnGameInput();

	void Resize(int w, int h);

	void AddMapBanner(const char* MapName, MBitmap* Bitmap) { MapBanners.insert({ MapName, Bitmap }); }

	void OnReceiveVoiceChat(ZCharacter *Char, const uint8_t *Buffer, int Length);

#ifdef _SWORDCOLOR 1
	void SetSwordColor(const MUID& UID, uint32_t Hue);
#endif

	bool IsCursorEnabled() const;

	double GetTime() const { return Time; }
	double GetElapsedTime() const { return Time - LastTime; }
#ifdef _SWORDCOLOR 1
	std::pair<bool, uint32_t> GetPlayerSwordColor(const MUID& UID);
#endif
	// Invokes a callback on the main thread
	template<typename T>
	void Invoke(T Callback, double Delay = 0)
	{
		std::lock_guard<std::mutex> lock(QueueMutex);
		QueuedInvokations.push_back({ Callback, Time + Delay });
	}

	Chat& GetChat() { return m_Chat.value(); }
	const Chat& GetChat() const { return m_Chat.value(); }

private:
	//friend void LoadRGCommands(ZChatCmdManager& CmdManager);

	double Time = 0;
	double LastTime = 0;

	struct Invokation
	{
		std::function<void()> fn;
		double Time;
	};

	std::vector<Invokation> QueuedInvokations;
	std::mutex QueueMutex;

	bool Selected = false;

	std::unordered_map<std::string, MBitmap *> MapBanners;
#ifdef _SWORDCOLOR 1
	std::unordered_map<MUID, uint32_t> SwordColors;
#endif
	optional<Chat> m_Chat;


	//bool NewChatEnabled = true;

	bool NewChatEnabled = false;


#ifdef _VOICE_CHAT
	VoiceChat m_VoiceChat;
#endif

};

 RGMain& GetRGMain();
 void CreateRGMain();
 void DestroyRGMain();

 inline Chat& GetNewChat() { return GetRGMain().GetChat(); }


inline ZMyCharacter* MyChar()
{
	return ZGetGame()->m_pMyCharacter;
}
